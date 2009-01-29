/*=============================================================================
//
//      mcfxxxx.c
//
//      ColdFire MCFxxxx miscellaneous support
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):   bartv
// Date:        2008-01-14
//
//####DESCRIPTIONEND####
//===========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_mcfxxxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_misc.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/drv_api.h>

#ifdef CYGINT_HAL_M68K_MCFxxxx_SOFTWARE_PROFILE_TIMER
// ----------------------------------------------------------------------------
// Profiling support. This requires a hardware timer set to interrupt
// at a rate determined by application code. The interrupt handler
// should call __profile_hit() with a single argument, the interrupted
// PC. One of the PIT timers, determined by the processor or platform
// HAL is used to implement the profiling timer.
//
// Usually this would involve installing an ISR. However there is no
// easy way for an ISR to get hold of the interrupted PC. In some
// configurations the save state will be stored in hal_saved_interrupt_state,
// but not always. It might be possible to extract the PC from the stack,
// but that gets messy if a separate interrupt stack is used and would be
// vulnerable to changes in the architectural VSR. Instead a custom VSR
// is installed.

extern void hal_mcfxxxx_profile_vsr(void);

# include <cyg/profile/profile.h>

int
hal_enable_profile_timer(int resolution)
{
    cyg_uint16  ticks;
    
    // Make sure the clock is not running but is otherwise initialized.
    HAL_WRITE_UINT16(HAL_MCFxxxx_PROFILE_TIMER_BASE + HAL_MCFxxxx_PITx_PCSR,
                     HAL_MCFxxxx_PITx_PCSR_PRE_64 | HAL_MCFxxxx_PITx_PCSR_OVW |
                     HAL_MCFxxxx_PITx_PCSR_PIE    | HAL_MCFxxxx_PITx_PCSR_PIF |
                     HAL_MCFxxxx_PITx_PCSR_RLD);
    
    // The resolution is a time interval in microseconds. The actual
    // cpu clock frequency is determined by the platform. This is divided
    // by 64, which means it may not be possible to get the exact resolution.
    ticks   = ((resolution * CYGHWR_HAL_SYSTEM_CLOCK_MHZ) / 64) - 1;
    HAL_WRITE_UINT16(HAL_MCFxxxx_PROFILE_TIMER_BASE + HAL_MCFxxxx_PITx_PMR, ticks);
    
    // Convert back to microseconds. This may actually increase rounding
    // errors for some arguments and platforms, but the result should
    // still be accurate enough for practical purposes.
    resolution  = ((ticks + 1) * 64) / CYGHWR_HAL_SYSTEM_CLOCK_MHZ;
    
    // Set up the interrupt handler. This is usually a high-priority
    // interrupt so that we can get profiling information for other
    // interrupt sources.
#ifdef HAL_VSR_SET    
    HAL_VSR_SET(HAL_MCFxxxx_PROFILE_TIMER_VECTOR, &hal_mcfxxxx_profile_vsr, (cyg_uint32)0);
#endif    
    HAL_INTERRUPT_SET_LEVEL(HAL_MCFxxxx_PROFILE_TIMER_ISR, CYGNUM_HAL_M68K_MCFxxxx_SOFTWARE_PROFILE_TIMER_ISR_PRIORITY);
    HAL_INTERRUPT_UNMASK(HAL_MCFxxxx_PROFILE_TIMER_ISR);

    // Now start the timer running.
    HAL_WRITE_UINT16(HAL_MCFxxxx_PROFILE_TIMER_BASE + HAL_MCFxxxx_PITx_PCSR,
                     HAL_MCFxxxx_PITx_PCSR_PRE_64 | HAL_MCFxxxx_PITx_PCSR_OVW |
                     HAL_MCFxxxx_PITx_PCSR_PIE    | HAL_MCFxxxx_PITx_PCSR_PIF |
                     HAL_MCFxxxx_PITx_PCSR_RLD    | HAL_MCFxxxx_PITx_PCSR_EN);

    // Return the actual resolution.
    return resolution;
}

#endif  // Profiling timer

/* End of mcfxxxx.c */
