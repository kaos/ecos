//==========================================================================
//
//        kill.cxx
//
//        Thread kill test
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
// Author(s):     nickg
// Contributors:    nickg
// Date:          1998-04-24
// Description:   Tests the functionality of thread kill() and
//                reinitalize().
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#define NTHREADS 3

#include "testaux.hxx"

static Cyg_Binary_Semaphore s0, s1;

volatile cyg_atomic thread0_state;
volatile cyg_atomic thread1_state;
volatile cyg_atomic thread2_state;

static void entry0( CYG_ADDRWORD data )
{
    Cyg_Thread *self = Cyg_Thread::self();

    thread0_state = 1;
    
    s0.wait();

    thread0_state = 2;
    
    CYG_TEST_FAIL_FINISH("Thread not killed");        

    self->exit();
}


static void entry1( CYG_ADDRWORD data )
{
    Cyg_Thread *self = Cyg_Thread::self();

    thread1_state = 1;
    
    self->delay(1);

    if( thread2_state != 1 )
        CYG_TEST_FAIL_FINISH("Thread2 in wrong state");        
    
    thread1_state = 2;
    
    thread[0]->kill();

    thread1_state = 3;    
    
    thread[2]->kill();

    thread1_state = 4;
    
    self->delay(1);

    thread1_state = 5;
    thread2_state = 0;
    
    thread[2]->reinitialize();
    thread[2]->resume();

    self->delay(1);

    if( thread2_state != 1 )
        CYG_TEST_FAIL_FINISH("Thread2 in wrong state");        
    
    thread1_state = 6;

    self->delay(1);

    if( thread2_state != 2 )
        CYG_TEST_FAIL_FINISH("Thread2 in wrong state");        
    
    thread[2]->kill();

    thread1_state = 7;
    
    CYG_TEST_PASS_FINISH("Kill OK");
    
    Cyg_Thread::self()->exit();
}

static void entry2( CYG_ADDRWORD data )
{
    thread2_state = 1;

    while( thread1_state != 6 ) continue;

    thread2_state = 2;
    
    for(;;) continue;
    
}

void release_main(void)
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
    release_main();
}
   
// EOF kill.cxx
