//==========================================================================
//
//        pthread3.cxx
//
//        POSIX pthread test 2 - cancellation
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
// Date:          2000-04-10
// Description:   Tests POSIX cancellation.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <sys/types.h>
#include <pthread.h>

#include <cyg/infra/testcase.h>

//--------------------------------------------------------------------------
// Thread info

#define NTHREADS 2

char thread_stack[NTHREADS][PTHREAD_STACK_MIN*2];

pthread_t thread[NTHREADS];

void *pthread_entry1( void *arg);
void *pthread_entry2( void *arg);

void *(*pthread_entry[NTHREADS])(void *) =
{
    pthread_entry1,
    pthread_entry2
};

//--------------------------------------------------------------------------

volatile cyg_bool cancel_handler1_called = false;
volatile cyg_bool cancel_handler2_called = false;
volatile cyg_bool cancel_handler3_called = false;

//--------------------------------------------------------------------------

void cancel_handler1( void * arg )
{
    CYG_TEST_INFO( "cancel_handler1 called" );

    CYG_TEST_CHECK( (long)arg == 0x12340000, "cancel_handler1: bad arg value");
    
    cancel_handler1_called = true;
}

//--------------------------------------------------------------------------

void cancel_handler2( void * arg )
{
    CYG_TEST_INFO( "cancel_handler2 called" );

    CYG_TEST_CHECK( (long)arg == 0xFFFF1111, "cancel_handler2: bad arg value");
    
    cancel_handler2_called = true;    
}

//--------------------------------------------------------------------------

void cancel_handler3( void * arg )
{
    CYG_TEST_INFO( "cancel_handler3 called" );

    CYG_TEST_CHECK( (long)arg == 0x12340001, "cancel_handler3: bad arg value");    
    
    cancel_handler3_called = true;
}

//--------------------------------------------------------------------------

void function1(void)
{

    pthread_cleanup_push( cancel_handler2, (void *)0xFFFF1111 );

    for(;;)
    {
        sched_yield();
        pthread_testcancel();
    }

    pthread_cleanup_pop( 0 );
}

//--------------------------------------------------------------------------

void *pthread_entry1( void *arg)
{
    int retval = 1;

    CYG_TEST_INFO( "pthread_entry1 entered");

    pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );
    
    pthread_cleanup_push( cancel_handler1, arg );

    function1();    
    
    pthread_cleanup_pop( 0 );

    pthread_exit( (void *)retval );
}

//--------------------------------------------------------------------------

void *pthread_entry2( void *arg)
{
    int retval = 1;

    CYG_TEST_INFO( "pthread_entry2 entered");

    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
    
    pthread_cleanup_push( cancel_handler3, arg );

    for(;;) sched_yield();
    
    pthread_cleanup_pop( 0 );

    pthread_exit( (void *)retval );
}

//--------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int i;
    int ret;
    void *retval[NTHREADS];

    CYG_TEST_INIT();

    // Create test threads
    for( i = 0; i < NTHREADS; i++ )
    {
        pthread_attr_t attr;
        pthread_attr_init( &attr );

        pthread_attr_setstackaddr( &attr, (void *)&thread_stack[i][sizeof(thread_stack)] );
        pthread_attr_setstacksize( &attr, sizeof(thread_stack[i]) );

        ret = pthread_create( &thread[i],
                              &attr,
                              pthread_entry[i],
                              (void *)(0x12340000+i));
        CYG_TEST_CHECK( ret == 0, "pthread_create() returned error");
    }

    // Let the threads get going
    sched_yield();
    sched_yield();
    sched_yield();

    // Now cancel them
    for( i = 0; i < NTHREADS; i++ )    
        pthread_cancel( thread[i] );
        
    // Now join with threads
    for( i = 0; i < NTHREADS; i++ )
        pthread_join( thread[i], &retval[i] );


    // check retvals
    for( i = 0; i < NTHREADS; i++ )
        if( retval[0] != PTHREAD_CANCELED )
            CYG_TEST_FAIL_FINISH( "pthread3" );

    if( !cancel_handler1_called ) CYG_TEST_FAIL_FINISH( "pthread3" );
    if( !cancel_handler2_called ) CYG_TEST_FAIL_FINISH( "pthread3" );
    if( !cancel_handler3_called ) CYG_TEST_FAIL_FINISH( "pthread3" );

    
    CYG_TEST_PASS_FINISH( "pthread3" );
        
}

//--------------------------------------------------------------------------
// end of pthread3.c
