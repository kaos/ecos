//==========================================================================
//
//        thread0.cxx
//
//        Thread test 0
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
// Date:          1998-02-11
// Description:   Limited to checking constructors/destructors
// Omissions:
//     Thread constructors with 2 or 3 args are not supported at time
// of writing test.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/thread.inl>

#include "testaux.hxx"

#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static char stack[STACKSIZE];

static cyg_thread_entry entry;

static void entry( CYG_ADDRWORD data )
{
}

static int *p;

static bool flash( void )
{
#if 0 // no facility to allocate stack exists yet.
    Cyg_Thread t0( entry, 0x111 );

    CYG_ASSERTCLASS(&t0, "error");

    Cyg_Thread t1( entry, (CYG_ADDRWORD)&t0, STACKSIZE );

    CYG_ASSERTCLASS(&t1, "error");
#endif
    Cyg_Thread t2( CYG_SCHED_DEFAULT_INFO,
                   entry, (CYG_ADDRWORD)p, 
                   "thread t2",
                   (CYG_ADDRESS)stack, STACKSIZE );

    CYG_ASSERTCLASS(&t2, "error");

    return true;
}

void thread0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_TEST_PASS_FINISH("Thread 0 OK");
    
}

externC void
cyg_start( void )
{ 
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    thread0_main();
}
// EOF thread0.cxx
