/*==========================================================================
//
//      lpc2xxx_misc.c
//
//      HAL misc variant support code for Philips LPC2xxx
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2004 eCosCentric Limited 
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jani 
// Contributors: gthomas, jskov, nickg, tkoeller
// Date:         2001-07-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif
#include <cyg/hal/var_io.h>             // platform registers

static cyg_uint32 lpc_cclk;	//CPU clock frequency
static cyg_uint32 lpc_pclk;	//peripheral devices clock speed
                                //(equal to, half, or quarter of CPU
                                //clock)

cyg_uint32 hal_lpc_get_pclk(void)
{
    return lpc_pclk; 
}
// -------------------------------------------------------------------------
// Clock support
// Use TIMER0
static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC2XXX_REG_TIMER0_BASE;

    period = period/(lpc_cclk/lpc_pclk);

    // Disable and reset counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTCR, 2);
    
    // set prescale register to 0
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxPR, 0);

    // Set up match register 
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxMR0, period);
    
    // Reset and generate interrupt on match
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxMCR, 
                     CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_INT | 
                     CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_RESET);

    // Enable counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTCR, 1);
}

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC2XXX_REG_TIMER0_BASE;

    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxIR, 
                     CYGARC_HAL_LPC2XXX_REG_TxIR_MR0);  // Clear interrupt

    if (period != _period) {
        hal_clock_initialize(period);
    }
    _period = period;

}

void hal_clock_read(cyg_uint32 *pvalue)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC2XXX_REG_TIMER0_BASE;
    cyg_uint32 val;

    HAL_READ_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTC, val);
    *pvalue = val;
}

// -------------------------------------------------------------------------
//
// Delay for some number of micro-seconds
// use TIMER1
//
void hal_delay_us(cyg_int32 usecs)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC2XXX_REG_TIMER1_BASE;
    cyg_uint32 stat;
    cyg_uint64 ticks;

    // Calculate how many timer ticks the required number of
    // microseconds equate to. We do this calculation in 64 bit
    // arithmetic to avoid overflow.
    ticks = (((cyg_uint64)usecs) * ((cyg_uint64)lpc_pclk))/1000000LL;
    
    // Disable and reset counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTCR, 2);
    
    // Stop on match
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxMR0, ticks);
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxMCR, 
                     CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_STOP | 
                     CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_RESET);

    //set prescale register to 0
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxPR, 0);			

    // Enable counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTCR, 1);

    // Wait for the match
    do {
        HAL_READ_UINT32(timer+CYGARC_HAL_LPC2XXX_REG_TxTC, stat);
    } while (stat < ticks);
}

// -------------------------------------------------------------------------
// Hardware init

// Return value of VPBDIV register. According to errata doc
// we need to read twice consecutively to get correct value
cyg_uint32 lpc_get_vpbdiv(void)
{   
    cyg_uint32 div;	
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VPBDIV, div);
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VPBDIV, div);

    return div;
}

//Set the two bits in VPBDIV which control peripheral clock division
//div must be 1,2 or 4
void lpc_set_vpbdiv(int div)
{   
    cyg_uint8 orig = lpc_get_vpbdiv();
    
    // update VPBDIV register
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, (div % 4) | (orig & 0xFC));
    
    lpc_pclk = lpc_cclk/div;
}

void hal_hardware_init(void)
{
    lpc_cclk = CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED;
    lpc_set_vpbdiv(4);
    // Set up eCos/ROM interfaces
    hal_if_init();

}

// -------------------------------------------------------------------------
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num,irq_stat;
    // Find out which interrupt caused the IRQ
    // picks the lowest if there are more.
    // FIXME:try to make use of the VIC for better latency.
    //       That will probably need changes to vectors.S and 
    //       other int-related code
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VICIRQSTAT, irq_stat);
    for (irq_num = 0; irq_num < 32; irq_num++)
	    if (irq_stat & (1<<irq_num)) break;
    // No valid interrrupt source, treat as spurious interrupt    
    if (irq_num < CYGNUM_HAL_ISR_MIN || irq_num > CYGNUM_HAL_ISR_MAX)
      irq_num = CYGNUM_HAL_INTERRUPT_NONE;
    
    return irq_num;
}

// -------------------------------------------------------------------------
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICINTENCLEAR, 1<<vector);
}

void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICINTENABLE, 1<<vector);
}

void hal_interrupt_acknowledge(int vector)
{
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICVECTADDR, 0);  
}

void hal_interrupt_configure(int vector, int level, int up)
{
    cyg_uint32 mode;
    cyg_uint32 pol;
    // only external interrupts are configurable	
    CYG_ASSERT(vector <= CYGNUM_HAL_INTERRUPT_EINT3 &&
               vector >= CYGNUM_HAL_INTERRUPT_EINT0 , "Invalid vector");
#if CYGHWR_HAL_ARM_LPC2XXX_EXTINT_ERRATA
    // Errata sheet says VPBDIV is corrupted when accessing EXTPOL or EXTMOD
    // Must be written as 0 and at the end restored to original value
    
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, 0);
#endif    
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTMODE, mode);
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, pol);
    
    // map int vector to corresponding bit (0..3)
    vector = 1 << (vector - CYGNUM_HAL_INTERRUPT_EINT0);
    
    // level or edge
    if (level) {
	mode &= ~vector;   
    } else {
	mode |= vector;   
    }
    
    // high/low or falling/rising
    if (up) {
	pol |= vector;   
    } else {
	pol &= ~vector;   
    }
    
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTMODE, mode);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, pol);
    
#if CYGHWR_HAL_ARM_LPC2XXX_EXTINT_ERRATA    
    // we know this was the original value
    lpc_set_vpbdiv(lpc_cclk/lpc_pclk);
#endif
}

void hal_interrupt_set_level(int vector, int level)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    CYG_ASSERT(level >= 0 && level <= 15, "Invalid level");

}

// Use the watchdog to generate a reset
void hal_lpc_watchdog_reset(void)
{
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_WDTC, 0xFF);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_WDMOD, 
                     CYGARC_HAL_LPC2XXX_REG_WDMOD_WDEN | 
                     CYGARC_HAL_LPC2XXX_REG_WDMOD_WDRESET);

    // feed WD with the two magic values
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                     CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC1);	
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                     CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC2);
    
    while(1);
}

//--------------------------------------------------------------------------
// EOF lpc_misc.c
