/*=================================================================
//
//        kmutex1.c
//
//        Kernel C API Mutex test 1
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
// Date:          1998-03-23
// Description:   Tests basic mutex functionality.
// Omissions:     Timed wait.
//####DESCRIPTIONEND####
*/

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#define NTHREADS 3
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


static cyg_mutex_t m0, m1;
static cyg_cond_t cvar0, cvar1, cvar2;

static cyg_ucount8 m0d=0, m1d=0;

static void finish( cyg_ucount8 t )
{
    cyg_mutex_lock( &m1 ); {
        m1d |= 1<<t;
        if( 0x7 == m1d )
            CYG_TEST_PASS_FINISH("Kernel C API Mutex 1 OK");
        cyg_cond_wait( &cvar2 );
    } /* cyg_mutex_unlock( &m1 ); */
    CYG_TEST_FAIL_FINISH("Not reached");    
}

static void entry0( cyg_addrword_t data )
{
    cyg_mutex_lock( &m0 ); {
        CHECK( ! cyg_mutex_trylock( &m0 ) );
        cyg_mutex_lock( &m1 ); {
            CHECK( ! cyg_mutex_trylock( &m0 ) );            
        } cyg_mutex_unlock( &m1 );
    } cyg_mutex_unlock( &m0 );

    cyg_mutex_lock( &m0 ); {
        while ( 0 == m0d )
            cyg_cond_wait( &cvar0 );
        CHECK( 1 == m0d++ );
        cyg_cond_signal( &cvar0 );
        while ( 4 != m0d )
            cyg_cond_wait( &cvar1 );
        CHECK( 4 == m0d );
    } cyg_mutex_unlock( &m0 );

    finish( (cyg_ucount8)data );
}

static void entry1( cyg_addrword_t data )
{
    cyg_mutex_lock( &m0 ); {
        CHECK( cyg_mutex_trylock( &m1 ) ); {
        } cyg_mutex_unlock( &m1 );
    } cyg_mutex_unlock( &m0 );

    cyg_mutex_lock( &m0 ); {
        CHECK( 0 == m0d++ );
        cyg_cond_broadcast( &cvar0 );
    } cyg_mutex_unlock( &m0 );
    
    cyg_mutex_lock( &m0 ); {
        while( 1 == m0d )
            cyg_cond_wait( &cvar0 );
        CHECK( 2 == m0d++ );
        cyg_cond_signal( &cvar0 );
        while( 3 == m0d )
            cyg_cond_wait( &cvar1 );
    } cyg_mutex_unlock( &m0 );

    finish( (cyg_ucount8)data );
}

static void entry2( cyg_addrword_t data )
{
    cyg_mutex_lock( &m0 ); {
        while( 3 != m0d ) {
            cyg_cond_wait( &cvar0 );
        }
        CHECK( 3 == m0d++ );
        cyg_cond_broadcast( &cvar1 );
    } cyg_mutex_unlock( &m0 );

    finish( (cyg_ucount8)data );
}

void kmutex1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "kmutex1-0",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_thread_create(4, entry1 , (cyg_addrword_t)1, "kmutex1-1",
        (void *)stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
    cyg_thread_resume(thread[1]);

    cyg_thread_create(4, entry2 , (cyg_addrword_t)2, "kmutex1-2",
        (void *)stack[2], STACKSIZE, &thread[2], &thread_obj[2]);
    cyg_thread_resume(thread[2]);

    cyg_mutex_init( &m0 );
    cyg_mutex_init( &m1 );

    cyg_cond_init( &cvar0, &m0 );
    cyg_cond_init( &cvar1, &m0 );
    cyg_cond_init( &cvar2, &m1 );

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    kmutex1_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF kmutex1.c */
