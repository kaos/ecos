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
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2004 eCosCentric Limited 
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
// Author(s):   jani 
// Contributors:
// Date:        2004-09-12
// Purpose:     LPC2XXX/P2106 platform specific support routines
// Description:
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include <cyg/hal/var_io.h>

//===========================================================================*/

	.macro  _led_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
        ldr r1,=(1<<7)	                // GPIO0 pins 7 is LED output
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IODIR]
        .endm

        .macro _led x
	ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
	ldr r1,=(1<<7)
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IOCLR]
	ldr r1,=((\x & 1)<<7)
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IOSET]
	.endm
	
	.macro _pll_init
	ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE

	mov r2,#0xAA
	mov r3,#0x55
	
	mov r1,#1			// enable PLL	
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]
	
	mov r1,#(0x20 | (CYGNUM_HAL_ARM_LPC2XXX_PLL_MUL - 1))
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCFG]	
	
	str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]	// update PLL registers
	str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

1:
	ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLSTAT]	// wait for it to lock
	ands r1,r1,#(1<<10)
	beq 1b
	
	mov r1,#3			                // connect PLL
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]

	str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]	// update PLL registers
	str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]
	
	.endm
	
	.macro _mem_init
	mov r0,#0			// copy first 64 bytes of flash to RAM
	mov r1,#0x40000000
	mov r2,#0x40
1:
	ldr r3,[r0,#4]!
	str r3,[r1,#4]!
	cmps r0,r2
	bne 1b

	ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE
	mov r1,#2
        // interrupt vector table is mapped to flash (1) or RAM(2)	
	str r1, [r0,#CYGARC_HAL_LPC2XXX_REG_MEMMAP]	
	mov r1,#4
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMTIM]	// flash timings
	mov r1,#2
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMCR]	// enable full MAM
	.endm
		
        .macro _gpio_init
        // enable  RX and TX on UART0 and the button
	ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE	
	ldr r1,=0x80000005
	str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL0]
	.endm
	
#define CYGHWR_LED_MACRO _led \x 

//===========================================================================*/
                        
#if defined(CYG_HAL_STARTUP_ROM)

        .macro  _setup
	
	_pll_init
	
	_mem_init
	
	_gpio_init
	
	_led_init
	
        .endm

#define CYGSEM_HAL_ROM_RESET_USES_JUMP

#else

        .macro  _setup
        .endm
        
#endif

#define PLATFORM_SETUP1     _setup

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
