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

#ifndef TIMESYS_H
#define TIMESYS_H

#include "lwip/sys.h"
#include "lwip/err.h"

typedef void (*ppp_timeout_handler_t)(void *arg);

#define TIMEOUT(f, a, t)    ppp_timeout(f, a, t)
#define UNTIMEOUT(f, a)     ppp_untimeout(f, a)

/**
 * Initialize the timesys.
 * @return Returns ERR_OK if successful.
 */
err_t ppp_timesys_init(void);

/**
 * Reset the timesys. Removes all timeouts from the list.
 */
void ppp_timesys_reset(void);

/**
 * Update the timesys.
 * @param delta Update interval in milliseconds.
 */
void ppp_timesys_update(u32_t delta);

/**
 * Returns the current time.
 * @return Returns the time in jiffies.
 */
u32_t ppp_jiffies(void);

/**
 * Setup a timeout.
 * @param handler Timeout handler callback
 * @param arg Timeout handler argument
 * @param timeout Timeout in seconds
 */
void ppp_timeout(ppp_timeout_handler_t handler, void *arg, int timeout);

/**
 * Clear a timeout.
 * @param handler Timeout handler callback
 * @param arg Timeout handler argument
 */
void ppp_untimeout(ppp_timeout_handler_t handler, void *arg);

#endif /* TIMESYS_H */
