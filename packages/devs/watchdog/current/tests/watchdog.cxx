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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
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

#include <cyg/kernel/sched.inl>

// -------------------------------------------------------------------------
// Data for the test

#define STACKSIZE       (2*1024)        // size of thread stack

char thread_stack[STACKSIZE];

inline void *operator new(size_t size, void *ptr) { return ptr; };

// array of threads.
char thread[sizeof(Cyg_Thread)];

Cyg_Thread *th;

volatile cyg_count8 watchdog_state;

cyg_tick_count one_sec;


// -------------------------------------------------------------------------
// Action functions:

void watchdog_action1( CYG_ADDRWORD data )
{
    CYG_TEST_FAIL_FINISH("Watchdog triggered unexpectedly");
}

void watchdog_action2( CYG_ADDRWORD data )
{
    CYG_TEST_PASS_FINISH("Watchdog OK");    
}

// -------------------------------------------------------------------------
// Thread body


void watchdog_thread( CYG_ADDRWORD id )
{
    Cyg_Watchdog::watchdog.start();

    // First test that the watchdog does not trigger when being reset.
    {
        Cyg_Watchdog_Action wdaction( watchdog_action1, 0 );

        wdaction.install();
        
        for( int i = 0; i < 10; i++ )
        {
            th->delay( one_sec-2 );
            Cyg_Watchdog::watchdog.reset();
        }
    }

    // Now check that it triggers when not reset
    {
        Cyg_Watchdog_Action wdaction( watchdog_action2, 0 );

        wdaction.install();
        
        Cyg_Watchdog::watchdog.reset();
        
        th->delay( one_sec*10 );        
    }

    CYG_TEST_FAIL_FINISH("Watchdog failed to trigger");    
}

// -------------------------------------------------------------------------

    
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();
    
#if defined(CYG_HAL_MIPS_SIM)
    // Reduce time in simulated TX39 so that it runs within the timeout.
    one_sec = ( res.divisor * 200000000LL ) / res.dividend ;
#else
    one_sec = ( res.divisor * 1000000000LL ) / res.dividend ;
#endif

    th = new((void *)&thread) Cyg_Thread(watchdog_thread,
                                         0,
                                         STACKSIZE,
                                         (CYG_ADDRESS)thread_stack
        );

    th->resume();

    // Get the world going
    Cyg_Scheduler::scheduler.start();

}

// -------------------------------------------------------------------------
// EOF watchdog.cxx
