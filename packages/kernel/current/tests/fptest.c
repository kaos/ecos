//==========================================================================
//
//        fptest.cxx
//
//        Basic FPU test
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003 Nick Garnett
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg@calivar.com
// Contributors:  nickg@calivar.com
// Date:          2003-01-27
// Description:   Simple FPU test. This is not very sophisticated as far
//                as checking FPU performance or accuracy. It is more
//                concerned with checking that several threads doing FP
//                operations do not interfere with eachother's use of the
//                FPU.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>

//#include <cyg/kernel/test/stackmon.h>
//#include CYGHWR_MEMORY_LAYOUT_H

//==========================================================================

#if defined(CYGFUN_KERNEL_API_C) &&             \
    defined(CYGSEM_KERNEL_SCHED_MLQUEUE) &&     \
    (CYGNUM_KERNEL_SCHED_PRIORITIES > 12)

//==========================================================================
// Multiplier for loop counter. This allows us to tune the runtime of
// the whole program easily. The current value gives a runtime of
// about 90s on an 800Mz Pentium III.

#define MULTIPLIER 100

//==========================================================================
// Thread parameters

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_MINIMUM+4096)

cyg_uint8 stacks[3][STACK_SIZE];
cyg_handle_t thread[3];
cyg_thread thread_struct[3];

//==========================================================================
// Random number generator. Ripped out of the C library.

static int rand( unsigned int *seed )
{
// This is the code supplied in Knuth Vol 2 section 3.6 p.185 bottom

#define RAND_MAX 0x7fffffff
#define MM 2147483647    // a Mersenne prime
#define AA 48271         // this does well in the spectral test
#define QQ 44488         // (long)(MM/AA)
#define RR 3399          // MM % AA; it is important that RR<QQ

    *seed = AA*(*seed % QQ) - RR*(unsigned int)(*seed/QQ);
    if (*seed < 0)
        *seed += MM;

    return (int)( *seed & RAND_MAX );
}

//==========================================================================
// Test calculation.
//
// Generates an array of random FP values and then repeatedly applies
// a calculation to them and checks that the same result is reached
// each time. The calculation, in the macro CALC, is intended to make
// maximum use of the FPU registers. However, the i386 compiler
// doesn't let this expression get very complex before it starts
// spilling values out to memory.

static void do_test( double *values,
                    int count,
                    int loops,
                    char *name)
{
    int i, j;
    double sum = 1.0;
    double last_sum;
    unsigned int seed;

#define V(__i) (values[(__i)%count])
#define CALC ((V(i-1)*V(i+1))*(V(i-2)*V(i+2))*(V(i-3)*sum))

    seed = ((unsigned int)&i)*loops*count;

    // Set up an array of values...
    for( i = 0; i < count; i++ )
        values[i] = (double)rand( &seed )/(double)0x7fffffff;

    // Now calculate something from them...
    for( i = 0; i < count; i++ )
        sum += CALC;
    last_sum = sum;
    
    // Now recalculate the sum in a loop and look for errors
    for( j = 0; j < loops; j++ )
    {
        sum = 1.0;
        for( i = 0; i < count; i++ )
            sum += CALC;

        if( sum != last_sum )
            diag_printf("%s: Sum mismatch! %d\n",name,j);

        last_sum = sum;
    }

}

//==========================================================================

volatile int done[4];

volatile cyg_tick_count_t start, end;

//==========================================================================

#define FP1_COUNT 1000
#define FP1_LOOPS 1000*MULTIPLIER

static double fpt1_values[FP1_COUNT];

void fptest1( CYG_ADDRWORD id )
{
    diag_printf("fptest1: start\n");
    
    do_test( &fpt1_values, FP1_COUNT, FP1_LOOPS, "fptest1" );

    done[id] = 1;
    
    diag_printf("fptest1: done\n");
}

//==========================================================================

#define FP2_COUNT 10000
#define FP2_LOOPS 100*MULTIPLIER

static double fpt2_values[FP2_COUNT];

void fptest2( CYG_ADDRWORD id )
{
    diag_printf("fptest2: start\n");
    
    do_test( &fpt2_values, FP2_COUNT, FP2_LOOPS, "fptest2" );

    done[id] = 1;
    
    diag_printf("fptest2: done\n");
}

//==========================================================================

#define FP3_COUNT 10000
#define FP3_LOOPS 100*MULTIPLIER

static double fpt3_values[FP3_COUNT];

void fptest3( CYG_ADDRWORD id )
{
    int all_done;
    
    diag_printf("fptest3: start\n");
    
    do_test( &fpt3_values, FP3_COUNT, FP3_LOOPS, "fptest3" );

    done[id] = 1;
    
    diag_printf("fptest3: done\n");

    // Spin here waiting for the other threads to finish. We should
    // only wake up and test every third or second timeslice.
    do {
        int i;

        cyg_thread_yield();
        
        all_done = 0;
        for( i = 0; i < 4; i++ )
            all_done += done[i];
        
    } while(all_done != 4 );

    end = cyg_current_time();

    diag_printf("Elapsed time %d ticks\n",end-start);
    
    CYG_TEST_PASS_FINISH("FP Test OK");
    
}

//==========================================================================

void fptest_main( void )
{
    
    CYG_TEST_INIT();

    start = cyg_current_time();
    
    diag_printf("Run fptest1 in cyg_start\n");
    fptest1( 0 );
    
    cyg_thread_create( 5,
                       fptest1,
                       1,
                       "fptest1",
                       &stacks[0][0],
                       STACK_SIZE,
                       &thread[0],
                       &thread_struct[0]);

    cyg_thread_resume( thread[0] );

    cyg_thread_create( 5,
                       fptest2,
                       2,
                       "fptest2",
                       &stacks[1][0],
                       STACK_SIZE,
                       &thread[1],
                       &thread_struct[1]);

    cyg_thread_resume( thread[1] );

    cyg_thread_create( 5,
                       fptest3,
                       3,
                       "fptest3",
                       &stacks[2][0],
                       STACK_SIZE,
                       &thread[2],
                       &thread_struct[2]);

    cyg_thread_resume( thread[2] );
    
    cyg_scheduler_start();

}

//==========================================================================

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
externC void
cyg_hal_invoke_constructors();
#endif

externC void
cyg_start( void )
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    fptest_main();
}   

//==========================================================================

#else // CYGFUN_KERNEL_API_C...

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA("FP test requires:\n"
                "CYGFUN_KERNEL_API_C && \n"
                "CYGSEM_KERNEL_SCHED_MLQUEUE && \n"
                "(CYGNUM_KERNEL_SCHED_PRIORITIES > 12)\n");
}

#endif // CYGFUN_KERNEL_API_C, etc.

//==========================================================================
// EOF fptest.cxx
