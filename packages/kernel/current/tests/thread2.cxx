//==========================================================================
//
//        thread2.cxx
//
//        Thread test 2
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
// Date:          1998-02-19
// Description:
//    tests scheduler & threads & priorities
//    + create multiple threads with various priorities
//    + check highest priority running thread is always running
//    + check next highest runs when highest suspended
//    + check several threads of equal priority share time
//      (only !CYG_SCHED_UNIQUE_PRIORITIES)
//    + check other threads are starved
//    + check setting priority dynamically causes a thread to
//      become/stay current/non-current
// Omissions:
//     check yield
//     check can set threads with min and max priority
// Options:
//    CYG_SCHED_UNIQUE_PRIORITIES
//    CYGIMP_THREAD_PRIORITY
//    CYGNUM_KERNEL_SCHED_PRIORITIES
//    CYGSEM_KERNEL_SCHED_BITMAP
//    CYGSEM_KERNEL_SCHED_MLQUEUE
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
#error "Test requires thread priorities"
#endif

static Cyg_Counting_Semaphore s0, s1, s2;

static volatile cyg_ucount8 q = 0;

static Cyg_Thread *thread0, *thread1, *thread2;

#define NTHREADS 3
#include "testaux.hxx"

static void entry0( CYG_ADDRWORD data )
{
    CHECK(  0 == q++ );
    s0.wait();
    CHECK(  3 == q++ );
    s1.post();
    CHECK(  4 == q++ );
    s0.wait();
    s0.wait();
    CYG_TEST_PASS_FINISH("Thread 2 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    CHECK(  1 == q++ );
    s1.wait();
    CHECK(  5 == q++ );
    thread0->set_priority(9);
    s0.post();
    CHECK(  6 == q++ );
    thread2->set_priority(3);
    CHECK(  8 == q++ );
    s2.post();
    CHECK( 12 == q++ );
    CHECK( 9 == thread0->get_priority() );
    CHECK( 6 == thread1->get_priority() );
    CHECK( 7 == thread2->get_priority() );
    q = 100;
#if !(CYG_SCHED_UNIQUE_PRIORITIES) && defined(CYGSEM_KERNEL_SCHED_TIMESLICE)
    thread2->set_priority(6);
    CHECK( 6 == thread1->get_priority() );
    CHECK( 6 == thread2->get_priority() );

    while ( 100 == q )
        ;
    CHECK( 101 == q++ );
    s1.wait();
    CHECK( 103 == q++ );
#endif
    s0.post();
    s1.wait();
}

static void entry2( CYG_ADDRWORD data )
{
    CHECK(  2 == q++ );
    s0.post();
    CHECK(  7 == q++ );
    s2.wait();
    CHECK(  9 == q++ );
    thread1->set_priority(6);
    CHECK( 10 == q++ );
    thread2->set_priority(2);
    CHECK( 11 == q++ );
    thread2->set_priority(7);

#if !(CYG_SCHED_UNIQUE_PRIORITIES) && defined(CYGSEM_KERNEL_SCHED_TIMESLICE)
    CHECK( 6 == thread1->get_priority() );
    CHECK( 6 == thread2->get_priority() );

    CHECK( 100 == q++ );
    while ( 101 == q )
        ;
    CHECK( 102 == q++ );
    s1.post();
#endif
    s0.post();
    s2.wait();
}


void thread2_main( void )
{
    CYG_TEST_INIT();
    
    thread0 = new_thread( entry0, 0 );
    thread1 = new_thread( entry1, 1 );
    thread2 = new_thread( entry2, 2 );

    thread0->resume();
    thread1->resume();
    thread2->resume();

    thread0->set_priority(5);
    thread1->set_priority(6);
    thread2->set_priority(7);

    if( 9 >= CYG_THREAD_MIN_PRIORITY )
        CYG_TEST_FAIL_FINISH("Test requires priorities up to 9");

    if( 2 <= CYG_THREAD_MAX_PRIORITY )
        CYG_TEST_FAIL_FINISH("Test requires priorities as low as 2");

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Unresolved");
}

externC void
cyg_start( void )
{
    thread2_main();
}

// EOF thread2.cxx
