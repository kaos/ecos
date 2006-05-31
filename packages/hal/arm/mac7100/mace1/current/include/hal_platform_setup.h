#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

//=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
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
// Date:        2006-03-12
// Purpose:     MAC7100/MACE1 platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <cyg/hal/var_io.h>

//===========================================================================

   .macro _led y
        ldr r0,=MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET)
        ldrh r1,[r0]
        and r1,r1,#0x00ff
        add r1,r1,#(\y<<8)
        strh r1,[r0]
    .endm


// Initialize LED PORT
// Set appropriate peripheral pins
.macro  _led_init
        mov r1, #MAC7100_PIM_DDR  // Pin setting: Output+Low
        ldr r0,=MAC7100_PIM_CONFIG(MAC7100_PORT_A_OFFSET, 8) // LED pin cfg
        strh r1,[r0],#2 // LED: LSB ...
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2
        strh r1,[r0],#2 // LED: MSB ...
      _led    0         // Set initial LED state. 
    .endm

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

.macro  _pclock_init
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
        // Wait PLL lock  <---
1:      ldrb r3,[r2,#(MAC7100_CRG_CRGFLG-MAC7100_CRG_BASE)] 
        tst r3,#MAC7100_CRG_LOCK
        bne 2f      // PLL locked, GO ON  ----->>
        _led 0x55   
        b 1b        // Still waiting for PLL lock  -------------------------->
2:      mov r3,#MAC7100_CRG_CLKSEL_VAL  // <<----
        // Select PLL clock
        strb r3,[r2,#(MAC7100_CRG_CLKSEL-MAC7100_CRG_BASE)]  
        mov r3,#0
        strb r3,[r2,#(MAC7100_CRG_BDMCTL-MAC7100_CRG_BASE)]  // Set CRG BDMCTL 
        mov r3,r3
    .endm

    .macro  _pio_init
    .endm
                
// Memory re-mapping
    .macro _TelePort
        _led    1
        ldr     r1, TelePort    //AAMR_REG
        ldr     r0, [r1]

        mvn     r4, #0xf0000000      // 1. Copy telepoter to RAM
        ldr     r2, TelePort+4       // TelePorter
        ldr     r3, TelePort+8       // TelePort (TelePorter end)
        and     r2,r2,r4
        and     r3,r3,r4
        mov     r4, #0x40000000      // RAM address
1: //CopyTelePorter:                 // copying teleporter                 
            ldr     r5,[r2],#4       // <---
            str     r5,[r4],#4
            cmp     r2,r3
            bne     1b               //CopyTelePorter  // --->
        ldr     r3,TelePort+12
        mov     pc,#0x40000000      // 2. Jump to TelePorter in RAM
TelePorter:
        bic     r0,r0,#0x000000ff   // 3. Re-map memory
        orr     r0,r0,#0x8b         //    Flash -> 0x20000000
        str     r0,[r1]             //    RAM   -> 0x00000000 and 0x40000000
        mov     pc,r3               // 4. Teleport back to Flash
TelePort:
        .long MAC7100_MCM_AAMR
        .long TelePorter
        .long TelePort
        .long TelePortReturn
TelePortReturn:
    .endm
// End memory re-mapping

#define CYGHWR_LED_MACRO _led \x

//===========================================================================
                        
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

    .macro  _setup
        ldr r0,=VAE_MAC7100_FlashSecurity
        _led_init
        _TelePort
        _pclock_init
        _pio_init
    .endm

#define CYGSEM_HAL_ROM_RESET_USES_JUMP

#else

        .macro  _setup
        _led_init
        _led 16
        _pclock_init
        _pio_init        
        .endm
        
#endif

#define PLATFORM_SETUP1     _setup

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
