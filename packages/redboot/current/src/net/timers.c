//==========================================================================
//
//      net/timers.c
//
//      Stand-alone networking support for RedBoot
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <net/net.h>

static timer_t *tmr_list;


/*
 * Set a timer. Caller is responsible for providing the timer_t struct.
 */
void
__timer_set(timer_t *t, unsigned long delay,
	    tmr_handler_t handler, void *user_data)
{
    timer_t *p;

    t->delay = delay;
    t->start = MS_TICKS();
    t->handler = handler;
    t->user_data = user_data;

    for (p = tmr_list; p; p = p->next)
	if (p == t) {
	    return;
	}

    t->next = tmr_list;
    tmr_list = t;
}


/*
 * Remove a given timer from timer list.
 */
void
__timer_cancel(timer_t *t)
{
    timer_t *prev, *p;

    for (prev = NULL, p = tmr_list; p; prev = p, p = p->next)
	if (p == t) {
	    if (prev)
		prev->next = p->next;
	    else
		tmr_list = p->next;
	    return;
	}
}


/*
 * Poll timer list for timer expirations.
 */
void
__timer_poll(void)
{
    timer_t *prev, *t;

    prev = NULL;
    t = tmr_list;
    while (t) {
	if ((MS_TICKS() - t->start) >= t->delay) {

	    /* remove it before calling handler */
	    if (prev)
		prev->next = t->next;
	    else
		tmr_list = t->next;
	    /* now, call the handler */
	    t->handler(t->user_data);
	    
	    /*
	     * handler may be time consuming, so start
	     * from beginning of list.
	     */
	    prev = NULL;
	    t = tmr_list;
	} else {
	    prev = t;
	    t = t->next;
	}
    }
}
