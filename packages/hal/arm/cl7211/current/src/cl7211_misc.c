//==========================================================================
//
//      cl7211_misc.c
//
//      HAL misc board support code for ARM CL7211-1
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-02-20
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_cl7211.h>         // Hardware definitions

#define CYGHWR_HAL_ARM_CL7211_BATLOW
#ifdef CYGHWR_HAL_ARM_CL7211_BATLOW
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support
static cyg_interrupt batlow_interrupt;
static cyg_handle_t  batlow_interrupt_handle;
#endif

#if (CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK == 18432)
#define CPU_CLOCK 0
#elif (CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK == 36864)
#define CPU_CLOCK 1
#elif (CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK == 49152)
#define CPU_CLOCK 2
#elif (CYGHWR_HAL_ARM_CL7211_PROCESSOR_CLOCK == 73728)
#define CPU_CLOCK 3
#else
#err Invalid CPU clock frequency
#endif

static cyg_uint32 _period;

// Use Timer/Counter #2 for system clock

void hal_clock_initialize(cyg_uint32 period)
{
    volatile cyg_uint32 *syscon1 = (volatile cyg_uint32 *)SYSCON1;
    volatile cyg_uint32 *tc2d = (volatile cyg_uint32 *)TC2D;
    // Set timer to 512KHz, prescale mode
    *syscon1 = (*syscon1 & ~(SYSCON1_TC2M|SYSCON1_TC2S)) | SYSCON1_TC2S | SYSCON1_TC2M;
    // Initialize counter
    *tc2d = period;
    _period = period;
}

// This routine is called during a clock interrupt.

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    volatile cyg_uint32 *tc2d = (volatile cyg_uint32 *)TC2D;
    if (period != _period) {
        *tc2d = period;
        _period = period;
    }
}

// Read the current value of the clock, returning the number of hardware "ticks"
// that have occurred (i.e. how far away the current value is from the start)

void hal_clock_read(cyg_uint32 *pvalue)
{
    volatile cyg_int32 *tc2d = (volatile cyg_int32 *)TC2D;
    static cyg_int32 clock_val;
    clock_val = *tc2d & 0x0000FFFF;                 // Register has only 16 bits
    if (clock_val & 0x00008000) clock_val |= 0xFFFF8000;  // Extend sign bit
    *pvalue = (cyg_uint32)(_period - clock_val);    // 'clock_val' counts down and wraps
}

// These tables map the various [soft] interrupt numbers onto the hardware

static cyg_uint32 hal_interrupt_bitmap[] = {
    0,              // CYGNUM_HAL_INTERRUPT_unused     0
    INTSR1_EXTFIQ,  // CYGNUM_HAL_INTERRUPT_EXTFIQ     1
    INTSR1_BLINT,   // CYGNUM_HAL_INTERRUPT_BLINT      2
    INTSR1_WEINT,   // CYGNUM_HAL_INTERRUPT_WEINT      3
    INTSR1_MCINT,   // CYGNUM_HAL_INTERRUPT_MCINT      4
    INTSR1_CSINT,   // CYGNUM_HAL_INTERRUPT_CSINT      5
    INTSR1_EINT1,   // CYGNUM_HAL_INTERRUPT_EINT1      6    
    INTSR1_EINT2,   // CYGNUM_HAL_INTERRUPT_EINT2      7
    INTSR1_EINT3,   // CYGNUM_HAL_INTERRUPT_EINT3      8
    INTSR1_TC1OI,   // CYGNUM_HAL_INTERRUPT_TC1OI      9
    INTSR1_TC2OI,   // CYGNUM_HAL_INTERRUPT_TC2OI      10
    INTSR1_RTCMI,   // CYGNUM_HAL_INTERRUPT_RTCMI      11
    INTSR1_TINT,    // CYGNUM_HAL_INTERRUPT_TINT       12
    INTSR1_UTXINT1, // CYGNUM_HAL_INTERRUPT_UTXINT1    13
    INTSR1_URXINT1, // CYGNUM_HAL_INTERRUPT_URXINT1    14
    INTSR1_UMSINT,  // CYGNUM_HAL_INTERRUPT_UMSINT     15
    INTSR1_SSEOTI,  // CYGNUM_HAL_INTERRUPT_SSEOTI     16
    INTSR2_KBDINT,  // CYGNUM_HAL_INTERRUPT_KBDINT     17
    INTSR2_SS2RX,   // CYGNUM_HAL_INTERRUPT_SS2RX      18
    INTSR2_SS2TX,   // CYGNUM_HAL_INTERRUPT_SS2TX      19
    INTSR2_UTXINT2, // CYGNUM_HAL_INTERRUPT_UTXINT2    20
    INTSR2_URXINT2, // CYGNUM_HAL_INTERRUPT_URXINT2    21
#if defined(__CL7211)
    INTSR3_MCPINT   // CYGNUM_HAL_INTERRUPT_MCPINT     22
#endif
};

static cyg_uint32 hal_interrupt_mask_regmap[] = {
    0,      // CYGNUM_HAL_INTERRUPT_unused     0
    INTMR1, // CYGNUM_HAL_INTERRUPT_EXTFIQ     1
    INTMR1, // CYGNUM_HAL_INTERRUPT_BLINT      2
    INTMR1, // CYGNUM_HAL_INTERRUPT_WEINT      3
    INTMR1, // CYGNUM_HAL_INTERRUPT_MCINT      4
    INTMR1, // CYGNUM_HAL_INTERRUPT_CSINT      5
    INTMR1, // CYGNUM_HAL_INTERRUPT_EINT1      6    
    INTMR1, // CYGNUM_HAL_INTERRUPT_EINT2      7
    INTMR1, // CYGNUM_HAL_INTERRUPT_EINT3      8
    INTMR1, // CYGNUM_HAL_INTERRUPT_TC1OI      9
    INTMR1, // CYGNUM_HAL_INTERRUPT_TC2OI      10
    INTMR1, // CYGNUM_HAL_INTERRUPT_RTCMI      11
    INTMR1, // CYGNUM_HAL_INTERRUPT_TINT       12
    INTMR1, // CYGNUM_HAL_INTERRUPT_UTXINT1    13
    INTMR1, // CYGNUM_HAL_INTERRUPT_URXINT1    14
    INTMR1, // CYGNUM_HAL_INTERRUPT_UMSINT     15
    INTMR1, // CYGNUM_HAL_INTERRUPT_SSEOTI     16
    INTMR2, // CYGNUM_HAL_INTERRUPT_KBDINT     17
    INTMR2, // CYGNUM_HAL_INTERRUPT_SS2RX      18
    INTMR2, // CYGNUM_HAL_INTERRUPT_SS2TX      19
    INTMR2, // CYGNUM_HAL_INTERRUPT_UTXINT2    20
    INTMR2, // CYGNUM_HAL_INTERRUPT_URXINT2    21
#if defined(__CL7211)
    INTMR3, // CYGNUM_HAL_INTERRUPT_MCPINT     22
#endif
};

static cyg_uint32 hal_interrupt_clear_map[] = {
    0,      // CYGNUM_HAL_INTERRUPT_unused     0
    0,      // CYGNUM_HAL_INTERRUPT_EXTFIQ     1
    BLEOI,  // CYGNUM_HAL_INTERRUPT_BLINT      2
    TEOI,   // CYGNUM_HAL_INTERRUPT_WEINT      3
    MCEOI,  // CYGNUM_HAL_INTERRUPT_MCINT      4
    COEOI,  // CYGNUM_HAL_INTERRUPT_CSINT      5
    0,      // CYGNUM_HAL_INTERRUPT_EINT1      6    
    0,      // CYGNUM_HAL_INTERRUPT_EINT2      7
    0,      // CYGNUM_HAL_INTERRUPT_EINT3      8
    TC1EOI, // CYGNUM_HAL_INTERRUPT_TC1OI      9
    TC2EOI, // CYGNUM_HAL_INTERRUPT_TC2OI      10
    RTCEOI, // CYGNUM_HAL_INTERRUPT_RTCMI      11
    TEOI,   // CYGNUM_HAL_INTERRUPT_TINT       12
    0,      // CYGNUM_HAL_INTERRUPT_UTXINT1    13
    0,      // CYGNUM_HAL_INTERRUPT_URXINT1    14
    UMSEOI, // CYGNUM_HAL_INTERRUPT_UMSINT     15
    0,      // CYGNUM_HAL_INTERRUPT_SSEOTI     16
    0,      // CYGNUM_HAL_INTERRUPT_KBDINT     17
    0,      // CYGNUM_HAL_INTERRUPT_SS2RX      18
    0,      // CYGNUM_HAL_INTERRUPT_SS2TX      19
    0,      // CYGNUM_HAL_INTERRUPT_UTXINT2    20
    0,      // CYGNUM_HAL_INTERRUPT_URXINT2    21
#if defined(__CL7211)
    0,      // CYGNUM_HAL_INTERRUPT_MCPINT     22
#endif
};

static struct regmap {
    int        first_int, last_int;
    cyg_uint32 stat_reg, mask_reg;
} hal_interrupt_status_regmap[] = {
    { CYGNUM_HAL_INTERRUPT_EXTFIQ, CYGNUM_HAL_INTERRUPT_SSEOTI,  INTSR1, INTMR1},
    { CYGNUM_HAL_INTERRUPT_KBDINT, CYGNUM_HAL_INTERRUPT_URXINT2, INTSR2, INTMR2},
#if defined(__CL7211)
    { CYGNUM_HAL_INTERRUPT_MCPINT, CYGNUM_HAL_INTERRUPT_MCPINT,  INTSR3, INTMR3},
#endif
    { 0, 0, 0}
};

#ifdef CYGHWR_HAL_ARM_CL7211_BATLOW
// This ISR is called when the battery low interrupt occurs
int 
cyg_hal_batlow_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    diag_printf("Battery low\n");
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_BLINT);
    // Presumably, one would leave this masked until the battery changed
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_BLINT);
    return 0;  // No need to run DSR
}
#endif

//
// Early stage hardware initialization
//   Some initialization has already been done before we get here.  For now
// just set up the interrupt environment.

void hal_hardware_init(void)
{
    volatile cyg_uint32 *icr;
    int vector;
    // Clear and initialize instruction cache
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_ICACHE_ENABLE();
    // Any hardware/platform initialization that needs to be done.
#if 0
    diag_printf("IMR1: %04x, IMR2: %04x\n",
                *(volatile cyg_uint32 *)INTMR1,
                *(volatile cyg_uint32 *)INTMR2);
    diag_printf("Memcfg1: %08x, Memcfg2: %08x, DRAM refresh: %08x\n", 
                *(volatile cyg_uint32 *)MEMCFG1,
                *(volatile cyg_uint32 *)MEMCFG2,
                *(volatile cyg_uint8 *)DRFPR);
#endif
    // Reset all interrupt masks (disable all interrupt sources)
    *(volatile cyg_uint32 *)INTMR1 = 0;
    *(volatile cyg_uint32 *)INTMR2 = 0;
#if defined(__CL7211)
    *(volatile cyg_uint32 *)INTMR3 = 0;
    *(volatile cyg_uint8 *)SYSCON3 = SYSCON3_CLKCTL(CPU_CLOCK);
#endif
    for (vector = CYGNUM_HAL_ISR_MIN;  vector < CYGNUM_HAL_ISR_COUNT;  vector++) {
        icr = (volatile cyg_uint32 *)hal_interrupt_clear_map[vector];
        if (icr) *icr = 0;  // Just a write clears the latch
    }
    // Turn on the DIAG LED to let the world know the board is alive
    *(volatile unsigned char *)LEDFLSH = LEDFLSH_ENABLE|LEDFLSH_DUTY(16)|LEDFLSH_PERIOD(1);
#ifdef CYGHWR_HAL_ARM_CL7211_BATLOW
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_BLINT,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             cyg_hal_batlow_isr,
                             0,
                             &batlow_interrupt_handle,
                             &batlow_interrupt);
    cyg_drv_interrupt_attach(batlow_interrupt_handle);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_BLINT);
#endif
}

//
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.

// This code is a little convoluted to keep it general while still avoiding
// reading the hardware a lot, since the interrupt status is split across
// three separate registers.

int hal_IRQ_handler(void)
{
    struct regmap *map = hal_interrupt_status_regmap;
    cyg_uint32 stat;
    int vector;
    while (map->first_int) {
        stat = *(volatile cyg_uint32 *)map->stat_reg & *(volatile cyg_uint32 *)map->mask_reg;
        for (vector = map->first_int;  vector <= map->last_int;  vector++) {
            if (stat & hal_interrupt_bitmap[vector]) return vector;
        }
        map++;  // Next interrupt status register
    }
    return CYGNUM_HAL_INTERRUPT_unused; // This shouldn't happen!
}

//
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    volatile cyg_uint32 *imr;
    imr = (volatile cyg_uint32 *)hal_interrupt_mask_regmap[vector];
    *imr &= ~hal_interrupt_bitmap[vector];
}

void hal_interrupt_unmask(int vector)
{
    volatile cyg_uint32 *imr;
    imr = (volatile cyg_uint32 *)hal_interrupt_mask_regmap[vector];
    *imr |= hal_interrupt_bitmap[vector];
}

void hal_interrupt_acknowledge(int vector)
{
    // Some interrupt sources have a register for this.
    volatile cyg_uint32 *icr;
    icr = (volatile cyg_uint32 *)hal_interrupt_clear_map[vector];
    if (icr) {
        *icr = 0;  // Any data clears interrupt
    }
}

void hal_interrupt_configure(int vector, int level, int up)
{
    // No interrupts are configurable on this hardware
}

void hal_interrupt_set_level(int vector, int level)
{
    // No interrupts are configurable on this hardware
}

/*------------------------------------------------------------------------*/
// EOF hal_misc.c
