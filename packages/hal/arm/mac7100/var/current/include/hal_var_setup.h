#ifndef CYGONCE_HAL_VAR_SETUP_H
#define CYGONCE_HAL_VAR_SETUP_H

//=============================================================================
//
//      hal_var_setup.h
//
//      Variant specific support for HAL (assembly code)
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2006 eCosCentric Ltd
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors:
// Date:        2006-06-05
// Purpose:     MAC7100 variant specific support routines
// Description:
// Usage:       in <cyg/hal/hal_platform_serup.h> include following:
//              #include <cyg/hal/hal_var_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// Clock initilalization
// Initialize PLL
#if defined(CYGNUM_HAL_ARM_MAC7100_FDIV)  // Divider set by user.
#   define MAC7100_CRG_REFDV_VAL (CYGNUM_HAL_ARM_MAC7100_FDIV-1)
#else                                     //  Divider calculated. 
      //NOTE: works for f_osc <= 8MHz
#   define MAC7100_CRG_REFDV_VAL (((CYGNUM_HAL_ARM_MAC7100_F_OSC/500000) - 1) & 0x0F) // 15 (1)
#endif
#define MAC7100_CRG_SYNR_VAL (CYGNUM_HAL_ARM_MAC7100_CLOCK_SPEED / 2 / \
        (CYGNUM_HAL_ARM_MAC7100_F_OSC / (MAC7100_CRG_REFDV_VAL+1))-1) // 47 (5)

#define MAC7100_CRG_PLLCTL_VAL                                          \
      (MAC7100_CRG_CME|MAC7100_CRG_PLLON|MAC7100_CRG_AUTO|              \
       MAC7100_CRG_ACQ|MAC7100_CRG_SCME)
#define MAC7100_CRG_CLKSEL_VAL (MAC7100_CRG_PLLSEL)

        .macro  _mac7100_setpll
        ldr r2,=MAC7100_CRG_BASE
        mov r3,#0
        // Disable clock interrupts
        strb r3,[r2,#(MAC7100_CRG_CRGINT-MAC7100_CRG_BASE)]  
        // DeSelect PLL clock
        mov r3,#MAC7100_CRG_REFDV_VAL
        // Reference Divider reg.
        strb r3,[r2,#(MAC7100_CRG_REFDV-MAC7100_CRG_BASE)]   
        mov r3,#MAC7100_CRG_SYNR_VAL
        // Synthesizer register
        strb r3,[r2,#(MAC7100_CRG_SYNR-MAC7100_CRG_BASE)]   
        mov r3,#MAC7100_CRG_PLLCTL_VAL
        // PLL control register
        strb r3,[r2,#(MAC7100_CRG_PLLCTL-MAC7100_CRG_BASE)]   
        // Wait PLL lock  <-----------------------------------<
1:      ldrb r3,[r2,#(MAC7100_CRG_CRGFLG-MAC7100_CRG_BASE)] 
        tst r3,#MAC7100_CRG_LOCK
        bne 2f      // PLL locked, GO ON  ---------->>
        b 1b        // Still waiting for PLL lock ------------>
2:      mov r3,#MAC7100_CRG_CLKSEL_VAL  // <<-------<<
        // Select PLL clock
        strb r3,[r2,#(MAC7100_CRG_CLKSEL-MAC7100_CRG_BASE)]  
        mov r3,#0
        strb r3,[r2,#(MAC7100_CRG_BDMCTL-MAC7100_CRG_BASE)]  // Set CRG BDMCTL 
        mov r3,r3
    .endm

// Memory re-mapping for single chip configuration
// Re-map internal memory so that vectors can reside in RAM.
//     RAM base   0x00000000 (as well as 0x40000000)
//     Flash base 0x20000000
    .macro _mac7100_remap_single_chip
        _led    1
        ldr     r1, _mac7100_teleport
        ldr     r0, [r1]             //AAMR Register

        mvn     r4, #0xf0000000      // 1.Copy telepoter to RAM
        ldr     r2, _mac7100_teleport+4  //    TelePorter
        ldr     r3, _mac7100_teleport+8  //    TelePort (TelePorter end)
        and     r2,r2,r4
        and     r3,r3,r4
        mov     r4, #0x40000000      //    RAM address
1: // Copy teleporter:               //    copying teleporter <---<                 
            ldr     r5,[r2],#4      
            str     r5,[r4],#4
            cmp     r2,r3
            bne     1b                //    Copy teleporter -------->
        ldr     r3,_mac7100_teleport+12
        mov     pc,#0x40000000        // 2.Jump to _mac7100_teleporter in RAM
_mac7100_teleporter:
        bic     r0,r0,#0x000000ff     // 3.Re-map memory
        orr     r0,r0,#0x8b           //   Flash -> 0x20000000
        str     r0,[r1]               //   RAM   -> 0x00000000 and 0x40000000
        mov     pc,r3                 // 4.Teleport back to Flash
_mac7100_teleport:
        .long MAC7100_MCM_AAMR
        .long _mac7100_teleporter
        .long _mac7100_teleport
        .long _mac7100_teleport_return
_mac7100_teleport_return:
    .endm

// End memory re-mapping for single chip configuration

//-----------------------------------------------------------------------------
// end of hal_var_setup.h
#endif // CYGONCE_HAL_VAR_SETUP_H
