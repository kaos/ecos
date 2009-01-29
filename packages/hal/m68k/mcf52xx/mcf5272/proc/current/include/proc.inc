##=============================================================================
##
##      proc.inc
##
##      mcf5272 processor
##
##=============================================================================
## ####ECOSGPLCOPYRIGHTBEGIN####                                            
## -------------------------------------------                              
## This file is part of eCos, the Embedded Configurable Operating System.   
## Copyright (C) 2003, 2004, 2006, 2008 Free Software Foundation, Inc.      
##
## eCos is free software; you can redistribute it and/or modify it under    
## the terms of the GNU General Public License as published by the Free     
## Software Foundation; either version 2 or (at your option) any later      
## version.                                                                 
##
## eCos is distributed in the hope that it will be useful, but WITHOUT      
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
## for more details.                                                        
##
## You should have received a copy of the GNU General Public License        
## along with eCos; if not, write to the Free Software Foundation, Inc.,    
## 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
##
## As a special exception, if other files instantiate templates or use      
## macros or inline functions from this file, or you compile this file      
## and link it with other works to produce a work based on this file,       
## this file does not by itself cause the resulting work to be covered by   
## the GNU General Public License. However the source code for this file    
## must still be made available in accordance with section (3) of the GNU   
## General Public License v2.                                               
##
## This exception does not invalidate any other reasons why a work based    
## on this file might be covered by the GNU General Public License.         
## -------------------------------------------                              
## ####ECOSGPLCOPYRIGHTEND####                                              
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   bartv
## Date:        2003-06-04
######DESCRIPTIONEND####
##=============================================================================

#include <pkgconf/hal_m68k_mcf5272.h>

// Vectors support.
// The 5272's interrupt vectors go up to slot 96: the standard 64 plus another
// 32 for the on-chip peripherals.
#define HAL_M68K_VSR_COUNT              96

// The chip has 4K on-chip RAM so enable the relevant init code in vectors.S
#define _HAL_M68K_INITIALIZE_IRAM_      1

// Fixed vectors data. The cacr cache control register is write-only so
// a shadow copy is needed. This has to be shared between RedBoot and the
// application, otherwise confusing things can happen when both manipulate
// the cache e.g. during debugging. Shadow copies are also needed to
// implement interrupt priorities. Some space is allocated for future
// expansion.
        .macro  _hal_mcf5272_ram_vectors_
        .global hal_mcf5272_cacr
hal_mcf5272_cacr:
        .long   0
        .global hal_mcf5272_icr_pri_mirror
hal_mcf5272_icr_pri_mirror:
        .long   0, 0, 0, 0
        .rept 11
        .long 0
        .endr
        .endm

#define _HAL_M68K_PROCESSOR_RAM_VECTORS_        _hal_mcf5272_ram_vectors_

// ----------------------------------------------------------------------------
// Utility macros for initialization. It is assumed that the platform HAL
// has provided the magic numbers.

// Remap. Typically on an MCF5272 board flash memory starts of at location
// 0, everything else is still disabled. The flash needs to be moved to
// its final location, usually in high memory, while we are still running
// out of flash. The way to do this is to set the mask in the cs option
// register to 0, causing the flash to be mirrored throughout the address
// space, then branch to the actual flash location, and then the rest of
// the memory map can be sorted out. To access CSOR0, first we need to map
// the MBAR into memory. MBAR has priority over chip selects so it will
// remain visible even if the flash is mirrored.

        .macro  _hal_mcf5272_remap_flash_
        move.l  # _HAL_MCF5272_MBAR_VALUE_, %d0
        movec.l %d0, %mbar
        move.l  # HAL_MCFxxxx_MBAR, %a5
        move.l  # (_HAL_MCF5272_CSOR0_VALUE_ & ~HAL_MCF5272_CSOR_BA_MASK), %d0
        move.l  %d0, HAL_MCF5272_CSOR0 (%a5)

        // dBUG inserts a nop here and another in the branch location.
        // Presumably this avoids pipeline problems if the memory map
        // changes under the pipeline's feet.
        nop

        // Now we can branch to the real location in the flash. The linker
        // script is set up so that this code is mapped to the right place.
        jmp     _hal_mcf5272_flash_

        // Put the next code in a separate section to stop the assembler turning
        // the above jump into a PC-relative one
        .section ".m68k_start.0", "ax"

_hal_mcf5272_flash_:
        // Another nop as per dBUG
        nop
        // We are now running out of flash. CSOR0 and MBAR have been changed,
        // everything else is still at reset values.
        .endm

// Initialize all the system registers. Typically this is done unconditionally
// on the off-chance that RedBoot and the application need different values.
        .macro  _hal_mcf5272_init_sysregs_
        move.l  # (HAL_MCFxxxx_CACR_CINV), %d0
        movec.l %d0, %cacr
        move.l  # (CYGNUM_HAL_M68K_MCF5272_ACR0), %d0
        movec.l %d0, %acr0
        move.l  # (CYGNUM_HAL_M68K_MCF5272_ACR1), %d0
        movec.l %d0, %acr1
        move.l  # (CYGNUM_HAL_M68K_MCF5272_ROMBAR), %d0
        movec.l %d0, %rombar
        move.l  # (CYGNUM_HAL_M68K_MCF5272_RAMBAR), %d0
        movec.l %d0, %rambar0
        move.l  # (CYGNUM_HAL_M68K_MCF5272_CACR), %d0
        movec.l %d0, %cacr
        .endm

// Initialize memory, including starting the SDRAM controller
        .macro  _hal_mcf5272_init_memory_
        move.w  # CYGNUM_HAL_M68K_MCF5272_SCR, %d0
        move.w  %d0,                            HAL_MCF5272_SCR (%a5)
        move.w  # 0xFFFF,                       %d0
        move.w  %d0,                            HAL_MCF5272_SPR (%a5)

        // chip selects and SDRAM. CS0 should always be connected to
        // a boot flash. The BW bits are set from external pins
        // QSPI_CS0/CSPI_CLK and are preserved here. Arguably that
        // should not be necessary but it allows for platforms where
        // the hardware has gone through several revisions and it
        // is undesirable to build different images for different
        // revisions. The overhead is small, just two instructions.
        move.l  HAL_MCF5272_CSBR0 (%a5),        %d0
        andi.l  # HAL_MCF5272_CSBR_BW_MASK,     %d0
        ori.l   # (_HAL_MCF5272_CSBR0_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR0 (%a5)
        move.l  # (_HAL_MCF5272_CSOR0_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR0 (%a5)

        // Chip selects 1-6 may or may not be connected. If not just leave
        // them to their default disabled state.
        .ifdef _HAL_MCF5272_CSBR1_VALUE_
        move.l  # (_HAL_MCF5272_CSBR1_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR1 (%a5)
        move.l  # (_HAL_MCF5272_CSOR1_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR1 (%a5)
        .endif
        .ifdef _HAL_MCF5272_CSBR2_VALUE_
        move.l  # (_HAL_MCF5272_CSBR2_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR2 (%a5)
        move.l  # (_HAL_MCF5272_CSOR2_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR2 (%a5)
        .endif
        .ifdef _HAL_MCF5272_CSBR3_VALUE_
        move.l  # (_HAL_MCF5272_CSBR3_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR3 (%a5)
        move.l  # (_HAL_MCF5272_CSOR3_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR3 (%a5)
        .endif
        .ifdef _HAL_MCF5272_CSBR4_VALUE_
        move.l  # (_HAL_MCF5272_CSBR4_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR4 (%a5)
        move.l  # (_HAL_MCF5272_CSOR4_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR4 (%a5)
        .endif
        .ifdef _HAL_MCF5272_CSBR5_VALUE_
        move.l  # (_HAL_MCF5272_CSBR5_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR5 (%a5)
        move.l  # (_HAL_MCF5272_CSOR5_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR5 (%a5)
        .endif
        .ifdef _HAL_MCF5272_CSBR6_VALUE_
        move.l  # (_HAL_MCF5272_CSBR6_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR6 (%a5)
        move.l  # (_HAL_MCF5272_CSOR6_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR6 (%a5)
        .endif

        // CS7 will go the SDRAM.
        move.l  # (_HAL_MCF5272_CSBR7_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSBR7 (%a5)
        move.l  # (_HAL_MCF5272_CSOR7_VALUE_),  %d0
        move.l  %d0,                            HAL_MCF5272_CSOR7 (%a5)

        // The SDRAM should now be ready for its first commands.
        // The timer register needs to be done first, then the
        // control register, and finally a dummy write to memory.
        // That dummy write causes the SDRAM controller to take
        // the necessary actions, and the memory should be usable
        // a few cycles later.
        move.l  # (_HAL_MCF5272_SDTR_VALUE_),   %d0
        move.l  %d0,                            HAL_MCF5272_SDTR (%a5)
        move.l  # (_HAL_MCF5272_SDCR_VALUE_),   %d0
        move.l  %d0,                            HAL_MCF5272_SDCR (%a5)
        clr.l   0
        .endm

// ----------------------------------------------------------------------------
// Now that the utility macros are defined include the platform HAL.
#include <cyg/hal/plf.inc>

// And if the platform HAL requests it, provide the standard initialization
// code.
#if defined(_HAL_MCF5272_STANDARD_INIT_ROM_)
	.macro	_hal_m68k_mcf5272_start_
	// Initial stack and program counter
	.long	0
	.long	8
	_hal_mcf5272_remap_flash_
	.endm

	.macro	_hal_m68k_mcf5272_setup1_
	_hal_mcf5272_init_sysregs_
	_hal_mcf5272_init_memory_
	.endm

# define _HAL_M68K_START_		_hal_m68k_mcf5272_start_
# define _HAL_M68K_PLATFORM_SETUP1_	_hal_m68k_mcf5272_setup1_
# define _HAL_M68K_COPY_ROM_DATA_TO_RAM_ 1

#elif defined(_HAL_MCF5272_STANDARD_INIT_RAM_)
	.macro	_hal_m68k_mcf5272_start_
	.endm

	.macro	_hal_m68k_mcf5272_setup1_
	// Always initialize the remaining system registers, in case the
        // application needs different values from RedBoot for some reason.
	_hal_mcf5272_init_sysregs_
	.endm

# define _HAL_M68K_START_		_hal_m68k_mcf5272_start_
# define _HAL_M68K_PLATFORM_SETUP1_	_hal_m68k_mcf5272_setup1_
#endif

// ----------------------------------------------------------------------------
// end of proc.inc
