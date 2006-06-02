#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      AT91SAM7S board specific registers
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Author(s):   tkoeller
// Contributors: andrew lunn, Oliver Munz
// Date:        2005-12-31
// Purpose:     Atmel AT91SAM7S board specific registers
// Description: 
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================
#include <pkgconf/hal_arm_at91sam7.h>

#define CYGARC_PHYSICAL_ADDRESS(_x_)

//SPI - Serial Peripheral Interface
#define AT91_SPI0   0xFFFE0000

#ifdef CYGHWR_HAL_ARM_AT91SAM7X
#define AT91_SPI1   0xFFFE4000
#endif

#define AT91_SPI AT91_SPI0

// DMA registers 
#define AT91_SPI_RPR  0x100 // Receive Pointer Register
#define AT91_SPI_RCR  0x104 // Receive Counter Register
#define AT91_SPI_TPR  0x108 // Transmit Pointer Register
#define AT91_SPI_TCR  0x10C // Transmit Counter Register
#define AT91_SPI_NRPR 0x110 // Next Receive Pointer Register
#define AT91_SPI_NRCR 0x114 // Next Receive Counter Register
#define AT91_SPI_NTPR 0x118 // Next Transmit Pointer Register
#define AT91_SPI_NTCR 0x11C // Next Trsnsmit Counter Register
#define AT91_SPI_PTCR 0x120 // PDC Transfer Control Register
#define AT91_SPI_PTSR 0x124 // PDC Transfer Status Register

// Peripheral Input/Output Controllers
#define AT91_PIOA   0xFFFFF400
#ifdef CYGHWR_HAL_ARM_AT91SAM7X
#define AT91_PIOB   0xFFFFF600
#endif

#define AT91_WSTC   0xFFFFFD40

// USART

#define AT91_USART0 0xFFFC0000
#define AT91_USART1 0xFFFC4000

// Define USART2 to be the debug UART. It is similar enough to a USART
// that both the hal_diag and interrupt driven driver will work.
// However trying to change parity, start/stop bits etc will not work.
#define CYGNUM_HAL_INTERRUPT_USART2 CYGNUM_HAL_INTERRUPT_DBG 
#define AT91_USART2 AT91_DBG 

#ifndef __ASSEMBLER__
#ifdef CYGBLD_HAL_ARM_AT91_BAUD_DYNAMIC
extern cyg_uint32 hal_at91_us_baud(cyg_uint32 baud);
#define AT91_US_BAUD(baud) hal_at91_us_baud(baud)
#endif
#endif // __ASSEMBLER__

#define AT91_US_RPR  0x100 // Receive Pointer Register
#define AT91_US_RCR  0x104 // Receive Counter Register
#define AT91_US_TPR  0x108 // Transmit Pointer Register
#define AT91_US_TCR  0x10C // Transmit Counter Register
#define AT91_US_NRPR 0x110 // Next Receive Pointer Register
#define AT91_US_NRCR 0x114 // Next Receive Counter Register
#define AT91_US_NTPR 0x118 // Next Transmit Pointer Register 
#define AT91_US_NTCR 0x11C // Next Trsnsmit Counter Register
#define AT91_US_PTCR 0x120 // PDC Transfer Control Register
#define AT91_US_PTSR 0x124 // PDC Transfer Status Register

// PIO - Programmable I/O

#define AT91_PIO    AT91_PIOA

// AIC - Advanced Interrupt Controller

#define AT91_AIC    0xFFFFF000

// TC - Timer Counter

#define AT91_TC     0xFFFA0000

// Power Management Controller

#define AT91_PMC    0xFFFFFC00

#define AT91_PMC_MOR  0x20 // Main Oscillator Register
#define AT91_PMC_MOR_MOSCEN    (1 << 0) // Main Oscillator Enable
#define AT91_PMC_MOR_OSCBYPASS (1 << 1) // Main Oscillator Bypass
#define AT91_PMC_MOR_OSCCOUNT(x) (x << 8) // Slow clocks ticks
#define AT91_PMC_MCFR 0x24 // Main Clock Frequency Register
#define AT91_PMC_PLLR 0x2c // PLL Register
#define AT91_PMC_PLLR_DIV(x)      ((x) <<  0)  // PLL Devide
#define AT91_PMC_PLLR_PLLCOUNT(x) ((x) <<  8)  // PLL Count
#define AT91_PMC_PLLR_MUL(x)      ((x) << 16)  // PLL Devide
#define AT91_PMC_PLLR_OUT_0 (0 << 14)
#define AT91_PMC_PLLR_OUT_1 (1 << 14)
#define AT91_PMC_PLLR_OUT_2 (2 << 14)
#define AT91_PMC_PLLR_OUT_3 (3 << 14)
#define AT91_PMC_PLLR_USBDIV_0 (0 << 28) // USB clock is PLL clock / 1
#define AT91_PMC_PLLR_USBDIV_1 (1 << 28) // USB clock is PLL clock / 2
#define AT91_PMC_PLLR_USBDIV_2 (2 << 28) // USB clock is PLL clock / 4
#define AT91_PMC_MCKR 0x30 // Master Clock Register
#define AT91_PMC_MCKR_SLOW_CLK (0 << 0) // Slow clock selected
#define AT91_PMC_MCKR_MAIN_CLK (1 << 0) // Main clock selected
#define AT91_PMC_MCKR_PLL_CLK  (3 << 0) // PLL clock selected
#define AT91_PMC_MCKR_PRES_CLK    (0 << 2) // divide by 1
#define AT91_PMC_MCKR_PRES_CLK_2  (1 << 2) // divide by 2
#define AT91_PMC_MCKR_PRES_CLK_4  (2 << 2) // divide by 4
#define AT91_PMC_MCKR_PRES_CLK_8  (3 << 2) // divide by 8
#define AT91_PMC_MCKR_PRES_CLK_16 (4 << 2) // divide by 16
#define AT91_PMC_MCKR_PRES_CLK_32 (5 << 2) // divide by 32
#define AT91_PMC_MCKR_PRES_CLK_64 (6 << 2) // divide by 64
#define AT91_PMC_PCKR0 0x40  // Programmable Clock Register 0
#define AT91_PMC_PCKR1 0x44  // Programmable Clock Register 1
#define AT91_PMC_PCKR2 0x48  // Programmable Clock Register 2
#define AT91_PMC_IER  0x60 // Interrupt Enable Register
#define AT91_PMC_IDR  0x64 // Interrupt Disable Register
#define AT91_PMC_SR   0x68 // Status Register
#define AT91_PMC_SR_MOSCS   (1 << 0) // Main oscillator stable
#define AT91_PMC_SR_LOCK    (1 << 2) // PLL Locked
#define AT91_PMC_SR_MCKRDY  (1 << 3) // MCK is ready to be enabled
#define AT91_PMC_SR_PCK0RDY (1 << 8) // Pad clock 0 is ready to be enabled
#define AT91_PMC_SR_PCK1RDY (1 << 9) // Pad clock 1 is ready to be enabled
#define AT91_PMC_SR_PCK2RDY (1 << 10) // Pad clock 2 is ready to be enabled
#define AT91_PMC_SR_PCK3RDY (1 << 11) // Pad clock 3 is ready to be enabled
#define AT91_PMC_IMR  0x6c // Interrupt Mask Register

#ifdef CYGHWR_HAL_ARM_AT91SAM7X

// EMAC - Ethernet Medium Access Controller

#define AT91_EMAC 0xFFFDC000

// CAN - Controller Area Network

#define AT91_CAN 0xFFFD0000 

#endif

//----------------------------------------------------------------------
// The platform needs this initialization during the
// hal_hardware_init() function in the varient HAL.
#ifndef __ASSEMBLER__
extern void hal_plf_hardware_init(void);
#define HAL_PLF_HARDWARE_INIT() \
    hal_plf_hardware_init()
#endif          



#endif //CYGONCE_HAL_PLF_IO_H

