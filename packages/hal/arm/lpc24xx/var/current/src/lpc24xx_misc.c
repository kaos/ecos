/*==========================================================================
//
//      lpc24xx_misc.c
//
//      HAL misc variant support code for NXP LPC24xx
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
// Author(s):    Uwe Kindler 
// Contributors: gthomas, jskov, nickg, tkoeller
// Date:         2008-07-06
// Purpose:      Prozessor support
// Description:  Implementations of LPC24xx processor support
//
//####DESCRIPTIONEND####
//
//========================================================================*/


//===========================================================================
//                                INCLUDES
//===========================================================================
#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_lpc24xx.h>

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


//===========================================================================
// Get peripheral clock for a certain peripheral
//===========================================================================
cyg_uint32 hal_lpc_get_pclk(cyg_uint32 pclk_id)
{
    static const cyg_uint8 divider_tbl[4] =
    {
        4, 1, 2, 8
    };
    cyg_uint32 pclkselreg;
    cyg_uint32 regval;
    cyg_uint8  divider;
    
    CYG_ASSERT(pclk_id >= CYNUM_HAL_LPC24XX_PCLK_WDT &&
               pclk_id <= CYNUM_HAL_LPC24XX_PCLK_SYSCON,
               "Invalid peripheral clock ID");

    //
    // decide if we need PCLKSEL0 or PCLKSEL1
    //
    pclkselreg = ((pclk_id <= CYNUM_HAL_LPC24XX_PCLK_ACF) ? 
                   CYGARC_HAL_LPC24XX_REG_PCLKSEL0 : 
                   CYGARC_HAL_LPC24XX_REG_PCLKSEL1); 
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + pclkselreg, regval);
    regval  = (regval >> ((pclk_id & 0xF) << 1)) & 0x03;
    divider = divider_tbl[regval];    
    if ((8 == divider) && (pclk_id >= CYNUM_HAL_LPC24XX_PCLK_CAN1)
        && (pclk_id <= CYNUM_HAL_LPC24XX_PCLK_ACF))
    {
        divider = 6;
    }           
    return CYGNUM_HAL_ARM_LPC24XX_CLOCK_SPEED / divider;
}


//===========================================================================
// Set peripheral clock
//===========================================================================
void hal_lpc_set_pclk(cyg_uint32 peripheral_id, cyg_uint8 divider)
{
    static const cyg_uint8 clock_tbl[4] =
    {
        0x01, 0x02, 0x00, 0x03
    }; 
    cyg_uint32 clock;
    cyg_uint32 pclkselreg;
    cyg_uint32 regval;
    
    CYG_ASSERT(peripheral_id >= CYNUM_HAL_LPC24XX_PCLK_WDT &&
               peripheral_id <= CYNUM_HAL_LPC24XX_PCLK_SYSCON,
               "Invalid peripheral clock ID");
    CYG_ASSERT(divider <= 8, "Wrong peripheral clock divider value"); 
    //
    // decide if we need PCLKSEL0 or PCLKSEL1
    //
    pclkselreg = (peripheral_id <= CYNUM_HAL_LPC24XX_PCLK_ACF) ? 
                  CYGARC_HAL_LPC24XX_REG_PCLKSEL0 : 
                  CYGARC_HAL_LPC24XX_REG_PCLKSEL1;  
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + pclkselreg, regval);
    divider = (6 == divider) ? 8 : divider;
    clock = clock_tbl[divider >> 1];
    regval &= ~(0x03 << ((peripheral_id & 0xF) << 1));
    regval |= (clock << ((peripheral_id & 0xF) << 1));   
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + pclkselreg, regval);
}


//===========================================================================
// Enable/Disable power for certain peripheral
//===========================================================================
void hal_lpc_set_power(cyg_uint8 pconp_id, int on)
{
    cyg_uint32 regval;
    
    CYG_ASSERT(pconp_id >= CYNUM_HAL_LPC24XX_PCONP_TIMER0 &&
               pconp_id <= CYNUM_HAL_LPC24XX_PCONP_USB,
               "Invalid peripheral power ID");
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC24XX_REG_PCONP, regval);
    if (on)
    {
        regval |= (0x01 << pconp_id);
    }
    else
    {
        regval &= ~(0x01 << pconp_id);
    }
    
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC24XX_REG_PCONP, regval);
}


//===========================================================================
// Set pin function
//===========================================================================
void hal_set_pin_function(cyg_uint8 port, cyg_uint8 pin, cyg_uint8 function)
{
    CYG_ASSERT(port <= 4,  "Port value out of bounds");
    CYG_ASSERT(pin  <= 31, "Pin value out of bounds");
    CYG_ASSERT(function <= 3, "Invalid function value");
    
    cyg_uint32 regval;
    cyg_uint8 pinsel_reg = port << 1;
    pinsel_reg += (pin > 15) ? 1 :0;
    pinsel_reg <<= 2;
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_PIN_BASE + pinsel_reg, regval);
    regval &= ~(0x03 << ((pin & 0xF) << 1));
    regval |= (function << ((pin & 0xF) << 1));  
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_PIN_BASE + pinsel_reg, regval);
}


//===========================================================================
// initialize timer 0 as eCos realtime clock source
//===========================================================================
void hal_clock_initialize(cyg_uint32 period)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC24XX_REG_TIMER0_BASE;

    period = period / (CYGNUM_HAL_ARM_LPC24XX_CLOCK_SPEED / 
                       hal_lpc_get_pclk(CYNUM_HAL_LPC24XX_PCLK_TIMER0));

    // 
    // Disable and reset counter, set prescale register to 0 and
    // Set up match register 
    //
    HAL_WRITE_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxTCR, 2);
    HAL_WRITE_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxPR, 0);
    HAL_WRITE_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxMR0, period);
    
    //
    // Reset and generate interrupt on match and Enable counter
    //
    HAL_WRITE_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxMCR, 
                     CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_INT | 
                     CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_RESET);
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxTCR, 1);
}


//===========================================================================
// Reset clock
//===========================================================================
void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    static cyg_uint32 _period = 0;
    CYG_ADDRESS        timer  = CYGARC_HAL_LPC24XX_REG_TIMER0_BASE;

    HAL_WRITE_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxIR, 
                     CYGARC_HAL_LPC24XX_REG_TxIR_MR0);  // Clear interrupt

    if (period != _period) 
    {
        hal_clock_initialize(period);
    }
    _period = period;
}


//===========================================================================
// Reset clock value
//===========================================================================
void hal_clock_read(cyg_uint32 *pvalue)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC24XX_REG_TIMER0_BASE;
    cyg_uint32 val;

    HAL_READ_UINT32(timer + CYGARC_HAL_LPC24XX_REG_TxTC, val);
    *pvalue = val;
}


//===========================================================================
// Delay for some number of micro-seconds - use TIMER1
//===========================================================================
void hal_delay_us(cyg_int32 usecs)
{
    CYG_ADDRESS timer = CYGARC_HAL_LPC24XX_REG_TIMER1_BASE;
    cyg_uint32 stat;
    cyg_uint64 ticks;

    // Calculate how many timer ticks the required number of
    // microseconds equate to. We do this calculation in 64 bit
    // arithmetic to avoid overflow.
    ticks = (((cyg_uint64)usecs) * 
             ((cyg_uint64)hal_lpc_get_pclk(CYNUM_HAL_LPC24XX_PCLK_TIMER1))) / 
               1000000LL;
    
    // Disable and reset counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxTCR, 2);
    
    // Stop on match
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxMR0, ticks);
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxMCR, 
                     CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_STOP | 
                     CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_RESET);

    //set prescale register to 0
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxPR, 0);         

    // Enable counter
    HAL_WRITE_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxTCR, 1);

    // Wait for the match
    do {
        HAL_READ_UINT32(timer+CYGARC_HAL_LPC24XX_REG_TxTC, stat);
    } while (stat < ticks);
}


//===========================================================================
// Perform variant setup. This optionally calls into the platform
// HAL if it has defined HAL_PLF_HARDWARE_INIT.
//===========================================================================
void hal_hardware_init(void)
{
    cyg_uint32 i; 
    
    //
    // Setup peripheral clocks here according to configuration
    // 
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_CAN1, 
                     CYGNUM_HAL_ARM_LPC24XX_CAN_CLK_DIV);
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_CAN2,   
                     CYGNUM_HAL_ARM_LPC24XX_CAN_CLK_DIV);
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_ACF,    
                     CYGNUM_HAL_ARM_LPC24XX_CAN_CLK_DIV);
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_I2C0,  
                     CYGNUM_HAL_ARM_LPC24XX_I2C0_CLK_DIV);
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_I2C1,  
                     CYGNUM_HAL_ARM_LPC24XX_I2C1_CLK_DIV);
    hal_lpc_set_pclk(CYNUM_HAL_LPC24XX_PCLK_I2C2,  
                     CYGNUM_HAL_ARM_LPC24XX_I2C2_CLK_DIV);
                     
    //
    // Enable power for all used on-chip peripherals
    //
#ifdef CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C0, 1);
#else
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C0, 0);
#endif // CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP 

#ifdef CYGHWR_HAL_ARM_LPC24XX_I2C1_SUPP
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C1, 1);
#else
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C1, 0);
#endif // CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP  

#ifdef CYGHWR_HAL_ARM_LPC24XX_I2C2_SUPP
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C2, 1);
#else
    hal_lpc_set_power(CYNUM_HAL_LPC24XX_PCONP_I2C2, 0);
#endif // CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP    
    
    //
    // Fill vector address registers with interrupt number. If an interrupt
    // occurs we can simply read the interrupt number from the vector
    // address register later
    //
    cyg_uint32 addr = CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                      CYGARC_HAL_LPC24XX_REG_VICVECTADDR0;    
    for (i = 0; i < 32; ++i)
    {
        HAL_WRITE_UINT32(addr, i);
        addr += 4;
    }
#ifdef HAL_PLF_HARDWARE_INIT
    // Perform any platform specific initializations
    HAL_PLF_HARDWARE_INIT();
#endif

    // Set up eCos/ROM interfaces
    hal_if_init();
}


//===========================================================================
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
//===========================================================================
int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num;
    
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                    CYGARC_HAL_LPC24XX_REG_VICVECTADDR, irq_num);
       
    return (irq_num);
}


//===========================================================================
// Block the interrupt associated with the vector
//===========================================================================
void hal_interrupt_mask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC24XX_REG_VICINTENCLEAR, 1 << vector);
}


//===========================================================================
// Unblock the interrupt associated with the vector
//===========================================================================
void hal_interrupt_unmask(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");

    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC24XX_REG_VICINTENABLE, 1 << vector);
}


//===========================================================================
// Acknowledge the interrupt associated with the vector. This
// clears the interrupt but may result in another interrupt being
// delivered
//===========================================================================
void hal_interrupt_acknowledge(int vector)
{

    // External interrupts have to be cleared from the EXTINT register
    if (vector >= CYGNUM_HAL_INTERRUPT_EINT0 &&
        vector <= CYGNUM_HAL_INTERRUPT_EINT3)
    {
        // Map int vector to corresponding bit (0..3)
        vector = 1 << (vector - CYGNUM_HAL_INTERRUPT_EINT0);
        
        // Clear the external interrupt
        HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                         CYGARC_HAL_LPC24XX_REG_EXTINT, vector);
    }
    
    // 
    // Write any value to vector address register to
    // acknowledge the interrupt
    //
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                     CYGARC_HAL_LPC24XX_REG_VICVECTADDR, 0xFFFFFFFF);  
}


//===========================================================================
// This provides control over how an interrupt signal is detected.
// Options are between level or edge sensitive (level) and high/low
// level or rising/falling edge triggered (up).
//===========================================================================
void hal_interrupt_configure(int vector, int level, int up)
{
    cyg_uint32 regval;

    // Only external interrupts are configurable    
    CYG_ASSERT(vector <= CYGNUM_HAL_INTERRUPT_EINT3 &&
               vector >= CYGNUM_HAL_INTERRUPT_EINT0 , "Invalid vector");

    // Map int vector to corresponding bit (0..3)
    vector = 1 << (vector - CYGNUM_HAL_INTERRUPT_EINT0);
    
    // Read current mode and update for level (0) or edge detection (1)
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC24XX_REG_EXTMODE, regval);
    if (level)
    {
      regval &= ~vector;
    }
    else
    {
      regval |= vector;
    }
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC24XX_REG_EXTMODE, regval);
    
    // Read current polarity and update for trigger level or edge
    // level: high (1), low (0) edge: rising (1), falling (0)
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                    CYGARC_HAL_LPC24XX_REG_EXTPOLAR, regval);
    if (up)
    {
      regval |= vector;
    }
    else
    {
      regval &= ~vector;
    }
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC24XX_REG_EXTPOLAR, regval);

    // Clear any spurious interrupt that might have been generated
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE + 
                     CYGARC_HAL_LPC24XX_REG_EXTINT, vector);
}


//===========================================================================
// These selects select a priority level for the 32 vectored IRQs. 
// There are 16 priority levels, corresponding to the values 0 
// through 15 decimal, of which 15 is the lowest priority.
// The reset value of these registers defaults all interrupt to the 
// lowest priority, allowing a single write to elevate the priority 
// of an individual interrupt.
//===========================================================================
void hal_interrupt_set_level(int vector, int level)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    CYG_ASSERT(level >= 0 && level <= 15, "Invalid level");
       
    cyg_uint32 prioreg_addr = CYGARC_HAL_LPC24XX_REG_VIC_BASE + 
                              CYGARC_HAL_LPC24XX_REG_VICVECTPRIO0 + 
                              (vector << 2);                   
    HAL_WRITE_UINT32(prioreg_addr, level & 0xF);
}


//===========================================================================
// Use the watchdog to generate a reset
//===========================================================================
void hal_lpc_watchdog_reset(void)
{
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_WD_BASE + 
                     CYGARC_HAL_LPC24XX_REG_WDTC, 0xFF);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_WD_BASE + 
                     CYGARC_HAL_LPC24XX_REG_WDMOD, 
                     CYGARC_HAL_LPC24XX_REG_WDMOD_WDEN | 
                     CYGARC_HAL_LPC24XX_REG_WDMOD_WDRESET);

    // feed WD with the two magic values
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_WD_BASE + 
                     CYGARC_HAL_LPC24XX_REG_WDFEED, 
                     CYGARC_HAL_LPC24XX_REG_WDFEED_MAGIC1); 
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_WD_BASE + 
                     CYGARC_HAL_LPC24XX_REG_WDFEED, 
                     CYGARC_HAL_LPC24XX_REG_WDFEED_MAGIC2);
    
    while(1)
      continue;
}


//--------------------------------------------------------------------------
// EOF lpc24xx_misc.c
