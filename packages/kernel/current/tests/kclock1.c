/*=================================================================
//
//        kclock1.c
//
//        Kernel C API Clock test 1 - Real Time Clock
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-03-20
// Description:   Tests the Kernel Real Time Clock
//                This test creates a thread, starts the scheduler and
//                delays for a time of about 5 seconds.  This test should
//                be expected to run for about this length of time.
// Omissions:
//     Doesn't test alarms attached to RTC.
// Assumptions:
//     CYGVAR_KERNEL_COUNTERS_CLOCK must be set.
//     Resolution of clock small compared with 5s.
//     Overhead small compared with 5s.
// Options:
//     CYGIMP_KERNEL_COUNTERS_SINGLE_LIST
//     CYGIMP_KERNEL_COUNTERS_MULTI_LIST
//     CYGVAR_KERNEL_COUNTERS_CLOCK
//     CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE
//####DESCRIPTIONEND####
*/

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#if defined(CYG_HAL_MIPS_SIM)

// Reduce time in simulated TX39 so that
// it runs within the timeout.

#define TEST_DELAY     1000000000ll
    
#else

#define TEST_DELAY     5000000000ll

#endif    

#if defined(CYGVAR_KERNEL_COUNTERS_CLOCK) \
    && defined(CYGFUN_KERNEL_THREADS_TIMER)

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#define NTHREADS 1
#define STACKSIZE 4096

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

static void entry0( cyg_addrword_t data )
{
    cyg_resolution_t res;
    cyg_uint32 ticks;
    cyg_tick_count_t count0, count1;
    cyg_handle_t rtclock, rtcounter;

    rtclock = cyg_real_time_clock();
    cyg_clock_to_counter(rtclock, &rtcounter);

    res = cyg_clock_get_resolution (rtclock);

    /* RTC takes res.dividend/res.divisor ns/tick */
    ticks = ((cyg_uint64)TEST_DELAY * res.divisor) / res.dividend;

    count0 = cyg_counter_current_value(rtcounter);
    cyg_thread_delay(ticks);
    count1 = cyg_counter_current_value(rtcounter);

    CYG_TEST_CHECK(count0+ticks <= count1,
                   "real time clock's counter not counting");

    CYG_TEST_CHECK(count1 <= cyg_current_time(),"cyg_current_time()");

    CYG_TEST_PASS_FINISH("Kernel C API Clock 1 OK");
}

void kclock1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "kclock1",
    	(void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();
}

externC void
cyg_start( void )
{
    kclock1_main();
}

#else // def CYGFUN_KERNEL_API_C
#define N_A_MSG "Kernel C API layer disabled"
#endif // def CYGFUN_KERNEL_API_C
#else // def CYGVAR_KERNEL_COUNTERS_CLOCK && CYGFUN_KERNEL_THREADS_TIMER
#define N_A_MSG "Kernel real-time clock/threads timer disabled"
#endif // def CYGVAR_KERNEL_COUNTERS_CLOCK && CYGFUN_KERNEL_THREADS_TIMER

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH("N/A: " N_A_MSG);
}
#endif // N_A_MSG

// EOF kclock1.c
