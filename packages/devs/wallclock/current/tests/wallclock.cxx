//==========================================================================
//
//        wallclock.cxx
//
//        WallClock test
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
// Author(s):     nickg
// Contributors:    nickg
// Date:          1998-07-20
// Description:   Tests the Kernel Wall Clock
//                This test exercises the WallClock class and checks that it
//                keeps time correctly.
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

#include <cyg/devs/wallclock.hxx>

#include <cyg/infra/testcase.h>

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

#define EPOCH 900000000U

// MN10300 sim takes 1min15secs to do one loop on 300MHz PII.
#define LOOPS_SIM       2
#define LOOPS_HW        20

cyg_int32 loops = LOOPS_HW;
cyg_tick_count one_sec;

// -------------------------------------------------------------------------
// Thread body


void wallclock_thread( CYG_ADDRWORD id )
{
    cyg_uint32 wtime;

    Cyg_WallClock::wallclock->set_current_time( EPOCH );

    for( int i = 0; i < loops; i++ )
    {
        CYG_TEST_STILL_ALIVE(i, "tick...");

        wtime = Cyg_WallClock::wallclock->get_current_time();

        if(wtime != EPOCH+i)
        {
            CYG_TEST_FAIL_FINISH( "Clock out of sync" );
        }

        th->delay( one_sec );        
    }

    CYG_TEST_PASS_FINISH("Wallclock OK");
    
}

// -------------------------------------------------------------------------


externC void
cyg_start( void )
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting wallclock test");

    if( cyg_test_is_simulator ) loops = LOOPS_SIM;
    
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();
    
    one_sec = ( res.divisor * 1000000000LL ) / res.dividend ;
    
    th = new((void *)&thread) Cyg_Thread(CYG_SCHED_DEFAULT_INFO,
                                         wallclock_thread,
                                         0,
                                         "wallclock_thread",
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
    CYG_TEST_PASS_FINISH("N/A: Kernel real-time clock/threads timer disabled");
}

#endif // if defined(CYGFUN_KERNEL_THREADS_TIMER) etc...

// -------------------------------------------------------------------------
// EOF wallclock.cxx
