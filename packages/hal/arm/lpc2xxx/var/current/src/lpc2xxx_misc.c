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
#include <pkgconf/hal_arm_lpc2xxx.h>

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

#include <cyg/infra/diag.h>     // For diagnostic printing

// -------------------------------------------------------------------------
// Processor clock configuration values and accessor functions
static cyg_uint32 lpc_cclk; // CPU clock frequency
static cyg_uint32 lpc_pclk; // peripheral devices clock speed
                            // (equal to, half, or quarter of CPU clock)
static cyg_uint32 lpc_xclk; // XCLK speed (equal to, half, or quarter of
                            // CPU clock)

cyg_uint32 hal_lpc_get_cclk(void)
{
    return (lpc_cclk); 
}

cyg_uint32 hal_lpc_get_pclk(void)
{
    return (lpc_pclk); 
}

cyg_uint32 hal_lpc_get_xclk(void)
{
    return (lpc_xclk); 
}

// -------------------------------------------------------------------------
// eCos clock support
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
    cyg_uint32 vpbdiv_reg;
	
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VPBDIV, vpbdiv_reg);
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VPBDIV, vpbdiv_reg);

    return (vpbdiv_reg);
}

// Set the VPBDIV register. The vpb bits are 1:0 and the xclk bits are 5:4. The
// mapping of values passed to this routine to field values is:
//       4 = divide by 4 (register bits 00)
//       2 = divide by 2 (register bits 10)
//       1 = divide by 1 (register bits 01)
// This routine assumes that only these values can occur. As they are
// generated in the CDL hopefully this should be the case. Fortunately
// writing 11 merely causes the previous value to be retained.
void lpc_set_vpbdiv(int vpbdiv, int xclkdiv)
{
    CYG_ASSERT(((vpbdiv & 0x3) != 3) && ((xclkdiv & 0x3) != 3),
               "illegal VPBDIV register value");

    // Update VPBDIV register
#ifdef CYGHWR_HAL_ARM_LPC2XXX_FAMILY_LPC22XX
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE +
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV,
                     ((xclkdiv & 0x3) << 4) | (vpbdiv & 0x3));
    lpc_xclk = lpc_cclk / xclkdiv;
#else
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, vpbdiv & 0x3);
    lpc_xclk = 0;        // Obvious bad value
#endif
    
    lpc_pclk = lpc_cclk / vpbdiv;
}

// Perform variant setup. This optionally calls into the platform
// HAL if it has defined HAL_PLF_HARDWARE_INIT.
void hal_hardware_init(void)
{
    lpc_cclk = CYGNUM_HAL_ARM_LPC2XXX_CLOCK_SPEED;

#ifdef CYGHWR_HAL_ARM_LPC2XXX_FAMILY_LPC22XX
    lpc_set_vpbdiv(CYGNUM_HAL_ARM_LPC2XXX_VPBDIV,
                   CYGNUM_HAL_ARM_LPC2XXX_XCLKDIV);
#else
    lpc_set_vpbdiv(CYGNUM_HAL_ARM_LPC2XXX_VPBDIV, 1);
#endif

#ifdef HAL_PLF_HARDWARE_INIT
    // Perform any platform specific initializations
    HAL_PLF_HARDWARE_INIT();
#endif

    // Set up eCos/ROM interfaces
    hal_if_init();
}

// -------------------------------------------------------------------------
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num, irq_stat;

    // Find out which interrupt caused the IRQ. This picks the lowest
    // if there are more than 1.
    // FIXME:try to make use of the VIC for better latency.
    //       That will probably need changes to vectors.S and 
    //       other int-related code
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_VICIRQSTAT, irq_stat);
    for (irq_num = 0; irq_num < 32; irq_num++)
      if (irq_stat & (1 << irq_num))
        break;
    
    // If not a valid interrrupt source, treat as spurious interrupt    
    if (irq_num < CYGNUM_HAL_ISR_MIN || irq_num > CYGNUM_HAL_ISR_MAX)
      irq_num = CYGNUM_HAL_INTERRUPT_NONE;
    
    return (irq_num);
}

// -------------------------------------------------------------------------
// Interrupt control
//

// Block the the interrupt associated with the vector
void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICINTENCLEAR, 1 << vector);
}

// Unblock the the interrupt associated with the vector
void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICINTENABLE, 1 << vector);
}

// Acknowledge the the interrupt associated with the vector. This
// clears the interrupt but may result in another interrupt being
// delivered
void hal_interrupt_acknowledge(int vector)
{

    // External interrupts have to be cleared from the EXTINT register
    if (vector >= CYGNUM_HAL_INTERRUPT_EINT0 &&
        vector <= CYGNUM_HAL_INTERRUPT_EINT3)
      {
        // Map int vector to corresponding bit (0..3)
        vector = 1 << (vector - CYGNUM_HAL_INTERRUPT_EINT0);
        
        // Clear the external interrupt
        HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                         CYGARC_HAL_LPC2XXX_REG_EXTINT, vector);
      }
    
    // Acknowledge interrupt in the VIC
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VICVECTADDR, 0);  
}

// This provides control over how an interrupt signal is detected.
// Options are between level or edge sensitive (level) and high/low
// level or rising/falling edge triggered (up).
//
// This should be simple, but unfortunately on some processor revisions,
// it trips up on two errata issues (for the LPC2294 Rev.A these are
// EXTINT.1 and VPBDIV.1) and so on these devices a somewhat convoluted
// sequence in order to work properly. There is nothing in the errata
// sequence that won't work on a processor without these issues.
void hal_interrupt_configure(int vector, int level, int up)
{
    cyg_uint32 regval, saved_vpbdiv;

    // Only external interrupts are configurable	
    CYG_ASSERT(vector <= CYGNUM_HAL_INTERRUPT_EINT3 &&
               vector >= CYGNUM_HAL_INTERRUPT_EINT0 , "Invalid vector");

    // Map int vector to corresponding bit (0..3)
    vector = 1 << (vector - CYGNUM_HAL_INTERRUPT_EINT0);
    
#ifdef CYGHWR_HAL_ARM_LPC2XXX_EXTINT_ERRATA
    // From discussions with the Philips applications engineers on the
    // Yahoo LPC2000 forum, it appears that in order up change both
    // EXTMODE and EXTPOLAR, the operations have to be performed in
    // two passes as follows:
    // old=VPBDIV (x2),
    //     VPBDIV=0, EXTMODE=n, VPBDIV=n, VPBDIV=0, EXTPOLAR=y, VPBDIV=y
    // VPCDIV=old
    
    // Save current VPBDIV register settings
    saved_vpbdiv = lpc_get_vpbdiv();
    
    // Clear VPBDIV register
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, 0);
    
    // Read current mode and update for level (0) or edge detection (1)
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTMODE, regval);
    if (level)
      regval &= ~vector;
    else
      regval |= vector;
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTMODE, regval);
    
    // Set VPBDIV register to same value as mode
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, regval);
    
    // Clear VPBDIV register
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, 0);
    
    // Read current polarity and update for trigger level or edge
    // level: high (1), low (0) edge: rising (1), falling (0)
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, regval);
    if (up)
      regval |= vector;
    else
      regval &= ~vector;
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, regval);
    
    // Set VPBDIV register to same value as mode
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, regval);
    
    // Restore saved VPBDIV register
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_VPBDIV, saved_vpbdiv);
#else
    // Read current mode and update for level (0) or edge detection (1)
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTMODE, regval);
    if (level)
      regval &= ~vector;
    else
      regval |= vector;
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTMODE, regval);
    
    // Read current polarity and update for trigger level or edge
    // level: high (1), low (0) edge: rising (1), falling (0)
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, regval);
    if (up)
      regval |= vector;
    else
      regval &= ~vector;
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTPOLAR, regval);
#endif
    // Clear any spurious interrupt that might have been generated
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_EXTINT, vector);
}

// Change interrupt level. This is a non-operation on the LPC2XXX
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
    
    while(1)
      continue;
}

//--------------------------------------------------------------------------
// EOF lpc_misc.c
