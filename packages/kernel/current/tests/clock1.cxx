//==========================================================================
//
//        clock1.cxx
//
//        Clock test 1 - Real Time Clock
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-16
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

#include <pkgconf/kernel.h>

#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/thread.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/clock.inl>
#include <cyg/kernel/thread.inl>

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK


#define NTHREADS 1
#include "testaux.hxx"

static cyg_uint32 ticks;     // Number of ticks thread[0] will delay for

static cyg_uint64 TEST_DELAY;

static void entry0( CYG_ADDRWORD data )
{
    ((Cyg_Thread *)data)->delay(ticks);

    CYG_TEST_PASS_FINISH("Clock 1 OK");
}

void clock1_main( void )
{
    CYG_TEST_INIT();

    if (cyg_test_is_simulator) {
        TEST_DELAY = 100000000ll;
    } else {
        TEST_DELAY = 3000000000ll;
    }

    new_thread(entry0, (CYG_ADDRWORD)&thread_obj[0]);

    Cyg_Clock::cyg_resolution res;

    res = Cyg_Clock::real_time_clock->get_resolution ();

    // RTC takes res.dividend/res.divisor ns/tick
    ticks = ((cyg_uint64)TEST_DELAY * res.divisor) / res.dividend;

    Cyg_Scheduler::start();
}

externC void
cyg_start( void )
{
    clock1_main();
}

#else // def CYGVAR_KERNEL_COUNTERS_CLOCK

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( "Kernel real-time clock disabled");
}

#endif // def CYGVAR_KERNEL_COUNTERS_CLOCK

// EOF clock1.cxx
