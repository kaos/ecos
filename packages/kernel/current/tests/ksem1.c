/*=================================================================
//
//        ksem1.c
//
//        C API semaphore test 1
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-03-20
// Description:   Tests basic semaphore functionality.
//####DESCRIPTIONEND####
*/

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#define NTHREADS 2
#define STACKSIZE 4096

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


static cyg_sem_t s0, s1, s2;

static volatile cyg_ucount8 q = 0;

static void entry0( cyg_addrword_t data )
{
    cyg_ucount32 val;

    cyg_semaphore_wait(&s0);
    CHECK( 1 == q++ );
    cyg_semaphore_post(&s1);
    cyg_semaphore_wait(&s0);
    CHECK( 3 == q++ );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 0 == val);
    CHECK( ! cyg_semaphore_trywait(&s0) );
    cyg_semaphore_post(&s0);
    CHECK( 4 == q++ );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 1 == val);
    cyg_semaphore_post(&s0);
    cyg_semaphore_peek(&s0, &val);
    CHECK( 2 == val);
    cyg_semaphore_post(&s1);
    cyg_semaphore_peek(&s2, &val);
    CHECK( 0 == val);
    cyg_semaphore_wait(&s2);
    CHECK( 6 == q++ );
    CYG_TEST_PASS_FINISH("Kernel C API Semaphore 1 OK");
}

static void entry1( cyg_addrword_t data )
{
    cyg_ucount32 val;

    cyg_semaphore_peek(&s1, &val);
    CHECK( 2 == val);
    cyg_semaphore_wait(&s1);
    cyg_semaphore_peek(&s1, &val);
    CHECK( 1 == val);
    cyg_semaphore_wait(&s1);
    CHECK( 0 == q++ );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 0 == val);
    cyg_semaphore_post(&s0);
    cyg_semaphore_wait(&s1);
    CHECK( 2 == q++ );
    cyg_semaphore_post(&s0);
    cyg_semaphore_wait(&s1);
    CHECK( 5 == q++ );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 2 == val);
    CHECK( cyg_semaphore_trywait(&s0) );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 1 == val);
    CHECK( cyg_semaphore_trywait(&s0) );
    cyg_semaphore_peek(&s0, &val);
    CHECK( 0 == val);
    cyg_semaphore_post(&s2);
    cyg_semaphore_wait(&s0);
    CYG_TEST_FAIL_FINISH("Not reached");
}

void ksem1_main( void )
{
    CYG_TEST_INIT();

    cyg_semaphore_init( &s0, 0);
    cyg_semaphore_init( &s1, 2);
    cyg_semaphore_init( &s2, 0);

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "ksem1-0",
	(void *)stack[0], STACKSIZE,&thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_thread_create(4, entry1 , (cyg_addrword_t)1, "ksem1-1",
	(void *)stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
    cyg_thread_resume(thread[1]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    ksem1_main();
}


#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF ksem1.c */
