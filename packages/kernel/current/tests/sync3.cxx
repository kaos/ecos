//==========================================================================
//
//        sync3.cxx
//
//        Sync test 3 -- tests priorities and priority inheritance
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-18
// Description: 
//     Creates mutexes and threads to set up starvation condition.
//     Checks simple priority inheritance cures this.
//     
//     The starvation condition is caused by the highest priority
//     thread, t0 waiting on a mutex which is never released because
//     it is held by t2.  t2 never releases it because t1 will be
//     running at a priority level higher than t2 (but lower than t0).
//     
//     With priority inheritance enabled, t2 will inherit its priority
//     from t0 when t0 tries to grab the mutex.
//     
// Options:
//     CYGIMP_THREAD_PRIORITY
//     CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
//     CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#ifndef CYGIMP_THREAD_PRIORITY
#error "Thread priorities disabled"
#endif

#define NTHREADS 3

#include "testaux.hxx"

static Cyg_Mutex m0;
static Cyg_Binary_Semaphore s0, s1, s2;

static cyg_ucount8 m0d = 9;

static void check_priorities_normal()
{
    CHECK( 5 == thread[0]->get_priority());
    CHECK( 6 == thread[1]->get_priority());
    CHECK( 7 == thread[2]->get_priority());
}

static void check_priorities_inherited()
{
    CHECK( 5 == thread[0]->get_priority());
    CHECK( 6 == thread[1]->get_priority());
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
    CHECK( 5 == thread[2]->get_current_priority());
#endif
    CHECK( 7 == thread[2]->get_priority());

}

static void entry0( CYG_ADDRWORD data )
{
    s0.wait();                  // wait until t2 has gained m0.lock
    check_priorities_normal();
    m0.lock(); {
        check_priorities_normal();
        CHECK( 2 == m0d );
        m0d = 0;
    } m0.unlock();
    check_priorities_normal();
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
    CYG_TEST_PASS_FINISH("Sync 3 OK -- priority inheritance worked");
#else
    CYG_TEST_FAIL_FINISH("Sync 3: thread not starved");
#endif
    // NOT REACHED
}

static void entry1( CYG_ADDRWORD data )
{
    s1.wait();
    // The delay below will allow testing of the priority inheritance
    // mechanism when scheduler does not guarantee to schedule threads
    // in strict priority order.
    for ( volatile cyg_ucount32 i=0; i < 100000; i++ )
        ; // math is hard

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE
    // thread0 should have stopped by this point
    CYG_TEST_FAIL_FINISH("Sync 3: priority inheritance mechanism failed");
#else
    // With strict priority scheduling and no priority inheritance
    // this is expected to happen.
    CYG_TEST_PASS_FINISH("Sync 3 OK");
#endif
    CYG_TEST_FAIL_FINISH("Not reached");
}

void entry2( CYG_ADDRWORD data )
{
    m0.lock(); {
        CHECK( 9 == m0d );
        check_priorities_normal();
        s0.post();              // Now I have lock on m0, wake t0 then t1 
        check_priorities_inherited();
        s1.post();
        check_priorities_inherited();
        m0d = 2;
    } m0.unlock();
    check_priorities_normal();
    m0.lock(); {
        check_priorities_normal();
        CHECK( 0 == m0d );
        m0d = 21;
        s2.wait();              // never posted
    } m0.unlock();
}



void sync3_main(void)
{
    CYG_TEST_INIT();

    new_thread( entry0, 0);
    new_thread( entry1, 1);
    new_thread( entry2, 2);

    thread[0]->set_priority(5);
    thread[1]->set_priority(6);
    thread[2]->set_priority(7);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    sync3_main();
}
   
// EOF sync3.cxx
