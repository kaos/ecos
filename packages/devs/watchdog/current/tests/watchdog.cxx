//==========================================================================
//
//        watchdog.cxx
//
//        Watchdog test
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:    nickg
// Date:          1998-07-20
// Description:   Tests the Kernel Wall Clock
//                This test exercises the Watchdog class and checks that it
//                works as advertised.
//####DESCRIPTIONEND####
// -------------------------------------------------------------------------

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>
#include <cyg/kernel/clock.hxx>

#include <cyg/kernel/diag.h>

#include <cyg/devs/watchdog.hxx>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>

#if defined(CYGFUN_KERNEL_THREADS_TIMER) && \
    defined(CYGVAR_KERNEL_COUNTERS_CLOCK)

#include <cyg/kernel/sched.inl>

// -------------------------------------------------------------------------
// Data for the test

#ifdef CYGNUM_HAL_STACK_SIZE_TYPICAL
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
#else
#define STACKSIZE       (2*1024)        // size of thread stack
#endif

char thread_stack[STACKSIZE];

inline void *operator new(size_t size, void *ptr) { return ptr; };

// array of threads.
char thread[sizeof(Cyg_Thread)];

Cyg_Thread *th;

cyg_tick_count one_sec;
volatile bool watchdog_fired;
volatile cyg_tick_count watchdog_time;

#define WATCHDOG_LOOPS_HW           5
#define WATCHDOG_LOOPS_SIM          1
#define WATCHDOG_SECS_PER_LOOP_HW   10
#define WATCHDOG_SECS_PER_LOOP_SIM  2
#define WATCHDOG_SECS_TILL_TIMEOUT  10

// -------------------------------------------------------------------------
// Action functions:

void watchdog_action1( CYG_ADDRWORD data )
{
    watchdog_fired = true;
    watchdog_time = Cyg_Clock::real_time_clock->current_value();
}

void watchdog_action2( CYG_ADDRWORD data )
{
    CYG_TEST_PASS_FINISH("Watchdog OK");    
}

// -------------------------------------------------------------------------
// Thread body


void watchdog_thread( CYG_ADDRWORD id )
{
    cyg_tick_count watchdog_start_time;
    cyg_tick_count thread_start_time, thread_end_time;
    cyg_ucount8 watchdog_loops, watchdog_secs_per_loop;

    if (cyg_test_is_simulator) {
        watchdog_loops = WATCHDOG_LOOPS_SIM;
        watchdog_secs_per_loop = WATCHDOG_SECS_PER_LOOP_SIM;
    } else {
        watchdog_loops = WATCHDOG_LOOPS_HW;
        watchdog_secs_per_loop = WATCHDOG_SECS_PER_LOOP_HW;
    }

    watchdog_fired = false;
    Cyg_Watchdog::watchdog.start();

    CYG_TEST_INFO("Testing that watchdog does not fire early");

    for (cyg_ucount8 tries = 0;  tries < watchdog_loops;  tries++) {
        diag_printf("Iteration #%d (testing over %d seconds)\n",
                    tries, watchdog_secs_per_loop);

        // First test that the watchdog does not trigger when being reset.
        Cyg_Watchdog_Action wdaction( watchdog_action1, 0 );

        wdaction.install();
        Cyg_Watchdog::watchdog.reset();

        watchdog_start_time = Cyg_Clock::real_time_clock->current_value();
        watchdog_fired = false;
        
        for( cyg_ucount8 i = 0; i < watchdog_secs_per_loop; i++ ) {
            thread_start_time = Cyg_Clock::real_time_clock->current_value();
            th->delay( one_sec-2 );
            Cyg_Watchdog::watchdog.reset();
            if (watchdog_fired) {
                thread_end_time = Cyg_Clock::real_time_clock->current_value();
                diag_printf("Watchdog fired prematurely after %d ticks\n", 
                            (int)(watchdog_time - watchdog_start_time));
                diag_printf("  Thread slept for %d ticks, loop #%d\n", 
                            (int)(thread_end_time - thread_start_time), i);
                CYG_TEST_FAIL_FINISH("Watchdog triggered unexpectedly");
                break;
            } else {
                CYG_TEST_STILL_ALIVE(i, "Resetting watchdog...");
                Cyg_Watchdog::watchdog.reset();
                watchdog_fired = false;
                watchdog_start_time = Cyg_Clock::real_time_clock->current_value();
            }
        }
    }

    // Now check that it triggers when not reset
    CYG_TEST_INFO("Testing that watchdog fires");
    {
        Cyg_Watchdog_Action wdaction( watchdog_action2, 0 );

        wdaction.install();
        
        Cyg_Watchdog::watchdog.reset();
        
        th->delay( one_sec*WATCHDOG_SECS_TILL_TIMEOUT );        
    }

    CYG_TEST_FAIL_FINISH("Watchdog failed to trigger");    
}

// -------------------------------------------------------------------------

    
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();
    
#if !defined(CYGIMP_WATCHDOG_EMULATE) && defined(CYGPKG_HAL_MN10300_STDEVAL1)
    // Workaround for PR 17974
    if( cyg_test_is_simulator )
        CYG_TEST_NA("Watchdog device not implemented in MN10300 simulator.");
#endif

    one_sec = ( res.divisor * 1000000000LL ) / res.dividend ;

    th = new((void *)&thread) Cyg_Thread(CYG_SCHED_DEFAULT_INFO,
                                         watchdog_thread,
                                         0,
                                         "watchdog_thread",
                                         (CYG_ADDRESS)thread_stack,
                                         STACKSIZE
        );

    th->resume();

    // Get the world going
    Cyg_Scheduler::scheduler.start();

}

#else // if defined(CYGFUN_KERNEL_THREADS_TIMER) etc...

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel real-time clock/threads timer disabled");
}

#endif // if defined(CYGFUN_KERNEL_THREADS_TIMER) etc...

// -------------------------------------------------------------------------
// EOF watchdog.cxx
