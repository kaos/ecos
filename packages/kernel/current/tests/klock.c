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
//==========================================================================

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

//==========================================================================

#ifdef CYGFUN_KERNEL_API_C

//==========================================================================

#include "testaux.h"

#include <cyg/hal/hal_arch.h>           // for CYGNUM_HAL_STACK_SIZE_TYPICAL

//==========================================================================

#ifdef CYGNUM_HAL_STACK_SIZE_TYPICAL
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
#else
#define STACKSIZE 2000
#endif

//==========================================================================

static char stack[2][STACKSIZE];

static cyg_thread thread[2];

static cyg_handle_t pt0,pt1;

static cyg_mutex_t mx;
static cyg_cond_t cv;
static cyg_sem_t sem;
static cyg_flag_t fl;
static cyg_mbox mbox;
static cyg_handle_t mbh;

volatile static int thread0_state = 0;
volatile static int thread1_state = 0;

//==========================================================================

static void entry0( cyg_addrword_t data )
{
    void *mbret;

    CHECK( 222 == (int)data );

    // Do everything with the scheduler locked.
    cyg_scheduler_lock();

    // --------------------------------------------------
    // Mutex test
    
    cyg_mutex_lock( &mx );
    thread0_state = 1;
    
    // Get thread 2 running.
    cyg_thread_resume(pt1);
    thread0_state = 2;

    cyg_cond_wait( &cv );
    thread0_state = 3;

    while( thread1_state < 2 ) cyg_thread_yield();
    
    cyg_cond_broadcast( &cv );
    thread0_state = 4;

    cyg_mutex_unlock( &mx );
    thread0_state = 5;

    // --------------------------------------------------
    // Semaphore test
    
    cyg_semaphore_wait( &sem );
    thread0_state = 6;

    cyg_semaphore_post( &sem );
    thread0_state = 7;

    while( thread1_state < 7 ) cyg_thread_yield();
    
    // --------------------------------------------------
    // Flags test

    cyg_flag_wait( &fl, 1, CYG_FLAG_WAITMODE_OR|CYG_FLAG_WAITMODE_CLR);
    thread0_state = 8;    

    cyg_flag_setbits( &fl, 2 );
    thread0_state = 9;    
    
    // --------------------------------------------------
    // Message box test

    mbret = cyg_mbox_get( mbh );
    CYG_TEST_CHECK( mbret == (void *)0xAAAAAAAA , "bad result from cyg_mbox_timed_get()");
    thread0_state = 10;

    cyg_mbox_put( mbh, (void *)0xBBBBBBBB );
    thread0_state = 11;
    
    // --------------------------------------------------    
    
    thread0_state = 999;

    cyg_thread_yield();
    cyg_thread_yield();
    cyg_thread_yield();
    
    CYG_TEST_CHECK( thread0_state == 999, "thread 0 not in exit state");
    CYG_TEST_CHECK( thread1_state == 999, "thread 1 not in exit state");
    CYG_TEST_PASS_FINISH("Kernel C API Thread 1 OK");    
}

//==========================================================================

static void entry1( cyg_addrword_t data )
{
    cyg_bool res;
    void *mbret;
    
    CHECK( 333 == (int)data );

    // Do everything with the scheduler locked.
    cyg_scheduler_lock();

    // --------------------------------------------------
    // Mutex test
    
    cyg_mutex_lock( &mx );
    thread1_state = 1;

    while( thread0_state < 2 ) cyg_thread_yield();
    
    cyg_cond_signal( &cv );
    thread1_state = 2;

    res = cyg_cond_timed_wait( &cv, cyg_current_time()+10 );
    CYG_TEST_CHECK( res , "FALSE result from cyg_cond_timed_wait()" );
    thread1_state = 3;

    cyg_mutex_unlock( &mx );
    thread1_state = 4;

    // --------------------------------------------------
    // Semaphore test
    
    while( thread0_state < 5 ) cyg_thread_yield();
    
    cyg_semaphore_post( &sem );
    thread1_state = 5;

    while( thread0_state < 6 ) cyg_thread_yield();
    thread1_state = 6;
    
    res = cyg_semaphore_timed_wait( &sem, cyg_current_time()+10 );
    CYG_TEST_CHECK( res , "FALSE result from cyg_semaphore_timed_wait()" );    
    thread1_state = 7;

    // --------------------------------------------------
    // Flags test

    cyg_flag_setbits( &fl, 1 );
    thread1_state = 8;

    cyg_flag_timed_wait( &fl, 2, CYG_FLAG_WAITMODE_OR|CYG_FLAG_WAITMODE_CLR,
                         cyg_current_time()+10 );
    thread1_state = 9;
    
    // --------------------------------------------------
    // Message box test

    cyg_mbox_timed_put( mbh, (void *)0xAAAAAAAA, cyg_current_time()+10 );
    thread1_state = 10;

    mbret = cyg_mbox_timed_get( mbh, cyg_current_time()+10);
    CYG_TEST_CHECK( mbret == (void *)0xBBBBBBBB , "bad result from cyg_mbox_timed_get()");
    thread1_state = 9;
    
    // --------------------------------------------------
    
    thread1_state = 999;
    cyg_thread_exit();
}

//==========================================================================

void kthread1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0, (cyg_addrword_t)222, "kthread1-0",
        (void *)stack[0], STACKSIZE, &pt0, &thread[0] );
    cyg_thread_create(4, entry1, (cyg_addrword_t)333, "kthread1-1",
        (void *)stack[1], STACKSIZE, &pt1, &thread[1] );

    // Init all the objects

    cyg_mutex_init( &mx );
    cyg_cond_init( &cv, &mx );
    cyg_semaphore_init( &sem, 0 );
    cyg_flag_init( &fl );
    cyg_mbox_create( &mbh, &mbox );
    
    cyg_thread_resume(pt0);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

//==========================================================================

externC void
cyg_start( void )
{
    kthread1_main();
}

//==========================================================================

#else /* def CYGFUN_KERNEL_API_C */

//==========================================================================

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}

//==========================================================================

#endif /* def CYGFUN_KERNEL_API_C */

//==========================================================================
/* EOF kthread1.c */
