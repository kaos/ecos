//==========================================================================
//
//        mbox1.cxx
//
//        Mbox test 1
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
// Author:        dsm
// Contributors:    dsm
// Date:          1998-05-19
// Description:   Tests basic mbox functionality.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>        // Cyg_Thread
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>         // Cyg_Scheduler::start()

#include <cyg/kernel/mbox.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#include <cyg/kernel/timer.hxx>         // Cyg_Timer
#include <cyg/kernel/clock.inl>         // Cyg_Clock

#define NTHREADS 2
#include "testaux.hxx"

static Cyg_Mbox m0, m1, m2;

static volatile cyg_atomic q = 0;

#ifndef CYGMTH_MBOX_PUT_CAN_WAIT
#define PUT tryput
#endif

static void entry0( CYG_ADDRWORD data )
{
    cyg_count8 u,i;

    CYG_TEST_INFO("Testing put() and tryput() without wakeup");
    CYG_TEST_CHECK(!m0.waiting_to_get(), "mbox not initialized properly");
    CYG_TEST_CHECK(0==m0.peek(), "mbox not initialized properly");
    CYG_TEST_CHECK(NULL==m0.peek_item(), "mbox not initialized properly");
    m0.PUT((void *)55);
    CYG_TEST_CHECK(1==m0.peek(), "peek() wrong");
    CYG_TEST_CHECK(55==(cyg_count8)m0.peek_item(), "peek_item() wrong");
    for(u=1; m0.tryput((void*)u); u++) {
        CYG_TEST_CHECK(55==(cyg_count8)m0.peek_item(), "peek_item() wrong");
        CYG_TEST_CHECK(u+1==m0.peek(), "peek() wrong");
    }
    CYG_TEST_CHECK(u == CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE, "mbox not configured size");

    // m0 now contains ( 55 1 2 .. u-1 )
    CYG_TEST_CHECK(u==m0.peek(), "peek() wrong");
    CYG_TEST_CHECK(55==(cyg_count8)m0.peek_item(), "peek_item() wrong");

    CYG_TEST_INFO("Testing get(), tryget()");
    
    i = (cyg_count8)m0.tryget();
    CYG_TEST_CHECK( 55 == i, "Got wrong message" );
    for(cyg_count8 j=1; j<u;j++) {
        CYG_TEST_CHECK( j == (cyg_count8)m0.peek_item(), "peek_item()" );
        CYG_TEST_CHECK( m0.peek() == u - j, "peek() wrong" );
        i = (cyg_count8)m0.get();
        CYG_TEST_CHECK( j == i, "Got wrong message" );
    }
    
    CYG_TEST_CHECK( NULL == m0.peek_item(), "peek_item()" );
    CYG_TEST_CHECK( 0 == m0.peek(), "peek()");
    
    // m0 now empty

    CYG_TEST_CHECK(!m0.waiting_to_put(), "waiting_to_put()");
    CYG_TEST_CHECK(!m0.waiting_to_get(), "waiting_to_get()");

    CYG_TEST_INFO("Testing get(), blocking");
    
    CYG_TEST_CHECK(0==q++, "bad synchronization");
    m1.PUT((void*)99);                  // wakes t1
    i = (cyg_count8)m0.get();          // sent by t1
    CYG_TEST_CHECK(3==i, "Recieved wrong message");
    CYG_TEST_CHECK(2==q++, "bad synchronization");

#ifdef CYGFUN_KERNEL_THREADS_TIMER
    CYG_TEST_CHECK(NULL==m0.get(
        Cyg_Clock::real_time_clock->current_value() + 2),
                   "unexpectedly found message");
    CYG_TEST_CHECK(3==q++, "bad synchronization");
    // Allow t1 to run as this get times out
    // t1 must not be waiting...
    CYG_TEST_CHECK(m0.waiting_to_get(), "waiting_to_get()");

    m0.PUT((void*)7);                   // wake t1 from timed get
#ifdef CYGMTH_MBOX_PUT_CAN_WAIT
    q=10;
    while(m0.tryput((void*)6))          // fill m0's queue
        ;
    // m0 now contains ( 6 ... 6 )
    CYG_TEST_CHECK(10==q++, "bad synchronization");
    m1.put((void*)4);                   // wake t1
    CYG_TEST_CHECK(!m0.put((void*)8, 2), "timed put() unexpectedly worked");
    CYG_TEST_CHECK(12==q++, "bad synchronization");
    // m0 still contains ( 6 ... 6 )
    m0.put((void*)9);
    CYG_TEST_CHECK(13==q++, "bad synchronization");
#endif
#endif
    i=(cyg_count8)m2.get();
    CYG_TEST_FAIL_FINISH("Not reached");
}

static void entry1( CYG_ADDRWORD data )
{
    cyg_count8 i;
    i = (cyg_count8)m1.get();
    CYG_TEST_CHECK(1==q++, "bad synchronization");
    m0.PUT((void *)3);                  // wake t0

#ifdef CYGFUN_KERNEL_THREADS_TIMER
    CYG_TEST_INFO("Testing timed functions");
    CYG_TEST_CHECK(7==(cyg_count8)m0.get(
        Cyg_Clock::real_time_clock->current_value() + 4), "timed get()");
    CYG_TEST_CHECK(4==q++, "bad synchronization");
#ifdef CYGMTH_MBOX_PUT_CAN_WAIT
    CYG_TEST_CHECK(4==(cyg_count8)m1.get());

    CYG_TEST_CHECK(11==q++, "bad synchronization");
    thread[0]->delay(4);    // allow t0 to reach put on m1
    CYG_TEST_CHECK(14==q++, "bad synchronization");
    CYG_TEST_CHECK(m0.waiting_to_put(), "waiting_to_put()");
    do {
        // after first get m0 contains ( 6 .. 6 9 )
        i=(cyg_count8)m0.tryget();
    } while(6==i);
    CYG_TEST_CHECK(9==i,"put gone awry");
#endif
#endif
    CYG_TEST_PASS_FINISH("Mbox 1 OK");
}

void mbox1_main( void )
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);
    new_thread(entry1, 1);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    mbox1_main();
}

// EOF mbox1.cxx
