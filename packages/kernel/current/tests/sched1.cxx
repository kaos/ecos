//==========================================================================
//
//        sched1.cxx
//
//        Sched test 1
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
// Date:          1998-02-26
// Description:   Tests some basic sched functions.
// Omissions:
//     Doesn't test Cyg_Scheduler::get_thread_switches() very well
//     Cyg_SchedThread
//         inherit_priority
//         disinherit_priority
// Options:       
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>       // Cyg_Thread

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>

#define NTHREADS 2

#include "testaux.hxx"

static void entry0( CYG_ADDRWORD data )
{
    CHECK( 0 == Cyg_Scheduler::get_sched_lock() );
    Cyg_Scheduler::lock(); {
        CHECK( 1 == Cyg_Scheduler::get_sched_lock() );
        Cyg_Scheduler::lock(); {
            CHECK( 2 == Cyg_Scheduler::get_sched_lock() );
        } Cyg_Scheduler::unlock();
    } Cyg_Scheduler::unlock();
    cyg_ucount32 t0=Cyg_Scheduler::get_thread_switches();
    cyg_ucount32 t1=Cyg_Scheduler::get_thread_switches();
    CHECK( t1 >= t0 );
    CHECK( Cyg_Scheduler::get_current_thread() == 
                Cyg_Thread::self() );
    CYG_TEST_PASS_FINISH( "Sched 1 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    Cyg_Thread::self()->sleep();    
}

void sched1_main(void)
{
    CYG_TEST_INIT();

    new_thread(entry0, 222);
    new_thread(entry1, 333);

    Cyg_Scheduler::start();
    
    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    sched1_main();
}
// EOF sched1.cxx
