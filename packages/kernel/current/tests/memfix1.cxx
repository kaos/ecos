//==========================================================================
//
//        memfix1.cxx
//
//        Fixed memory pool test 1
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
// Date:          1998-06-09
// Description:   Tests basic fixed memory pool functionality
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>       // Cyg_Thread

#include <cyg/kernel/memfixed.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>

#include <cyg/kernel/timer.hxx>         // Cyg_Timer
#include <cyg/kernel/clock.inl>         // Cyg_Clock


#define NTHREADS 2
#include "testaux.hxx"

static const cyg_int32 memsize = 10240;

static cyg_uint8 mem[2][memsize];

static Cyg_Mempool_Fixed mempool0(mem[0], memsize, 100);

static Cyg_Mempool_Fixed mempool1(mem[1], memsize, 316);


static void check_in_mp0(cyg_uint8 *p, cyg_int32 size)
{
    CYG_TEST_CHECK(NULL != p,
                   "Allocation failed");
    CYG_TEST_CHECK(mem[0] <= p && p+size < mem[1],
                   "Block outside memory pool");
}


static void entry0( CYG_ADDRWORD data )
{
    cyg_uint8 *base;
    cyg_int32 size;
    CYG_ADDRWORD bs;
    cyg_int32 f0,f1,f2,t0;
    cyg_uint8 *p0, *p1, *p2;
    
    mempool0.get_arena(base, size, bs);
    cyg_int32 blocksize=(cyg_int32)bs;
    CYG_TEST_CHECK(mem[0] == base, "get_arena: base wrong");
    CYG_TEST_CHECK(memsize == size, "get_arena: size wrong");
    CYG_TEST_CHECK(100 == blocksize, "get_arena: blocksize wrong");
    
    CYG_TEST_CHECK(316 == mempool1.get_blocksize(), "get_blocksize wrong" );

    t0=mempool0.get_totalmem();
    CYG_TEST_CHECK(t0 > 0, "Negative total memory" );
    f0=mempool0.get_freemem();
    CYG_TEST_CHECK(f0 > 0, "Negative free memory" );
    CYG_TEST_CHECK(t0 <= memsize, "get_totalsize: Too much memory");
    CYG_TEST_CHECK(f0 <= t0 , "More memory free than possible" );

    CYG_TEST_CHECK( !mempool0.waiting(),
                    "Thread waiting for memory; there shouldn't be");
    
    CYG_TEST_CHECK( !mempool0.free(NULL), "Bogus free worked"); 

    p0 = mempool0.alloc();
    check_in_mp0(p0, 100);

    f1 = mempool0.get_freemem();
    CYG_TEST_CHECK(f1 > 0, "Negative free memory" );
    CYG_TEST_CHECK(f1 < f0, "Free memory didn't decrease after allocation" );

    p1 = NULL;
    while((p2 = mempool0.try_alloc()))
	p1 = p2;
    
    f1 = mempool0.get_freemem();
    CYG_TEST_CHECK(mempool0.free(p0), "Couldn't free");

    f2 = mempool0.get_freemem();
    CYG_TEST_CHECK(f2 > f1, "Free memory didn't increase after free" );
    
    // should be able to reallocate now a block is free
    p0 = mempool0.try_alloc();
    check_in_mp0(p0, 100);

    CYG_TEST_CHECK(p1+100 <= p0 || p1 >= p0+100,
                   "Ranges of allocated memory overlap");

    CYG_TEST_CHECK(mempool0.free(p0), "Couldn't free");
    CYG_TEST_CHECK(mempool0.free(p1), "Couldn't free");
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    // This shouldn't have to wait
    p0 = mempool0.alloc(Cyg_Clock::real_time_clock->current_value()+100000);
    check_in_mp0(p0, 100);
    p1 = mempool0.alloc(Cyg_Clock::real_time_clock->current_value()+2);
    check_in_mp0(p1, 10);
    p1 = mempool0.alloc(Cyg_Clock::real_time_clock->current_value()+2);
    CYG_TEST_CHECK(NULL == p1, "Timed alloc unexpectedly worked");
    
    // Expect thread 1 to have run while processing previous timed
    // allocation.  It should therefore tbe waiting.
    CYG_TEST_CHECK(mempool1.waiting(), "There should be a thread waiting");
#endif
    
    CYG_TEST_PASS_FINISH("Fixed memory pool 1 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    while(NULL != mempool1.alloc())
        ;
    CYG_TEST_FAIL("alloc returned NULL");
}


void memfix1_main( void )
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);
    new_thread(entry1, 1);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    memfix1_main();
}
// EOF memfix1.cxx
