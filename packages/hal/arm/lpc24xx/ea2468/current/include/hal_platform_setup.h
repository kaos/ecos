#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H
/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2008-06-05
// Purpose:      EA LPC2468 OEM platform specific support routines
// Description:
// Usage:        #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/
#include <pkgconf/system.h>
#include <cyg/hal/var_io.h>


//
// The minimum initialisation code - we simply setup a valid C stack in 
// internal SRAM and do any further initialisation in C code
//
#if defined(CYG_HAL_STARTUP_ROM)
.macro _setup
    //
    // While setting the stack pointer please note that the 
    // Flash programming routines use a
    // section of the on-chip SRAM. In-System Programming (ISP) uses 
    // the top 256 bytes and In-Application Programming (IAP) uses the 
    // top 128 bytes of the on-chip SRAM. The application stack should 
    // not overlap this area.
    //
    ldr r2,=0x4000ffff // ram end
    sub sp,r2,#0xff
        
    //
    // now map the vector table to internal flash - normally this should be
    // the default value after boot - but we go the save way here and force
    // the mapping to internal flash (the value for 
    // CYGARC_HAL_LPC24XX_REG_MEMMAP is 1)
    //
    ldr r0,=CYGARC_HAL_LPC24XX_REG_SCB_BASE
    mov r1,#1
    str r1, [r0,#CYGARC_HAL_LPC24XX_REG_MEMMAP]	
    
    //    
    // Now its is save to copy the first 64 bytes of flash to RAM
    //
    mov r0,#0					
    mov r1,#0x40000000
    mov r2,#0x40
1:
    ldr r3,[r0,#4]!
    str r3,[r1,#4]!
    cmps r0,r2
    bne 1b
        
    // 
    // Now we can map the vector table to internal SRAM	because the SRAM no
    // contains a copy of the vector table from flash (the value for 
    // CYGARC_HAL_LPC24XX_REG_MEMMAP is 2 = SRAM)
    //
    ldr r0,=CYGARC_HAL_LPC24XX_REG_SCB_BASE
    mov r1,#2                                   // User RAM Mode. Interrupt 
                                     // vectors are re-mapped to Static RAM.
    str r1, [r0,#CYGARC_HAL_LPC24XX_REG_MEMMAP]
    
    //
    // now we have a valid stack and we can jump into the beautiful
    // world of C and do any further initialisation in C code
    //
    bl hal_plf_startup
.endm
#define CYGSEM_HAL_ROM_RESET_USES_JUMP
#else
.macro  _setup
.endm    
#endif

#define PLATFORM_SETUP1 _setup

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
