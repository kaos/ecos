/**
 * @file
 * Chat implementation
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Simon Kallweit <simon.kallweit(at)intefo.ch>
 */

/*
 * This is an arch independent chat implementation. The specific serial hooks
 * must be provided by another file. They are sio_open, sio_recv and sio_send.
 */

#include "netif/ppp/chat.h"
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/sio.h"

#define BUFSIZE 64

/** Chat states */
enum chat_state {
  STATE_INITIAL,    /**< Initial state, checks the current item, executes it
                         directly or transitions to a new state to handle the
                         item. */
  STATE_WAIT,       /**< Waits for an answer from the modem. */
  STATE_SLEEP,      /**< Sleep for at least 1 second. */
  STATE_NEXT_ITEM,  /**< Moves to the next item. */
  STATE_IDLE,       /**< Chat finished. */
};

/** Chat */
struct chat {
  struct chat *next;        /**< Pointer to the next chat instance */
  /* Settings */
  sio_fd_t sd;              /**< Serial device descriptor */
  const struct chat_item const *items; /**< Chat items */
  int timeout;              /**< Default timeout in seconds */
  chat_cb_t chat_cb;        /**< Chat callback function */
  chat_send_cb_t send_cb;   /**< Chat send callback function */
  void *arg;                /**< Argument for callback function */
  /* State */
  enum chat_state state;    /**< Current state */
  const struct chat_item *item; /**< Current chat item */
  char buf[BUFSIZE + 1];   /**< Receive buffer */
  int recvd;                /**< Received bytes */
  int timeleft;             /**< Seconds left until timeout */
};

/** Linked list of chats */
static struct chat *chats;

static void chat_send(struct chat *chat, const struct chat_item *item);
static void chat_send_cb(struct chat *chat, const struct chat_item *item);
static int chat_wait(struct chat *chat, const struct chat_item *item);

/*
 * Creates a new chat. The chat is further processed by calling chat_poll().
 * The result of the chat is posted by the chat_cb() callback function.
 */
chat_t chat_new(sio_fd_t sd, const struct chat_item const *items, int timeout,
                chat_cb_t chat_cb, chat_send_cb_t send_cb, void *arg)
{
  struct chat *chat;
  const struct chat_item *item;

  /* Some sanity checks */
  LWIP_ASSERT("chat_new: sd != NULL", sd != NULL);
  LWIP_ASSERT("chat_new: items != NULL", items != NULL);
  LWIP_ASSERT("chat_new: chat_cb != NULL", chat_cb != NULL);

  /* Check for correct chat items */
  for (item = items; item->cmd != CHAT_LAST; item++)
    LWIP_ASSERT("chat_new: invalid chat item or CHAT_LAST missing",
                item->cmd >= CHAT_ABORT && item->cmd < CHAT_LAST);

  /* Allocate chat */
  chat = mem_malloc(sizeof(struct chat));
  if (!chat)
    return NULL;

  /* Iinitialize chat */
  chat->sd = sd;
  chat->items = items;
  chat->timeout = timeout * 1000 + 1;
  chat->chat_cb = chat_cb;
  chat->send_cb = send_cb;
  chat->arg = arg;

  /* Reset state */
  chat->state = STATE_INITIAL;
  chat->item = items;
  chat->recvd = 0;

  /* Add chat to list */
  chat->next = chats;
  chats = chat;

  return chat;
}

/*
 * Frees a chat.
 */
void chat_free(struct chat *chat)
{
  struct chat *t;

  if (!chat)
    return;

  /* Remove chat from list */
  if (chat == chats) {
    chats = chat->next;
  } else {
    for (t = chats; t->next; t = t->next)
      if (t->next == chat) {
        t->next = chat->next;
        break;
      }
  }

  mem_free(chat);
}

/*
 * Processes the chat. This needs to be called iteratively until the chat_cb()
 * callback function posts success or an error.
 */
void chat_poll(struct chat *chat)
{
  int done = 0;
  int err;

  LWIP_ASSERT("chat_poll: chat != NULL", chat != NULL);

  while (!done) {
    switch (chat->state) {
    case STATE_INITIAL:
      /* Check item command */
      switch (chat->item->cmd) {
      case CHAT_ABORT:
        chat->state = STATE_NEXT_ITEM;
        break;
      case CHAT_SAY:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: SAY '%s'\n", chat->item->arg));
        chat->state = STATE_NEXT_ITEM;
        break;
      case CHAT_SEND:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: SEND '%s'\n", chat->item->arg));
        chat_send(chat, chat->item);
        chat->state = STATE_NEXT_ITEM;
        break;
      case CHAT_SEND_CB:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: SEND_CB '%d'\n", (int) chat->item->arg));
        chat_send_cb(chat, chat->item);
        chat->state = STATE_NEXT_ITEM;
        break;
      case CHAT_WAIT:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: WAIT '%s'\n", chat->item->arg));
        chat->timeleft = chat->timeout;
        chat->state = STATE_WAIT;
        break;
      case CHAT_SLEEP:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: SLEEP\n"));
        chat->timeleft = 1000 + 1;
        if (chat->item->arg)
          chat->timeleft = (int) chat->item->arg + 1;
        chat->state = STATE_SLEEP;
        break;
      case CHAT_LAST:
        LWIP_DEBUGF(PPP_DEBUG, ("chat_poll: SUCCESS\n"));
        chat->chat_cb(chat, CHAT_ERR_OK, chat->arg);
        chat->state = STATE_IDLE;
        break;
      }
      break;
    case STATE_WAIT:
      err = chat_wait(chat, chat->item);
      if (err == -1) {
        done = 1;
      } else if (err == CHAT_ERR_OK) {
        chat->state = STATE_NEXT_ITEM;
      } else {
        chat->chat_cb(chat, err, chat->arg);
        chat->state = STATE_IDLE;
      }
      break;
    case STATE_SLEEP:
      if (chat->timeleft == 0)
        chat->state = STATE_NEXT_ITEM;
      else
        done = 1;
      break;
    case STATE_NEXT_ITEM:
      chat->item++;
      chat->state = STATE_INITIAL;
      break;
    case STATE_IDLE:
      done = 1;
      break;
    }
  }
}

/*
 * This should be called every CHAT_TMR_INTERVAL milliseconds.
 */
void chat_tmr(void)
{
  struct chat *chat;

  LWIP_DEBUGF(PPP_DEBUG, ("chat_tmr\n"));

  for (chat = chats; chat; chat = chat->next) {
    if (chat->timeleft > CHAT_TMR_INTERVAL)
      chat->timeleft -= CHAT_TMR_INTERVAL;
    else
      chat->timeleft = 0;
    LWIP_DEBUGF(PPP_DEBUG, ("chat_tmr: %d\n", chat->timeleft));
  }
}


/**
 * Sends a chat message to the modem.
 * @param chat Chat
 * @param item Item to send
 */
static void chat_send(struct chat *chat, const struct chat_item *item)
{
  sio_write(chat->sd, (u8_t *) item->arg, strlen(item->arg));
  chat->recvd = 0;
}

/**
 * Sends a chat message to the modem. Gets the send string via a callback.
 * @param chat Chat
 * @param item Item to send
 */
static void chat_send_cb(struct chat *chat, const struct chat_item *item)
{
  LWIP_ASSERT("chat_poll: chat->send_cb != NULL", chat->send_cb != NULL);
  
  // Get the command to send
  chat->send_cb(chat, (int) item->arg, chat->buf, BUFSIZE, chat->arg);
  chat->buf[BUFSIZE] = '\0';
  
  // Send command
  sio_write(chat->sd, (u8_t *) chat->buf, strlen(chat->buf));
  chat->recvd = 0;
}

/**
 * Waits for an response from the modem. This function returns:
 *   -1: when no more data could be read from the serial (polling)
 *   CHAT_ERR_OK: when the correct response was received
 *   CHAT_ERR_ABORT: when an abort response was received
 *   CHAT_ERR_TIMEOUT: when a timeout occured
 * @param chat Chat
 * @param item Item to wait for
 * @return See above.
 */
static int chat_wait(struct chat *chat, const struct chat_item *item)
{
  int count;
  const struct chat_item *abort;

  while (chat->recvd < BUFSIZE) {
    /* Receive data from serial */
    if ((count = sio_read(chat->sd, (u8_t *) &chat->buf[chat->recvd],
                          BUFSIZE - chat->recvd)) < 1)
      break;
    chat->recvd += count;
    chat->buf[chat->recvd] = '\0';

    /* Check abort strings */
    for (abort = chat->items; abort->cmd != CHAT_LAST; abort++) {
      if (abort->cmd != CHAT_ABORT)
        continue;
      if (strstr(chat->buf, abort->arg)) {
        LWIP_DEBUGF(PPP_DEBUG, ("chat_wait: ABORT '%s'\n", abort->arg));
        return CHAT_ERR_ABORT;
      }
    }

    /* Check wait string */
    if (strstr(chat->buf, item->arg)) {
      LWIP_DEBUGF(PPP_DEBUG, ("chat_wait: SUCCESS\n"));
      return CHAT_ERR_OK;
    }
  }

  /* Check for timeout */
  if (chat->timeleft == 0) {
    LWIP_DEBUGF(PPP_DEBUG, ("chat_wait: TIMEOUT\n"));
    return CHAT_ERR_TIMEOUT;
  }

  return -1;
}
