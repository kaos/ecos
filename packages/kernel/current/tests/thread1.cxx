//==========================================================================
//
//        thread1.cxx
//
//        Thread test 1
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
// Date:          1998-02-11
// Description:   Tests some basic thread functions.
// Omissions:     Cyg_ThreadTimer
//                Cyg_Thread
//                  exit -- not necessarily called
//                  yield
//                  set_priority
//                  get_priority
//                  get/set_sleep_reason
//                  get/set_wake_reason
//                  set/clear_timer
//                Cyg_ThreadQueue
//               
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/thread.hxx>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_THREADS_TIMER

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>

#include "testaux.hxx"

#ifdef CYGNUM_HAL_STACK_SIZE_TYPICAL
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
#else
#define STACKSIZE 2000
#endif

static char stack[2][STACKSIZE];

static char thread[2][sizeof(Cyg_Thread)];

static Cyg_Thread *pt0,*pt1;
static cyg_uint16 uid0,uid1;


static void entry0( CYG_ADDRWORD data )
{
    CHECK( 222 == data );

    uid0 = pt0->get_unique_id();

    pt1->suspend();       
    pt1->resume();

    do {
        pt0->delay(1);
    } while( Cyg_Thread::RUNNING == pt1->get_state() );
    
    CHECK( Cyg_Thread::SLEEPING == pt1->get_state() );

    pt1->wake();

    CHECK( uid0 != uid1 );

    CYG_TEST_PASS_FINISH("Thread 1 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    CHECK( 333 == data );

    uid1 = pt1->get_unique_id();

    Cyg_Thread *self = Cyg_Thread::self();
   
    CHECK( self == pt1 );

    pt1->sleep();
    pt1->suspend();

    Cyg_Thread::exit();         // no guarantee this will be called
}

void thread1_main( void )
{
    CYG_TEST_INIT();

    pt0 = new((void *)&thread[0])
            Cyg_Thread(CYG_SCHED_DEFAULT_INFO,
                       entry0, 222, 
                       "thread 0",
                       (CYG_ADDRESS)stack[0], STACKSIZE );
    pt1 = new((void *)&thread[1])
            Cyg_Thread(CYG_SCHED_DEFAULT_INFO,
                       entry1, 333, 
                       "thread 1",
                       (CYG_ADDRESS)stack[1], STACKSIZE );

    CYG_ASSERTCLASS( pt0, "error" );
    CYG_ASSERTCLASS( pt1, "error" );

    pt0->resume();
    pt1->resume();

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    thread1_main();
}

#else // ifdef CYGFUN_KERNEL_THREADS_TIMER

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel threads timer disabled");
}

#endif // ifdef CYGFUN_KERNEL_THREADS_TIMER

// EOF thread1.cxx
