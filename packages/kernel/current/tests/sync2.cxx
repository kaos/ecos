//==========================================================================
//
//        sync2.cxx
//
//        Sync test 2 -- test of different locking mechanisms
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-18
// Description: 
//     Creates some threads and tests the various synchronization
//     mechanisms.  Four threads are created t0..t3.  t0 and t3 grab a
//     mutex and check they have exclusive access to shared variable.
//     t0,t1,t2 post each other in a loop with a semaphore so that
//     only one is running at any time.  t1,t2,t3 do a similar thing
//     with counting semaphores, except that there are two active
//     threads.
// Omissions:
//     Doesn't test condition variables
//                
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#define NTHREADS 4

#include "testaux.hxx"

static Cyg_Mutex m0;
static Cyg_Binary_Semaphore s0, s1, s2(1);
static Cyg_Counting_Semaphore cs0, cs1, cs2, cs3;

static const cyg_ucount16 n = 1000; 
static cyg_ucount8 m0d=99, sd=2, cd0=99, cd1=99;

static void entry0( CYG_ADDRWORD data )
{
    for(cyg_ucount16 i=0; i<n; i++) {
        s2.wait();
        CHECK( 2 == sd );
        sd = 0;
        m0.lock(); {
            m0d = 0;
            s0.post();
            CHECK( 0 == m0d );
        } m0.unlock();
    }
    // wait for 3 explicit posts to indicate threads have stopped.
    for(cyg_ucount8 i=0; i<3; i++)
        cs3.wait();

    CHECK( ! s0.posted() );
    CHECK( ! s1.posted() );
    CHECK(   s2.posted() );

    CHECK( 0 == cs0.peek() );
    CHECK( 0 == cs1.peek() );
    CHECK( 0 == cs2.peek() );
    CHECK( 0 == cs3.peek() );

    CHECK( 0 == cd0 );
    CHECK( 0 == cd1 );
    CYG_TEST_PASS_FINISH("Sync 2 OK");
    CYG_TEST_FAIL_FINISH("Not reached");
}

static void entry1( CYG_ADDRWORD data )
{
    for(cyg_ucount16 i=0; i<n; i++) {
        s0.wait();
        CHECK( 0 == sd );
        sd = 1;
        cd0 = 1;
        cs1.post();
        cd1 = 1;
        cs1.post();
        s1.post();
        cs0.wait();
        CHECK( 0 == cd0 );
        cs0.wait();
        CHECK( 0 == cd1 );
    }
    cs3.post();
    s0.wait();
    CYG_TEST_FAIL_FINISH("Not reached");
}

static void entry2( CYG_ADDRWORD data )
{
    for(cyg_ucount16 i=0; i<n; i++) {
        s1.wait();
        CHECK( 1 == sd );
        sd = 2;
        cs1.wait();
        CHECK( 1 == cd0 );
        cd0 = 2;
        cs2.post();
        s2.post();
        cs1.wait();
        CHECK( 1 == cd1 );
        cd1 = 2;
        cs2.post();
    }
    cs3.post();
    s1.wait();
    CYG_TEST_FAIL_FINISH("Not reached");
}

static void entry3( CYG_ADDRWORD data )
{
    for(cyg_ucount16 i=0; i < n*2; i++)  {
        cs2.wait();
        CHECK( 2 == cd0 || 2 == cd1 );
        m0.lock(); {
            m0d = 3;
            if( 2 == cd0 ) 
                cd0 = 0;
            else {
                CHECK( 2 == cd1 );
                cd1 = 0;
            }
            cs0.post();
            CHECK( 3 == m0d );
        } m0.unlock();
    }
    cs3.post();
    cs1.wait();
    CYG_TEST_FAIL_FINISH("Not reached");
}


void sync2_main(void)
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);
    new_thread(entry1, 1);
    new_thread(entry2, 2);
    new_thread(entry3, 3);

    Cyg_Scheduler::start();

    CYG_TEST_PASS_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    sync2_main();
}

// EOF sync2.cxx
