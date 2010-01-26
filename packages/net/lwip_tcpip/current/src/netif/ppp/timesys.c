/**
 * @file
 * System time and timeout system
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

#include "lwip/mem.h"

#include "netif/ppp/ppp.h"
#include "netif/ppp/timesys.h"

/* Milliseconds per jiffy */
#define MS_PER_JIFFY    10

/* PPP timeout */
struct ppp_timeout {
  struct ppp_timeout *next;
  ppp_timeout_handler_t handler;
  void *arg;
  u32_t timeout;
};

/* Current system time in ms */
static u32_t current_time;

/* Linked list of timeouts */
static struct ppp_timeout *timeouts;



/*
 * Initialize the timesys.
 */
err_t
ppp_timesys_init(void)
{
  return ERR_OK;
}

/*
 * Reset the timesys. Removes all timeouts from the list.
 */
void
ppp_timesys_reset(void)
{
  struct ppp_timeout *t;
    
  for (t = timeouts; t; t = t->next)
    t->handler = NULL;
}

/*
 * Update the timesys.
 */
void
ppp_timesys_update(u32_t delta)
{
  struct ppp_timeout *t;

  current_time += delta;

  /* Update timeouts */
  for (t = timeouts; t; t = t->next)
    if (t->handler) {
      if (t->timeout > delta) {
        t->timeout -= delta;
      } else {
        t->handler(t->arg);
        t->handler = NULL;
      }
    }
}

/*
 * Returns the current time.
 */
u32_t
ppp_jiffies(void)
{
  return current_time / MS_PER_JIFFY;
}

/*
 * Setup a timeout.
 */
void
ppp_timeout(ppp_timeout_handler_t handler, void *arg, int timeout)
{
  struct ppp_timeout *t;

  /* Reuse timeout slot if possible */
  for (t = timeouts; t; t = t->next)
    if (!t->handler) {
      goto setup_timeout;
    }

  /* Allocate new timeout */
  t = mem_malloc(sizeof(struct ppp_timeout));
  LWIP_ASSERT("ppp_timeout: t != NULL", t != NULL);

  /* Append to list */
  t->next = timeouts;
  timeouts = t;

setup_timeout:
  t->handler = handler;
  t->arg = arg;
  t->timeout = timeout * 1000;
}

/*
 * Clear a timeout.
 */
void
ppp_untimeout(ppp_timeout_handler_t handler, void *arg)
{
  struct ppp_timeout *t;

  /* Reuse timeout slot if possible */
  for (t = timeouts; t; t = t->next)
    if ((t->handler == handler) && (t->arg == arg)) {
      t->handler = NULL;
    }
}
