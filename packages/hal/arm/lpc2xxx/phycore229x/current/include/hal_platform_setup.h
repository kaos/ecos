#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H
/*=============================================================================
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
// Copyright (C) 2008 eCosCentric Limited
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-12-02
// Purpose:      phyCORE-LPC229x platform specific support routines
// Description:
// Usage:        #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/
#include <pkgconf/system.h>
#include <cyg/hal/var_io.h>

//===========================================================================*/

#define LINES (0xFE<<16)
#define LINE  (1<<16)
.macro  _line_init
    // set to GPIO
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE
    ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL2]
    bic r1, r1, #8
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL2]
    // set to output
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
    ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1DIR]
    orr r1,r1,#LINE
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1DIR]
    // turn ON
    ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1PIN]
    bic r1,r1,#LINES
    orr r1,r1,#LINE
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1PIN]
.endm

//----------------------------------------------------------------------------
// The phyCORE Carrier Board HD200 offers a programmable LED at
// D3 for user implementations. This LED can be connected to port pin
// P0.8 (TxD1) of the phyCORE-LPC2292/94 which is available via
// signal GPIO0 (JP17 = closed). A low-level at port pin P0.8 causes the
// LED to illuminate, LED D3 remains off when writing a high-level to
// P0.8.
//
.macro  _led_init
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
    ldr r1,=(1<<8)                                // GPIO0 pins 8 is LED output
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0DIR]
.endm

.macro _led x
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
    ldr r1,=(1<<8)
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0CLR]
    ldr r1,=((\x & 1)<<8)
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0SET]
.endm
#define CYGHWR_LED_MACRO _led \x 


//----------------------------------------------------------------------------
// PLL initialisation
//
.macro _pll_init
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE

    mov r2,#0xAA
    mov r3,#0x55

    mov r1,#(0x20 | (CYGNUM_HAL_ARM_LPC2XXX_PLL_MUL - 1))
    // load the PLL configuration register
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCFG] 

    mov r1,#1
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON] // enable PLL

    // perform validation sequence 0XAA followed by 0x55
    str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED] 
    str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

1:
    ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLSTAT] // wait for it to lock
    ands r1,r1,#(1<<10)
    beq 1b

    mov r1,#3 // connect PLL
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]

    // perform validation sequence 0XAA followed by 0x55
    str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED] 
    str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED] 
.endm


//----------------------------------------------------------------------------
// External memory and bus initialisation
//
.macro _mem_init
    //
    // first map the vector table to internal flash - normally this should be
    // the default value after boot - but we go the safe way here and force
    // the mapping to internal flash (the value for
    // CYGARC_HAL_LPC2XXX_REG_MEMMAP is 1)
    //
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE
    mov r1,#1
    str r1, [r0,#CYGARC_HAL_LPC2XXX_REG_MEMMAP]	
    
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
    // contains a copy of the vector tablefrom flash (the value for 
    // CYGARC_HAL_LPC2XXX_REG_MEMMAP is 2 = SRAM)
    //
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE
    // User RAM Mode. Interrupt vectors are re-mapped to Static RAM.
    mov r1,#2                                   
    str r1, [r0,#CYGARC_HAL_LPC2XXX_REG_MEMMAP]	
    // 4 processor clocks fetch cycle
    mov r1,#4					                
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMTIM]	// flash timings
    mov r1,#2
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMCR]	// enable full MAM
    
    //
    // Set-up external memory - the main task here is to setup the
    // wait states for the external memory properly
    //
    // Bank Configuration Registers 0-3 (BCFG0-3)
    // [0..3]          IDCY: Min. number of idle Cycles <0-15>
    // [4]             Reserved     
    // [5..9]          WST1: Wait States 1 <0-31>
    // [10]            RBLE: Read Byte Lane Enable
    // [11..15]        WST2: Wait States 2 <0-31>
    // [16..23]        Reserved
    // [26]            WP: Write Protect
    // [27]            BM: Burst ROM
    // [28..29]        MW: Memory Width  <0=8-bit,1=16-bit,2=32-bit,3=Reserved>
    // [30..31]        Reserved
    //
    
    // enable external parallel bus signals
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE
    // A0..1 enabled, CS0..3, OE, WE, BLS0..3, D0..31, A2..23, JTAG Pins
    ldr r1,=0x0FE149E4 
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL2]	

    // setup external FLASH wait states
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG0
    ldr r1,=0x20003CE3
    str r1, [r0]
        
    // setup external SRAM wait states
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG1
    ldr r1,=0x020002483
    str r1, [r0]

    // setup Ethernet chip wait states for /CS2 and /CS3
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG2
    ldr r1,=0x020000C23
    str r1, [r0]
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG3
    ldr r1,=0x020000C23
    str r1, [r0]
.endm
		
.macro _gpio_init
    // enable  RX and TX on UART0 and UART1
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE	
    ldr r1,=0x00050005
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL0]
    
    // set pin function to EINT0
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE	
    ldr r1,=0x00000001
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL1]
.endm

.macro  _block
    ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
    ldr r1,=(1<<8)
    str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO0CLR]
2:
    nop
    b 2
.endm
#define PLATFORM_BLOCK _block 
//===========================================================================*/
                        
#if defined(CYG_HAL_STARTUP_ROM)
.macro  _setup
        _line_init
        _led_init
        _led 1
        _pll_init
        _mem_init
        _gpio_init
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
