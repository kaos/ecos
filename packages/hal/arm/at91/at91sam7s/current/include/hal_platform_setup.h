#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006 Free Software
// Foundation, Inc.
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
// Author(s):   gthomas
// Contributors:gthomas, asl, Oliver Munz
// Date:        2009-06-03
// Purpose:     AT91SAM7S platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>

#ifdef CYG_HAL_STARTUP_ROM


#define AT91_FLASH_FMCN_VALUE                           \
  (CYGNUM_HAL_ARM_AT91_CLOCK_SPEED * 15 / 10000000 + 1)
#if defined(CYGNUM_HAL_ARM_AT91_CLOCK_TYPE_EXTCLOCK)
#define AT91_PMC_MOR_VALUE (AT91_PMC_MOR_OSCBYPASS)
#else
#define AT91_PMC_MOR_VALUE                                              \
  (AT91_PMC_MOR_OSCCOUNT(CYGNUM_HAL_ARM_AT91_PMC_MOR_OSCCOUNT) |        \
   AT91_PMC_MOR_MOSCEN)
#endif
#define AT91_PMC_PLLR_VALUE                                             \
  (AT91_PMC_PLLR_DIV(CYGNUM_HAL_ARM_AT91_PLL_DIVIDER) |                 \
   AT91_PMC_PLLR_PLLCOUNT(CYGNUM_HAL_ARM_AT91_PLL_COUNT) |              \
   AT91_PMC_PLLR_MUL(CYGNUM_HAL_ARM_AT91_PLL_MULTIPLIER-1) |            \
   AT91_PMC_PLLR_USBDIV_1)
#define AT91_PMC_MCKR_VALUE                             \
  (AT91_PMC_MCKR_PRES_CLK_2 | AT91_PMC_MCKR_PLL_CLK)

#define AT91_PMC_FLASH_VALUE_SLOW                                       \
  (AT91_MC_FMR_2FWS | (AT91_FLASH_FMCN_VALUE << AT91_MC_FMR_FMCN_SHIFT))
#if CYGNUM_HAL_ARM_AT91_CLOCK_SPEED > 60000000
// When the clock is running faster than 60MHz we need two wait states
#define AT91_PMC_FLASH_VALUE                    \
  AT91_PMC_FLASH_VALUE_SLOW
#else
#if CYGNUM_HAL_ARM_AT91_CLOCK_SPEED > 30000000
// When the clock is running faster than 30MHz we need a wait state
#define AT91_PMC_FLASH_VALUE                                            \
  (AT91_MC_FMR_1FWS | (AT91_FLASH_FMCN_VALUE << AT91_MC_FMR_FMCN_SHIFT))
#else
// We have a slow clock, no extra wait states are needed
#define AT91_PMC_FLASH_VALUE                                            \
  (AT91_MC_FMR_0FWS | (AT91_FLASH_FMCN_VALUE << AT91_MC_FMR_FMCN_SHIFT))
#endif
#endif

// Macro with subroutines
        .macro  _subroutines
        b       subroutines_end


set_flash_from_r1:
        push    {lr}
        ldr     r0,=AT91_MC
        str     r1,[r0,#AT91_MC_FMR]
#if defined(AT91_MC_FMR1)
        str     r1,[r0,#AT91_MC_FMR1]
#endif
        pop     {pc}


wait_for_flag_pcm_r0r1:
        push    {r2, lr}
wait_loop:
        ldr     r2,[r0,#AT91_PMC_SR]
        ands    r2,r1,r2
        beq     wait_loop
        pop     {r2, pc}


subroutines_end:
        .endm

// Macro to start the main clock.
        .macro  _main_clock_init
__main_clock_init__:
        ldr     r0,=AT91_PMC

        ldr     r2,=AT91_PMC_MOR_VALUE // Load our PMC settings in registers
        ldr     r3,=AT91_PMC_PLLR_VALUE
        ldr     r4,=AT91_PMC_MCKR_VALUE

        ldr     r5,[r0,#AT91_PMC_MOR] // Test if the PMC is allready up
        cmp     r2, r5
        bne     set_pcm_registers // Do the init
        ldr     r5,[r0,#AT91_PMC_PLLR]
        cmp     r3, r5
        bne     set_pcm_registers // Do the init
        ldr     r5,[r0,#AT91_PMC_MCKR]
        cmp     r4, r5
        bne     set_pcm_registers // Do the init
        b       pmc_done // All registers are where we want it...

          // We have to set the PMC
set_pcm_registers:
        str     r2,[r0,#AT91_PMC_MOR]
        // Wait for oscilator start timeout
        ldr     r1, =AT91_PMC_SR_MOSCS
        bl      wait_for_flag_pcm_r0r1

        // Set the PLL multiplier and divider. 16 slow clocks go by
        // before the LOCK bit is set. */
        str     r3,[r0,#AT91_PMC_PLLR]
        // Wait for PLL locked indication
        ldr     r1, =AT91_PMC_SR_LOCK
        bl      wait_for_flag_pcm_r0r1

        // Enable the PLL clock and set the prescale to 2 */
        str     r4,[r0,#AT91_PMC_MCKR]
        // Wait for the MCLK ready indication
        ldr     r1, =AT91_PMC_SR_MCKRDY
        bl      wait_for_flag_pcm_r0r1

pmc_done:
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


        .macro  _setup //The "main" of the macros...
        _subroutines
        ldr     sp,.__startup_stack
        ldr     r1,=AT91_PMC_FLASH_VALUE_SLOW // May be we run >60Mhz at the moment
        bl 		set_flash_from_r1
        _main_clock_init
        ldr     r1,=AT91_PMC_FLASH_VALUE
        bl 		set_flash_from_r1
        _remap_flash
        .endm

#define PLATFORM_SETUP1     _setup
#else
#define PLATFORM_SETUP1
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
