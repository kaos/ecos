//==========================================================================
//
//        pthread2.cxx
//
//        POSIX pthread test 2 - per-thread data
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.      
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
// Description:   Tests POSIX per-thread data.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/testcase.h>
#include <pkgconf/posix.h>

#ifndef CYGPKG_POSIX_PTHREAD
#define NA_MSG "POSIX threads not enabled"
#endif

#ifdef NA_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(NA_MSG);
}
#else

#include <sys/types.h>
#include <pthread.h>

//--------------------------------------------------------------------------
// Thread stack.

char thread_stack[2][PTHREAD_STACK_MIN*2];

pthread_t thread[2];

pthread_key_t key;

//--------------------------------------------------------------------------

void key_destructor( void *val )
{
    int ret;

    CYG_TEST_INFO( "key destructor called" );

    if( (long)val == 0xAAAAAAAA )
    {
        ret = pthread_setspecific( key, NULL );
        CYG_TEST_CHECK( ret == 0, "pthread_setspecific() returned error");
    }
    else
    {
        ret = pthread_setspecific( key, (void *)0xAAAAAAAA );
        CYG_TEST_CHECK( ret == 0, "pthread_setspecific() returned error");
    }
}

//--------------------------------------------------------------------------

void *pthread_entry( void *arg)
{
    int ret;
    int retval = 1;
    void *val;
    
    CYG_TEST_INFO( "Thread running" );

    ret = pthread_setspecific( key, arg );
    CYG_TEST_CHECK( ret == 0, "pthread_setspecific() returned error");
 
    val = pthread_getspecific( key );
    CYG_TEST_CHECK( val == arg, "pthread_getspecific() did not return expected value");
    if( val != arg ) retval = 0;
    
    sched_yield();

    val = pthread_getspecific( key );
    CYG_TEST_CHECK( val == arg, "pthread_getspecific() did not return expected value");
    if( val != arg ) retval = 0;
    
    pthread_exit( (void *)retval );
}

//--------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int i;
    int ret;
    void *retval[2];

    CYG_TEST_INIT();

    // allocate data key
    ret = pthread_key_create( &key, key_destructor );
    CYG_TEST_CHECK( ret == 0, "pthread_key_create() returned error");
    
    // Create test threads
    for( i = 0; i < 2; i++ )
    {
        pthread_attr_t attr;
        pthread_attr_init( &attr );

        pthread_attr_setstackaddr( &attr, (void *)&thread_stack[i][sizeof(thread_stack[i])] );
        pthread_attr_setstacksize( &attr, sizeof(thread_stack[i]) );

        ret = pthread_create( &thread[i],
                              &attr,
                              pthread_entry,
                              (void *)(0x12340000+i));
        CYG_TEST_CHECK( ret == 0, "pthread_create() returned error");
    }
    
    // Now join with threads
    for( i = 0; i < 2; i++ )
        pthread_join( thread[i], &retval[i] );


    ret = pthread_key_delete( key );
    CYG_TEST_CHECK( ret == 0, "pthread_key_delete() returned error");
            
    // check retvals

    for( i = 0; i < 2; i++ )
        if( !(long)retval[0] )
            CYG_TEST_FAIL_FINISH( "pthread2" );
                
    CYG_TEST_PASS_FINISH( "pthread2" );
        
}

#endif

//--------------------------------------------------------------------------
// end of pthread2.c
