/*=================================================================
//
//        kthread0.c
//
//        Kernel C API Thread test 0
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
// Date:          1998-03-18
// Description:   Limited to checking constructors/destructors
//####DESCRIPTIONEND####
*/

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static char stack[STACKSIZE];

static cyg_thread_entry_t entry;

static void entry( cyg_addrword_t data )
{
}

static int *p;

#if 0
static cyg_handle_t t0,t1;
static cyg_thread thread0, thread1;
#endif

static cyg_handle_t t2;
static cyg_thread thread2;

static bool flash( void )
{
#if 0 // no facility to allocate stack exists yet.
    cyg_thread_create( entry, 0x111, NULL, 0, &t0, &thread0 );

    cyg_thread_create( entry, (cyg_addrword_t)&t0, STACKSIZE, 0, &t1, &thread0 );
#endif

    cyg_thread_create(4, entry, (cyg_addrword_t)p, "kthread0",
        (void *)stack, STACKSIZE, &t2, &thread2 );

    return true;
}

void kthread0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_TEST_PASS_FINISH("Kernel C API Thread 0 OK");
    
}

externC void
cyg_start( void )
{ 
    kthread0_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF kthread0.c */
