//==========================================================================
//
//        release.cxx
//
//        Thread release test
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
// Contributors:    nickg
// Date:          1998-04-24
// Description:   Tests the functionality of thread release().
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/mutex.hxx>
#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#define NTHREADS 2

#include "testaux.hxx"

static Cyg_Binary_Semaphore s0, s1;

static void entry0( CYG_ADDRWORD data )
{
    Cyg_Thread *self = Cyg_Thread::self();
    
    if (!s0.wait() )
    {
        if( self->get_wake_reason() != Cyg_Thread::BREAK )
            CYG_TEST_FAIL_FINISH("Wake reason not BREAK");
    }
    else
    {
            CYG_TEST_FAIL_FINISH("Thread not released");        
    }

    s1.post();
    
    self->exit();
}


static void entry1( CYG_ADDRWORD data )
{
    Cyg_Thread *self = Cyg_Thread::self();    

    // Give the other thread a chance to wait in SMP systems.
    for( int i = 0; i < 100; i++ )
        self->yield();
    
    thread[0]->release();

    s1.wait();
    
    CYG_TEST_PASS_FINISH("Release OK");
    
    Cyg_Thread::self()->exit();
}

void release_main(void)
{
    CYG_TEST_INIT();

    new_thread( entry0, 0);
    new_thread( entry1, 1);

    thread[0]->set_priority(5);
    thread[1]->set_priority(6);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");

}

externC void
cyg_start( void )
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    release_main();
}
   
// EOF release.cxx
