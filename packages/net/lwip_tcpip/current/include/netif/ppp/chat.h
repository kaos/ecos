/**
 * @file
 * Chat interface
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

#ifndef CHAT_H
#define CHAT_H

#include "lwip/sio.h"

#define CHAT_TMR_INTERVAL   500     /**< Chat timer interval in ms */

/** Chat commands */
typedef enum {
  CHAT_ABORT,   /**< Defines a string which aborts the chat conversation */
  CHAT_SAY,     /**< Prints an informational text */
  CHAT_SEND,    /**< Sends a command to the modem */
  CHAT_SEND_CB, /**< Sends a command to the modem (gets string via callback) */
  CHAT_WAIT,    /**< Waits for an answer from the modem */
  CHAT_SLEEP,   /**< Sleeps for at least 1 second */
  CHAT_LAST,    /**< Last item in chat table */
} chat_cmd_t;

/** Chat item */
struct chat_item {
  chat_cmd_t cmd;   /**< Chat command */
  char *arg;        /**< Command argument */
};

/** Chat error codes */
typedef enum {
  CHAT_ERR_OK,      /**< Chat finished successfully */
  CHAT_ERR_TIMEOUT, /**< Chat timeout */
  CHAT_ERR_ABORT,   /**< Chat aborted */
} chat_err_t;

/** Chat context */
typedef struct chat *chat_t;

/** Chat callback */
typedef void (*chat_cb_t)(chat_t chat, chat_err_t err, void *arg);

/** Chat send callback */
typedef void (*chat_send_cb_t)(chat_t chat, int id, char *buf, size_t len, void *arg);

/**
 * Creates a new chat. The chat is further processed by calling chat_poll().
 * The result of the chat is posted by the chat_cb() callback function.
 * @param sd Serial io descriptor
 * @param items Chat items
 * @param timeout Timeout in seconds
 * @param chat_cb Chat result callback
 * @param send_cb Chat send callback
 * @param arg Argument used in callback
 * @return Returns the newly allocated chat, or NULL if no memory was available.
 */
chat_t chat_new(sio_fd_t sd, const struct chat_item *items, int timeout,
                chat_cb_t chat_cb, chat_send_cb_t send_cb, void *arg);

/**
 * Frees a chat.
 * @param chat Chat
 */
void chat_free(chat_t chat);

/**
 * Processes the chat. This needs to be called iteratively until the chat_cb()
 * callback function posts success or an error.
 * @param chat Chat
 */
void chat_poll(chat_t chat);

/**
 * This should be called every CHAT_TMR_INTERVAL milliseconds.
 */
void chat_tmr(void);

#endif /* CHAT_H */
