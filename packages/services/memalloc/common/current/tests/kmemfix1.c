/*==========================================================================
//
//        kmemfix1.cxx
//
//        Kernel C API Fixed memory pool test 1
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
// Author(s):     dsm, jlarmour
// Contributors:  
// Date:          2000-06-19
// Description:   Tests basic fixed memory pool functionality
//####DESCRIPTIONEND####
*/

#include <pkgconf/memalloc.h>

#include <cyg/infra/testcase.h>

#ifdef CYGFUN_MEMALLOC_KAPI

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <pkgconf/kernel.h>

#include <cyg/kernel/kapi.h>

#define NTHREADS 2
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


#define MEMSIZE 10240

static cyg_uint8 mem[2][MEMSIZE];

static cyg_mempool_fix mempool_obj[2];
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
    cyg_uint8 *p0, *p1, *p2;
    cyg_mempool_info info0, info1, info2;

    cyg_mempool_fix_get_info(mempool0, &info0);
    CYG_TEST_CHECK(mem[0] == info0.base, "get_arena: base wrong");
    CYG_TEST_CHECK(MEMSIZE == info0.size, "get_arena: size wrong");

    CYG_TEST_CHECK(0 < info0.maxfree && info0.maxfree <= info0.size,
                   "get_arena: maxfree wildly wrong");
    
    CYG_TEST_CHECK(100 == info0.blocksize, "get_blocksize wrong" );

    CYG_TEST_CHECK(info0.totalmem > 0, "Negative total memory" );
    CYG_TEST_CHECK(info0.freemem > 0, "Negative free memory" );
    CYG_TEST_CHECK(info0.totalmem <= MEMSIZE, 
                   "info.totalsize: Too much memory");
    CYG_TEST_CHECK(info0.freemem <= info0.totalmem ,
                   "More memory free than possible" );

    CYG_TEST_CHECK( !cyg_mempool_fix_waiting(mempool0)    ,
                    "Thread waiting for memory; there shouldn't be");
    
#ifdef CYGSEM_MEMALLOC_ALLOCATOR_FIXED_THREADAWARE
    p0 = cyg_mempool_fix_alloc(mempool0);
    check_in_mp0(p0, 100);

    cyg_mempool_fix_get_info(mempool0, &info1);
    CYG_TEST_CHECK(info1.freemem > 0, "Negative free memory" );
    CYG_TEST_CHECK(info1.freemem < info0.freemem,
                   "Free memory didn't decrease after allocation" );

    p1 = NULL;
    while((p2 = cyg_mempool_fix_try_alloc(mempool0)    ))
        p1 = p2;
    
    cyg_mempool_fix_get_info(mempool0, &info1);
    cyg_mempool_fix_free(mempool0, p0);

    cyg_mempool_fix_get_info(mempool0, &info2);
    CYG_TEST_CHECK(info2.freemem > info1.freemem,
                   "Free memory didn't increase after free" );
#endif
    
    // should be able to reallocate now a block is free
    p0 = cyg_mempool_fix_try_alloc(mempool0);
    check_in_mp0(p0, 100);

    CYG_TEST_CHECK(p1+100 <= p0 || p1 >= p0+100,
                   "Ranges of allocated memory overlap");

    cyg_mempool_fix_free(mempool0, p0);
    cyg_mempool_fix_free(mempool0, p1);
    
#ifdef CYGSEM_MEMALLOC_ALLOCATOR_FIXED_THREADAWARE
# ifdef CYGFUN_KERNEL_THREADS_TIMER
    // This shouldn't have to wait
    p0 = cyg_mempool_fix_timed_alloc(mempool0, cyg_current_time()+100000);
    check_in_mp0(p0, 100);
    p1 = cyg_mempool_fix_timed_alloc(mempool0, cyg_current_time()+20);
    check_in_mp0(p1, 10);
    p1 = cyg_mempool_fix_timed_alloc(mempool0, cyg_current_time()+20);
    CYG_TEST_CHECK(NULL == p1, "Timed alloc unexpectedly worked");
    
    // Expect thread 1 to have run while processing previous timed
    // allocation.  It should therefore be waiting.
    CYG_TEST_CHECK(cyg_mempool_fix_waiting(mempool1),
                   "There should be a thread waiting");
# endif
#endif
    
    CYG_TEST_PASS_FINISH("Kernel C API Fixed memory pool 1 OK");
}

#ifdef CYGSEM_MEMALLOC_ALLOCATOR_FIXED_THREADAWARE
static void entry1( cyg_addrword_t data )
{
    while(NULL != cyg_mempool_fix_alloc(mempool1))
        ;
    CYG_TEST_FAIL("alloc returned NULL");
}
#endif


void kmemfix1_main( void )
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Starting Kernel C API Fixed memory pool 1 test");

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "kmemfix1-0",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

#ifdef CYGSEM_MEMALLOC_ALLOCATOR_FIXED_THREADAWARE
    cyg_thread_create(4, entry1 , (cyg_addrword_t)1, "kmemfix1-1",
        (void *)stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
    cyg_thread_resume(thread[1]);
#endif

    cyg_mempool_fix_create(mem[0], MEMSIZE, 100, &mempool0, &mempool_obj[0]);
    cyg_mempool_fix_create(mem[1], MEMSIZE, 316, &mempool1, &mempool_obj[1]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    kmemfix1_main();
}

#else /* def CYGFUN_MEMALLOC_KAPI */
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Kernel C API layer disabled");
}
#endif /* def CYGFUN_MEMALLOC_KAPI */

/* EOF kmemfix1.c */
