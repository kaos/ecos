#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:gthomas
// Date:        2001-07-12
// Purpose:     AT91/EB40 platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/plf_io.h>

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

        .macro  _setup
        ldr     r10,=_InitMemory        // Initialize memory controller
        movs    r0,pc,lsr #20           // If ROM startup, PC < 0x100000
        moveq   r10,r10,lsl #12         //   mask address to low 20 bits
        moveq   r10,r10,lsr #12
        ldmia   r10!,{r0-r9,r11-r12}    // Table of initialization constants
#if defined(CYG_HAL_STARTUP_ROMRAM)
        ldr     r10,=0x0000FFFF
        and     r12,r12,r10
        ldr     r10,=0x01010000
        orr     r12,r12,r10
#endif        
        stmia   r11!,{r0-r9}            // Write to controller
        mov     pc,r12                  // Change address space, break pipeline
_InitMemory:            
        .long   0x01002535  // 0x01000000, 16MB,  2 cycles added after transfer, 16-bit, 6 wait states
        .long   0x02002121  // 0x02000000, 16MB,  0 cycles added after transfer, 16-bit, 1 wait state
        .long   0x20000000  // unused
        .long   0x30000000  // unused
        .long   0x40000000  // unused
        .long   0x50000000  // unused
        .long   0x60000000  // unused
        .long   0x70000000  // unused
        .long   0x00000001  // REMAP commande
        .long   0x00000006  // 7 memory regions, standard read
        .long   AT91_EBI    // External Bus Interface address
        .long   10f         // address where to jump
10:
        
#if defined(CYG_HAL_STARTUP_ROMRAM)
        ldr     r0,=0x01010000          // Relocate FLASH/ROM to on-chip RAM
        ldr     r1,=0x02000000          // RAM base & length
        ldr     r2,=0x02010000
20:     ldr     r3,[r0],#4
        str     r3,[r1],#4
        cmp     r1,r2
        bne     20b
        ldr     r0,=30f
        mov     pc,r0
30:             
#endif
                
        ldr     r0,=AT91_PS             // Power saving interface
        ldr     r1,=0xFFFFFFFF          // Enable all peripheral [clocks]
        str     r1,[r0,#AT91_PS_PCER]
        ldr     r0,=AT91_PIO            // Disable PIO (so peripherals can use bits)
        ldr     r1,=0x0070FE49          // UART, FIQ, EINT, Timer clocks
        str     r1,[r0,#AT91_PIO_PDR]
        ldr     r1,=0x000001B6          // LEDs
        str     r1,[r0,#AT91_PIO_OER]
        str     r1,[r0,#AT91_PIO_SODR]
        .endm        

#define CYGSEM_HAL_ROM_RESET_USES_JUMP
#define PLATFORM_SETUP1     _setup
#else
#define PLATFORM_SETUP1
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
