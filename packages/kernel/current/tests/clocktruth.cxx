//==========================================================================
//
//        clocktruth.cxx
//
//        Clock Converter test
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     hmt
// Contributors:  hmt
// Date:          2001-06-05
// Description:   Tests the Kernel Real Time Clock for accuracy using a human
// 
//####DESCRIPTIONEND####


// This is for a human to watch to sanity check the clock rate.
// It's easier to see what's happening if you enable this:
#define nRUNFOREVER


#include <pkgconf/kernel.h>

#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/sema.hxx>
#include <cyg/kernel/thread.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/clock.inl>
#include <cyg/kernel/thread.inl>

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK

externC void diag_printf(const char *, ...);

#define NTHREADS 1
#include "testaux.hxx"

#ifdef RUNFOREVER
#define ENDPOINT 8192
#else
#define ENDPOINT 20
#endif

static cyg_alarm_fn alarmfunc;
static void alarmfunc( Cyg_Alarm *alarm, CYG_ADDRWORD data )
{
    Cyg_Binary_Semaphore *sp = (Cyg_Binary_Semaphore *)data;
    sp->post();
}


static void entry0( CYG_ADDRWORD data )
{
    cyg_uint32 now, then;
    int i;

    Cyg_Clock *rtc = Cyg_Clock::real_time_clock;

    Cyg_Binary_Semaphore sema;

    Cyg_Alarm alarm( rtc, &alarmfunc, (CYG_ADDRWORD)&sema );

    // First, print 100 lines as fast as you can, of distinct ticks.
    for ( i = 0; i < 100; i++ ) {
        now = rtc->current_value_lo();
        then = now;
        while ( then == now )
            now = rtc->current_value_lo();

        diag_printf( "INFO<time now %8d>\n", now );
    }

    diag_printf( "INFO<per-second times are: %8d>\n", rtc->current_value_lo() );
    for ( i = 0; i < 20; i++ ) {
        Cyg_Thread::counted_sleep( 100 );
        diag_printf( "INFO<per-second time %2d is %8d>\n",
                     i, rtc->current_value_lo() );
    }

    alarm.initialize( rtc->current_value() + 100, 100 );
    alarm.enable();
    for ( i = 0; i < ENDPOINT; i++ ) {
        sema.wait();
        diag_printf( "INFO<alarm time %2d is %8d>\n",
                     i, rtc->current_value_lo() );
    }

    CYG_TEST_PASS_FINISH("Clock truth OK");
}

void clocktruth_main( void )
{
    CYG_TEST_INIT();
    new_thread(entry0, (CYG_ADDRWORD)&thread_obj[0]);
    Cyg_Scheduler::start();
}

externC void
cyg_start( void )
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    clocktruth_main();
}

#else // def CYGVAR_KERNEL_COUNTERS_CLOCK

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( "Kernel real-time clock disabled");
}

#endif // def CYGVAR_KERNEL_COUNTERS_CLOCK

// EOF clocktruth.cxx
