//==========================================================================
//
//        mutex1.cxx
//
//        Mutex test 1
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
// Contributors:  nickg
// Date:          1999-02-19
// Description:   Tests mutex release functionality
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>       // Cyg_Thread

#include <cyg/kernel/mutex.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>


#define NTHREADS 4
#include "testaux.hxx"
#include "testaux.h"

static Cyg_Mutex m0, m1;
static Cyg_Condition_Variable cvar0( m0 ), cvar1( m0 ), cvar2( m1 );

volatile int thread_state[NTHREADS];

// This thread is meant to get hung up trying to re-acquire m0
// after waiting on the cv.
static void entry0( CYG_ADDRWORD data )
{
    CYG_TEST_INFO( "thread0: lock mutex 0");
    
    m0.lock();

    CYG_TEST_INFO( "thread0: wait cvar 0");

    thread_state[data] = 1;
    
    cvar0.wait();

    thread_state[data] = 2;
    
    CYG_TEST_INFO( "thread0: woke from cvar 0");

    CYG_TEST_INFO( "thread0: unlock mutex 0");
    
    m0.unlock();

    thread_state[data] = 3;
    
    CYG_TEST_INFO( "thread0: exit");

    thread[data]->exit();
}

// This thread is meant to claim and keep m0.
static void entry1( CYG_ADDRWORD data )
{
    CYG_TEST_INFO( "thread1: lock mutex 0");
    
    m0.lock();

    CYG_TEST_INFO( "thread1: lock mutex 1");

    thread_state[data] = 1;
    
    m1.lock();

    thread_state[data] = 2;
    
    CYG_TEST_INFO( "thread1: wait cvar 2");

    cvar2.wait();

    thread_state[data] = 3;
    
    CYG_TEST_INFO( "thread1: woke from cvar 2");

    CYG_TEST_INFO( "thread1: unlock mutex 1");    

    m1.unlock();

    thread_state[data] = 4;
    
    CYG_TEST_INFO( "thread1: unlock m0");    

    m0.unlock();

    thread_state[data] = 5;
    
    CYG_TEST_INFO( "thread1: exit");

    thread[data]->exit();
}

// This thread is meant to get hung trying to acquire m0, and then get
// released out of it by thread3.

static void entry2( CYG_ADDRWORD data )
{
    CYG_TEST_INFO( "thread2: lock mutex 0");

    thread_state[data] = 1;
    
    if( m0.lock() )
    {
        thread_state[data] = 2;
        
        CYG_TEST_INFO( "thread2: lock mutex 0 - returned TRUE");
        CYG_TEST_FAIL_FINISH(" m0.lock() returned TRUE" );
    }
    else
    {
        thread_state[data] = 3;
        CYG_TEST_INFO( "thread2: lock mutex 0 - returned FALSE");        
    }

    CYG_TEST_INFO( "thread2: exit");
    
    thread[data]->exit();    
}

static void entry3( CYG_ADDRWORD data )
{

    CHECK( thread_state[0] == 1 );
    CHECK( thread_state[1] == 2 );
    CHECK( thread_state[2] == 1 );
    
    CYG_TEST_INFO( "thread3: signal  cvar 0");
    
    cvar0.signal();

    CHECK( thread_state[0] == 1 );
    CHECK( thread_state[1] == 2 );
    CHECK( thread_state[2] == 1 );

    CYG_TEST_INFO( "thread3: release mutex 0");

    m0.release();

    CHECK( thread_state[0] == 1 );
    CHECK( thread_state[1] == 2 );
    CHECK( thread_state[2] == 3 );

    CYG_TEST_INFO( "thread3: signal cvar 2");
    
    cvar2.signal();

    CHECK( thread_state[0] == 3 );
    CHECK( thread_state[1] == 5 );
    CHECK( thread_state[2] == 3 );

    CYG_TEST_PASS_FINISH( "mutex2 finished OK");
        
    CYG_TEST_INFO( "thread3: exit");

    thread[data]->exit();    
}

void mutex2_main( void )
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);
    new_thread(entry1, 1);
    new_thread(entry2, 2);
    new_thread(entry3, 3);

    // Set priorities from the top to prevent two threads getting
    // the same priority: this causes an ASSERT on some configurations.
    thread[3]->set_priority( 5 );
    thread[2]->set_priority( 4 );
    thread[1]->set_priority( 3 );
    thread[0]->set_priority( 2 );
    
    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    mutex2_main();
}

// EOF mutex2.cxx
