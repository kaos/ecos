/*=================================================================
//
//        kthread1.c
//
//        Kernel C API Thread test 1
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
// Description:   Tests some basic thread functions.
//####DESCRIPTIONEND####
*/

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#include <cyg/hal/hal_arch.h>           // for CYGNUM_HAL_STACK_SIZE_TYPICAL

#ifdef CYGNUM_HAL_STACK_SIZE_TYPICAL
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
#else
#define STACKSIZE 2000
#endif

static char stack[2][STACKSIZE];

static cyg_thread thread[2];

static cyg_handle_t pt0,pt1;


static void entry0( cyg_addrword_t data )
{
    CHECK( 222 == (int)data );

    cyg_thread_suspend(pt1);       
    cyg_thread_resume(pt1);

    cyg_thread_delay(1);

    cyg_thread_resume(pt1);

    cyg_thread_delay(1);

    CYG_TEST_PASS_FINISH("Kernel C API Thread 1 OK");
}

static void entry1( cyg_addrword_t data )
{
    cyg_handle_t self;
    CHECK( 333 == (int)data );

    self = cyg_thread_self();
   
    CHECK( self == pt1 );

    cyg_thread_suspend(pt1);

    cyg_thread_exit();          // no guarantee this will be called
}

void kthread1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0, (cyg_addrword_t)222, "kthread1-0",
        (void *)stack[0], STACKSIZE, &pt0, &thread[0] );
    cyg_thread_create(4, entry1, (cyg_addrword_t)333, "kthread1-1",
        (void *)stack[1], STACKSIZE, &pt1, &thread[1] );

    cyg_thread_resume(pt0);
    cyg_thread_resume(pt1);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    kthread1_main();
}


#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF kthread1.c */
