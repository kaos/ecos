//==========================================================================
//
//        lib/timeout.c
//
//        timeout support
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
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas, hmt
// Contributors:  gthomas, hmt
// Date:          1999-02-05
// Description:   Simple timeout functions
//####DESCRIPTIONEND####

#include <sys/param.h>
#include <pkgconf/net.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_ass.h>

// Timeout support

#ifndef NTIMEOUTS
#define NTIMEOUTS 8
#endif
typedef struct {
    cyg_int32     delta;  // Number of "ticks" in the future for this timeout
    timeout_fun  *fun;    // Function to execute when it expires
    void         *arg;    // Argument to pass when it does
} timeout_entry;
static timeout_entry timeouts[NTIMEOUTS];
static cyg_handle_t timeout_alarm_handle;
static cyg_alarm timeout_alarm;
static cyg_int32 last_delta;
static cyg_tick_count_t last_set_time;

extern cyg_uint32 cyg_in_softnet( void );

static void
do_timeout(cyg_handle_t alarm, cyg_addrword_t data)
{
    int i;
    cyg_int32 min_delta;
    timeout_entry *e;

    CYG_ASSERT( 0 < last_delta, "last_delta underflow" );

    min_delta = last_delta; // local copy
    last_delta = -1; // flag recursive call underway

    for (e = timeouts, i = 0;  i < NTIMEOUTS;  i++, e++) {
        if (e->delta) {
            CYG_ASSERT( e->delta >= min_delta, "e->delta underflow" );
            e->delta -= min_delta;
            if (e->delta <= 0) { // Defensive
                // Time for this item to 'fire'
                timeout_fun *fun = e->fun;
                void *arg = e->arg;
                // Call it *after* cleansing the record
                e->fun = 0;
                e->delta = 0;
                (*fun)(arg);
            }
        }
    }

    // Now scan for a new timeout *after* running all the callbacks
    // (because they can add timeouts themselves)
    min_delta = 0x7FFFFFFF;  // Maxint
    for (e = timeouts, i = 0;  i < NTIMEOUTS;  i++, e++)
        if (e->delta)
            if (e->delta < min_delta)
                min_delta = e->delta;

    CYG_ASSERT( 0 < min_delta, "min_delta underflow" );

    if (min_delta != 0x7FFFFFFF) {
        // Still something to do, schedule it
        last_set_time = cyg_current_time();
        cyg_alarm_initialize(timeout_alarm_handle, last_set_time+min_delta, 0);
        last_delta = min_delta;
    } else {
        last_delta = 0; // flag no activity
    }
}

cyg_uint32
timeout(timeout_fun *fun, void *arg, cyg_int32 delta)
{
    int i;
    static bool init = false;
    timeout_entry *e;
    cyg_uint32 stamp;

    CYG_ASSERT( 0 < delta, "delta is right now, or even sooner!" );

    // this needs to be atomic wrt threads and DSRs
    cyg_scheduler_lock();

    if (!init) {
        cyg_handle_t h;
        cyg_clock_to_counter(cyg_real_time_clock(), &h);
        cyg_alarm_create(h, do_timeout, 0, &timeout_alarm_handle, &timeout_alarm);
        init = true;
    }

    // Renormalize delta wrt the existing set alarm, if there is one
    if ( last_delta > 0 )
        delta += (cyg_int32)(cyg_current_time() - last_set_time);
    // So recorded_delta is set to either:
    // alarm is active:   delta + NOW - THEN
    // alarm is inactive: delta

    stamp = 0;  // Assume no slots available
    for (e = timeouts, i = 0;  i < NTIMEOUTS;  i++, e++) {
        if ((e->delta == 0) && (e->fun == 0)) {
            // Free entry
            e->delta = delta;
            e->fun = fun;
            e->arg = arg;
            stamp = (cyg_uint32)e;
            break;
        }
    }

    if ( stamp &&                 // we did add a record AND
         (0 == last_delta ||      // alarm was inactive  OR
          delta < last_delta) ) { // alarm was active but later than we need

        // (if last_delta is -1, this call is recursive from the handler so
        //  also do nothing in that case)

        // Here, we know the new item added is sooner than that which was
        // most recently set, if any, so we can just go and set it up.
        if ( 0 == last_delta )
            last_set_time = cyg_current_time();
        
        // So we use, to set the alarm either:
        // alarm is active:   (delta + NOW - THEN) + THEN
        // alarm is inactive:  delta + NOW
        // and in either case it is true that
        //  (recorded_delta + last_set_time) == (delta + NOW)
        cyg_alarm_initialize(timeout_alarm_handle, last_set_time+delta, 0);
        last_delta = delta;
    }
    // Otherwise, the alarm is active, AND it is set to fire sooner than we
    // require, so when it does, that will sort out calling the item we
    // just added.  Or we didn't actually add a record, so nothing has
    // changed.

#ifdef CYGPKG_INFRA_DEBUG
    // Do some more checking akin to that in the alarm handler:
    if ( last_delta != -1 ) { // not a recursive call
        CYG_ASSERT( last_delta >= 0, "Bad last delta" );
        delta = 0x7fffffff;
        for (e = timeouts, i = 0;  i < NTIMEOUTS;  i++, e++) {
            if (e->delta) {
                CYG_ASSERT( e->delta >= last_delta, "e->delta underflow" );
                CYG_ASSERT( last_set_time + e->delta > cyg_current_time(),
                            "Recorded alarm not in the future!" );
                if ( e->delta < delta )
                    delta = e->delta;
            } else {
                CYG_ASSERT( 0 == e->fun, "Function recorded for 0 delta" );
            }
        }
        CYG_ASSERT( delta == last_delta, "We didn't pick the smallest delta!" );
    }
#endif

    cyg_scheduler_unlock();

    return stamp;
}

void
untimeout(timeout_fun *fun, void * arg)
{
    int i;
    timeout_entry *e;
    cyg_scheduler_lock();
    for (e = timeouts, i = 0; i < NTIMEOUTS; i++, e++) {
        if (e->delta && (e->fun == fun) && (e->arg == arg)) {
            e->delta = 0;
            e->fun = 0;
            break;
        }
    }
    cyg_scheduler_unlock();
}

// EOF timeout.c
