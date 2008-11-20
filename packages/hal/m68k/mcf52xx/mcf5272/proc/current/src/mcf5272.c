//==========================================================================
//
//      mcf5272.c
//
//      MCF5272 processor support functions.
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2005,2006,2008 Free Software Foundation, Inc.
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
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>
#include <pkgconf/hal_m68k_mcfxxxx.h>
#include <pkgconf/hal_m68k_mcf5272.h>
#include CYGBLD_HAL_PLATFORM_H
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/cyg_type.h>

#ifdef CYGHWR_HAL_M68K_MCF5272_GPIO
# include "gpio.c"
#endif

// ----------------------------------------------------------------------------
// Processor initialization.
void
hal_m68k_mcf5272_init(void)
{
    int i;
    
    hal_mcf5272_cacr    = CYGNUM_HAL_M68K_MCF5272_CACR;

#ifdef CYGNUM_HAL_M68K_MCF5272_SCR
    // SCR. This is configurable since applications may want different
    // bus arbitration schemes. The #ifdef is for backwards compatibility.
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_SCR, CYGNUM_HAL_M68K_MCF5272_SCR);
#endif

    // Set up the GPIO pins if the platform HAL defines the appropriate settings.
#ifdef CYGHWR_HAL_M68K_MCF5272_GPIO
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PADAT, A_DAT);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PADDR, A_DDR);
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PACNT, A_PAR);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PBDAT, B_DAT);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PBDDR, B_DDR);
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PBCNT, B_PAR);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PCDAT, C_DAT);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_PCDDR, C_DDR);
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PDCNT, D_PAR);
#endif    
    
    // Enable all exceptions.
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_SPR, 0x00FF);

    // If we should enter low power mode when idling, set the PMR here.
    // Application programs may tweak it as required.
#if   defined(CYGIMP_HAL_M68K_MCF5272_IDLE_run)
    // Leave PMR to its default
#elif defined(CYGIMP_HAL_M68K_MCF5272_IDLE_sleep)
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PMR, HAL_MCF5272_PMR_SLPEN);
#elif defined(CYGIMP_HAL_M68K_MCF5272_IDLE_stop)
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PMR, HAL_MCF5272_PMR_MOS);
#else
# error Unknown low power mode    
#endif    
    
    // Make sure that the interrupt controller matches the proc_intr.h
    // vector definitions.
    HAL_WRITE_UINT8( HAL_MCFxxxx_MBAR + HAL_MCF5272_PIVR, HAL_MCF5272_INT_VEC_BASE);
    
    // The interrupt controller. All interrupts are set to priority 0,
    // disabled until configured. This cannot be done when running on
    // top of RedBoot, it would interfere with any devices in use for
    // communication e.g. ethernet.
#ifndef CYGSEM_HAL_USE_ROM_MONITOR    
    for (i = 0; i < 4; i++) {
        hal_mcf5272_icr_pri_mirror[i]   = 0x0;
    }
#endif    

    // Exception vectors. First the exceptions themselves. If running
    // with a ROM monitor then leave these alone, otherwise claim the
    // lot.
#ifndef CYGSEM_HAL_USE_ROM_MONITOR
    HAL_VSR_SET(CYGNUM_HAL_VECTOR_SSP, (void*) 0, (void*) 0);
    HAL_VSR_SET(CYGNUM_HAL_VECTOR_RESET, &hal_m68k_exception_reset, (void*) 0);
    
    for (i = CYGNUM_HAL_VECTOR_BUSERR; i < CYGNUM_HAL_VECTOR_SPURINT; i++) {
        HAL_VSR_SET(i, &hal_m68k_exception_vsr, (void*) 0);
    }
    for (i = CYGNUM_HAL_VECTOR_TRAP0; i <= CYGNUM_HAL_VECTOR_TRAPLAST; i++) {
        HAL_VSR_SET(i, &hal_m68k_exception_vsr, (void*) 0);
    }
#endif
    
    // All the external interrupts can be handled by the same VSR, the default
    // architectural one.
    for (i = HAL_MCF5272_INT_VEC_BASE; i < CYGNUM_HAL_VECTOR_RES1; i++) {
        HAL_VSR_SET(i, &hal_m68k_interrupt_vsr, (void*) 0);
    }
}

// ----------------------------------------------------------------------------
// Profiling support. This requires a hardware timer set to interrupt at
// a rate determined by application code. The interrupt handler should
// call __profile_hit() with a single argument, the interrupted PC.
// Timer 2 is used to implement the profiling timer. Timer 3 is already
// allocated for the system clock. Timers 0 and 1 have some extra
// functionality so these are left for application code.
//
// Usually this would involve installing an ISR. However there is no
// easy way for an ISR to get hold of the interrupted PC. In some
// configurations the save state will be stored in hal_saved_interrupt_state,
// but not always. It might be possible to extract the PC from the stack,
// but that gets messy if a separate interrupt stack is used and would be
// vulnerable to changes in the architectural VSR. Instead a custom VSR
// is installed.

#ifdef CYGFUN_HAL_M68K_MCF5272_PROFILE_TIMER

extern void hal_mcf5272_profile_vsr(void);

# include <cyg/profile/profile.h>

int
hal_enable_profile_timer(int resolution)
{
    // Reset the timer
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER2_BASE + HAL_MCF5272_TIMER_TMR, 0);

    // The resolution is a time interval in microseconds. The clock is
    // set to tick in microseconds by dividing by the system clock
    // value.
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER2_BASE + HAL_MCF5272_TIMER_TRR,
                     resolution - 1);

    // Timer captures are of no interest, but reset them just in case.
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER2_BASE + HAL_MCF5272_TIMER_TCN, 0);
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER2_BASE + HAL_MCF5272_TIMER_TER,
                     HAL_MCF5272_TIMER_TER_REF | HAL_MCF5272_TIMER_TER_CAP);

    // Set up the interrupt handler. This is a high-priority interrupt
    // so that we can get profiling information for other interrupt
    // sources.
    HAL_VSR_SET(CYGNUM_HAL_VECTOR_TMR2, &hal_mcf5272_profile_vsr, (cyg_uint32)0);
    HAL_INTERRUPT_SET_LEVEL(CYGNUM_HAL_ISR_TMR2, 6);
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_ISR_TMR2);
    
    // Now start the timer running.
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER2_BASE + HAL_MCF5272_TIMER_TMR,
                     ((CYGHWR_HAL_SYSTEM_CLOCK_MHZ - 1) << HAL_MCF5272_TIMER_TMR_PS_SHIFT) |
                     HAL_MCF5272_TIMER_TMR_ORI | HAL_MCF5272_TIMER_TMR_FRR |
                     HAL_MCF5272_TIMER_TMR_CLK_MASTER | HAL_MCF5272_TIMER_TMR_RST);

    // The desired resolution is always supported
    return resolution;
}
#endif
