//==========================================================================
//
//        pthread1.cxx
//
//        POSIX pthread test 1
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
// Description:   Tests POSIX join functionality.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <sys/types.h>
#include <pthread.h>

#include <cyg/infra/testcase.h>

//--------------------------------------------------------------------------
// Thread stack.

char thread_stack[PTHREAD_STACK_MIN*2];

//--------------------------------------------------------------------------

void *pthread_entry1( void *arg)
{
    CYG_TEST_INFO( "Thread 1 running" );
    
    pthread_exit( (void *)((int)arg+1) );
}

//--------------------------------------------------------------------------

int main(int argc, char **argv)
{
    pthread_t thread;
    pthread_attr_t attr;
    void *retval;

    CYG_TEST_INIT();

    // Create test thread
    pthread_attr_init( &attr );

    pthread_attr_setstackaddr( &attr, (void *)&thread_stack[sizeof(thread_stack)] );
    pthread_attr_setstacksize( &attr, sizeof(thread_stack) );

    pthread_create( &thread,
                    &attr,
                    pthread_entry1,
                    (void *)0x12345678);

    // Now join with it
    pthread_join( thread, &retval );

    // check retval
    
    if( (long)retval == 0x12345679 )
        CYG_TEST_PASS_FINISH( "pthread1" );
    else
        CYG_TEST_FAIL_FINISH( "pthread1" );
}

//--------------------------------------------------------------------------
// end of pthread1.c
