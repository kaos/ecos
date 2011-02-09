//=============================================================================
//
//      timers.c
//
//      Test for Stellaris Cortex-M3 Device Timers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with eCos; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// As a special exception, if other files instantiate templates or use
// macros or inline functions from this file, or you compile this file
// and link it with other works to produce a work based on this file,
// this file does not by itself cause the resulting work to be covered by
// the GNU General Public License. However the source code for this file
// must still be made available in accordance with section (3) of the GNU
// General Public License v2.
//
// This exception does not invalidate any other reasons why a work based
// on this file might be covered by the GNU General Public License.
// -------------------------------------------
// ####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg for STM32
//               ccoutand updated for Stellaris Cortex-M3 Devices
// Date:         2011-01-18
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#if defined(CYGPKG_KERNEL)
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/testcase.h>

//=============================================================================
// Check all required packages and components are present

#if !defined(CYGPKG_KERNEL) || !defined(CYGPKG_KERNEL_API)
# define NA_MSG  "Configuration insufficient"
#endif

//=============================================================================
// If everything is present, compile the full test.

#ifndef NA_MSG

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <string.h>

//=============================================================================

#define LOOPS      24                  // 2 minutes
#define SINGLE_TIMER 0                 // Full / Single timer test

static int      test_stack[( CYGNUM_HAL_STACK_SIZE_MINIMUM / sizeof( int ) )];
static cyg_thread test_thread;
static cyg_handle_t main_thread;

//=============================================================================

struct timer {
    cyg_uint32      timer;
    cyg_uint32      base;
    cyg_uint32      periph;
    cyg_uint32      vector;
    cyg_uint32      priority;
    cyg_uint32      interval;

    cyg_uint32      ticks;

    cyg_uint32      preempt[10];

    cyg_uint32      preempt_dsr[10];
    cyg_uint32      dsr_count[10];

    cyg_interrupt   interrupt_object;
    cyg_handle_t    interrupt_handle;
};

struct timer    timers[] = {
#if SINGLE_TIMER
    {1, CYGHWR_HAL_LM3S_GPTIM0, CYGHWR_HAL_LM3S_P_TIMER0,
     CYGNUM_HAL_INTERRUPT_GTIM0_A, 0x20, 1000},
#else
    {1, CYGHWR_HAL_LM3S_GPTIM0, CYGHWR_HAL_LM3S_P_TIMER0,
     CYGNUM_HAL_INTERRUPT_GTIM0_A, 0x20, 127},
    {2, CYGHWR_HAL_LM3S_GPTIM1, CYGHWR_HAL_LM3S_P_TIMER1,
     CYGNUM_HAL_INTERRUPT_GTIM1_A, 0x60, 355},
    {3, CYGHWR_HAL_LM3S_GPTIM2, CYGHWR_HAL_LM3S_P_TIMER2,
     CYGNUM_HAL_INTERRUPT_GTIM2_A, 0x80, 731},
#endif
    {0, 0, 0, 0}
};


//=============================================================================

volatile cyg_uint32 ticks = 0;
volatile cyg_uint32 nesting = 0;
volatile cyg_uint32 max_nesting = 0;
volatile cyg_uint32 max_nesting_seen = 0;
volatile cyg_uint32 current = 0;
volatile cyg_uint32 in_dsr = 0;


//=============================================================================

void
init_timer( cyg_uint32 base, cyg_uint32 periph, cyg_uint32 interval )
{
    cyg_uint32      period = CYGHWR_HAL_CORTEXM_LM3S8XX_SYSCLK;

    CYGHWR_HAL_LM3S_PERIPH_SET( periph, 1 );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_CTL, 0x0000 );

    period = period / 1000000;

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_TAPR, period );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_CFG,
                      CYGHWR_HAL_LM3S_GPTIM_CFG_16BIT );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_TAMR,
                      CYGHWR_HAL_LM3S_GPTIM_TAMR_PERIODIC );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_TAILR, interval );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_ICR,
                      CYGHWR_HAL_LM3S_GPTIM_ICR_TATOCINT );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_IMR,
                      CYGHWR_HAL_LM3S_GPTIM_IMR_TATOIM );

    HAL_WRITE_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_CTL,
                      CYGHWR_HAL_LM3S_GPTIM_CTL_TAEN );
}


//=============================================================================

cyg_uint32
timer_isr( cyg_uint32 vector, CYG_ADDRWORD data )
{
    struct timer   *t = ( struct timer * )data;
    cyg_uint32      preempt = current;
    CYG_ADDRWORD    base = t->base;
    cyg_uint32      cnt;

    current = t->timer;
    t->ticks++;
    ticks++;
    t->preempt[preempt]++;
    nesting++;

    // Count only first ISR to preempt a DSR
    if( preempt == 0 )
        t->preempt_dsr[in_dsr]++;

    HAL_WRITE_UINT32( t->base + CYGHWR_HAL_LM3S_GPTIM_ICR,
                      CYGHWR_HAL_LM3S_GPTIM_ICR_TATOCINT );

    if( nesting > max_nesting )
        max_nesting = nesting;

    // Loiter here for a proportion of the timer interval to give
    // other timers the chance to preempt us.
    do {
        HAL_READ_UINT32( base + CYGHWR_HAL_LM3S_GPTIM_TAR, cnt );
    } while( cnt < t->interval / 10 );

    nesting--;
    current = preempt;

    if( ( t->ticks % 10 ) == 0 )
        return 3;
    else
        return 1;
}


//=============================================================================

void
timer_dsr( cyg_uint32 vector, cyg_uint32 count, CYG_ADDRWORD data )
{
    struct timer   *t = ( struct timer * )data;
    int             i;

    in_dsr = t->timer;

    if( count >= 8 )
        count = 8;

    t->dsr_count[count]++;

    // Loiter for a while
    for( i = 0; i < t->interval / 10; i++ )
        continue;

    in_dsr = 0;
}


//=============================================================================

void
timers_test( cyg_addrword_t data )
{
    int             loops = LOOPS;
    int             i;
    CYG_INTERRUPT_STATE istate;

    CYG_TEST_INIT(  );

    CYG_TEST_INFO( "Start Timers test" );

    for( i = 0; timers[i].timer != 0; i++ ) {
        struct timer   *t = &timers[i];

        init_timer( t->base, t->periph, t->interval );

        cyg_interrupt_create( t->vector,
                              t->priority,
                              ( cyg_addrword_t )t,
                              timer_isr,
                              timer_dsr,
                              &t->interrupt_handle, &t->interrupt_object );

        cyg_interrupt_attach( t->interrupt_handle );
        cyg_interrupt_unmask( t->vector );

    }

    while( loops-- ) {
        int             j;

        // 5 second delay
        cyg_thread_delay( 5 * 100 );

        // Disable interrupts while we print details, otherwise it
        // comes out very slowly.
        HAL_DISABLE_INTERRUPTS( istate );

        if( max_nesting > max_nesting_seen )
            max_nesting_seen = max_nesting;

        diag_printf( "\nISRs max_nesting %d max_nesting_seen %d\n",
                     max_nesting, max_nesting_seen );
        max_nesting = 0;

        diag_printf( " T      Ticks " );

        for( j = 0; j < 9; j++ )
            diag_printf( "%9d ", j );
        diag_printf( "\n" );

        for( i = 0; timers[i].timer != 0; i++ ) {
            struct timer   *t = &timers[i];

            diag_printf( "%2d: %9d ", t->timer, t->ticks );

            for( j = 0; j < 9; j++ )
                diag_printf( "%9d ", t->preempt[j] );
            diag_printf( "\n" );

        }

        diag_printf( "DSRs\n" );

        diag_printf( " T:           " );

        for( j = 0; j < 9; j++ )
            diag_printf( "%9d ", j );
        diag_printf( "\n" );

        for( i = 0; timers[i].timer != 0; i++ ) {
            struct timer   *t = &timers[i];

            diag_printf( "%2d:  preempt: ", t->timer );

            for( j = 0; j < 9; j++ )
                diag_printf( "%9d ", t->preempt_dsr[j] );
            diag_printf( "\n" );

            diag_printf( "       count: " );

            for( j = 0; j < 9; j++ )
                diag_printf( "%9d ", t->dsr_count[j] );
            diag_printf( "\n" );
        }

        HAL_RESTORE_INTERRUPTS( istate );
    }

    CYG_TEST_PASS_FINISH( "Timers test" );
}


//=============================================================================

void
cyg_user_start( void )
{
    cyg_thread_create( 0,              // Priority
                       timers_test,    //
                       0,              //
                       "timers test",  // Name
                       test_stack,     // Stack
                       CYGNUM_HAL_STACK_SIZE_MINIMUM,   // Stack size
                       &main_thread,   // Handle
                       &test_thread    // Thread data structure
         );
    cyg_thread_resume( main_thread );
}

//=============================================================================

#else // NA_MSG

void
cyg_user_start( void )
{
    CYG_TEST_NA( NA_MSG );
}

#endif // NA_MSG

//=============================================================================
// EOF timers.c
