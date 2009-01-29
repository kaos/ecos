// #========================================================================
// #
// #    vectors.S
// #
// #    M68K startup and exception handling.
// #
// #========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2004, 2005, 2006, 2008 Free Software Foundation, Inc.
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
//========================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
// 
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>
#include CYGBLD_HAL_TARGET_H
#include <cyg/hal/hal_io.h>
#include <cyg/hal/arch.inc>

// ----------------------------------------------------------------------------
// These are for compatibility with older HALs
#ifdef HAL_M68K_EXCEPTION_VECTORS
# define _HAL_M68K_START_               HAL_M68K_EXCEPTION_VECTORS
# undef HAL_M68K_EXCEPTION_VECTORS        
#endif
#ifdef HAL_M68K_COPY_ROM_DATA_TO_RAM
# define _HAL_M68K_COPY_ROM_DATA_TO_RAM_        1
# undef HAL_M68K_COPY_ROM_DATA_TO_RAM
#endif
                
// ----------------------------------------------------------------------------
// Exception vectors. Typically there are two exception vectors. There is
// one in flash which contains the h/w entry point. This is the section
// .m68k_start. Then there is the run-time set of exception vectors, typically
// held in RAM, which is the section .ram_vectors.  In addition to the pointers
// to the exception and interrupt VSRs that section contains the virtual vector
// table and any data that needs to be shared between RedBoot and the application,
// for example shadow copies of read-only registers.
//
// Typical scenarios include the following:
//        
//   1) _HAL_M68K_START_ code relocates the flash from its boot location of 0x0
//      to somewhere else. _HAL_M68K_PLATFORM_SETUP1_ initializes SDRAM and
//      places it at 0x0. _HAL_M68K_START_ only contains the two initial exception
//      vector slots for the start address and stack, the RAM vectors are fully
//      populated at run-time by the architectural HAL. This is the typical
//      scenario for "large" systems.        
//        
//   2) for "small" systems where usually there is no RedBoot and the application
//      runs directly from flash, all the exception vectors also reside in flash
//      and are statically initialized. _HAL_M68K_START_ contains the full set
//      of vectors.
//
//   3) as a variant of (2) the run-time exception vectors may live in RAM,
//      typically relocated via the VBR register. The vectors will usually be
//      dynamically initialized, but if the application is loaded via gdb then
//      some code may be saved by static initialization.


        .section        ".m68k_start", "ax"
        // This will typically be defined by the platform HAL to get the world
        // in a sane state. It should do one-off initializations like getting
        // the system into a state matching the memory map. If booting from
        // flash it should also contain the initial PC and stack exception
        // vectors.
#ifdef _HAL_M68K_START_
        _HAL_M68K_START_
#endif                

        .section        ".m68k_start.hal_m68k_exception_reset", "ax"
        // The actual entry point, as defined in the linker script. The exact
        // state of the system at this point depends on the configuration and
        // the processor.
        .global hal_m68k_exception_reset
        .type   hal_m68k_exception_reset, function
hal_m68k_exception_reset:

        // Make absolutely sure that the status register is in a
	// sensible state with interrupts disabled, irrespective of
        // the platform.
        mov.w   #0x2700,%sr

        // Platform-specific initialization. This will do things like starting
        // up the SDRAM controller. It may also run variant and processor
        // initialization code, but the platform HAL gets to decide the order.
#ifdef _HAL_M68K_PLATFORM_SETUP1_
        _HAL_M68K_PLATFORM_SETUP1_
#endif

        // All memory should now be running so we can get the C environment
        // set up. Clear BSS before switching to C mode
        .extern _hal_bss_start
        .extern _hal_bss_end
        move.l   # _hal_bss_start,%a0
        move.l   # _hal_bss_end,%a1
1:              
        cmp.l   %a0,%a1
        ble     2f
        clr.l   (%a0)+
        jra     1b
2:

        // If booting from ROM, move .data from ROM to RAM
#ifdef _HAL_M68K_COPY_ROM_DATA_TO_RAM_
        .extern _hal_data_section_start_lma
        .extern _hal_data_section_end_lma
        .extern _hal_data_section_start_vma
        move.l  # _hal_data_section_start_lma, %a0
        move.l  # _hal_data_section_end_lma,   %a1
        move.l  # _hal_data_section_start_vma, %a2
1:              
        cmp.l   %a0,%a1
        ble     2f
        mov.l   (%a0)+,(%a2)+
        jra     1b
2:                      
#endif

        // If there is an FPU, set the default mode
#ifdef CYGINT_HAL_M68K_VARIANT_FPU
        mov.w   # CYGNUM_HAL_M68K_FPU_CR_DEFAULT, %fpcr
#endif

        // If there is on-chip RAM, copy code and data there and clear
        // its BSS.
#ifdef _HAL_M68K_INITIALIZE_IRAM_
        .extern _hal_iram_text_section_start_lma
        .extern _hal_iram_data_section_end_lma
        .extern _hal_iram_text_section_start_vma
        .extern _hal_iram_bss_section_start
        .extern _hal_iram_bss_section_end
        
        move.l  # _hal_iram_text_section_start_lma, %a0
        move.l  # _hal_iram_data_section_end_lma,   %a1
        move.l  # _hal_iram_text_section_start_vma, %a2
1:      
        cmp.l   %a0, %a1
        ble     2f
        mov.l   (%a0)+, (%a2)+
        jra     1b
2:      
        move.l  # _hal_iram_bss_section_start, %a0
        move.l  # _hal_iram_bss_section_end,   %a1
3:      
        cmp.l   %a0, %a1
        ble     4f
        clr.l   (%a0)+
        jra     3b
4:      
#endif                

        // Now we just need a valid stack and we can switch to C
        // for the remaining initialization. Clearing the frame
        // pointer may make life easier for gdb.
        mov.l   _HAL_M68K_STARTUP_STACK_,%sp
        suba.l  %a6, %a6
        
        .extern hal_m68k_c_startup
        jmp     hal_m68k_c_startup
                
// The exception vector table. Usually this will be held in RAM
// at location 0x0, although it may get moved around via e.g.
// the %vbr register. In addition to the exception vectors the
// virtual vector table is held here, as is any data that needs
// to be shared between RedBoot and eCos.
//
// Some targets in some configurations may define their own version
// of this, if for example the exception vector is held in ROM.
#if defined(_HAL_M68K_RAM_VECTORS_)
        
        _HAL_M68K_RAM_VECTORS_
                
#elif !defined(_HAL_M68K_RAM_VECTORS_DEFINED_)
        .section        ".ram_vectors", "aw", @nobits

# ifndef _HAL_M68K_SUPPRESS_RAM_VECTORS_VSR_TABLE_        
        // Start with the interrupt/exception vectors.
        .global hal_m68k_vsr_table
hal_m68k_vsr_table:  
        .rept   HAL_M68K_VSR_COUNT
        .long   0
        .endr
# endif
                
        // Next the virtual vector table. Space for this is usually allocated,
        // even if virtual vectors are not supported. That avoids confusion
        // if RedBoot and the application are configured differently. If a
        // platform will never support RedBoot, e.g. because of lack of memory,
        // then it can suppress the virtual vectors.
        //
        // The size of the table comes from <cyg/hal/hal_if.h>,
        // CYGNUM_CALL_IF_TABLE_SIZE, but that header cannot easily be included
        // in assembler.
# ifndef _HAL_M68K_SUPPRESS_VIRTUAL_VECTOR_
        .global hal_virtual_vector_table
hal_virtual_vector_table:       
        .rept   64
        .long   0
        .endr        
# endif
        
        // Allow the variant, processor and platform HALs to store additional
        // information in the global vectors section. Typically this is used for
        // data which needs to be shared between RedBoot and the application,
        // e.g. shadow copies of write-only hardware registers.
# ifdef _HAL_M68K_VARIANT_RAM_VECTORS_
        _HAL_M68K_VARIANT_RAM_VECTORS_
# endif
# ifdef _HAL_M68K_PROCESSOR_RAM_VECTORS_
        _HAL_M68K_PROCESSOR_RAM_VECTORS_
# endif
# ifdef _HAL_M68K_PLATFORM_RAM_VECTORS_
        _HAL_M68K_PLATFORM_RAM_VECTORS_
# endif
#endif  // _HAL_M68K_RAM_VECTORS_DEFINED_

                
// ----------------------------------------------------------------------------
// end of vectors.S
