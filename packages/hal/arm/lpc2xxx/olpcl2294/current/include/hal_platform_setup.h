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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:       Olimex LPC-L2294-1MB platform specific support routines
// Description:
// Usage:         #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/hal/var_io.h>

        // There is one only diagnostic led marked STAT on the board
        // |P1.23  330 Om    LED
        // |------\/\/\/-----|<|-----> 3.3 V
        // |

        .macro  _led_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
        ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1DIR]
        orr r1,r1,#(1<<23)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1DIR]

        .endm // _led_init

        .macro _led x
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_IO_BASE
        ldr r1,=(1<<23)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1SET]
        ldr r1,=((\x & 1)<<23)
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_IO1CLR]

        .endm // _led

        .macro _pll_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE

        mov r2,#0xAA
        mov r3,#0x55

        // enable PLL
        mov r1,#1
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]

        mov r1,#(0x20 | (CYGNUM_HAL_ARM_LPC2XXX_PLL_MUL - 1))
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCFG]

        // update PLL registers
        str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]
        str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

        // wait for it to lock
1:
        ldr r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLSTAT]
        ands r1,r1,#(1<<10)
        beq 1b

        // connect PLL
        mov r1,#3
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLCON]

        // update PLL registers
        str r2,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]
        str r3,[r0,#CYGARC_HAL_LPC2XXX_REG_PLLFEED]

        .endm // _pll_init

        .macro _mem_init
        // copy first 64 bytes from ROM to on-chip RAM
        mov r0,#0
        mov r1,#0x40000000
        mov r2,#0x40
1:
        ldr r3,[r0,#4]!
        str r3,[r1,#4]!
        cmps r0,r2
        bne 1b

        ldr r0,=CYGARC_HAL_LPC2XXX_REG_SCB_BASE
        mov r1,#2 // interrupt vector table is mapped to RAM
        str r1, [r0,#CYGARC_HAL_LPC2XXX_REG_MEMMAP]
        // flash timings
        mov r1,#4
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMTIM]
        mov r1,#2 // 2, full MAM
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_MAMCR]

        // External memory interface depends on the bank width (32, 16 or 8 bit
        // selected via MW bits in corresponding BCFG register).  Furthermore,
        // choice of the memory chip(s) will require an adequate setup of RBLE
        // bit in BCFG register, too. RBLE = 0 in case of 8-bit based external
        // memories, while memory chips capable of accepting 16 or 32 bit wide
        // data will work with RBLE = 1.
        //
        // BANK0: FLASH memory
        //     -TE28F160C3BD70 (1024Kx16 x 1 = 2MB, 70nS) for 1MB RAM version
        //     -TE28F320C3BD70 (2048Kx16 x 1 = 4MB, 70ns) for 8MB RAM version
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG0
        ldr r1,=  (0x3 << 0)    /* IDCY=3, idle timing  */\
                | (0x4 << 5)    /* WST1=4, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x6 << 11)   /* WST2=6, write timing */\
                | (0x1 << 28)   /* MW=1,   16-bits      */
        str r1,[r0]

        // BANK1: external RAM
        // Warning: changed these timings, you can fall dramatically the eCos
        // kernel performance. Check it then using the eCos 'tm_basic' test.
        // TODO A way to make the timing value dependent of CCLK
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG1
#if defined (CYGHWR_OLIMEX_BOARD_OLPCL2294_1M)
        // 1MB version uses K6R4016V1D (512Kx16 x 2, 10nS)
        ldr r1,=  (0x0 << 0)    /* IDCY=0, idle cycles  */\
                | (0x0 << 5)    /* WST1=0, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x0 << 11)   /* WST2=0, write timing */\
                | (0x2 << 28)   /* MW=2,   32-bits      */
#elif defined (CYGHWR_OLIMEX_BOARD_OLPCL2294_8M)
        // 8MB version uses a K1S321611C (2048Kx16 x 2, 70ns)
        ldr r1,=  (0x3 << 0)    /* IDCY=3, idle cycles TODO */\
                | (0x4 << 5)    /* WST1=4, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x4 << 11)   /* WST2=4, write timing */\
                | (0x2 << 28)   /* MW=2,   32-bits      */
#else
#error Board not supported, OLIMEX LPC-L2294 1M or 8M only for now.
#endif
        str r1,[r0]

        // BANK2: Ethernet
        // CS8900A (16-bit, an interrupt driven mode)
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_BCFG2
        ldr r1,=  (0x1 << 0)    /* IDCY=1, idle cycles  */\
                | (0x8 << 5)    /* WST1=8, read timing  */\
                | (0x1 << 10)   /* RBLE=1               */\
                | (0x6 << 11)   /* WST2=6, write timing */\
                | (0x1 << 28)   /* MW=1,   16-bits      */
        str r1,[r0]

        .endm // _mem_init

        .macro _gpio_init
        ldr r0,=CYGARC_HAL_LPC2XXX_REG_PIN_BASE

        // Configure P0.15:0 as PIO, but UART0, EINT2
        ldr r1,=  (0x1 << 0)    /* P0.0 as TxD0         */\
                | (0x1 << 2)    /* P0.1 as RxD0         */\
                | (0x2 << 30)   /* P0.1 as EINT2(CL/IRQ)*/
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL0]

        // Configure P0.30:16 as PIO, but EINT0 ('BUT' button)
        ldr r1,=  (0x1 << 0)    /* P0.16 as EINT0       */
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL1]

        // GPIO P1.25:16, P3.24 are used as PIO
        ldr r1,=  (0x1 << 2)    /* P1.31:26 Debug port  */\
                | (0x2 << 4)    /* D31:0,CS0,OE,BLS0-3  */\
                | (0x1 << 8)    /* WE enabled           */\
                | (0x1 << 11)   /* CS1 enabled          */\
                | (0x1 << 14)   /* CS2 enabled          */\
                | (0x1 << 23)   /* A0 enabled           */\
                | (0x1 << 24)   /* A1 enabled           */\
                | (0x7 << 25)   /* A23:2 enabled        */
        str r1,[r0,#CYGARC_HAL_LPC2XXX_REG_PINSEL2]

        .endm // _gpio_init

#define CYGHWR_LED_MACRO _led \x

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

#endif // CYG_HAL_STARTUP_ROM

#define PLATFORM_SETUP1     _setup

#endif // CYGONCE_HAL_PLATFORM_SETUP_H

