/*==========================================================================
//
//        kmemvar1.cxx
//
//        Kernel C API Variable memory pool test 1
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-06-08
// Description:   Tests basic variable memory pool functionality
//####DESCRIPTIONEND####
*/

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <pkgconf/kernel.h>

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_KERNEL_API_C

#include "testaux.h"

#define NTHREADS 2
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


#define MEMSIZE 10240

static cyg_uint8 mem[2][MEMSIZE];

static cyg_mempool_var mempool_obj[2];
static cyg_handle_t mempool0, mempool1;

static void check_in_mp0(cyg_uint8 *p, cyg_int32 size)
{
    CYG_TEST_CHECK(NULL != p,
                   "Allocation failed");
    CYG_TEST_CHECK(mem[0] <= p && p+size < mem[1],
                   "Block outside memory pool");
}

static void entry0( cyg_addrword_t data )
{
    cyg_uint8 *p0, *p1;
    cyg_int32 most_of_mem=MEMSIZE/4*3;
    cyg_mempool_info info0, info1, info2;
    
    cyg_mempool_var_get_info(mempool0, &info0);
    
    CYG_TEST_CHECK(mem[0] == info0.base, "get_arena: base wrong");
    CYG_TEST_CHECK(MEMSIZE == info0.size, "get_arena: size wrong");

    CYG_TEST_CHECK(0 < info0.maxfree && info0.maxfree <= info0.size,
                   "get_arena: maxfree wildly wrong");
    
    CYG_TEST_CHECK(-1 == info0.blocksize, "get_blocksize wrong" );

    CYG_TEST_CHECK(info0.totalmem > 0, "Negative total memory" );
    CYG_TEST_CHECK(info0.freemem > 0, "Negative free memory" );
    CYG_TEST_CHECK(info0.totalmem <= MEMSIZE, 
                   "info.totalsize: Too much memory");
    CYG_TEST_CHECK(info0.freemem <= info0.totalmem ,
                   "More memory free than possible" );

    CYG_TEST_CHECK( !cyg_mempool_var_waiting(mempool0),
                    "Thread waiting for memory; there shouldn't be");
    
    CYG_TEST_CHECK( NULL == cyg_mempool_var_try_alloc(mempool0, MEMSIZE+1),
                    "Managed to allocate too much memory");
    
    p0 = cyg_mempool_var_alloc(mempool0, most_of_mem);
    check_in_mp0(p0, most_of_mem);

    cyg_mempool_var_get_info(mempool0, &info1);

    CYG_TEST_CHECK(info1.freemem > 0, "Negative free memory" );
    CYG_TEST_CHECK(info1.freemem < info0.freemem,
                   "Free memory didn't decrease after allocation" );

    CYG_TEST_CHECK( NULL == cyg_mempool_var_try_alloc(mempool0, most_of_mem),
                    "Managed to allocate too much memory");
    
    cyg_mempool_var_free(mempool0, p0);

    cyg_mempool_var_get_info(mempool0, &info2);
    CYG_TEST_CHECK(info2.freemem > info1.freemem,
                   "Free memory didn't increase after free" );
    
    // should be able to reallocate now memory is free
    p0 = cyg_mempool_var_try_alloc(mempool0, most_of_mem);
    check_in_mp0(p0, most_of_mem);

    p1 = cyg_mempool_var_try_alloc(mempool0, 10);
    check_in_mp0(p1, 10);
    
    CYG_TEST_CHECK(p1+10 <= p0 || p1 >= p0+MEMSIZE,
                   "Ranges of allocated memory overlap");

    cyg_mempool_var_free(mempool0, p0);
    cyg_mempool_var_free(mempool0, p1);
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    // This shouldn't have to wait
    p0 = cyg_mempool_var_timed_alloc(mempool0, most_of_mem,
                                     cyg_current_time()+100000);
    check_in_mp0(p0, most_of_mem);
    p1 = cyg_mempool_var_timed_alloc(mempool0, most_of_mem,
                                     cyg_current_time()+2);
    CYG_TEST_CHECK(NULL == p1, "Timed alloc unexpectedly worked");
    p1 = cyg_mempool_var_timed_alloc(mempool0, 10,
                                     cyg_current_time()+2);
    check_in_mp0(p1, 10);
    
    // Expect thread 1 to have run while processing previous timed
    // allocation.  It should therefore tbe waiting.
    CYG_TEST_CHECK(cyg_mempool_var_waiting(mempool1), "There should be a thread waiting");
#endif
    
    CYG_TEST_PASS_FINISH("Kernel C API Variable memory pool 1 OK");
}

static void entry1( cyg_addrword_t data )
{
    cyg_mempool_var_alloc(mempool1, MEMSIZE+1);
    CYG_TEST_FAIL("Oversized alloc returned");
}


void kmemvar1_main( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "kmemvar1-0",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_thread_create(4, entry1 , (cyg_addrword_t)1, "kmemvar1-1",
        (void *)stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
    cyg_thread_resume(thread[1]);

    cyg_mempool_var_create(mem[0], MEMSIZE, &mempool0, &mempool_obj[0]);
    cyg_mempool_var_create(mem[1], MEMSIZE, &mempool1, &mempool_obj[1]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    kmemvar1_main();
}

#else /* def CYGFUN_KERNEL_API_C */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_KERNEL_API_C */

/* EOF kmemvar1.c */
