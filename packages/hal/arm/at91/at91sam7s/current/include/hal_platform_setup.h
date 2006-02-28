#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd
// Copy

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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:gthomas, asl
// Date:        2006-02-18
// Purpose:     AT91SAM7S platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>
        
// Macro to initialise the Memory Controller
        .macro _flash_init
__flash_init__:
#if CYGNUM_HAL_ARM_AT91_CLOCK_SPEED > 30000000
        // When the clock is running faster than 30MHz we need a wait state
        ldr     r0,=AT91_MC
        ldr     r1,=(AT91_MC_FMR_1FWS)
        str     r1,[r0,#AT91_MC_FMR]
#endif
#if CYGNUM_HAL_ARM_AT91_CLOCK_SPEED > 60000000
        ldr     r1,=(AT91_MC_FMR_2FWS)
        str     r1,[r0,#AT91_MC_FMR]
#endif
        .endm

// Macro to start the main clock.
        .macro  _main_clock_init
__main_clock_init__:
        ldr     r0,=AT91_PMC
          // Swap to the slow clock, just to be sure.
        ldr     r1,=(AT91_PMC_MCKR_PRES_CLK|AT91_PMC_MCKR_SLOW_CLK)
        str     r1,[r0,#AT91_PMC_MCKR]
	// startup time
        ldr     r1,=(AT91_PMC_MOR_OSCCOUNT(6)|AT91_PMC_MOR_MOSCEN)
        str     r1,[r0,#AT91_PMC_MOR]

        // Wait for oscilator start timeout
wait_pmc_sr_1:  
        ldr     r1,[r0,#AT91_PMC_SR]
        ands    r1,r1,#AT91_PMC_SR_MOSCS
        beq     wait_pmc_sr_1

        // Set the PLL multiplier and divider. 16 slow clocks go by
	// before the LOCK bit is set. */
        ldr     r1,=((AT91_PMC_PLLR_DIV(CYGNUM_HAL_ARM_AT91_PLL_DIVIDER))|(AT91_PMC_PLLR_PLLCOUNT(16))|(AT91_PMC_PLLR_MUL(CYGNUM_HAL_ARM_AT91_PLL_MULTIPLIER-1)))
        str     r1,[r0,#AT91_PMC_PLLR]

        // Wait for PLL locked indication
wait_pmc_sr_2:
        ldr     r1,[r0,#AT91_PMC_SR]
        ands    r1,r1,#AT91_PMC_SR_LOCK
        beq     wait_pmc_sr_2

        // Enable the PLL clock and set the prescale to 2 */
        ldr     r1,=(AT91_PMC_MCKR_PRES_CLK_2|AT91_PMC_MCKR_PLL_CLK)
        str     r1,[r0,#AT91_PMC_MCKR]

        // Wait for the MCLK ready indication
wait_pmc_sr_3:
        ldr     r1,[r0,#AT91_PMC_SR]
        ands    r1,r1,#AT91_PMC_SR_MCKRDY
        beq     wait_pmc_sr_3
        .endm

// Remap the flash from address 0x0 and place RAM there instead.
        .macro  _remap_flash
__remap_flash:
        ldr     r0,=0x000004 // Use the underfined instruction exception
        ldr     r1,=0x200004
        ldr     r2,[r0]      // Save away copies so we can restore them
        ldr     r3,[r1]
        ldr     r4,=0xffffff
        eor     r4,r3,r4     // XOR the contents of 0x20004 
        str     r4,[r1]      // and write it
        ldr     r5,[r0]      // Read from low memory
        cmp     r5,r4
        beq     remap_done
        ldr     r0,=AT91_MC  // Need to do a remap
        ldr     r5,=1
        str     r5,[r0,#AT91_MC_RCR]
remap_done:
        str     r3,[r1]      // restore the value we changed
        .endm
        
#if defined(CYG_HAL_STARTUP_ROM)
        .macro  _setup
        _flash_init
        _main_clock_init
        _remap_flash
        .endm

#define PLATFORM_SETUP1     _setup
#else
#define PLATFORM_SETUP1
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
