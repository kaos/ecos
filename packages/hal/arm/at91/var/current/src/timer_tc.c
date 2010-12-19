/*==========================================================================
//
//      timer_tc.c
//
//      HAL timer code using the Timer Counter
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2010 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov, nickg, tkoeller, jld
// Date:         2001-07-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_intr.h>           // necessary?

#ifdef CYGFUN_HAL_ARM_AT91_PROFILE_TIMER
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED
#include <cyg/profile/profile.h>        // __profile_hit()
#endif

// -------------------------------------------------------------------------
// Clock support

static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
    CYG_ADDRESS timer = AT91_TC+AT91_TC_TC0;

    CYG_ASSERT(period < 0x10000, "Invalid clock period");

    // Disable counter
    HAL_WRITE_UINT32(timer+AT91_TC_CCR, AT91_TC_CCR_CLKDIS);

    // Set registers
    HAL_WRITE_UINT32(timer+AT91_TC_CMR, AT91_TC_CMR_CPCTRG |        // Reset counter on CPC
                                        AT91_TC_CMR_CLKS_MCK32);    // 1 MHz
    HAL_WRITE_UINT32(timer+AT91_TC_RC, period);

    // Start timer
    HAL_WRITE_UINT32(timer+AT91_TC_CCR, AT91_TC_CCR_TRIG | AT91_TC_CCR_CLKEN);

    // Enable timer 0 interrupt    
    HAL_WRITE_UINT32(timer+AT91_TC_IER, AT91_TC_IER_CPC);
}

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    CYG_ADDRESS timer = AT91_TC+AT91_TC_TC0;
    cyg_uint32 sr;

    CYG_ASSERT(period < 0x10000, "Invalid clock period");

    HAL_READ_UINT32(timer+AT91_TC_SR, sr);  // Clear interrupt

    if (period != _period) {
        hal_clock_initialize(period);
    }
    _period = period;

}

void hal_clock_read(cyg_uint32 *pvalue)
{
    CYG_ADDRESS timer = AT91_TC+AT91_TC_TC0;
    cyg_uint32 val;

    HAL_READ_UINT32(timer+AT91_TC_CV, val);
    *pvalue = val;
}

// -------------------------------------------------------------------------
//
// Delay for some number of micro-seconds
//   Use timer #2 in MCLOCK/32 mode.
//
void hal_delay_us(cyg_int32 usecs)
{
    cyg_uint32 stat;
    cyg_uint64 ticks;
#if defined(CYGHWR_HAL_ARM_AT91_JTST)
    // TC2 is reserved for AD/DA. Use TC1 instead. 
    CYG_ADDRESS timer = AT91_TC+AT91_TC_TC1;
#else
    CYG_ADDRESS timer = AT91_TC+AT91_TC_TC2;
#endif
    // Calculate how many timer ticks the required number of
    // microseconds equate to. We do this calculation in 64 bit
    // arithmetic to avoid overflow.
    ticks = (((cyg_uint64)usecs) * 
             ((cyg_uint64)CYGNUM_HAL_ARM_AT91_CLOCK_SPEED))/32000000LL;
    
    //    CYG_ASSERT(ticks < (1 << 16), "Timer overflow");
    
    if (ticks > (1 << 16))
      ticks = (1 << 16) - 1;
    
    if (ticks == 0)
      return;
    
    // Disable counter
    HAL_WRITE_UINT32(timer+AT91_TC_CCR, AT91_TC_CCR_CLKDIS);

    // Set registers
    HAL_WRITE_UINT32(timer+AT91_TC_CMR, AT91_TC_CMR_CLKS_MCK32);  // 1MHz
    HAL_WRITE_UINT32(timer+AT91_TC_RA, 0);
    HAL_WRITE_UINT32(timer+AT91_TC_RC, ticks);

    // Clear status flags
    HAL_READ_UINT32(timer+AT91_TC_SR, stat);
    
    // Start timer
    HAL_WRITE_UINT32(timer+AT91_TC_CCR, AT91_TC_CCR_TRIG | AT91_TC_CCR_CLKEN);
    
    // Wait for the compare
    do {
      HAL_READ_UINT32(timer+AT91_TC_SR, stat);
    } while ((stat & AT91_TC_SR_CPC) == 0);
}

#ifdef CYGFUN_HAL_ARM_AT91_PROFILE_TIMER

// Use TC1 for profiling
#define AT91_TC_PROFILE AT91_TC_TC1
#define HAL_INTERRUPT_PROFILE CYGNUM_HAL_INTERRUPT_TIMER1

// Profiling timer ISR
static cyg_uint32 profile_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data, HAL_SavedRegisters *regs)
{
    cyg_uint32 status;

    HAL_READ_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_SR, status); // Clear interrupt
    HAL_INTERRUPT_ACKNOWLEDGE(HAL_INTERRUPT_PROFILE);
    __profile_hit(regs->pc);
    return CYG_ISR_HANDLED;
}

// Profiling timer setup
int hal_enable_profile_timer(int resolution)
{
    cyg_uint32 period;

    // Calculate how many timer ticks the requested resolution in
    // microseconds equates to. We do this calculation in 64 bit
    // arithmetic to avoid overflow.
    period = (cyg_uint32)((((cyg_uint64)resolution) *
        ((cyg_uint64)CYGNUM_HAL_ARM_AT91_CLOCK_SPEED))/32000000LL);

    CYG_ASSERT(period < 0x10000, "Invalid profile timer resolution"); // 16 bits only

    // Attach ISR
    HAL_INTERRUPT_ATTACH(HAL_INTERRUPT_PROFILE, &profile_isr, 0x1111, 0);
    HAL_INTERRUPT_UNMASK(HAL_INTERRUPT_PROFILE);

    // Disable counter
    HAL_WRITE_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_CCR, AT91_TC_CCR_CLKDIS);

    // Set registers
    HAL_WRITE_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_CMR, AT91_TC_CMR_CPCTRG | // Reset counter on CPC
                                                          AT91_TC_CMR_CLKS_MCK32); // Use MCLK/32
    HAL_WRITE_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_RC, period);

    // Start timer
    HAL_WRITE_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_CCR, AT91_TC_CCR_TRIG | AT91_TC_CCR_CLKEN);

    // Enable timer interrupt
    HAL_WRITE_UINT32(AT91_TC+AT91_TC_PROFILE+AT91_TC_IER, AT91_TC_IER_CPC);

    return resolution;
}

#endif // CYGFUN_HAL_ARM_AT91_PROFILE_TIMER

// timer_tc.c
