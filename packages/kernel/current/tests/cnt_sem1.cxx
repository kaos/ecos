//==========================================================================
//
//        cnt_sem1.cxx
//
//        Counting semaphore test 1
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
// Date:          1998-02-24
// Description:   Tests basic counting semaphore functionality.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>       // Cyg_Thread
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()

#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>


#define NTHREADS 2
#include "testaux.hxx"

static Cyg_Counting_Semaphore s0(0), s1(2), s2;

static volatile cyg_ucount8 q = 0;

static void entry0( CYG_ADDRWORD data )
{
    s0.wait();
    CHECK( 1 == q++ );
    s1.post();
    s0.wait();
    CHECK( 3 == q++ );
    CHECK( 0 == s0.peek() );
    CHECK( ! s0.trywait() );
    s0.post();
    CHECK( 4 == q++ );
    CHECK( 1 == s0.peek() );
    s0.post();
    CHECK( 2 == s0.peek() );
    s1.post();
    CHECK( 0 == s2.peek() );
    s2.wait();
    CHECK( 6 == q++ );
    CYG_TEST_PASS_FINISH("Counting Semaphore 1 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    CHECK( 2 == s1.peek() );
    s1.wait();
    CHECK( 1 == s1.peek() );
    s1.wait();
    CHECK( 0 == q++ );
    CHECK( 0 == s0.peek() );
    s0.post();
    s1.wait();
    CHECK( 2 == q++ );
    s0.post();
    s1.wait();
    CHECK( 5 == q++ );
    CHECK( 2 == s0.peek() );
    CHECK( s0.trywait() );
    CHECK( 1 == s0.peek() );
    CHECK( s0.trywait() );
    CHECK( 0 == s0.peek() );
    s2.post();
    s0.wait();
    CYG_TEST_FAIL_FINISH("Not reached");
}

void cnt_sem1_main( void )
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
    cnt_sem1_main();
}

// EOF cnt_sem1.cxx
