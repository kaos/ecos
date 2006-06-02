#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H
//=============================================================================
//
//      var_io.h
//
//      Variant specific registers
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2005, 2006 Andrew Lunn (andrew.lunn@ascom.ch>
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
// Author(s):   jskov
// Contributors:jskov, gthomas, tkoeller, tdrury, nickg, asl, John Eigelaar
// Date:        2001-07-12
// Purpose:     AT91 variant specific registers
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/plf_io.h>

//=============================================================================
// USART

#ifndef AT91_USART0
#define AT91_USART0 0xFFFD0000
#endif

#ifndef AT91_USART1
#define AT91_USART1 0xFFFCC000
#endif

#define AT91_US_CR  0x00  // Control register
#define AT91_US_CR_RxRESET (1<<2)
#define AT91_US_CR_TxRESET (1<<3)
#define AT91_US_CR_RxENAB  (1<<4)
#define AT91_US_CR_RxDISAB (1<<5)
#define AT91_US_CR_TxENAB  (1<<6)
#define AT91_US_CR_TxDISAB (1<<7)
#define AT91_US_CR_RSTATUS (1<<8)
#define AT91_US_CR_STTTO   (1<<11)
#define AT91_US_MR  0x04  // Mode register
#define AT91_US_MR_CLOCK   4
#define AT91_US_MR_CLOCK_MCK  (0<<AT91_US_MR_CLOCK)
#define AT91_US_MR_CLOCK_MCK8 (1<<AT91_US_MR_CLOCK)
#define AT91_US_MR_CLOCK_SCK  (2<<AT91_US_MR_CLOCK)
#define AT91_US_MR_LENGTH  6
#define AT91_US_MR_LENGTH_5   (0<<AT91_US_MR_LENGTH)
#define AT91_US_MR_LENGTH_6   (1<<AT91_US_MR_LENGTH)
#define AT91_US_MR_LENGTH_7   (2<<AT91_US_MR_LENGTH)
#define AT91_US_MR_LENGTH_8   (3<<AT91_US_MR_LENGTH)
#define AT91_US_MR_SYNC    8
#define AT91_US_MR_SYNC_ASYNC (0<<AT91_US_MR_SYNC)
#define AT91_US_MR_SYNC_SYNC  (1<<AT91_US_MR_SYNC)
#define AT91_US_MR_PARITY  9
#define AT91_US_MR_PARITY_EVEN  (0<<AT91_US_MR_PARITY)
#define AT91_US_MR_PARITY_ODD   (1<<AT91_US_MR_PARITY)
#define AT91_US_MR_PARITY_SPACE (2<<AT91_US_MR_PARITY)
#define AT91_US_MR_PARITY_MARK  (3<<AT91_US_MR_PARITY)
#define AT91_US_MR_PARITY_NONE  (4<<AT91_US_MR_PARITY)
#define AT91_US_MR_PARITY_MULTI (6<<AT91_US_MR_PARITY)
#define AT91_US_MR_STOP   12
#define AT91_US_MR_STOP_1       (0<<AT91_US_MR_STOP)
#define AT91_US_MR_STOP_1_5     (1<<AT91_US_MR_STOP)
#define AT91_US_MR_STOP_2       (2<<AT91_US_MR_STOP)
#define AT91_US_MR_MODE   14
#define AT91_US_MR_MODE_NORMAL  (0<<AT91_US_MR_MODE)
#define AT91_US_MR_MODE_ECHO    (1<<AT91_US_MR_MODE)
#define AT91_US_MR_MODE_LOCAL   (2<<AT91_US_MR_MODE)
#define AT91_US_MR_MODE_REMOTE  (3<<AT91_US_MR_MODE)
#define AT91_US_MR_MODE9  17
#define AT91_US_MR_CLKO   18
#define AT91_US_IER 0x08  // Interrupt enable register
#define AT91_US_IER_RxRDY   (1<<0)  // Receive data ready
#define AT91_US_IER_TxRDY   (1<<1)  // Transmitter ready
#define AT91_US_IER_RxBRK   (1<<2)  // Break received
#define AT91_US_IER_ENDRX   (1<<3)  // Rx end
#define AT91_US_IER_ENDTX   (1<<4)  // Tx end
#define AT91_US_IER_OVRE    (1<<5)  // Rx overflow
#define AT91_US_IER_FRAME   (1<<6)  // Rx framing error
#define AT91_US_IER_PARITY  (1<<7)  // Rx parity
#define AT91_US_IER_TIMEOUT (1<<8)  // Rx timeout
#define AT91_US_IER_TxEMPTY (1<<9)  // Tx empty
#define AT91_US_IDR 0x0C  // Interrupt disable register
#define AT91_US_IMR 0x10  // Interrupt mask register
#define AT91_US_CSR 0x14  // Channel status register
#define AT91_US_CSR_RxRDY 0x01 // Receive data ready
#define AT91_US_CSR_TxRDY 0x02 // Transmit ready
#define AT91_US_CSR_RXBRK 0x04 // Transmit ready
#define AT91_US_CSR_ENDRX 0x08 // Transmit ready
#define AT91_US_CSR_ENDTX 0x10 // Transmit ready
#define AT91_US_CSR_OVRE  0x20 // Overrun error
#define AT91_US_CSR_FRAME 0x40 // Framing error
#define AT91_US_CSR_TIMEOUT 0x80 // Timeout
#define AT91_US_RHR 0x18  // Receive holding register
#define AT91_US_THR 0x1C  // Transmit holding register
#define AT91_US_BRG 0x20  // Baud rate generator
#define AT91_US_RTO 0x24  // Receive time out
#define AT91_US_TTG 0x28  // Transmit timer guard

// PDC US registers may have different addresses in at91 targets (i.e jtst)
#ifndef AT91_US_RPR
#define AT91_US_RPR 0x30  // Receive pointer register
#endif

#ifndef AT91_US_RCR
#define AT91_US_RCR 0x34  // Receive counter register
#endif

#ifndef AT91_US_TPR
#define AT91_US_TPR 0x38  // Transmit pointer register
#endif

#ifndef AT91_US_TCR
#define AT91_US_TCR 0x3c  // Transmit counter register
#endif

// PDC Control register bits
#define AT91_US_PTCR_RXTEN  (1 << 0)
#define AT91_US_PTCR_RXTDIS (1 << 1)
#define AT91_US_PTCR_TXTEN  (1 << 8)
#define AT91_US_PTCR_TXTDIS (1 << 9)

// macro could be different from target to target (i.e jtst)
#ifndef AT91_US_BAUD
#define AT91_US_BAUD(baud) ((CYGNUM_HAL_ARM_AT91_CLOCK_SPEED/(8*(baud))+1)/2)
#endif
//=============================================================================
// PIO

#ifndef AT91_PIO
#define AT91_PIO      0xFFFF0000
#endif

#define AT91_PIN(_ctrl_, _periph_, _pin_) \
  ((_ctrl_ << 16) | (_periph_ << 8) | (_pin_))

#define AT91_PIO_PER  0x00  // PIO enable
#define AT91_PIO_PDR  0x04  // PIO disable
#define AT91_PIO_PSR  0x08  // PIO status

// GPIO pins on PIO A.
#define AT91_GPIO_PA0       AT91_PIN(0,0, 0)
#define AT91_GPIO_PA1       AT91_PIN(0,0, 1)
#define AT91_GPIO_PA2       AT91_PIN(0,0, 2)
#define AT91_GPIO_PA3       AT91_PIN(0,0, 3)
#define AT91_GPIO_PA4       AT91_PIN(0,0, 4)
#define AT91_GPIO_PA5       AT91_PIN(0,0, 5)
#define AT91_GPIO_PA6       AT91_PIN(0,0, 6)
#define AT91_GPIO_PA7       AT91_PIN(0,0, 7)
#define AT91_GPIO_PA8       AT91_PIN(0,0, 8)
#define AT91_GPIO_PA9       AT91_PIN(0,0, 9)
#define AT91_GPIO_PA10      AT91_PIN(0,0,10)
#define AT91_GPIO_PA11      AT91_PIN(0,0,11)
#define AT91_GPIO_PA12      AT91_PIN(0,0,12)
#define AT91_GPIO_PA13      AT91_PIN(0,0,13)
#define AT91_GPIO_PA14      AT91_PIN(0,0,14)
#define AT91_GPIO_PA15      AT91_PIN(0,0,15)
#define AT91_GPIO_PA16      AT91_PIN(0,0,16)
#define AT91_GPIO_PA17      AT91_PIN(0,0,17)
#define AT91_GPIO_PA18      AT91_PIN(0,0,18)
#define AT91_GPIO_PA19      AT91_PIN(0,0,19)
#define AT91_GPIO_PA20      AT91_PIN(0,0,20)
#define AT91_GPIO_PA21      AT91_PIN(0,0,21)
#define AT91_GPIO_PA22      AT91_PIN(0,0,22)
#define AT91_GPIO_PA23      AT91_PIN(0,0,23)
#define AT91_GPIO_PA24      AT91_PIN(0,0,24)
#define AT91_GPIO_PA25      AT91_PIN(0,0,25)
#define AT91_GPIO_PA26      AT91_PIN(0,0,26)
#define AT91_GPIO_PA27      AT91_PIN(0,0,27)
#define AT91_GPIO_PA28      AT91_PIN(0,0,28)
#define AT91_GPIO_PA29      AT91_PIN(0,0,29)
#define AT91_GPIO_PA30      AT91_PIN(0,0,30)
#define AT91_GPIO_PA31      AT91_PIN(0,0,31)

#ifdef AT91_PIOB
// GPIO pins on PIOB.
#define AT91_GPIO_PB0       AT91_PIN(1,0, 0)
#define AT91_GPIO_PB1       AT91_PIN(1,0, 1)
#define AT91_GPIO_PB2       AT91_PIN(1,0, 2)
#define AT91_GPIO_PB3       AT91_PIN(1,0, 3)
#define AT91_GPIO_PB4       AT91_PIN(1,0, 4)
#define AT91_GPIO_PB5       AT91_PIN(1,0, 5)
#define AT91_GPIO_PB6       AT91_PIN(1,0, 6)
#define AT91_GPIO_PB7       AT91_PIN(1,0, 7)
#define AT91_GPIO_PB8       AT91_PIN(1,0, 8)
#define AT91_GPIO_PB9       AT91_PIN(1,0, 9)
#define AT91_GPIO_PB10      AT91_PIN(1,0,10)
#define AT91_GPIO_PB11      AT91_PIN(1,0,11)
#define AT91_GPIO_PB12      AT91_PIN(1,0,12)
#define AT91_GPIO_PB13      AT91_PIN(1,0,13)
#define AT91_GPIO_PB14      AT91_PIN(1,0,14)
#define AT91_GPIO_PB15      AT91_PIN(1,0,15)
#define AT91_GPIO_PB16      AT91_PIN(1,0,16)
#define AT91_GPIO_PB17      AT91_PIN(1,0,17)
#define AT91_GPIO_PB18      AT91_PIN(1,0,18)
#define AT91_GPIO_PB19      AT91_PIN(1,0,19)
#define AT91_GPIO_PB20      AT91_PIN(1,0,20)
#define AT91_GPIO_PB21      AT91_PIN(1,0,21)
#define AT91_GPIO_PB22      AT91_PIN(1,0,22)
#define AT91_GPIO_PB23      AT91_PIN(1,0,23)
#define AT91_GPIO_PB24      AT91_PIN(1,0,24)
#define AT91_GPIO_PB25      AT91_PIN(1,0,25)
#define AT91_GPIO_PB26      AT91_PIN(1,0,26)
#define AT91_GPIO_PB27      AT91_PIN(1,0,27)
#define AT91_GPIO_PB28      AT91_PIN(1,0,28)
#define AT91_GPIO_PB29      AT91_PIN(1,0,29)
#define AT91_GPIO_PB30      AT91_PIN(1,0,30)
#define AT91_GPIO_PB31      AT91_PIN(1,0,31)
#endif //AT91_PIOB

#if defined(CYGHWR_HAL_ARM_AT91_M55800A)

#define AT91_TC_TCLK3       AT91_PIN(0,0, 0) // Timer 3 Clock signal
#define AT91_TC_TIOA3       AT91_PIN(0,0, 1) // Timer 3 Signal A
#define AT91_TC_TIOB3       AT91_PIN(0,0, 2) // Timer 3 Signal B
#define AT91_TC_TCLK4       AT91_PIN(0,0, 3) // Timer 4 Clock signal
#define AT91_TC_TIOA4       AT91_PIN(0,0, 4) // Timer 4 Signal A
#define AT91_TC_TIOB4       AT91_PIN(0,0, 5) // Timer 4 Signal B
#define AT91_TC_TCLK5       AT91_PIN(0,0, 6) // Timer 5 Clock signal
#define AT91_TC_TIOA5       AT91_PIN(0,0, 7) // Timer 5 Signal A
#define AT91_TC_TIOB5       AT91_PIN(0,0, 8) // Timer 5 Signal B
#define AT91_INT_IRQ0       AT91_PIN(0,0, 9) // External Interrupt 0
#define AT91_INT_IRQ1       AT91_PIN(0,0,10) // External Interrupt 1
#define AT91_INT_IRQ2       AT91_PIN(0,0,11) // External Interrupt 2
#define AT91_INT_IRQ3       AT91_PIN(0,0,12) // External Interrupt 3
#define AT91_INT_FIQ        AT91_PIN(0,0,13) // Fast Interrupt
#define AT91_USART_SCK0     AT91_PIN(0,0,14) // USART 0 Clock signal
#define AT91_USART_TXD0     AT91_PIN(0,0,15) // USART 0 transmit data
#define AT91_USART_RXD0     AT91_PIN(0,0,16) // USART 0 receive data
#define AT91_USART_SCK1     AT91_PIN(0,0,17) // USART 1 Clock signal
#define AT91_USART_TXD1     AT91_PIN(0,0,18) // USART 1 transmit data
#define AT91_USART_RXD1     AT91_PIN(0,0,19) // USART 1 receive data
#define AT91_USART_SCK2     AT91_PIN(0,0,20) // USART 2 Clock signal
#define AT91_USART_TXD2     AT91_PIN(0,0,21) // USART 2 transmit data
#define AT91_USART_RXD2     AT91_PIN(0,0,22) // USART 2 receive data
#define AT91_SPI_SPCK       AT91_PIN(0,0,23) // SPI Clock signal 
#define AT91_SPI_MISO       AT91_PIN(0,0,24) // SPI Master In Slave Out 
#define AT91_SPI_MOIS       AT91_PIN(0,0,25) // SPI Master Out Slave In 
#define AT91_SPI_NPCS0      AT91_PIN(0,0,26) // SPI Peripheral Chip Select 0
#define AT91_SPI_NPCS1      AT91_PIN(0,0,27) // SPI Peripheral Chip Select 1
#define AT91_SPI_NPCS2      AT91_PIN(0,0,28) // SPI Peripheral Chip Select 2
#define AT91_SPI_NPCS3      AT91_PIN(0,0,29) // SPI Peripheral Chip Select 3

#define AT91_INT_IRQ4       AT91_PIN(1,0, 3) // External Interrupt 4
#define AT91_INT_IRQ5       AT91_PIN(1,0, 4) // External Interrupt 5
#define AT91_ADC_AD0TRIG    AT91_PIN(1,0, 6) // ADC0 External Trigger
#define AT91_ADC_AD1TRIG    AT91_PIN(1,0, 7) // ADC1 External Trigger
#define AT91_BOOT_BMS       AT91_PIN(1,0,12) // Boot Mode Select
#define AT91_TC_TCLK0       AT91_PIN(1,0,14) // Timer 0 Clock signal
#define AT91_TC_TIOA0       AT91_PIN(1,0,15) // Timer 0 Signal A
#define AT91_TC_TIOB0       AT91_PIN(1,0,16) // Timer 0 Signal B
#define AT91_TC_TCLK1       AT91_PIN(1,0,17) // Timer 1 Clock signal
#define AT91_TC_TIOA1       AT91_PIN(1,0,18) // Timer 1 Signal A
#define AT91_TC_TIOB1       AT91_PIN(1,0,19) // Timer 1 Signal B
#define AT91_TC_TCLK2       AT91_PIN(1,0,20) // Timer 2 Clock signal
#define AT91_TC_TIOA2       AT91_PIN(1,0,21) // Timer 2 Signal A
#define AT91_TC_TIOB2       AT91_PIN(1,0,22) // Timer 2 Signal B

// PIOA
#define AT91_PIO_PSR_TCLK3   0x00000001 // Timer 3 Clock signal
#define AT91_PIO_PSR_TIOA3   0x00000002 // Timer 3 Signal A
#define AT91_PIO_PSR_TIOB3   0x00000004 // Timer 3 Signal B
#define AT91_PIO_PSR_TCLK4   0x00000008 // Timer 4 Clock signal
#define AT91_PIO_PSR_TIOA4   0x00000010 // Timer 4 Signal A
#define AT91_PIO_PSR_TIOB4   0x00000020 // Timer 4 Signal B
#define AT91_PIO_PSR_TCLK5   0x00000040 // Timer 5 Clock signal
#define AT91_PIO_PSR_TIOA5   0x00000080 // Timer 5 Signal A
#define AT91_PIO_PSR_TIOB5   0x00000100 // Timer 5 Signal B
#define AT91_PIO_PSR_IRQ0    0x00000200 // External Interrupt 0
#define AT91_PIO_PSR_IRQ1    0x00000400 // External Interrupt 1
#define AT91_PIO_PSR_IRQ2    0x00000800 // External Interrupt 2
#define AT91_PIO_PSR_IRQ3    0x00001000 // External Interrupt 3
#define AT91_PIO_PSR_FIQ     0x00002000 // Fast Interrupt
#define AT91_PIO_PSR_SCK0    0x00004000 // USART 0 Clock signal
#define AT91_PIO_PSR_TXD0    0x00008000 // USART 0 transmit data
#define AT91_PIO_PSR_RXD0    0x00010000 // USART 0 receive data
#define AT91_PIO_PSR_SCK1    0x00020000 // USART 1 Clock signal
#define AT91_PIO_PSR_TXD1    0x00040000 // USART 1 transmit data
#define AT91_PIO_PSR_RXD1    0x00080000 // USART 1 receive data
#define AT91_PIO_PSR_SCK2    0x00100000 // USART 2 Clock signal
#define AT91_PIO_PSR_TXD2    0x00200000 // USART 2 transmit data
#define AT91_PIO_PSR_RXD2    0x00400000 // USART 2 receive data
#define AT91_PIO_PSR_SPCK    0x00800000 // SPI Clock signal 
#define AT91_PIO_PSR_MISO    0x01000000 // SPI Master In Slave Out 
#define AT91_PIO_PSR_MOIS    0x02000000 // SPI Master Out Slave In 
#define AT91_PIO_PSR_NPCS0   0x04000000 // SPI Peripheral Chip Select 0
#define AT91_PIO_PSR_NPCS1   0x08000000 // SPI Peripheral Chip Select 1
#define AT91_PIO_PSR_NPCS2   0x10000000 // SPI Peripheral Chip Select 2
#define AT91_PIO_PSR_NPCS3   0x20000000 // SPI Peripheral Chip Select 3

// PIOB
#define AT91_PIO_PSR_IRQ4    0x00000008 // External Interrupt 4
#define AT91_PIO_PSR_IRQ5    0x00000010 // External Interrupt 5
#define AT91_PIO_PSR_AD0TRIG 0x00000040 // ADC0 External Trigger
#define AT91_PIO_PSR_AD1TRIG 0x00000080 // ADC1 External Trigger
#define AT91_PIO_PSR_BMS     0x00040000 // Boot Mode Select
#define AT91_PIO_PSR_TCLK0   0x00080000 // Timer 0 Clock signal
#define AT91_PIO_PSR_TIOA0   0x00100000 // Timer 0 Signal A
#define AT91_PIO_PSR_TIOB0   0x00200000 // Timer 0 Signal B
#define AT91_PIO_PSR_TCLK1   0x00400000 // Timer 1 Clock signal
#define AT91_PIO_PSR_TIOA1   0x00800000 // Timer 1 Signal A
#define AT91_PIO_PSR_TIOB1   0x01000000 // Timer 1 Signal B
#define AT91_PIO_PSR_TCLK2   0x02000000 // Timer 2 Clock signal
#define AT91_PIO_PSR_TIOA2   0x04000000 // Timer 2 Signal A
#define AT91_PIO_PSR_TIOB2   0x08000000 // Timer 2 Signal B

#elif defined (CYGHWR_HAL_ARM_AT91SAM7)
#include <pkgconf/hal_arm_at91sam7.h>

#ifdef CYGHWR_HAL_ARM_AT91SAM7S
#define AT91_PWM_PWM0       AT91_PIN(0,0, 0) // Pulse Width Modulation 0
#define AT91_PWM_PWM1       AT91_PIN(0,0, 1) // Pulse Width Modulation 1
#define AT91_PWM_PWM2       AT91_PIN(0,0, 2) // Pulse Width Modulation 2
#define AT91_TWI_TWD        AT91_PIN(0,0, 3) // Two Wire Data
#define AT91_TWI_TWCK       AT91_PIN(0,0, 4) // Two Wire Clock
#define AT91_USART_RXD0     AT91_PIN(0,0, 5) // USART 0 Receive Data
#define AT91_USART_TXD0     AT91_PIN(0,0, 6) // USART 0 Transmit Data
#define AT91_USART_RTS0     AT91_PIN(0,0, 7) // USART 0 Ready To Send
#define AT91_USART_CTS0     AT91_PIN(0,0, 8) // USART 0 Clear To Send
#define AT91_DBG_DRXD       AT91_PIN(0,0, 9) // Debug UART Receive
#define AT91_DBG_DTXD       AT91_PIN(0,0,10) // Debug UART Transmit
#define AT91_SPI_NPCS0      AT91_PIN(0,0,11) // SPI Chip Select 0
#define AT91_SPI_MISO       AT91_PIN(0,0,12) // SPI Input
#define AT91_SPI_MOIS       AT91_PIN(0,0,13) // SPI Output
#define AT91_SPI_SPCK       AT91_PIN(0,0,14) // SPI clock
#define AT91_S2C_TF         AT91_PIN(0,0,15) // S2C Transmit Frame Sync
#define AT91_S2C_TK         AT91_PIN(0,0,16) // S2C Transmit Clock
#define AT91_S2C_TD         AT91_PIN(0,0,17) // S2C Transmit Data
#define AT91_S2C_RD         AT91_PIN(0,0,18) // S2C Receive Data
#define AT91_S2C_RK         AT91_PIN(0,0,19) // S2C Receive Clock
#define AT91_S2C_RF         AT91_PIN(0,0,20) // S2C Receive Frame Sync
#if !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)
#define AT91_USART_RXD1     AT91_PIN(0,0,21) // USART 1 Receive Data
#define AT91_USART_TXD1     AT91_PIN(0,0,22) // USART 1 Transmit Data
#define AT91_USART_SCK1     AT91_PIN(0,0,23) // USART 1 Serial Clock
#define AT91_USART_RTS1     AT91_PIN(0,0,24) // USART 1 Ready To Send
#define AT91_USART_CTS1     AT91_PIN(0,0,25) // USART 1 Clear To Send
#define AT91_USART_DVD1     AT91_PIN(0,0,26) // USART 1 Data Carrier Detect
#define AT91_USART_DTR1     AT91_PIN(0,0,27) // USART 1 Data Terminal Ready
#define AT91_USART_DSR1     AT91_PIN(0,0,28) // USART 1 Data Set Ready
#define AT91_USART_RI1      AT91_PIN(0,0,29) // USART 2 Ring Indicator
#define AT91_INT_IRQ1       AT91_PIN(0,0,30) // Interrupt Request 1
#define AT91_SPI_NPCS1      AT91_PIN(0,0,31) // SPI Chip Select 1
#endif

#define AT91_TC_TI0A0       AT91_PIN(0,1, 0) // Timer/Counter 0 IO Line A
#define AT91_TC_TI0B0       AT91_PIN(0,1, 1) // Timer/Counter 0 IO Line B
#define AT91_USART_SCK0     AT91_PIN(0,1, 2) // USART 0 Serial Clock
#define AT91_SPI_NPCS3      AT91_PIN(0,1, 3) // SPI Chip Select 3
#define AT91_TC_TCLK0       AT91_PIN(0,1, 4) // Timer/Counter 0 Clock Input
#define AT91_SPI_NPCS3X     AT91_PIN(0,1, 5) // SPI Chip Select 3 (again)
#define AT91_PCK_PCK0       AT91_PIN(0,1, 6) // Programmable Clock Output 0
#define AT91_PWM_PWM3       AT91_PIN(0,1, 7) // Pulse Width Modulation #3
#define AT91_ADC_ADTRG      AT91_PIN(0,1, 8) // ADC Trigger
#define AT91_SPI_NPCS1X     AT91_PIN(0,1, 9) // SPI Chip Select 1
#define AT91_SPI_NPCS2      AT91_PIN(0,1,10) // SPI Chip Select 2
#define AT91_PWM_PWM0X      AT91_PIN(0,1,11) // Pulse Width Modulation #0
#define AT91_PIO_PWM_PWM1X  AT91_PIN(0,1,12) // Pulse Width Modulation #1
#define AT91_PIO_PWM_PWM2X  AT91_PIN(0,1,13) // Pulse Width Modulation #2
#define AT91_PIO_PWM_PWM4X  AT91_PIN(0,1,14) // Pulse Width Modulation #4
#define AT91_TC_TIOA1       AT91_PIN(0,1,15) // Timer/Counter 1 IO Line A
#define AT91_TC_TIOB1       AT91_PIN(0,1,16) // Timer/Counter 1 IO Line B
#define AT91_PCK_PCK1       AT91_PIN(0,1,17) // Programmable Clock Output 1
#define AT91_PCK_PCK2       AT91_PIN(0,1,18) // Programmable Clock Output 2
#define AT91_INT_FIQ        AT91_PIN(0,1,19) // Fast Interrupt Request
#define AT91_INT_IRQ0       AT91_PIN(0,1,20) // Interrupt Request 0
#if !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)
#define AT91_PCK_PCK1X      AT91_PIN(0,1,21) // Programmable Clock Output 1
#define AT91_SPI_NPCS3XX    AT91_PIN(0,1,22) // SPI Chip Select 3 (yet again)
#define AT91_PWM_PWM0XX     AT91_PIN(0,1,23) // Pulse Width Modulation #0 
#define AT91_PWM_PWM1XX     AT91_PIN(0,1,24) // Pulse Width Modulation #1
#define AT91_PWM_PWM2XX     AT91_PIN(0,1,25) // Pulse Width Modulation 2
#define AT91_TC_TIOA2       AT91_PIN(0,1,26) // Timer/Counter 2 IO Line A
#define AT91_TC_TIOB2       AT91_PIN(0,1,27) // Timer/Counter 2 IO Line B
#define AT91_TC_TCLK1       AT91_PIN(0,1,28) // External Clock Input 1
#define AT91_TC_TCLK2       AT91_PIN(0,1,29) // External Clock Input 2
#define AT91_SPI_NPCS2X     AT91_PIN(0,1,30) // SPI Chip Select 2 (again)
#define AT91_PCK_PCK2X      AT91_PIN(0,1,31) // Programmable Clock Output 2
#endif //!defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)

// PIO Peripheral A
#define AT91_PIO_PSR_PWM0     0x00000001 // Pulse Width Modulation 0
#define AT91_PIO_PSR_PWM1     0x00000002 // Pulse Width Modulation 1
#define AT91_PIO_PSR_PWM2     0x00000004 // Pulse Width Modulation 2
#define AT91_PIO_PSR_TWD      0x00000008 // Two Wire Data
#define AT91_PIO_PSR_TWCK     0x00000010 // Two Wire Clock
#define AT91_PIO_PSR_RXD0     0x00000020 // USART 0 Receive Data
#define AT91_PIO_PSR_TXD0     0x00000040 // USART 0 Transmit Data
#define AT91_PIO_PSR_RTS0     0x00000080 // USART 0 Ready To Send
#define AT91_PIO_PSR_CTS0     0x00000100 // USART 0 Clear To Send
#define AT91_PIO_PSR_DRXD     0x00000200 // Debug UART Receive
#define AT91_PIO_PSR_DTXD     0x00000400 // Debug UART Transmit
#define AT91_PIO_PSR_NPCS0    0x00000800 // SPI Chip Select 0
#define AT91_PIO_PSR_MISO     0x00001000 // SPI Input
#define AT91_PIO_PSR_MOIS     0x00002000 // SPI Output
#define AT91_PIO_PSR_SPCK     0x00004000 // SPI clock
#define AT91_PIO_PSR_TF       0x00008000 // S2C Transmit Frame Sync
#define AT91_PIO_PSR_TK       0x00010000 // S2C Transmit Clock
#define AT91_PIO_PSR_TD       0x00020000 // S2C Transmit Data
#define AT91_PIO_PSR_RD       0x00040000 // S2C Receive Data
#define AT91_PIO_PSR_RK       0x00080000 // S2C Receive Clock
#define AT91_PIO_PSR_RF       0x00100000 // S2C Receive Frame Sync
#if !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)
#define AT91_PIO_PSR_RXD1     0x00200000 // USART 1 Receive Data
#define AT91_PIO_PSR_TXD1     0x00400000 // USART 1 Transmit Data
#define AT91_PIO_PSR_SCK1     0x00800000 // USART 1 Serial Clock
#define AT91_PIO_PSR_RTS1     0x01000000 // USART 1 Ready To Send
#define AT91_PIO_PSR_CTS1     0x02000000 // USART 1 Clear To Send
#define AT91_PIO_PSR_DCD1     0x04000000 // USART 1 Data Carrier Detect
#define AT91_PIO_PSR_DTR1     0x08000000 // USART 1 Data Terminal Ready
#define AT91_PIO_PSR_DSR1     0x10000000 // USART 1 Data Set Ready
#define AT91_PIO_PSR_RI1      0x20000000 // USART 2 Ring Indicator
#define AT91_PIO_PSR_IRQ1     0x40000000 // Interrupt Request 1
#define AT91_PIO_PSR_NPCS1    0x80000000 // SPI Chip Select 1
#endif // !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s64)

// PIO Peripheral B
#define AT91_PIO_PSR_TIOA0     0x00000001 // Timer/Counter 0 IO Line A
#define AT91_PIO_PSR_TIOB0     0x00000002 // Timer/Counter 0 IO Line B
#define AT91_PIO_PSR_SCK0      0x00000004 // USART 0 Serial Clock
#define AT91_PIO_PSR_NPCS3     0x00000008 // SPI Chip Select 3
#define AT91_PIO_PSR_TCLK0     0x00000010 // Timer/Counter 0 Clock Input
#define AT91_PIO_PSR_NPCS3X    0x00000020 // SPI Chip Select 3 (again)
#define AT91_PIO_PSR_PCK0      0x00000040 // Programmable Clock Output 0
#define AT91_PIO_PSR_PWM3      0x00000080 // Pulse Width Modulation #3
#define AT91_PIO_PSR_ADTRG     0x00000100 // ADC Trigger
#define AT91_PIO_PSR_NPCS1X    0x00000200 // SPI Chip Select 1 (again)
#define AT91_PIO_PSR_NPCS2     0x00000400 // SPI Chip Select 2
#define AT91_PIO_PSR_PWMOX     0x00000800 // Pulse Width Modulation #0 (again)
#define AT91_PIO_PSR_PWM1X     0x00001000 // Pulse Width Modulation #1 (again)
#define AT91_PIO_PSR_PWM2X     0x00002000 // Pulse Width Modulation #2 (again)
#define AT91_PIO_PSR_PWM3X     0x00004000 // Pulse Width Modulation #4 (again)
#define AT91_PIO_PSR_TIOA1     0x00008000 // Timer/Counter 1 IO Line A
#define AT91_PIO_PSR_TIOB1     0x00010000 // Timer/Counter 1 IO Line B
#define AT91_PIO_PSR_PCK1      0x00020000 // Programmable Clock Output 1
#define AT91_PIO_PSR_PCK2      0x00040000 // Programmable Clock Output 2
#define AT91_PIO_PSR_FIQ       0x00080000 // Fast Interrupt Request
#define AT91_PIO_PSR_IRQ0      0x00100000 // Interrupt Request 0
#if !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)
#define AT91_PIO_PSR_PCK1X     0x00200000 // Programmable Clock Output 1(again)
#define AT91_PIO_PSR_NPCS3XX   0x00400000 // SPI Chip Select 3 (yet again)
#define AT91_PIO_PSR_PWMOXX    0x00800000 // Pulse Width Modulation #0 (again)
#define AT91_PIO_PSR_PWM1XX    0x01000000 // Pulse Width Modulation #1 (again)
#define AT91_PIO_PSR_PWM2XX    0x02000000 // Pulse Width Modulation #2 (again)
#define AT91_PIO_PSR_TIOA2     0x04000000 // Timer/Counter 2 IO Line A
#define AT91_PIO_PSR_TIOB2     0x08000000 // Timer/Counter 2 IO Line B
#define AT91_PIO_PSR_TCLK1     0x10000000 // External Clock Input 1
#define AT91_PIO_PSR_TCLK2     0x20000000 // External Clock Input 2
#define AT91_PIO_PSR_NPCS2X    0x40000000 // SPI Chip Select 2 (again)
#define AT91_PIO_PSR_PCK2X     0x80000000 // Programmable Clock Output 2(again)
#endif // !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s64)
#endif // CYGHWR_HAL_ARM_AT91SAM7S

#ifdef CYGHWR_HAL_ARM_AT91SAM7X

// PIO Controller A, peripheral A
#define AT91_USART_RXD0 AT91_PIN(0,0, 0) // USART 0 Receive Data
#define AT91_USART_TXD0 AT91_PIN(0,0, 1) // USART 0 Transmit Data 
#define AT91_USART_SCK0 AT91_PIN(0,0, 2) // USART 0 Serial Clock
#define AT91_USART_RTS0 AT91_PIN(0,0, 3) // USART 0 Request To Send
#define AT91_USART_CTS0 AT91_PIN(0,0, 4) // USART 0 Clear To Send
#define AT91_USART_RXD1 AT91_PIN(0,0, 5) // USART 1 Receive Data 
#define AT91_USART_TXD1 AT91_PIN(0,0, 6) // USART 1 Transmit Data 
#define AT91_USART_SCK1 AT91_PIN(0,0, 7) // USART 1 Serial Clock
#define AT91_USART_RTS1 AT91_PIN(0,0, 8) // USART 1 Request To Send
#define AT91_USART_CTS1 AT91_PIN(0,0, 9) // USART 1 Clear To Send
#define AT91_TWI_TWD    AT91_PIN(0,0,10) // Two Wire Data
#define AT91_TWI_TWCK   AT91_PIN(0,0,11) // Two Wire Clock
#define AT91_SPI_NPCS0  AT91_PIN(0,0,12) // SPI 0 Chip Select 0
#define AT91_SPI_NPCS1  AT91_PIN(0,0,13) // SPI 0 Chip Select 1 
#define AT91_SPI_NPCS2  AT91_PIN(0,0,14) // SPI 0 Chip Select 2 
#define AT91_SPI_NPCS3  AT91_PIN(0,0,15) // SPI 0 Chip Select 3 
#define AT91_SPI_MISO   AT91_PIN(0,0,16) // SPI 0 Master In Slave Out
#define AT91_SPI_MOIS   AT91_PIN(0,0,17) // SPI 0 Master Out Slave In 
#define AT91_SPI_SPCK   AT91_PIN(0,0,18) // SPI 0 Clock
#define AT91_CAN_CANRX  AT91_PIN(0,0,19) // CAN Receive
#define AT91_CAN_CANTX  AT91_PIN(0,0,20) // CAN Transmit 
#define AT91_SSC_TF     AT91_PIN(0,0,21) // SSC Transmit Frame Sync 
#define AT91_S2C_TK     AT91_PIN(0,0,22) // SSC Transmit Clock 
#define AT91_S2C_TD     AT91_PIN(0,0,23) // SSC Transmit Data 
#define AT91_S2C_RD     AT91_PIN(0,0,24) // SSC Receive Data 
#define AT91_S2C_RK     AT91_PIN(0,0,25) // SSC Receive Clock 
#define AT91_S2C_RF     AT91_PIN(0,0,26) // SSC Receive Frame Sync 
#define AT91_DBG_DRXD   AT91_PIN(0,0,27) // DBGU Receive Data 
#define AT91_DBG_DTXD   AT91_PIN(0,0,28) // DBGU Transmit Data 
#define AT91_INT_FIQ    AT91_PIN(0,0,29) // Fast Interrupt Request 
#define AT91_INT_IRQ0   AT91_PIN(0,0,39) // Interrupt Request 0

//PIO controller A, peripheral B
#define AT91_SPI1_NPCS1  AT91_PIN(0,1, 2) // SPI 1 Chip Select 1 
#define AT91_SPI1_NPCS2  AT91_PIN(0,1, 3) // SPI 1 Chip Select 2 
#define AT91_SPI1_NPCS3  AT91_PIN(0,1, 4) // SPI 1 Chip Select 3 
#define AT91_SPI_NPCS1X  AT91_PIN(0,1, 7) // SPI 0 Chip Select 1 
#define AT91_SPI_NPCS2X  AT91_PIN(0,1, 8) // SPI 0 Chip Select 2 
#define AT91_SPI_NPCS3X  AT91_PIN(0,1, 9) // SPI 0 Chip Select 3 
#define AT91_PCK_PCK1    AT91_PIN(0,1,13) // Programmable Clock Output 1 
#define AT91_INT_IRQ1    AT91_PIN(0,1,14) // Interrupt Request 1
#define AT91_TC_TCLK1    AT91_PIN(0,1,15) // Timer/Counter 1 Clock Input
#define AT91_SPI1_NPCS0  AT91_PIN(0,1,21) // SPI 1 Chip Select 0 
#define AT91_SPI1_SPCK   AT91_PIN(0,1,22) // SPI 1 Clock
#define AT91_SPI1_MOSI   AT91_PIN(0,1,23) // SPI 1 Master Out Slave In 
#define AT91_SPI1_MISO   AT91_PIN(0,1,24) // SPI 0 Master In Slave Out
#define AT91_SPI1_NPCS1X AT91_PIN(0,1,25) // SPI 1 Chip Select 1 
#define AT91_SPI1_NPCS2X AT91_PIN(0,1,26) // SPI 1 Chip Select 2 
#define AT91_PCK_PCK3    AT91_PIN(0,1,27) // Programmable Clock Output 3
#define AT91_SPI1_NPCS3X AT91_PIN(0,1,29) // SPI 1 Chip Select 3 
#define AT91_PCK_PCK2    AT91_PIN(0,1,30) // Programmable Clock Output 2

//PIO Controller B, Peripheral A
#define AT91_EMAC_EREFCK AT91_PIN(1,0, 0) // EMAC Reference Clock 
#define AT91_EMAC_ETXEN  AT91_PIN(1,0, 1) // EMAC Transmit Enable 
#define AT91_EMAC_ETX0   AT91_PIN(1,0, 2) // EMAC Transmit Data 0 
#define AT91_EMAC_ETX1   AT91_PIN(1,0, 3) // EMAC Transmit Data 1  
#define AT91_EMAC_ECRS   AT91_PIN(1,0, 4) // EMAC Carrier Sense 
#define AT91_EMAC_ERX0   AT91_PIN(1,0, 5) // EMAC Receive Data 0   
#define AT91_EMAC_ERX1   AT91_PIN(1,0, 6) // EMAC Receive Data 1    
#define AT91_EMAC_ERXER  AT91_PIN(1,0, 7) // EMAC Receive Error 
#define AT91_EMAC_EMDC   AT91_PIN(1,0, 8) // EMAC Management Data Clock 
#define AT91_EMAC_EMDIO  AT91_PIN(1,0, 9) // EMAC Management Data IO
#define AT91_EMAC_ETX2   AT91_PIN(1,0,10) // EMAC Transmit Data 2    
#define AT91_EMAC_ETX3   AT91_PIN(1,0,11) // EMAC Transmit Data 3    
#define AT91_EMAC_ETXER  AT91_PIN(1,0,12) // EMAC Transmit Coding Error 
#define AT91_EMAC_ERX2   AT91_PIN(1,0,13) // EMAC Receive Data 2     
#define AT91_EMAC_ERX3   AT91_PIN(1,0,14) // EMAC Receive Data 3     
#define AT91_EMAC_ECRSDV AT91_PIN(1,0,15) // EMAC Carrier Sense And Data Valid 
#define AT91_EMAC_ERXDV  AT91_PIN(1,0,15) // EMAC Receive Data Valid 
#define AT91_EMAC_ECOL   AT91_PIN(1,0,16) // EMAC Collision Detected 
#define AT91_EMAC_ERXCK  AT91_PIN(1,0,17) // EMAC Receive Clock 
#define AT91_EMAC_EF100  AT91_PIN(1,0,18) // EMAC Force 100Mb/s 
#define AT91_PWM_PWM0    AT91_PIN(1,0,19) // Pulse Width Modulation #0
#define AT91_PWM_PWM1    AT91_PIN(1,0,20) // Pulse Width Modulation #1
#define AT91_PWM_PWM2    AT91_PIN(1,0,21) // Pulse Width Modulation #2
#define AT91_PWM_PWM3    AT91_PIN(1,0,22) // Pulse Width Modulation #3
#define AT91_TC_TIOA0    AT91_PIN(1,0,23) // Timer/Counter 0 IO Line A
#define AT91_TC_TIOB0    AT91_PIN(1,0,24) // Timer/Counter 0 IO Line B
#define AT91_TC_TIOA1    AT91_PIN(1,0,25) // Timer/Counter 1 IO Line A
#define AT91_TC_TIOB1    AT91_PIN(1,0,26) // Timer/Counter 1 IO Line B
#define AT91_TC_TIOA2    AT91_PIN(1,0,27) // Timer/Counter 2 IO Line A
#define AT91_TC_TIOB2    AT91_PIN(1,0,28) // Timer/Counter 2 IO Line B
#define AT91_PCK_PCK1X   AT91_PIN(1,0,29) // Programmable Clock Output 1
#define AT91_PCK_PCK2X   AT91_PIN(1,0,30) // Programmable Clock Output 2

//PIO Controller B Peripheral B
#define AT91_PCK_PCK0     AT91_PIN(1,1, 0) // Programmable Clock Output 0
#define AT91_SPI1_NPCS1XX AT91_PIN(1,1,10) // SPI 1 Chip Select 1 
#define AT91_SPI1_NPCS2XX AT91_PIN(1,1,11) // SPI 1 Chip Select 2 
#define AT91_TC_TCLK0     AT91_PIN(1,1,12) // Timer/Counter 0 Clock Input
#define AT91_SPI_NPCS1XX  AT91_PIN(1,1,13) // SPI 0 Chip Select 1 
#define AT91_SPI_NPCS2XX  AT91_PIN(1,1,14) // SPI 0 Chip Select 2 
#define AT91_SPI1_NPCS3XX AT91_PIN(1,1,16) // SPI 1 Chip Select 3 
#define AT91_SPI_NPCS3XX  AT91_PIN(1,1,17) // SPI 0 Chip Select 3 
#define AT91_ADC_ADTRG    AT91_PIN(1,1,18) // ADC Trigger
#define AT91_TC_TCLK1X    AT91_PIN(1,1,19) // Timer/Counter 1 Clock Input
#define AT91_PCK_PCK0X    AT91_PIN(1,1,20) // Programmable Clock Output 0
#define AT91_PCK_PCK1XX   AT91_PIN(1,1,21) // Programmable Clock Output 1
#define AT91_PCK_PCK2XX   AT91_PIN(1,1,22) // Programmable Clock Output 2
#define AT91_USART_DCD1   AT91_PIN(1,1,23) // USART 1 Data Carrier Detect
#define AT91_USART_DSR1   AT91_PIN(1,1,24) // USART 1 Data Set Ready
#define AT91_USART_DTR1   AT91_PIN(1,1,25) // USART 1 Data Terminal Ready
#define AT91_USART_RI1    AT91_PIN(1,1,26) // USART 1 Ring Indication
#define AT91_PWM_PWM0X    AT91_PIN(1,1,27) // Pulse Width Modulation #0
#define AT91_PWM_PWM1X    AT91_PIN(1,1,28) // Pulse Width Modulation #1
#define AT91_PWM_PWM2X    AT91_PIN(1,1,29) // Pulse Width Modulation #2
#define AT91_PWM_PWM3X    AT91_PIN(1,1,30) // Pulse Width Modulation #3

// PIO Controller A, peripheral A
#define AT91_PIO_PSR_RXD0      (1<< 0) // USART 0 Receive Data
#define AT91_PIO_PSR_TXD0      (1<< 1) // USART 0 Transmit Data 
#define AT91_PIO_PSR_SCK0      (1<< 2) // USART 0 Serial Clock
#define AT91_PIO_PSR_RTS0      (1<< 3) // USART 0 Request To Send
#define AT91_PIO_PSR_CTS0      (1<< 4) // USART 0 Clear To Send
#define AT91_PIO_PSR_RXD1      (1<< 5) // USART 1 Receive Data 
#define AT91_PIO_PSR_TXD1      (1<< 6) // USART 1 Transmit Data 
#define AT91_PIO_PSR_SCK1      (1<< 7) // USART 1 Serial Clock
#define AT91_PIO_PSR_RTS1      (1<< 8) // USART 1 Request To Send
#define AT91_PIO_PSR_CTS1      (1<< 9) // USART 1 Clear To Send
#define AT91_PIO_PSR_TWD       (1<<10) // Two Wire Data
#define AT91_PIO_PSR_TWCK      (1<<11) // Two Wire Clock
#define AT91_PIO_PSR_SPI_NPCS0 (1<<12) // SPI 0 Chip Select 0
#define AT91_PIO_PSR_SPI_NPCS1 (1<<13) // SPI 0 Chip Select 1 
#define AT91_PIO_PSR_SPI_NPCS2 (1<<14) // SPI 0 Chip Select 2 
#define AT91_PIO_PSR_SPI_NPCS3 (1<<15) // SPI 0 Chip Select 3 
#define AT91_PIO_PSR_SPI_MISO  (1<<16) // SPI 0 Master In Slave Out
#define AT91_PIO_PSR_SPI_MOSI  (1<<17) // SPI 0 Master Out Slave In 
#define AT91_PIO_PSR_SPI_SPCK  (1<<18) // SPI 0 Clock
#define AT91_PIO_PSR_CANRX     (1<<19) // CAN Receive
#define AT91_PIO_PSR_CANTX     (1<<20) // CAN Transmit 
#define AT91_PIO_PSR_TF        (1<<21) // SSC Transmit Frame Sync 
#define AT91_PIO_PSR_TK        (1<<22) // SSC Transmit Clock 
#define AT91_PIO_PSR_TD        (1<<23) // SSC Transmit Data 
#define AT91_PIO_PSR_RD        (1<<24) // SSC Receive Data 
#define AT91_PIO_PSR_RK        (1<<25) // SSC Receive Clock 
#define AT91_PIO_PSR_RF        (1<<26) // SSC Receive Frame Sync 
#define AT91_PIO_PSR_DRXD      (1<<27) // DBGU Receive Data 
#define AT91_PIO_PSR_DTXD      (1<<28) // DBGU Transmit Data 
#define AT91_PIO_PSR_FIQ       (1<<29) // Fast Interrupt Request 
#define AT91_PIO_PSR_IRQ0      (1<<30) // Interrupt Request 0

//PIO controller A, peripheral B
#define AT91_PIO_PSR_SPI1_NPCS1  (1<< 2) // SPI 1 Chip Select 1 
#define AT91_PIO_PSR_SPI1_NPCS2  (1<< 3) // SPI 1 Chip Select 2 
#define AT91_PIO_PSR_SPI1_NPCS3  (1<< 4) // SPI 1 Chip Select 3 
#define AT91_PIO_PSR_SPI_NPCS1X  (1<< 7) // SPI 0 Chip Select 1 
#define AT91_PIO_PSR_SPI_NPCS2X  (1<< 8) // SPI 0 Chip Select 2 
#define AT91_PIO_PSR_SPI_NPCS3X  (1<< 9) // SPI 0 Chip Select 3 
#define AT91_PIO_PSR_PCK1        (1<<13) // Programmable Clock Output 1 
#define AT91_PIO_PSR_IRQ1        (1<<14) // Interrupt Request 1
#define AT91_PIO_PSR_TCLK1       (1<<15) // Timer/Counter 1 Clock Input
#define AT91_PIO_PSR_SPI1_NPCS0  (1<<21) // SPI 1 Chip Select 0 
#define AT91_PIO_PSR_SPI1_SPCK   (1<<22) // SPI 1 Clock
#define AT91_PIO_PSR_SPI1_MOSI   (1<<23) // SPI 1 Master Out Slave In 
#define AT91_PIO_PSR_SPI1_MISO   (1<<24) // SPI 0 Master In Slave Out
#define AT91_PIO_PSR_SPI1_NPCS1X (1<<25) // SPI 1 Chip Select 1 
#define AT91_PIO_PSR_SPI1_NPCS2X (1<<26) // SPI 1 Chip Select 2 
#define AT91_PIO_PSR_PCK3        (1<<27) // Programmable Clock Output 3
#define AT91_PIO_PSR_SPI1_NPCS3X (1<<29) // SPI 1 Chip Select 3 
#define AT91_PIO_PSR_PCK2        (1<<30) // Programmable Clock Output 2

//PIO Controller B, Peripheral A
#define AT91_PIO_PSR_EREFCK   (1<< 0) // EMAC Reference Clock 
#define AT91_PIO_PSR_ETXEN    (1<< 1) // EMAC Transmit Enable 
#define AT91_PIO_PSR_ETX0     (1<< 2) // EMAC Transmit Data 0 
#define AT91_PIO_PSR_ETX1     (1<< 3) // EMAC Transmit Data 1  
#define AT91_PIO_PSR_ECRS     (1<< 4) // EMAC Carrier Sense 
#define AT91_PIO_PSR_ERX0     (1<< 5) // EMAC Receive Data 0   
#define AT91_PIO_PSR_ERX1     (1<< 6) // EMAC Receive Data 1    
#define AT91_PIO_PSR_ERXER    (1<< 7) // EMAC Receive Error 
#define AT91_PIO_PSR_EMDC     (1<< 8) // EMAC Management Data Clock 
#define AT91_PIO_PSR_EMDIO    (1<< 9) // EMAC Management Data IO
#define AT91_PIO_PSR_ETX2     (1<<10) // EMAC Transmit Data 2    
#define AT91_PIO_PSR_ETX3     (1<<11) // EMAC Transmit Data 3    
#define AT91_PIO_PSR_ETXER    (1<<12) // EMAC Transmit Coding Error 
#define AT91_PIO_PSR_ERX2     (1<<13) // EMAC Receive Data 2     
#define AT91_PIO_PSR_ERX3     (1<<14) // EMAC Receive Data 3     
#define AT91_PIO_PSR_ECRSDV   (1<<15) // EMAC Carrier Sense And Data Valid 
#define AT91_PIO_PSR_ECOL     (1<<16) // EMAC Collision Detected 
#define AT91_PIO_PSR_ERXCK    (1<<17) // EMAC Receive Clock 
#define AT91_PIO_PSR_EF100    (1<<18) // EMAC Force 100Mb/s 
#define AT91_PIO_PSR_PWM0     (1<<19) // Pulse Width Modulation #0
#define AT91_PIO_PSR_PWM1     (1<<20) // Pulse Width Modulation #1
#define AT91_PIO_PSR_PWM2     (1<<21) // Pulse Width Modulation #2
#define AT91_PIO_PSR_PWM3     (1<<22) // Pulse Width Modulation #3
#define AT91_PIO_PSR_TIOA0    (1<<23) // Timer/Counter 0 IO Line A
#define AT91_PIO_PSR_TIOB0    (1<<24) // Timer/Counter 0 IO Line B
#define AT91_PIO_PSR_TIOA1    (1<<25) // Timer/Counter 1 IO Line A
#define AT91_PIO_PSR_TIOB1    (1<<26) // Timer/Counter 1 IO Line B
#define AT91_PIO_PSR_TIOA2    (1<<27) // Timer/Counter 2 IO Line A
#define AT91_PIO_PSR_TIOB2    (1<<28) // Timer/Counter 2 IO Line B
#define AT91_PIO_PSR_PCK1X    (1<<29) // Programmable Clock Output 1
#define AT91_PIO_PSR_PCK2     (1<<30) // Programmable Clock Output 2

//PIO Controller B Peripheral B
#define AT91_PIO_PSR_PCK0         (1<< 0) // Programmable Clock Output 0
#define AT91_PIO_PSR_SPI1_NPCS1XX (1<<10) // SPI 1 Chip Select 1 
#define AT91_PIO_PSR_SPI1_NPCS2XX (1<<11) // SPI 1 Chip Select 2 
#define AT91_PIO_PSR_TCLK0        (1<<12) // Timer/Counter 0 Clock Input
#define AT91_PIO_PSR_SPI_NPCS1    (1<<13) // SPI 0 Chip Select 1 
#define AT91_PIO_PSR_SPI_NPCS2    (1<<14) // SPI 0 Chip Select 2 
#define AT91_PIO_PSR_SPI1_NPCS3XX (1<<16) // SPI 1 Chip Select 3 
#define AT91_PIO_PSR_SPI_NPCS3XX  (1<<17) // SPI 0 Chip Select 3 
#define AT91_PIO_PSR_ADTRG        (1<<18) // ADC Trigger
#define AT91_PIO_PSR_TCLK1X       (1<<19) // Timer/Counter 1 Clock Input
#define AT91_PIO_PSR_PCK0X        (1<<20) // Programmable Clock Output 0
#define AT91_PIO_PSR_PCK1XX       (1<<21) // Programmable Clock Output 1
#define AT91_PIO_PSR_PCK2X        (1<<22) // Programmable Clock Output 2
#define AT91_PIO_PSR_DCD1         (1<<23) // USART 1 Data Carrier Detect
#define AT91_PIO_PSR_DSR1         (1<<24) // USART 1 Data Set Ready
#define AT91_PIO_PSR_DTR1         (1<<25) // USART 1 Data Terminal Ready
#define AT91_PIO_PSR_RI1          (1<<26) // USART 1 Ring Indication
#define AT91_PIO_PSR_PWM0X        (1<<27) // Pulse Width Modulation #0
#define AT91_PIO_PSR_PWM1X        (1<<28) // Pulse Width Modulation #1
#define AT91_PIO_PSR_PWM2X        (1<<29) // Pulse Width Modulation #2
#define AT91_PIO_PSR_PWM3X        (1<<30) // Pulse Width Modulation #3
#endif

#ifdef CYGHWR_HAL_ARM_AT91SAM7XC
#error Sorry, still missing. Happy typing
#endif

#else
#define AT91_TC_TCLK0    AT91_PIN(0,0, 0) // Timer #0 clock
#define AT91_TC_TIOA0    AT91_PIN(0,0, 1) // Timer #0 signal A
#define AT91_TC_TIOB0    AT91_PIN(0,0, 2) // Timer #0 signal B
#define AT91_TC_TCLK1    AT91_PIN(0,0, 3) // Timer #1 clock
#define AT91_TC_TIOA1    AT91_PIN(0,0, 4) // Timer #1 signal A
#define AT91_TC_TIOB1    AT91_PIN(0,0, 5) // Timer #1 signal B
#define AT91_TC_TCLK2    AT91_PIN(0,0, 6) // Timer #2 clock
#define AT91_TC_TIOA2    AT91_PIN(0,0, 7) // Timer #2 signal A
#define AT91_TC_TIOB2    AT91_PIN(0,0, 8) // Timer #2 signal B
#define AT91_INT_IRQ0    AT91_PIN(0,0, 9) // IRQ #0
#define AT91_INT_IRQ1    AT91_PIN(0,0,10) // IRQ #1
#define AT91_INT_IRQ2    AT91_PIN(0,0,11) // IRQ #2
#define AT91_INT_FIQ     AT91_PIN(0,0,12) // FIQ
#define AT91_USART_SCK0  AT91_PIN(0,0,13) // Serial port #0 clock
#define AT91_USART_TXD0  AT91_PIN(0,0,14) // Serial port #0 TxD
#define AT91_USART_RXD0  AT91_PIN(0,0,15) // Serial port #0 RxD
#define AT91_USART_SCK1  AT91_PIN(0,0,20) // Serial port #1 clock
#define AT91_USART_TXD1  AT91_PIN(0,0,21) // Serial port #1 TxD
#define AT91_USART_RXD1  AT91_PIN(0,0,22) // Serial port #1 RxD
#define AT91_CLK_MCKO    AT91_PIN(0,0,25) // Master clock out

#define AT91_PIO_PSR_TCLK0    0x00000001 // Timer #0 clock
#define AT91_PIO_PSR_TIOA0    0x00000002 // Timer #0 signal A
#define AT91_PIO_PSR_TIOB0    0x00000004 // Timer #0 signal B
#define AT91_PIO_PSR_TCLK1    0x00000008 // Timer #1 clock
#define AT91_PIO_PSR_TIOA1    0x00000010 // Timer #1 signal A
#define AT91_PIO_PSR_TIOB1    0x00000020 // Timer #1 signal B
#define AT91_PIO_PSR_TCLK2    0x00000040 // Timer #2 clock
#define AT91_PIO_PSR_TIOA2    0x00000080 // Timer #2 signal A
#define AT91_PIO_PSR_TIOB2    0x00000100 // Timer #2 signal B
#define AT91_PIO_PSR_IRQ0     0x00000200 // IRQ #0
#define AT91_PIO_PSR_IRQ1     0x00000400 // IRQ #1
#define AT91_PIO_PSR_IRQ2     0x00000800 // IRQ #2
#define AT91_PIO_PSR_FIQ      0x00001000 // FIQ
#define AT91_PIO_PSR_SCK0     0x00002000 // Serial port #0 clock
#define AT91_PIO_PSR_TXD0     0x00004000 // Serial port #0 TxD
#define AT91_PIO_PSR_RXD0     0x00008000 // Serial port #0 RxD
#define AT91_PIO_PSR_P16      0x00010000 // PIO port #16
#define AT91_PIO_PSR_P17      0x00020000 // PIO port #17
#define AT91_PIO_PSR_P18      0x00040000 // PIO port #18
#define AT91_PIO_PSR_P19      0x00080000 // PIO port #19
#define AT91_PIO_PSR_SCK1     0x00100000 // Serial port #1 clock
#define AT91_PIO_PSR_TXD1     0x00200000 // Serial port #1 TxD
#define AT91_PIO_PSR_RXD1     0x00400000 // Serial port #1 RxD
#define AT91_PIO_PSR_P23      0x00800000 // PIO port #23
#define AT91_PIO_PSR_P24      0x01000000 // PIO port #24
#define AT91_PIO_PSR_MCKO     0x02000000 // Master clock out
#define AT91_PIO_PSR_NCS2     0x04000000 // Chip select #2
#define AT91_PIO_PSR_NCS3     0x08000000 // Chip select #3
#define AT91_PIO_PSR_CS7_A20  0x10000000 // Chip select #7 or A20
#define AT91_PIO_PSR_CS6_A21  0x20000000 // Chip select #6 or A21
#define AT91_PIO_PSR_CS5_A22  0x40000000 // Chip select #5 or A22
#define AT91_PIO_PSR_CS4_A23  0x80000000 // Chip select #4 or A23
#endif

#define AT91_PIO_OER  0x10  // Output enable
#define AT91_PIO_ODR  0x14  // Output disable
#define AT91_PIO_OSR  0x18  // Output status
#define AT91_PIO_IFER 0x20  // Input Filter enable
#define AT91_PIO_IFDR 0x24  // Input Filter disable
#define AT91_PIO_IFSR 0x28  // Input Filter status
#define AT91_PIO_SODR 0x30  // Set out bits
#define AT91_PIO_CODR 0x34  // Clear out bits
#define AT91_PIO_ODSR 0x38  // Output data status
#define AT91_PIO_PDSR 0x3C  // Pin data status
#define AT91_PIO_IER  0x40  // Interrupt enable
#define AT91_PIO_IDR  0x44  // Interrupt disable
#define AT91_PIO_IMR  0x48  // Interrupt mask
#define AT91_PIO_ISR  0x4C  // Interrupt status

#if defined(CYGHWR_HAL_ARM_AT91SAM7)
#define AT91_PIO_MDER  0x50  // Multi-drive Enable Register
#define AT91_PIO_MDDR  0x54  // Multi-drive Disable Register
#define AT91_PIO_MDSR  0x58  // Multi-drive Status Register
#define AT91_PIO_PPUDR 0x60  // Pad Pull-up Disable Register
#define AT91_PIO_PPUER 0x64  // Pad Pull-up Enable Register
#define AT91_PIO_PPUSR 0x68  // Pad Pull-Up Status Register
#define AT91_PIO_ASR   0x70  // Select A Register
#define AT91_PIO_BSR   0x74  // Select B Regsiter
#define AT91_PIO_ABS   0x78  // AB Select Regsiter
#define AT91_PIO_OWER  0xa0  // Output Write Enable Register
#define AT91_PIO_OWDR  0xa4  // Output Write Disable Register
#define AT91_PIO_OWSR  0xa8  // Output Write Status Register
#endif // CYGHWR_HAL_ARM_AT91SAM7

//=============================================================================
// Advanced Interrupt Controller (AIC)

#ifndef AT91_AIC
#define AT91_AIC      0xFFFFF000
#endif

#define AT91_AIC_SMR0   ((0*4)+0x000)
#define AT91_AIC_SMR1   ((1*4)+0x000)
#define AT91_AIC_SMR2   ((2*4)+0x000)
#define AT91_AIC_SMR3   ((3*4)+0x000)
#define AT91_AIC_SMR4   ((4*4)+0x000)
#define AT91_AIC_SMR5   ((5*4)+0x000)
#define AT91_AIC_SMR6   ((6*4)+0x000)
#define AT91_AIC_SMR7   ((7*4)+0x000)
#define AT91_AIC_SMR8   ((8*4)+0x000)
#define AT91_AIC_SMR9   ((9*4)+0x000)
#define AT91_AIC_SMR10  ((10*4)+0x000)
#define AT91_AIC_SMR11  ((11*4)+0x000)
#define AT91_AIC_SMR12  ((12*4)+0x000)
#define AT91_AIC_SMR13  ((13*4)+0x000)
#define AT91_AIC_SMR14  ((14*4)+0x000)
#define AT91_AIC_SMR15  ((15*4)+0x000)
#define AT91_AIC_SMR16  ((16*4)+0x000)
#define AT91_AIC_SMR17  ((17*4)+0x000)
#define AT91_AIC_SMR18  ((18*4)+0x000)
#define AT91_AIC_SMR19  ((19*4)+0x000)
#define AT91_AIC_SMR20  ((20*4)+0x000)
#define AT91_AIC_SMR21  ((21*4)+0x000)
#define AT91_AIC_SMR22  ((22*4)+0x000)
#define AT91_AIC_SMR23  ((23*4)+0x000)
#define AT91_AIC_SMR24  ((24*4)+0x000)
#define AT91_AIC_SMR25  ((25*4)+0x000)
#define AT91_AIC_SMR26  ((26*4)+0x000)
#define AT91_AIC_SMR27  ((27*4)+0x000)
#define AT91_AIC_SMR28  ((28*4)+0x000)
#define AT91_AIC_SMR29  ((29*4)+0x000)
#define AT91_AIC_SMR30  ((30*4)+0x000)
#define AT91_AIC_SMR31  ((31*4)+0x000)
#define AT91_AIC_SMR_LEVEL_LOW  (0<<5)
#define AT91_AIC_SMR_LEVEL_HI   (2<<5)
#define AT91_AIC_SMR_EDGE_NEG   (1<<5)
#define AT91_AIC_SMR_EDGE_POS   (3<<5)
#define AT91_AIC_SMR_PRIORITY   0x07
#define AT91_AIC_SVR0   ((0*4)+0x080)
#define AT91_AIC_SVR1   ((1*4)+0x080)
#define AT91_AIC_SVR2   ((2*4)+0x080)
#define AT91_AIC_SVR3   ((3*4)+0x080)
#define AT91_AIC_SVR4   ((4*4)+0x080)
#define AT91_AIC_SVR5   ((5*4)+0x080)
#define AT91_AIC_SVR6   ((6*4)+0x080)
#define AT91_AIC_SVR7   ((7*4)+0x080)
#define AT91_AIC_SVR8   ((8*4)+0x080)
#define AT91_AIC_SVR9   ((9*4)+0x080)
#define AT91_AIC_SVR10  ((10*4)+0x080)
#define AT91_AIC_SVR11  ((11*4)+0x080)
#define AT91_AIC_SVR12  ((12*4)+0x080)
#define AT91_AIC_SVR13  ((13*4)+0x080)
#define AT91_AIC_SVR14  ((14*4)+0x080)
#define AT91_AIC_SVR15  ((15*4)+0x080)
#define AT91_AIC_SVR16  ((16*4)+0x080)
#define AT91_AIC_SVR17  ((17*4)+0x080)
#define AT91_AIC_SVR18  ((18*4)+0x080)
#define AT91_AIC_SVR19  ((19*4)+0x080)
#define AT91_AIC_SVR20  ((20*4)+0x080)
#define AT91_AIC_SVR21  ((21*4)+0x080)
#define AT91_AIC_SVR22  ((22*4)+0x080)
#define AT91_AIC_SVR23  ((23*4)+0x080)
#define AT91_AIC_SVR24  ((24*4)+0x080)
#define AT91_AIC_SVR25  ((25*4)+0x080)
#define AT91_AIC_SVR26  ((26*4)+0x080)
#define AT91_AIC_SVR27  ((27*4)+0x080)
#define AT91_AIC_SVR28  ((28*4)+0x080)
#define AT91_AIC_SVR29  ((29*4)+0x080)
#define AT91_AIC_SVR30  ((30*4)+0x080)
#define AT91_AIC_SVR31  ((31*4)+0x080)
#define AT91_AIC_IVR    0x100
#define AT91_AIC_FVR    0x104
#define AT91_AIC_ISR    0x108
#define AT91_AIC_IPR    0x10C
#define AT91_AIC_IMR    0x110
#define AT91_AIC_CISR   0x114
#define AT91_AIC_IECR   0x120
#define AT91_AIC_IDCR   0x124
#define AT91_AIC_ICCR   0x128
#define AT91_AIC_ISCR   0x12C
#define AT91_AIC_EOI    0x130
#define AT91_AIC_SVR    0x134

#ifdef CYGHWR_HAL_ARM_AT91SAM7
#define AT91_AIC_DCR    0x138 // Debug Control Register
#define AT91_AIC_FFER   0x140 // Fast Forcing Enable Register
#define AT91_AIC_FFDR   0x144 // Fast Forcing Enable Register
#define AT91_AIC_FFSR   0x148 // Fast Forcing Enable Register
#endif // CYGHWR_HAL_ARM_AT91SAM7

//=============================================================================
// Timer / counter

#ifndef AT91_TC
#define AT91_TC         0xFFFE0000
#endif

#define AT91_TC_TC0     0x00
#define AT91_TC_CCR     0x00
#define AT91_TC_CCR_CLKEN  0x01
#define AT91_TC_CCR_CLKDIS 0x02
#define AT91_TC_CCR_TRIG   0x04
// Channel Mode Register
#define AT91_TC_CMR		   0x04
#define AT91_TC_CMR_CLKS	   0
#define AT91_TC_CMR_CLKS_MCK2      (0<<0)
#define AT91_TC_CMR_CLKS_MCK8      (1<<0)
#define AT91_TC_CMR_CLKS_MCK32     (2<<0)
#define AT91_TC_CMR_CLKS_MCK128    (3<<0)
#define AT91_TC_CMR_CLKS_MCK1024   (4<<0)
#define AT91_TC_CMR_CLKS_XC0       (5<<0)
#define AT91_TC_CMR_CLKS_XC1       (6<<0)
#define AT91_TC_CMR_CLKS_XC2       (7<<0)
#define AT91_TC_CMR_CLKI           (1<<3)
#define AT91_TC_CMR_BURST_NONE     (0<<4)
#define AT91_TC_CMR_BURST_XC0      (1<<4)
#define AT91_TC_CMR_BURST_XC1      (2<<4)
#define AT91_TC_CMR_BURST_XC2      (3<<4)
// Capture mode definitions
#define AT91_TC_CMR_LDBSTOP        (1<<6)
#define AT91_TC_CMR_LDBDIS         (1<<7)
#define AT91_TC_CMR_TRIG_NONE      (0<<8)
#define AT91_TC_CMR_TRIG_NEG       (1<<8)
#define AT91_TC_CMR_TRIG_POS       (2<<8)
#define AT91_TC_CMR_TRIG_BOTH      (3<<8)
#define AT91_TC_CMR_EXT_TRIG_TIOB  (0<<10)
#define AT91_TC_CMR_EXT_TRIG_TIOA  (1<<10)
#define AT91_TC_CMR_CPCTRG         (1<<14)
#define AT91_TC_CMR_LDRA_NONE      (0<<16)
#define AT91_TC_CMR_LDRA_TIOA_NEG  (1<<16)
#define AT91_TC_CMR_LDRA_TIOA_POS  (2<<16)
#define AT91_TC_CMR_LDRA_TIOA_BOTH (3<<16)
#define AT91_TC_CMR_LDRB_NONE      (0<<18)
#define AT91_TC_CMR_LDRB_TIOA_NEG  (1<<18)
#define AT91_TC_CMR_LDRB_TIOA_POS  (2<<18)
#define AT91_TC_CMR_LDRB_TIOA_BOTH (3<<18)
// Waveform mode definitions
#define AT91_TC_CMR_CPCSTOP        (1<<6)
#define AT91_TC_CMR_CPCDIS	   (1<<7)
#define AT91_TC_CMR_EEVTEDG_NONE   (0<<8)
#define AT91_TC_CMR_EEVTEDG_NEG    (1<<8)
#define AT91_TC_CMR_EEVTEDG_POS    (2<<8)
#define AT91_TC_CMR_EEVTEDG_BOTH   (3<<8)
#define AT91_TC_CMR_EEVT_TIOB	   (0<<10)
#define AT91_TC_CMR_EEVT_XC0       (1<<10)
#define AT91_TC_CMR_EEVT_XC1       (2<<10)
#define AT91_TC_CMR_EEVT_XC2       (3<<10)
#define AT91_TC_CMR_ENETRG	   (1<<12)
#define AT91_TC_CMR_CPCTRG	   (1<<14)
#define AT91_TC_CMR_WAVE	   (1<<15)
#define AT91_TC_CMR_ACPA_NONE	   (0<<16)
#define AT91_TC_CMR_ACPA_SET	   (1<<16)
#define AT91_TC_CMR_ACPA_CLEAR	   (2<<16)
#define AT91_TC_CMR_ACPA_TOGGLE	   (3<<16)
#define AT91_TC_CMR_ACPC_NONE	   (0<<18)
#define AT91_TC_CMR_ACPC_SET	   (1<<18)
#define AT91_TC_CMR_ACPC_CLEAR	   (2<<18)
#define AT91_TC_CMR_ACPC_TOGGLE	   (3<<18)
#define AT91_TC_CMR_AEEVT_NONE	   (0<<20)
#define AT91_TC_CMR_AEEVT_SET	   (1<<20)
#define AT91_TC_CMR_AEEVT_CLEAR	   (2<<20)
#define AT91_TC_CMR_AEEVT_TOGGLE   (3<<20)
#define AT91_TC_CMR_ASWTRG_NONE	   (0<<22)
#define AT91_TC_CMR_ASWTRG_SET	   (1<<22)
#define AT91_TC_CMR_ASWTRG_CLEAR   (2<<22)
#define AT91_TC_CMR_ASWTRG_TOGGLE  (3<<22)
#define AT91_TC_CMR_BCPB_NONE	   (0<<24)
#define AT91_TC_CMR_BCPB_SET	   (1<<24)
#define AT91_TC_CMR_BCPB_CLEAR	   (2<<24)
#define AT91_TC_CMR_BCPB_TOGGLE	   (3<<24)
#define AT91_TC_CMR_BCPC_NONE	   (0<<26)
#define AT91_TC_CMR_BCPC_SET	   (1<<26)
#define AT91_TC_CMR_BCPC_CLEAR	   (2<<26)
#define AT91_TC_CMR_BCPC_TOGGLE	   (3<<26)
#define AT91_TC_CMR_BEEVT_NONE	   (0<<28)
#define AT91_TC_CMR_BEEVT_SET	   (1<<28)
#define AT91_TC_CMR_BEEVT_CLEAR	   (2<<28)
#define AT91_TC_CMR_BEEVT_TOGGLE   (3<<28)
#define AT91_TC_CMR_BSWTRG_NONE	   (0<<30)
#define AT91_TC_CMR_BSWTRG_SET	   (1<<30)
#define AT91_TC_CMR_BSWTRG_CLEAR   (2<<30)
#define AT91_TC_CMR_BSWTRG_TOGGLE  (3<<30)

#define AT91_TC_CV      0x10
#define AT91_TC_RA      0x14
#define AT91_TC_RB      0x18
#define AT91_TC_RC      0x1C
#define AT91_TC_SR      0x20
#define AT91_TC_SR_COVF    (1<<0)  // Counter overrun
#define AT91_TC_SR_LOVR    (1<<1)  // Load overrun
#define AT91_TC_SR_CPA     (1<<2)  // RA compare
#define AT91_TC_SR_CPB     (1<<3)  // RB compare
#define AT91_TC_SR_CPC     (1<<4)  // RC compare
#define AT91_TC_SR_LDRA    (1<<5)  // Load A status
#define AT91_TC_SR_LDRB    (1<<6)  // Load B status
#define AT91_TC_SR_EXT     (1<<7)  // External trigger
#define AT91_TC_SR_CLKSTA  (1<<16) // Clock enable/disable status
#define AT91_TC_SR_MTIOA   (1<<17) // TIOA mirror
#define AT91_TC_SR_MTIOB   (1<<18) // TIOB mirror
#define AT91_TC_IER     0x24
#define AT91_TC_IER_COVF   (1<<0)  // Counter overrun
#define AT91_TC_IER_LOVR   (1<<1)  // Load overrun
#define AT91_TC_IER_CPA    (1<<2)  // RA compare
#define AT91_TC_IER_CPB    (1<<3)  // RB compare
#define AT91_TC_IER_CPC    (1<<4)  // RC compare
#define AT91_TC_IER_LDRA   (1<<5)  // Load A status
#define AT91_TC_IER_LDRB   (1<<6)  // Load B status
#define AT91_TC_IER_EXT    (1<<7)  // External trigger
#define AT91_TC_IDR     0x28
#define AT91_TC_IMR     0x2C
#define AT91_TC_TC1     0x40
#define AT91_TC_TC2     0x80
#define AT91_TC_BCR     0xC0
#define AT91_TC_BCR_SYNC   0x01
#define AT91_TC_BMR     0xC4

//=============================================================================
// External Bus Interface

#ifndef AT91_EBI
#define AT91_EBI        0xFFE00000
#endif

#define AT91_EBI_CSR0 	0x00
#define AT91_EBI_CSR1 	0x04
#define AT91_EBI_CSR2 	0x08
#define AT91_EBI_CSR3 	0x0C
#define AT91_EBI_CSR4 	0x10
#define AT91_EBI_CSR5 	0x14
#define AT91_EBI_CSR6 	0x18
#define AT91_EBI_CSR7 	0x1C  	   // Chip select
#define AT91_EBI_CSR_DBW_16 0x1    // Data bus 16 bits wide
#define AT91_EBI_CSR_DBW_8  0x2    // Data bus  8 bits wide
#define AT91_EBI_CSR_NWS_1  (0x0 << 2)
#define AT91_EBI_CSR_NWS_2  (0x1 << 2)
#define AT91_EBI_CSR_NWS_3  (0x2 << 2)
#define AT91_EBI_CSR_NWS_4  (0x3 << 2)
#define AT91_EBI_CSR_NWS_5  (0x4 << 2)
#define AT91_EBI_CSR_NWS_6  (0x5 << 2)
#define AT91_EBI_CSR_NWS_7  (0x6 << 2)
#define AT91_EBI_CSR_NWS_8  (0x7 << 2)	// Number of wait states
#define AT91_EBI_CSR_WSE    (0x1 << 5)	// Wait state enable
#define AT91_EBI_CSR_PAGES_1M  (0x0 << 7)
#define AT91_EBI_CSR_PAGES_4M  (0x1 << 7)
#define AT91_EBI_CSR_PAGES_16M (0x2 << 7)
#define AT91_EBI_CSR_PAGES_64M (0x3 << 7) // Page size
#define AT91_EBI_CSR_TDF_0  (0x0 << 9)
#define AT91_EBI_CSR_TDF_1  (0x1 << 9)
#define AT91_EBI_CSR_TDF_2  (0x2 << 9)
#define AT91_EBI_CSR_TDF_3  (0x3 << 9)
#define AT91_EBI_CSR_TDF_4  (0x4 << 9)
#define AT91_EBI_CSR_TDF_5  (0x5 << 9)
#define AT91_EBI_CSR_TDF_6  (0x6 << 9)
#define AT91_EBI_CSR_TDF_7  (0x7 << 9)	// Data float output time
#define AT91_EBI_CSR_BAT    (0x1 << 12) // Byte access type
#define AT91_EBI_CSR_CSEN   (0x1 << 13) // Chip select enable
#define AT91_EBI_CSR_BA     (0xFFF << 20) // Base address
#define AT91_EBI_RCR    0x20       // Reset control
#define AT91_EBI_RCR_RCB    0x1    // Remap command bit
#define AT91_EBI_MCR  	0x24  	   // Memory control
#define AT91_EBI_MCR_ALE_16M  0x0
#define AT91_EBI_MCR_ALE_8M   0x4
#define AT91_EBI_MCR_ALE_4M   0x5
#define AT91_EBI_MCR_ALE_2M   0x6
#define AT91_EBI_MCR_ALE_1M   0x7   // Address line enable
#define AT91_EBI_MCR_DRP      (0x1 << 4)  // Data read protocol

//=============================================================================
// Power Saving or Management

#if defined(CYGHWR_HAL_ARM_AT91_R40807) || \
    defined(CYGHWR_HAL_ARM_AT91_R40008)

// Power Saving

#ifndef AT91_PS
#define AT91_PS         0xFFFF4000
#endif

#define AT91_PS_CR        0x000    // Control
#define AT91_PS_CR_CPU    (1<<0)   // Disable CPU clock (idle mode)
#define AT91_PS_PCER      0x004    // Peripheral clock enable
#define AT91_PS_PCDR      0x008    // Peripheral clock disable
#define AT91_PS_PCSR      0x00c    // Peripheral clock status

#elif defined(CYGHWR_HAL_ARM_AT91_M42800A) || \
      defined(CYGHWR_HAL_ARM_AT91_M55800A) || \
      defined(CYGHWR_HAL_ARM_AT91SAM7)

// (Advanced) Power Management

#ifndef AT91_PMC
#define AT91_PMC        0xFFFF4000
#endif

#define AT91_PMC_SCER           0x00
#define AT91_PMC_SCDR           0x04
#define AT91_PMC_SCSR           0x08

#define AT91_PMC_PCER           0x10
#define AT91_PMC_PCDR           0x14
#define AT91_PMC_PCSR           0x18

#define AT91_PMC_CGMR           0x20

#ifndef AT91_PMC_SR
#define AT91_PMC_SR             0x30
#endif

#ifndef AT91_PMC_IER
#define AT91_PMC_IER            0x34
#endif

#ifndef AT91_PMC_IDR
#define AT91_PMC_IDR            0x38
#endif

#ifndef AT91_PMC_IMR
#define AT91_PMC_IMR            0x3c
#endif

#if defined(CYGHWR_HAL_ARM_AT91_M42800A)

#define AT91_PMC_PCER_US0       (1<<2)
#define AT91_PMC_PCER_US1       (1<<3)
#define AT91_PMC_PCER_SPIA      (1<<4)
#define AT91_PMC_PCER_SPIB      (1<<5)
#define AT91_PMC_PCER_TC0       (1<<6)
#define AT91_PMC_PCER_TC1       (1<<7)
#define AT91_PMC_PCER_TC2       (1<<8)
#define AT91_PMC_PCER_TC3       (1<<9)
#define AT91_PMC_PCER_TC4       (1<<10)
#define AT91_PMC_PCER_TC5       (1<<11)
#define AT91_PMC_PCER_PIOA      (1<<13)
#define AT91_PMC_PCER_PIOB      (1<<14)
    
#define AT91_PMC_CGMR_PRES_NONE       0
#define AT91_PMC_CGMR_PRES_DIV2       1
#define AT91_PMC_CGMR_PRES_DIV4       2
#define AT91_PMC_CGMR_PRES_DIV8       3
#define AT91_PMC_CGMR_PRES_DIV16      4
#define AT91_PMC_CGMR_PRES_DIV32      5
#define AT91_PMC_CGMR_PRES_DIV64      6
#define AT91_PMC_CGMR_PRES_RES        7
#define AT91_PMC_CGMR_PLLA         0x00
#define AT91_PMC_CGMR_PLLB         0x08
#define AT91_PMC_CGMR_MCK_SLCK   (0<<4)
#define AT91_PMC_CGMR_MCK_MCK    (1<<4)
#define AT91_PMC_CGMR_MCK_MCKINV (2<<4)
#define AT91_PMC_CGMR_MCK_MCKD2  (3<<4)
#define AT91_PMC_CGMR_MCKO_ENA   (0<<6)
#define AT91_PMC_CGMR_MCKO_DIS   (1<<6)
#define AT91_PMC_CGMR_CSS_SLCK   (0<<7)
#define AT91_PMC_CGMR_CSS_PLL    (1<<7)

#define AT91_PMC_CGMR_PLL_MUL(x) ((x)<<8)
#define AT91_PMC_CGMR_PLL_CNT(x) ((x)<<24)

#define AT91_PMC_SR_LOCK        0x01
    
#elif defined(CYGHWR_HAL_ARM_AT91_M55800A)

#define AT91_PMC_PCER_US0       (1<<2)
#define AT91_PMC_PCER_US1       (1<<3)
#define AT91_PMC_PCER_US2       (1<<4)
#define AT91_PMC_PCER_SPI       (1<<5)
#define AT91_PMC_PCER_TC0       (1<<6)
#define AT91_PMC_PCER_TC1       (1<<7)
#define AT91_PMC_PCER_TC2       (1<<8)
#define AT91_PMC_PCER_TC3       (1<<9)
#define AT91_PMC_PCER_TC4       (1<<10)
#define AT91_PMC_PCER_TC5       (1<<11)
#define AT91_PMC_PCER_PIOA      (1<<13)
#define AT91_PMC_PCER_PIOB      (1<<14)
#define AT91_PMC_PCER_ADC0      (1<<15)
#define AT91_PMC_PCER_ADC1      (1<<16)
#define AT91_PMC_PCER_DAC0      (1<<17)
#define AT91_PMC_PCER_DAC1      (1<<18)

#define AT91_PMC_CGMR_MOSC_XTAL       0
#define AT91_PMC_CGMR_MOSC_BYP        1
#define AT91_PMC_CGMR_MOSC_DIS   (0<<1)
#define AT91_PMC_CGMR_MOSC_ENA   (1<<1)
#define AT91_PMC_CGMR_MCKO_ENA   (0<<2)
#define AT91_PMC_CGMR_MCKO_DIS   (1<<2)
#define AT91_PMC_CGMR_PRES_NONE  (0<<4)
#define AT91_PMC_CGMR_PRES_DIV2  (1<<4)
#define AT91_PMC_CGMR_PRES_DIV4  (2<<4)
#define AT91_PMC_CGMR_PRES_DIV8  (3<<4)
#define AT91_PMC_CGMR_PRES_DIV16 (4<<4)
#define AT91_PMC_CGMR_PRES_DIV32 (5<<4)
#define AT91_PMC_CGMR_PRES_DIV64 (6<<4)
#define AT91_PMC_CGMR_PRES_RES   (7<<4)
#define AT91_PMC_CGMR_CSS_LF     (0<<14)
#define AT91_PMC_CGMR_CSS_MOSC   (1<<14)
#define AT91_PMC_CGMR_CSS_PLL    (2<<14)
#define AT91_PMC_CGMR_CSS_RES    (3<<14)
    
#define AT91_PMC_CGMR_PLL_MUL(x) ((x)<<8)
#define AT91_PMC_CGMR_OSC_CNT(x) ((x)<<16)
#define AT91_PMC_CGMR_PLL_CNT(x) ((x)<<24)

#define AT91_PMC_PCR            0x28
#define AT91_PMC_PCR_SHDALC     1
#define AT91_PMC_PCR_WKACKC     2
    
#define AT91_PMC_PMR            0x2c
#define AT91_PMC_PMR_SHDALS_TRI         0
#define AT91_PMC_PMR_SHDALS_LEVEL0      1
#define AT91_PMC_PMR_SHDALS_LEVEL1      2
#define AT91_PMC_PMR_SHDALS_RES         3
#define AT91_PMC_PMR_WKACKS_TRI    (0<<2)
#define AT91_PMC_PMR_WKACKS_LEVEL0 (1<<2)
#define AT91_PMC_PMR_WKACKS_LEVEL1 (2<<2)
#define AT91_PMC_PMR_WKACKS_RES    (3<<2)
#define AT91_PMC_PMR_ALWKEN        (1<<4)
#define AT91_PMC_PMR_ALSHEN        (1<<5)

#define AT91_PMC_PMR_WKEDG_NONE    (0<<6)
#define AT91_PMC_PMR_WKEDG_POS     (1<<6)
#define AT91_PMC_PMR_WKEDG_NEG     (2<<6)
#define AT91_PMC_PMR_WKEDG_BOTH    (3<<6)

#define AT91_PMC_SR_MOSCS       0x01
#define AT91_PMC_SR_LOCK        0x02

#elif defined(CYGHWR_HAL_ARM_AT91_JTST)
// No power management control for the JTST

#elif defined(CYGHWR_HAL_ARM_AT91SAM7S)
#define AT91_PMC_SCER_PCK  (1 << 0) // Processor Clock
#define AT91_PMC_SCER_UDP  (1 << 7) // USB Device Clock
#define AT91_PMC_SCER_PCK0 (1 << 8) // Programmable Clock Output
#define AT91_PMC_SCER_PCK1 (1 << 9) // Programmable Clock Output
#define AT91_PMC_SCER_PCK2 (1 << 10) // Programmable Clock Output
#define AT91_PMC_SCER_PCK3 (1 << 11) // Programmable Clock Output

#define AT91_PMC_PCER_PIOA (1 << 2) // Parallel IO Controller
#define AT91_PMC_PCER_ADC  (1 << 4) // Analog-to-Digital Conveter
#define AT91_PMC_PCER_SPI  (1 << 5) // Serial Peripheral Interface
#define AT91_PMC_PCER_US0  (1 << 6) // USART 0
#define AT91_PMC_PCER_US1  (1 << 7) // USART 1
#define AT91_PMC_PCER_SSC  (1 << 8) // Serial Synchronous Controller
#define AT91_PMC_PCER_TWI  (1 << 9) // Two-Wire Interface
#define AT91_PMC_PCER_PWMC (1 <<10) // PWM Controller
#define AT91_PMC_PCER_UDP  (1 <<11) // USB Device Port
#define AT91_PMC_PCER_TC0  (1 <<12) // Timer Counter 0
#define AT91_PMC_PCER_TC1  (1 <<13) // Timer Counter 1
#define AT91_PMC_PCER_TC2  (1 <<14) // Timer Counter 2

#elif defined(CYGHWR_HAL_ARM_AT91SAM7X)
#define AT91_PMC_SCER_PCK  (1 << 0) // Processor Clock
#define AT91_PMC_SCER_UDP  (1 << 7) // USB Device Clock
#define AT91_PMC_SCER_PCK0 (1 << 8) // Programmable Clock Output
#define AT91_PMC_SCER_PCK1 (1 << 9) // Programmable Clock Output
#define AT91_PMC_SCER_PCK2 (1 << 10) // Programmable Clock Output
#define AT91_PMC_SCER_PCK3 (1 << 11) // Programmable Clock Output

#define AT91_PMC_PCER_PIOA (1 << 2) // Parallel IO Controller
#define AT91_PMC_PCER_PIOB (1 << 3) // Parallel IO Controller
#define AT91_PMC_PCER_SPI  (1 << 4) // Serial Peripheral Interface
#define AT91_PMC_PCER_SPI1 (1 << 5) // Serial Peripheral Interface
#define AT91_PMC_PCER_US0  (1 << 6) // USART 0
#define AT91_PMC_PCER_US1  (1 << 7) // USART 1
#define AT91_PMC_PCER_SSC  (1 << 8) // Serial Synchronous Controller
#define AT91_PMC_PCER_TWI  (1 << 9) // Two-Wire Interface
#define AT91_PMC_PCER_PWMC (1 <<10) // PWM Controller
#define AT91_PMC_PCER_UDP  (1 <<11) // USB Device Port
#define AT91_PMC_PCER_TC0  (1 <<12) // Timer Counter 0
#define AT91_PMC_PCER_TC1  (1 <<13) // Timer Counter 1
#define AT91_PMC_PCER_TC2  (1 <<14) // Timer Counter 2
#define AT91_PMC_PCER_CAN  (1 <<15) // Controller Area Network
#define AT91_PMC_PCER_EMAC (1 <<16) // Ethernet MAC
#define AT91_PMC_PCER_ADC  (1 <<17) // Analog-to-Digital Conveter
#else // Something unknown

#error Unknown AT91 variant

#endif 
#endif 

//=============================================================================
// Watchdog

#ifndef AT91_WD
#define AT91_WD             0xFFFF8000
#endif

#define AT91_WD_OMR         0x00
#define AT91_WD_OMR_WDEN    0x00000001
#define AT91_WD_OMR_RSTEN   0x00000002
#define AT91_WD_OMR_IRQEN   0x00000004
#define AT91_WD_OMR_EXTEN   0x00000008
#define AT91_WD_OMR_OKEY    (0x00000234 << 4)
#define AT91_WD_CMR         0x04
#define AT91_WD_CMR_WDCLKS  0x00000003
#define AT91_WD_CMR_HPCV    0x0000003C
#define AT91_WD_CMR_CKEY    (0x0000006E << 7)
#define AT91_WD_CR          0x08
#define AT91_WD_CR_RSTKEY   0x0000C071
#define AT91_WD_SR          0x0C
#define AT91_WD_SR_WDOVF    0x00000001

//=============================================================================
// SPI 

#ifndef AT91_SPI
#define AT91_SPI               0xFFFBC000
#endif

#define AT91_SPI_CR            0x00              // Control Register 
#define AT91_SPI_CR_SPIEN      0x00000001        // SPI Enable
#define AT91_SPI_CR_SPIDIS     0x00000002        // SPI Disable
#define AT91_SPI_CR_SWRST      0x00000080        // SPI Software reset
#define AT91_SPI_MR            0x04              // Mode Register
#define AT91_SPI_MR_MSTR       0x00000001        // Master/Slave Mode 
#define AT91_SPI_MR_PS         0x00000002        // Peripheral Select
#define AT91_SPI_MR_PCSDEC     0x00000004        // Chip Select Decode
#define AT91_SPI_MR_DIV32      0x00000008        // Clock Selection 
#define AT91_SPI_MR_LLB        0x00000080        // Local Loopback Enable
#define AT91_SPI_MR_PCS(x)     (((x)&0x0F)<<16)  // Peripheral Chip Select
#define AT91_SPI_MR_DLYBCS(x)  (((x)&0xFF)<<24)  // Delay Between Chip Selects
#define AT91_SPI_RDR           0x08              // Receive Data Register
#define AT91_SPI_TDR           0x0C              // Transmit Data Register
#define AT91_SPI_SR            0x10              // Status Register
#define AT91_SPI_SR_RDRF       0x00000001        // Receive Data Register Full
#define AT91_SPI_SR_TDRE       0x00000002        // Tx Data Register Empty
#define AT91_SPI_SR_MODF       0x00000004        // Mode Fault Error
#define AT91_SPI_SR_OVRES      0x00000008        // Overrun Error Status
#define AT91_SPI_SR_ENDRX      0x00000010        // End of Receiver Transfer
#define AT91_SPI_SR_ENDTX      0x00000020        // End of Transmitter Transfer
#define AT91_SPI_SR_SPIENS     0x00010000        // SPI Enable Status
#define AT91_SPI_IER           0x14              // Interrupt Enable Register
#define AT91_SPI_IDR           0x18              // Interrupt Disable Register
#define AT91_SPI_IMR           0x1C              // Interrupt Mask Register
// DMA registers are PDC registers
// can be different from target to target
#ifndef AT91_SPI_RPR
#define AT91_SPI_RPR           0x20              // Receive Pointer Register
#endif
#ifndef AT91_SPI_RCR
#define AT91_SPI_RCR           0x24              // Receive Counter Register
#endif
#ifndef AT91_SPI_TPR
#define AT91_SPI_TPR           0x28              // Transmit Pointer Register
#endif
#ifndef AT91_SPI_TCR
#define AT91_SPI_TCR           0x2C              // Transmit Counter Register
#endif

// PDC Control register bits
#define AT91_SPI_PTCR_RXTEN  (1 << 0)
#define AT91_SPI_PTCR_RXTDIS (1 << 1)
#define AT91_SPI_PTCR_TXTEN  (1 << 8)
#define AT91_SPI_PTCR_TXTDIS (1 << 9)

#define AT91_SPI_CSR0          0x30              // Chip Select Register 0
#define AT91_SPI_CSR1          0x34              // Chip Select Register 1
#define AT91_SPI_CSR2          0x38              // Chip Select Register 2
#define AT91_SPI_CSR3          0x3C              // Chip Select Register 3
#define AT91_SPI_CSR_CPOL      0x00000001        // Clock Polarity
#define AT91_SPI_CSR_NCPHA     0x00000002        // Clock Phase
#define AT91_SPI_CSR_BITS(x)   (((x)&0x0F)<<4)   // Bits Per Transfer
#define AT91_SPI_CSR_BITS8     AT91_SPI_CSR_BITS(0)
#define AT91_SPI_CSR_BITS9     AT91_SPI_CSR_BITS(1)
#define AT91_SPI_CSR_BITS10    AT91_SPI_CSR_BITS(2)
#define AT91_SPI_CSR_BITS11    AT91_SPI_CSR_BITS(3)
#define AT91_SPI_CSR_BITS12    AT91_SPI_CSR_BITS(4)
#define AT91_SPI_CSR_BITS13    AT91_SPI_CSR_BITS(5)
#define AT91_SPI_CSR_BITS14    AT91_SPI_CSR_BITS(6)
#define AT91_SPI_CSR_BITS15    AT91_SPI_CSR_BITS(7)
#define AT91_SPI_CSR_BITS16    AT91_SPI_CSR_BITS(8)
#define AT91_SPI_CSR_SCBR(x)   (((x)&0xFF)<<8)   // Serial Clock Baud Rate 
#define AT91_SPI_CSR_DLYBS(x)  (((x)&0xFF)<<16)  // Delay Before SPCK
#define AT91_SPI_CSR_DLYBCT(x) (((x)&0xFF)<<24)  // Delay Between two transfers

//=============================================================================
// Watchdog Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_WDTC
#define AT91_WDTC 0xFFFFFD40
#endif

#define AT91_WDTC_WDCR 0x00 // Watchdog Control Register
#define AT91_WDTC_WDCR_RELOAD  (1 << 0)  // Reload the watchdog
#define AT91_WDTC_WDCR_KEY     (0xa5 << 24) // Password for the write op
#define AT91_WDTC_WDMR 0x04 // Watchdog Mode Register
#define AT91_WDTC_WDMR_FIEN    (1 << 12) // Fault Interrupt Mode Enable
#define AT91_WDTC_WDMR_RSTEN   (1 << 13) // Reset Enable
#define AT91_WDTC_WDMR_RPROC   (1 << 14) // Trigger a processor reset
#define AT91_WDTC_WDMR_DIS     (1 << 15) // Disable
#define AT91_WDTC_WDMR_WDD_SHIFT (16)    // Delta Value shift
#define AT91_WDTC_WDMR_DBGHLT  (1 << 28) // Stop when in debug state
#define AT91_WDTC_WDMR_IDLEHLT (1 << 29) // Stop when in idle more
#define AT91_WDTC_WDSR 0x08 // Watchdog Status Register
#define AT91_WDTC_WDSR_UNDER   (1 << 0)  // Underflow has occurred
#define AT91_WDTC_WDSR_ERROR   (1 << 1)  // Error has occurred
#endif //CYGHWR_HAL_ARM_AT91SAM7 

//=============================================================================
// Reset Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_RST
#define AT91_RST 0xFFFFFD00
#endif

#define AT91_RST_RCR 0x00 // Reset Control Register
#define AT91_RST_RCR_PROCRST  (1 << 0) // Processor Reset
#define AT91_RST_RCR_ICERST   (1 << 1) // ICE Reset
#define AT91_RST_RCR_PERRST   (1 << 2) // Peripheral Reset
#define AT91_RST_RCR_EXTRST   (1 << 3) // External Reset
#define AT91_RST_RCR_KEY      (0xA5 << 24) // Key
#define AT91_RST_RSR 0x04 // Reset Status Register
#define AT91_RST_RSR_USER          (1 << 0) // User Reset
#define AT91_RST_RSR_BROWN         (1 << 1) // Brownout detected
#define AT91_RST_RSR_TYPE_POWERUP  (0 << 8) // Power on Reset
#define AT91_RST_RSR_TYPE_WATCHDOG (2 << 8) // Watchdog Reset
#define AT91_RST_RSR_TYPE_SW       (3 << 8) // Software Reset
#define AT91_RST_RSR_TYPE_USER     (4 << 8) // NRST pin Reset
#define AT91_RST_RSR_TYPE_BROWNOUT (5 << 8) // Brown-out Reset
#define AT91_RST_RSR_NRST_SET (1 << 16) // NRST pin set
#define AT91_RST_RSR_SRCMP    (1 << 17) // Software reset in progress
#define AT91_RST_RMR 0x08 // Reset Mode Register
#define AT91_RST_RMR_URSTEN  (1 << 0)  // User Reset Enabled
#define AT91_RST_RMR_URSTIEN (1 << 4)  // User Reset Interrupt Enabled
#define AT91_RST_RMR_BODIEN  (1 << 16) // Brownout Dection Interrupt Enabled
#define AT91_RST_RMR_KEY     (0xA5 << 24) // Key

#endif

//=============================================================================
// Memory Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_MC
#define AT91_MC 0xFFFFFF00
#endif

#define AT91_MC_RCR  0x00 // Remap Control Register
#define AT91_MC_ASR  0x04 // Abort Status Register
#define AT91_MC_AASR 0x08 // Abort Address Status Register
#define AT91_MC_FMR  0x60 // Flash Mode Register
#define AT91_MC_FMR_FRDY  (1 << 0) // Enable interrupt for Flash Ready
#define AT91_MC_FMR_LOCKE (1 << 2) // Enable interrupt for Flash Lock Error
#define AT91_MC_FMR_PROGE (1 << 3) // Enable interrupt for Flash Prog Error
#define AT91_MC_FMR_NEBP  (1 << 7) // No erase before programming
#define AT91_MC_FMR_0FWS  (0 << 8) // 1R,2W wait states
#define AT91_MC_FMR_1FWS  (1 << 8) // 2R,3W wait states
#define AT91_MC_FMR_2FWS  (2 << 8) // 3R,4W wait states
#define AT91_MC_FMR_3FWS  (3 << 8) // 4R,4W wait states
#define AT91_MC_FMR_FMCN_MASK (0xff << 16)
#define AT91_MC_FMR_FMCN_SHIFT 16
#define AT91_MC_FCR  0x64 // Flash Command Register
#define AT91_MC_FCR_START_PROG (0x1 << 0) // Start Programming of Page
#define AT91_MC_FCR_LOCK       (0x2 << 0) // Lock sector
#define AT91_MC_FCR_PROG_LOCK  (0x3 << 0) // Program and Lock
#define AT91_MC_FCR_UNLOCK     (0x4 << 0) // Unlock a segment
#define AT91_MC_FCR_ERASE_ALL  (0x8 << 0) // Erase everything
#define AT91_MC_FCR_SET_GP_NVM (0xb << 0) // Set general purpose NVM bits
#define AT91_MC_FCR_CLR_GP_NVM (0xd << 0) // Clear general purpose NVM bits
#define AT91_MC_FCR_SET_SECURITY (0xf << 0) // Set security bit
#define AT91_MC_FCR_PAGE_MASK  (0x3ff)
#define AT91_MC_FCR_PAGE_SHIFT 8
#define AT91_MC_FCR_KEY        (0x5a << 24) // Key to enable command
#define AT91_MC_FSR  0x68 // Flash Status Register
#define AT91_MC_FSR_FRDY       (1 << 0) // Flash Ready for next command
#define AT91_MC_FSR_LOCKE      (1 << 2) // Programming of a locked block
#define AT91_MC_FSR_PROGE      (1 << 3) // Programming error
#define AT91_MC_FSR_SECURITY   (1 << 4) // Security bit is set
#define AT91_MC_FSR_GPNVM0     (1 << 8) // General purpose NVM bit 0
#define AT91_MC_FSR_GPNVM1     (1 << 9) // General purpose NVM bit 1
#endif

//=============================================================================
// Debug Unit

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_DBG
#define AT91_DBG 0xFFFFF200
#endif

#define AT91_DBG_CR   0x00 // Control Register
#define AT91_DBG_CR_RSTRX  (0x1 << 2)   // Reset Receiver
#define AT91_DBG_CR_RSTTX  (0x1 << 3)   // Reset Transmitter
#define AT91_DBG_CR_RXEN   (0x1 << 4)   // Receiver Enable
#define AT91_DBG_CR_RXDIS  (0x1 << 5)   // Receiver Disable
#define AT91_DBG_CR_TXEN   (0x1 << 6)   // Transmitter Enable
#define AT91_DBG_CR_TXDIS  (0x1 << 7)   // Transmitter Disable
#define AT91_DBG_CR_RSTSTA (0x1 << 8)   // Reset Status Bits
#define AT91_DBG_MR   0x04 // Mode Register
#define AT91_DBG_MR_PAR_EVEN  (0x0 << 9) // Even Parity
#define AT91_DBG_MR_PAR_ODD   (0x1 << 9) // Odd Parity
#define AT91_DBG_MR_PAR_SPACE (0x2 << 9) // Parity forced to Space
#define AT91_DBG_MR_PAR_MARK  (0x3 << 9) // Parity forced to Mark
#define AT91_DBG_MR_PAR_NONE  (0x4 << 9) // No Parity
#define AT91_DBG_MR_PAR_MULTI (0x6 << 9) // Multi-drop mode
#define AT91_DBG_MR_CHMODE_NORMAL  (0x0 << 14) // Normal mode
#define AT91_DBG_MR_CHMODE_AUTO    (0x1 << 14) // Automatic Echo
#define AT91_DBG_MR_CHMODE_LOCAL   (0x2 << 14) // Local Loopback
#define AT91_DBG_MR_CHMODE_REMOTE  (0x3 << 14) // Remote Loopback
#define AT91_DBG_IER  0x08 // Interrupt Enable Register
#define AT91_DBG_IDR  0x0c // Interrupt Disable Register
#define AT91_DBG_IMR  0x10 // Interrupt Mask Register
#define AT91_DBG_CSR  0x14 // Channel Status Register
#define AT91_DBG_CSR_RXRDY  (1 << 0) // Receiver Ready
#define AT91_DBG_CSR_TXRDY  (1 << 1) // Transmitter Ready
#define AT91_DBG_RHR  0x18 // Receiver Holding Register
#define AT91_DBG_THR  0x1c // Transmitter Holding Register
#define AT91_DBG_BRGR 0x20 // Baud Rate Generator Register
#define AT91_DBG_C1R  0x40 // Chip ID1 register
#define AT91_DBG_C1R_ARM945ES (1 << 5) 
#define AT91_DBG_C1R_ARM7TDMI (2 << 5)
#define AT91_DBG_C1R_ARM920T  (4 << 5)
#define AT91_DBG_C1R_ARM926EJ (5 << 5)
#define AT91_DBG_C1R_CPU_MASK  (0x7 << 5)
#define AT91_DBG_C1R_FLASH_0K    (0x0 << 8)
#define AT91_DBG_C1R_FLASH_8K    (0x1 << 8)
#define AT91_DBG_C1R_FLASH_16K   (0x2 << 8)
#define AT91_DBG_C1R_FLASH_32K   (0x3 << 8)
#define AT91_DBG_C1R_FLASH_64K   (0x5 << 8)
#define AT91_DBG_C1R_FLASH_128K  (0x7 << 8)
#define AT91_DBG_C1R_FLASH_256K  (0x9 << 8)
#define AT91_DBG_C1R_FLASH_512K  (0xa << 8)
#define AT91_DBG_C1R_FLASH_1024K (0xc << 8)
#define AT91_DBG_C1R_FLASH_2048K (0xe << 8)
#define AT91_DBG_C1R_FLASH_MASK  (0xf << 8)
#define AT91_DBG_C1R_FLASH2_0K    (0x0 << 12)
#define AT91_DBG_C1R_FLASH2_8K    (0x1 << 12)
#define AT91_DBG_C1R_FLASH2_16K   (0x2 << 12)
#define AT91_DBG_C1R_FLASH2_32K   (0x3 << 12)
#define AT91_DBG_C1R_FLASH2_64K   (0x5 << 12)
#define AT91_DBG_C1R_FLASH2_128K  (0x7 << 12)
#define AT91_DBG_C1R_FLASH2_256K  (0x9 << 12)
#define AT91_DBG_C1R_FLASH2_512K  (0xa << 12)
#define AT91_DBG_C1R_FLASH2_1024K (0xc << 12)
#define AT91_DBG_C1R_FLASH2_2048K (0xe << 12)
#define AT91_DBG_C1R_FLASH2_MASK  (0xf << 12)
#define AT91_DBG_C1R_SRAM_1K      (0x1 << 16)  
#define AT91_DBG_C1R_SRAM_2K      (0x2 << 16)
#define AT91_DBG_C1R_SRAM_112K    (0x4 << 16)
#define AT91_DBG_C1R_SRAM_4K      (0x5 << 16)
#define AT91_DBG_C1R_SRAM_80K     (0x6 << 16)
#define AT91_DBG_C1R_SRAM_160K    (0x7 << 16)
#define AT91_DBG_C1R_SRAM_8K      (0x8 << 16)
#define AT91_DBG_C1R_SRAM_16K     (0x9 << 16)
#define AT91_DBG_C1R_SRAM_32K     (0xa << 16)
#define AT91_DBG_C1R_SRAM_64K     (0xb << 16)
#define AT91_DBG_C1R_SRAM_128K    (0xc << 16)
#define AT91_DBG_C1R_SRAM_256K    (0xd << 16)
#define AT91_DBG_C1R_SRAM_96K     (0xe << 16)
#define AT91_DBG_C1R_SRAM_512K    (0xf << 16)
#define AT91_DBG_C1R_SRAM_MASK    (0xf << 16)
#define AT91_DBG_C1R_ARCH_AT75Cxx (0xf0 << 20)
#define AT91_DBG_C1R_ARCH_AT91x40 (0x40 << 20)
#define AT91_DBG_C1R_ARCH_AT91x63 (0x63 << 20)
#define AT91_DBG_C1R_ARCH_AT91x55 (0x55 << 20)
#define AT91_DBG_C1R_ARCH_AT91x42 (0x42 << 20)
#define AT91_DBG_C1R_ARCH_AT91x92 (0x92 << 20)
#define AT91_DBG_C1R_ARCH_AT91x34 (0x24 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7Axx  (0x60 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7Sxx  (0x70 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7XC   (0x71 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7SExx (0x72 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7Lxx  (0x73 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM7Xxx  (0x75 << 20)
#define AT91_DBG_C1R_ARCH_AT91SAM9xx   (0x19 << 20)
#define AT91_DBG_C1R_ARCH_MASK         (0xff << 20)
#define AT91_DBG_C1R_NVPTYP_ROM      (0 << 28) // ROM only
#define AT91_DBG_C1R_NVPTYP_RLOCF    (1 << 28) // ROMless of on chip Flash
#define AT91_DBG_C1R_NVPTYP_SRAMROM  (4 << 28) // SRAM emulating ROM
#define AT91_DBG_C1R_NVPTYP_EFLASH   (2 << 28) // Embedded Flash
#define AT91_DBG_C1R_NVPTYP_ROMFLASH (3 << 28) // ROM & FLASH
#define AT91_DBG_C1R_NVPTYP_MASK     (7 << 28)
#define AT91_DBG_C1R_EXT (1 << 31) // Extension Register Exists
#define AT91_DBG_C2R  0x44 // Chip ID2 register
#define AT91_DBG_FNTR 0x48 // Force NTRST Register
#define AT91_DBG_RPR  0x100 // Receiver Pointer Register
#define AT91_DBG_RCR  0x104 // Receiver Counter Register
#define AT91_DBG_TPR  0x108 // Transmit Pointer Register
#define AT91_DBG_TCR  0x10c // Transmit Counter Register
#define AT91_DBG_RNPR 0x110 // Receiver Next Pointer Register
#define AT91_DBG_RNCR 0x114 // Receiver Next Counter Register
#define AT91_DBG_TNPR 0x118 // Transmit Next Pointer Register
#define AT91_DBG_TNCR 0x11c // Transmit Next Counter Register
#define AT91_DBG_PTCR 0x120 // PDC Transfer Control Register
#define AT91_DBG_PTSR 0x124 // PDC Transfer Status Register
#endif

//=============================================================================
// Periodic Interval Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_PITC
#define AT91_PITC 0xfffffd30
#endif

#define AT91_PITC_PIMR 0x00  // Period Interval Mode Register
#define AT91_PITC_PIMR_PITEN  (1 << 24) // Periodic Interval Timer Enable
#define AT91_PITC_PIMR_PITIEN (1 << 25) // Periodic Interval Timer Intr Enable
#define AT91_PITC_PISR 0x04  // Period Interval Status Register
#define AT91_PITC_PISR_PITS   (1 << 0)  // Periodic Interval Timer Status
#define AT91_PITC_PIVR 0x08  // Period Interval Status Register
#define AT91_PITC_PIIR 0x0C  // Period Interval Image Register
#endif

//=============================================================================
// Real Time Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_RTTC
#define AT91_RTTC 0xFFFFFD20
#endif

#define AT91_RTTC_RTMR 0x00 // Real Time Mode Register
#define AT91_RTTC_RTMR_ALMIEN    (1 << 16) // Alarm Interrupt Enable
#define AT91_RTTC_RTMR_RTTINCIEN (1 << 17) // Timer Increment Interrupt Enable
#define AT91_RTTC_RTMR_RTTRST    (1 << 18) // Timer Reset
#define AT91_RTTC_RTAR 0x04 // Real Time Alarm Register
#define AT91_RTTC_RTVR 0x08 // Real Time Value Register
#define AT91_RTTC_RTSR 0x0C // Real Time Status Register
#define AT91_RTTC_RTSR_ALMS      (1 << 0) // Alarm Status
#define AT91_RTTC_RTSR_RTTINC    (1 << 1) // Timer Increment
#endif

//=============================================================================
// USB Device Port

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_UDP
#define AT91_UDP 0xFFFB0000
#endif

#define AT91_UDP_FRM_NUM    0x00  // Frame Number
#define AT91_UDP_FRM_ERR     (1 << 16) // Frame Error
#define AT91_UDP_FRM_OK      (1 << 17) // Frame OK
#define AT91_UDP_GLB_STATE  0x04  // Global State
#define AT91_UDP_GLB_FADDEN  (1 <<  0) // Function Address Enable
#define AT91_UDP_GLB_CONFG   (1 <<  1) // Configured
#define AT91_UDP_GLB_ESR     (1 <<  2) // Enable Send Resume
#define AT91_UDP_GLB_RSMINPR (1 <<  3) // A Resume has been seen
#define AT91_UDP_GLB_RMWUPE  (1 <<  4) // Remote Wake Up Enable
#define AT91_UDP_FADDR      0x08  // Function Address
#define AT91_UDP_FADDR_FEN   (1 <<  8) // Function Enable
#define AT91_UDP_IER        0x10  // Interrupt Enable
#define AT91_UDP_EPINT0      (1 <<  0) // Endpoint 0 Interrupt
#define AT91_UDP_EPINT1      (1 <<  1) // Endpoint 1 Interrupt
#define AT91_UDP_EPINT2      (1 <<  2) // Endpoint 2 Interrupt
#define AT91_UDP_EPINT3      (1 <<  3) // Endpoint 3 Interrupt
#define AT91_UDP_EPINT4      (1 <<  4) // Endpoint 4 Interrupt
#define AT91_UDP_EPINT5      (1 <<  5) // Endpoint 5 Interrupt
#define AT91_UDP_EPINT6      (1 <<  6) // Endpoint 6 Interrupt
#define AT91_UDP_EPINT7      (1 <<  7) // Endpoint 7 Interrupt
#define AT91_UDP_RXSUSP      (1 <<  8) // USB Suspend Interrupt
#define AT91_UDP_RXRSM       (1 <<  9) // USB Resume Interrupt
#define AT91_UDP_EXTRSM      (1 << 10) // USB External Resume Interrupt
#define AT91_UDP_SOFINT      (1 << 11) // USB start of frame Interrupt
#define AT91_UDP_ENDBUSRES   (1 << 12) // USB End of Bus Reset Interrupt
#define AT91_UDP_WAKEUP      (1 << 13) // USB Resume Interrupt
#define AT91_UDP_IDR        0x14  // Interrupt Disable
#define AT91_UDP_IMR        0x18  // Interrupt Mask
#define AT91_UDP_ISR        0x1C  // Interrupt Status
#define AT91_UDP_ICR        0x20  // Interrupt Clear
#define AT91_UDP_RST_EP     0x28  // Reset Endpoint
#define AT91_UDP_CSR        0x30  // Endpoint Control and Status
#define AT91_UDP_CSR_TXCOMP      (1 <<  0) // Generates an IN packet
#define AT91_UDP_CSR_RX_DATA_BK0 (1 <<  1) // Receive Data Bank 0
#define AT91_UDP_CSR_RXSETUP     (1 <<  2) // Sends a STALL to the host
#define AT91_UDP_CSR_ISOERROR    (1 <<  3) // Isochronous error
#define AT91_UDP_CSR_TXPKTRDY    (1 <<  4) // Transmit Packet Ready
#define AT91_UDP_CSR_FORCESTALL  (1 <<  5) // Force Stall
#define AT91_UDP_CSR_RX_DATA_BK1 (1 <<  6) // Receive Data Bank 1
#define AT91_UDP_CSR_DIR         (1 <<  7) // Transfer Direction
#define AT91_UDP_CSR_DIR_OUT     (0 <<  7) // Transfer Direction OUT
#define AT91_UDP_CSR_DIR_IN      (1 <<  7) // Transfer Direction IN
#define AT91_UDP_CSR_EPTYPE_CTRL     (0 << 8) // Control
#define AT91_UDP_CSR_EPTYPE_ISO_OUT  (1 << 8) // Isochronous OUT
#define AT91_UDP_CSR_EPTYPE_BULK_OUT (2 << 8) // Bulk OUT
#define AT91_UDP_CSR_EPTYPE_INT_OUT  (3 << 8) // Interrupt OUT
#define AT91_UDP_CSR_EPTYPE_ISO_IN   (5 << 8) // Isochronous IN
#define AT91_UDP_CSR_EPTYPE_BULK_IN  (6 << 8) // Bulk IN
#define AT91_UDP_CSR_EPTYPE_INT_IN   (7 << 8) // Interrupt IN
#define AT91_UDP_CSR_DTGLE       (1 << 11) // Data Toggle
#define AT91_UDP_CSR_EPEDS       (1 << 15) // Endpoint Enable Disable
#define AT91_UDP_FDR        0x50  // Endpoint FIFO Data
#define AT91_UDP_TXVC       0x74  // Transceiver Control
#define AT91_UDP_TXVC_TXVDIS     (1 <<  8) // Disable Transceiver
#define AT91_UDP_TXVC_PUON       (1 <<  9) // Pull-up ON
#endif

//=============================================================================
// Synchronous Serial Controller (SSC)

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_SSC
#define AT91_SSC 0xFFFD4000
#endif

#define AT91_SSC_CR   (0x00)
#define AT91_SSC_CR_RXEN   (1<<0)  //Enable Receiver
#define AT91_SSC_CR_RXDIS  (1<<1)  //Disable Receiver
#define AT91_SSC_CR_TXEN   (1<<8)  //Enable Transmitter
#define AT91_SSC_CR_TXDIS  (1<<9)  //Disable Transmitter
#define AT91_SSC_CR_SWRST  (1<<15) //Soft Reset
#define AT91_SSC_CMR  (0x04)
#define AT91_SSC_RCMR (0x10)
#define AT91_SSC_RCMR_CKS_DIV    (0<<0)  //Select Divider Clock
#define AT91_SSC_RCMR_CKS_TX     (1<<0)  //Select Transmit Clock
#define AT91_SSC_RCMR_CKS_RK     (2<<0)  //Select Receiver Clock
#define AT91_SSC_RCMR_CKO_NONE   (0<<2)  //No Clock Output
#define AT91_SSC_RCMR_CKO_CONT   (1<<2)  //Continuous Clock Output
#define AT91_SSC_RCMR_CKO_TFER   (2<<2)  //Clock Output During Transfer only
#define AT91_SSC_RCMR_CKI        (1<<5)  //Clock Invert
#define AT91_SSC_RCMR_CKG_NONE   (0<<6)  //No Clock Gating, Continuous Clock
#define AT91_SSC_RCMR_CKG_RFLOW  (1<<6)  //Clock Enabled by RF Low
#define AT91_SSC_RCMR_CKG_RFHIGH (2<<6)  //Clock Enabled by RF HIGH
#define AT91_SSC_RCMR_START_CONT    (0<<8) //Start when data in RHR, Continuous
#define AT91_SSC_RCMR_START_TX      (1<<8) //Start when TX Start
#define AT91_SSC_RCMR_START_RFLOW   (2<<8) //Start when LOW level on RF 
#define AT91_SSC_RCMR_START_RFHIGH  (3<<8) //Start when HIGH level on RF
#define AT91_SSC_RCMR_START_RFFALL  (4<<8) //Start when Falling Edge on RF
#define AT91_SSC_RCMR_START_RFRISE  (5<<8) //Start when Rising Edge on RF 
#define AT91_SSC_RCMR_START_RFLEVEL (6<<8) //Start when any Level Change on RF 
#define AT91_SSC_RCMR_START_RFEDGE  (7<<8) //Start when any Edge on RF
#define AT91_SSC_RCMR_START_CMP0    (8<<8) //Start when Compare 0 match 
#define AT91_SSC_RCMR_STOP_CMP1  (1<<12)   //Stop when Compare 1 Match
#define AT91_SSC_RCMR_STTDLY(x)  ((x&0xFF)<<16) //Start Delay
#define AT91_SSC_RCMR_PERIOD(x)  ((x&0xFF)<<24) //Frame Period
#define AT91_SSC_RFMR (0x14)
#define AT91_SSC_RFMR_DATLEN(x)  (x&0x1F) //Data word length 
#define AT91_SSC_RFMR_LOOP       (1<<5)   //Loop Mode
#define AT91_SSC_RFMR_MSBF       (1<<7)   //MSB First 
#define AT91_SSC_RFMR_DATNB(x)   ((x&0xf)<<8)  //Data Number, # words per frame
#define AT91_SSC_RFMR_FSLEN(x)   ((x&0xf)<<16) //Frame sync length
#define AT91_SSC_RFMR_FSOS_NONE     (0<<16) //No Frame Synch Output
#define AT91_SSC_RFMR_FSOS_NEGPULSE (1<<16) //Negative Pulse Frame Sync Output 
#define AT91_SSC_RFMR_FSOS_POSPULSE (2<<16) //Positive Pulse Frame Sync Output
#define AT91_SSC_RFMR_FSOS_LOW      (3<<16) //Low Level Frame Synch Output  
#define AT91_SSC_RFMR_FSOS_HIGH     (4<<16) //High Level Frame Synch Output   
#define AT91_SSC_RFMR_FSOS_TOGGLE   (5<<16) //Toggle Frame Synch Output    
#define AT91_SSC_RFMR_FSEDGE_POS    (0<<24) //Intr on +ve edge of Frame Sync
#define AT91_SSC_RFMR_FSEDGE_NEG    (1<<24) //Intr on -ve edge of Frame Sync 
#define AT91_SSC_TCMR (0x18)
#define AT91_SSC_TCMR_CKS_DIV    (0<<0) //Select Divider Clock               
#define AT91_SSC_TCMR_CKS_TX     (1<<0) //Select Transmit Clock              
#define AT91_SSC_TCMR_CKS_RK     (2<<0) //Select Receiver Clock              
#define AT91_SSC_TCMR_CKO_NONE   (0<<2) //No Clock Output                    
#define AT91_SSC_TCMR_CKO_CONT   (1<<2) //Continuous Clock Output            
#define AT91_SSC_TCMR_CKO_TFER   (2<<2) //Clock Output During Transfer only  
#define AT91_SSC_TCMR_CKI        (1<<5) //Clock Invert                       
#define AT91_SSC_TCMR_CKG_NONE   (0<<6) //No Clock Gating, Continuous Clock  
#define AT91_SSC_TCMR_CKG_RFLOW  (1<<6) //Clock Enabled by RF Low            
#define AT91_SSC_TCMR_CKG_RFHIGH (2<<6) //Clock Enabled by RF HIGH           
#define AT91_SSC_TCMR_START_CONT    (0<<8) //Start when data in THR, Continuous
#define AT91_SSC_TCMR_START_TX      (1<<8) //Start when TX Start          
#define AT91_SSC_TCMR_START_RFLOW   (2<<8) //Start when LOW level on RF
#define AT91_SSC_TCMR_START_RFHIGH  (3<<8) //Start when HIGH level on RF
#define AT91_SSC_TCMR_START_RFFALL  (4<<8) //Start when Falling Edge on RF
#define AT91_SSC_TCMR_START_RFRISE  (5<<8) //Start when Rising Edge on RF
#define AT91_SSC_TCMR_START_RFLEVEL (6<<8) //Start when any Level Change on RF
#define AT91_SSC_TCMR_START_RFEDGE  (6<<8) //Start when any Edge on RF
#define AT91_SSC_TCMR_STDDLY(x)  ((x&0xFF)<<16) //Start Delay 
#define AT91_SSC_TCMR_PERIOD(x)  ((x&0xFF)<<24) //Frame Period
#define AT91_SSC_TFMR (0x1C)
#define AT91_SSC_TFMR_DATLEN(x)  (x&0x1F) //Data word length  
#define AT91_SSC_TFMR_DATDEF     (1<<5)   //Default Data is 1's
#define AT91_SSC_TFMR_MSBF       (1<<7)   //MSB First 
#define AT91_SSC_TFMR_DATNB(x)   ((x&0xf)<<8)  //Data Number, # words per frame
#define AT91_SSC_TFMR_FSLEN(x)   ((x&0xf)<<16) //Frame sync length
#define AT91_SSC_TFMR_FSOS_NONE     (0<<16) //No Frame Synch Output
#define AT91_SSC_TFMR_FSOS_NEGPULSE (1<<16) //Negative Pulse Frame Sync Output
#define AT91_SSC_TFMR_FSOS_POSPULSE (2<<16) //Positive Pulse Frame Sync Output
#define AT91_SSC_TFMR_FSOS_LOW      (3<<16) //Low Level Frame Synch Output
#define AT91_SSC_TFMR_FSOS_HIGH     (4<<16) //High Level Frame Synch Output
#define AT91_SSC_TFMR_FSOS_TOGGLE   (5<<16) //Toggle Frame Synch Output
#define AT91_SSC_RFMR_FSDEN_DEF     (0<<23) //Frame Sync is Default Data
#define AT91_SSC_RFMR_FSDEN_TSHR    (1<<23) //Frame Sync is TSHR Data
#define AT91_SSC_RFMR_FSEDGE_POS    (0<<24) //Intr on +ve edge of Frame Sync
#define AT91_SSC_RFMR_FSEDGE_NEG    (1<<24) //Intr on -ve edge of Frame Sync
#define AT91_SSC_RHR  (0x20)
#define AT91_SSC_THR  (0x24)
#define AT91_SSC_RSHR (0x30)
#define AT91_SSC_TSHR (0x34)
#define AT91_SSC_RC0R (0x38)
#define AT91_SSC_RC1R (0x3C)
#define AT91_SSC_SR   (0x40)
#define AT91_SSC_SR_TXRDY   (1<<0) //Transmit Ready
#define AT91_SSC_SR_TXEMPTY (1<<1) //Transmit Empty
#define AT91_SSC_SR_ENDTX   (1<<2) //End of Transmission
#define AT91_SSC_SR_TXBUFE  (1<<3) //Transmit Buffer Empty
#define AT91_SSC_SR_RXRDY   (1<<4) //Receiver Ready
#define AT91_SSC_SR_OVRUN   (1<<5) //Receiver Overrun
#define AT91_SSC_SR_ENDRX   (1<<6) //End of Reception
#define AT91_SSC_SR_RXBUFF  (1<<7) //Receive Buffer Full
#define AT91_SSC_SR_CP0     (1<<8) //Compare 0 match
#define AT91_SSC_SR_CP1     (1<<9) //Compare 1 Match
#define AT91_SSC_SR_TXSYN   (1<<10) //Transmit Frame Sync
#define AT91_SSC_SR_RXSYN   (1<<11) //Receive Frame Sync 
#define AT91_SSC_SR_TXEN    (1<<16) //Transmitter Enabled
#define AT91_SSC_SR_RXEN    (1<<17) //Receiver Enabled
#define AT91_SSC_IER  (0x44)
#define AT91_SSC_IDR  (0x48)
#define AT91_SSC_IMR  (0x4C)

#define AT91_SSC_RPR  0x100 // Receiver Pointer Register
#define AT91_SSC_RCR  0x104 // Receiver Counter Register
#define AT91_SSC_TPR  0x108 // Transmit Pointer Register
#define AT91_SSC_TCR  0x10c // Transmit Counter Register
#define AT91_SSC_RNPR 0x110 // Receiver Next Pointer Register
#define AT91_SSC_RNCR 0x114 // Receiver Next Counter Register
#define AT91_SSC_TNPR 0x118 // Transmit Next Pointer Register
#define AT91_SSC_TNCR 0x11c // Transmit Next Counter Register
#define AT91_SSC_PTCR 0x120 // PDC Transfer Control Register
#define AT91_SSC_PTSR 0x124 // PDC Transfer Status Register

#define AT91_SSC_PTCR_RXTEN  (1 << 0) //Receive Transfers Enabled
#define AT91_SSC_PTCR_RXTDIS (1 << 1) //Receive Transfers Disabled 
#define AT91_SSC_PTCR_TXTEN  (1 << 8) //Receive Transfers Enabled  
#define AT91_SSC_PTCR_TXTDIS (1 << 9) //Receive Transfers Disabled 

#endif

//=============================================================================
// Ethernet Controller (EMAC)

#if defined(CYGHWR_HAL_ARM_AT91SAM7X)

#ifndef AT91_EMAC
#define AT91_EMAC 0xFFFBC000
#endif


#define AT91_EMAC_CTL  (0x00) // Network Control
#define AT91_EMAC_CTL_LB     (1 <<  0) // Loopback
#define AT91_EMAC_CTL_LBL    (1 <<  1) // Loopback Local 
#define AT91_EMAC_CTL_RE     (1 <<  2) // Receiver Enable
#define AT91_EMAC_CTL_TX     (1 <<  3) // Transmit Enable
#define AT91_EMAC_CTL_MPE    (1 <<  4) // Management Port Enable
#define AT91_EMAC_CTL_CSR    (1 <<  5) // Clear Statistics Registers
#define AT91_EMAC_CTL_ISR    (1 <<  6) // Increment Statistics Registers
#define AT91_EMAC_CTL_WES    (1 <<  7) // Write Enable for Statistics Registers
#define AT91_EMAC_CTL_BP     (1 <<  8) // Back Pressure 
#define AT91_EMAC_CTL_TSTART (1 <<  9) // Start Transmitter
#define AT91_EMAC_CTL_THALT  (1 << 10) // Halt Transmitter

#define AT91_EMAC_CFG  (0x04) // Network Configuration
#define AT91_EMAC_CFG_SPD_10Mbps  (0 <<  0) // 10Mbps line speed
#define AT91_EMAC_CFG_SPD_100Mbps (1 <<  0) // 100Mbps line speed
#define AT91_EMAC_CFG_FD          (1 <<  1) // Full Deplex
#define AT91_EMAC_CFG_BR          (1 <<  2) // Bit Rate
#define AT91_EMAC_CFG_CAF         (1 <<  4) // Copy All Frames
#define AT91_EMAC_CFG_NBC         (1 <<  5) // Don't receiver Broadcasts
#define AT91_EMAC_CFG_MTI         (1 <<  6) // Multicast Hash Enable
#define AT91_EMAC_CFG_UNI         (1 <<  7) // Unicast hash enable
#define AT91_EMAC_CFG_BIG         (1 <<  8) // Receive upto 1522 byte frames
#define AT91_EMAC_CFG_EAE         (1 <<  9) // External Address match Enable
#define AT91_EMAC_CFG_CLK_HCLK_8  (0 << 10) // HCLK divided by 8
#define AT91_EMAC_CFG_CLK_HCLK_16 (1 << 10) // HCLK divided by 16
#define AT91_EMAC_CFG_CLK_HCLK_32 (2 << 10) // HCLK divided by 32
#define AT91_EMAC_CFG_CLK_HCLK_64 (3 << 10) // HCLK divided by 64
#define AT91_EMAC_CFG_CLK_MASK    (3 << 10) // HCLK mask
#define AT91_EMAC_CFG_CLK_RTY     (1 << 12) // Retry Test
#define AT91_EMAC_CFG_CLK_RMII    (1 << 13) // Enable RMII mode
#define AT91_EMAC_CFG_CLK_MII     (0 << 13) // Enable MII mode
#define AT91_EMAC_SR   (0x08) // Network Status
#define AT91_EMAC_SR_MDIO_MASK (1 << 1) // MDIO Pin status
#define AT91_EMAC_SR_IDLE      (1 << 2) // PHY logical is idle
#define AT91_EMAC_TAR  (0x0c) // Transmit Address 
#define AT91_EMAC_TCR  (0x10) // Transmit Control
#define AT91_EMAC_TCR_LEN_MASK (0x3ff <<  0) // Transmit frame length
#define AT91_EMAC_TCR_NCRC     (    1 << 15) // No CRC added by MAC
#define AT91_EMAC_TSR  (0x14) // Transmit Status
#define AT91_EMAC_TSR_OVR    (1 << 0) // Overrun
#define AT91_EMAC_TSR_COL    (1 << 1) // Collision occurred
#define AT91_EMAC_TSR_RLE    (1 << 2) // Retry Limit Exceeded
#define AT91_EMAC_TSR_TXIDLE (1 << 3) // Transmitter Idle
#define AT91_EMAC_TSR_BNQ    (1 << 4) // Buffer Not Queues
#define AT91_EMAC_TSR_COMP   (1 << 5) // Transmission Complete
#define AT91_EMAC_TSR_UND    (1 << 6) // Transmit Underrun
#define AT91_EMAC_RBQP (0x18) // Receiver Buffer Queue Pointer
#define AT91_EMAC_TBQP (0x1c) // Transmit Buffer Queue Pointer
#define AT91_EMAC_RSR  (0x20) // Receiver Status
#define AT91_EMAC_RSR_BNA (1 << 0) // Buffer Not Available
#define AT91_EMAC_RSR_REC (1 << 1) // Frame Received
#define AT91_EMAC_RSR_OVR (1 << 2) // Transmit Buffer Overrun
#define AT91_EMAC_ISR  (0x24) // Interrupt Status
#define AT91_EMAC_ISR_DONE  (1 <<  0) // Management Done
#define AT91_EMAC_ISR_RCOM  (1 <<  1) // Receiver Complete
#define AT91_EMAC_ISR_RBNA  (1 <<  2) // Receiver Buffer Not Available
#define AT91_EMAC_ISR_TOVR  (1 <<  3) // Transmit Buffer Overrun
#define AT91_EMAC_ISR_TUND  (1 <<  4) // Transmit Error: Buffer under run
#define AT91_EMAC_ISR_RTRY  (1 <<  5) // Transmit Error: Retry Limit Exceeded
#define AT91_EMAC_ISR_TBRE  (1 <<  6) // Transmit Buffer Register Empty
#define AT91_EMAC_ISR_TCOM  (1 <<  7) // Transmit Complete
#define AT91_EMAC_ISR_TIDLE (1 <<  8) // Transmitter Idle
#define AT91_EMAC_ISR_LINK  (1 <<  9) // Link pin changed state
#define AT91_EMAC_ISR_ROVR  (1 << 10) // Receiver Overrun
#define AT91_EMAC_ISR_HRESP (1 << 11) // HRESP not OK
#define AT91_EMAC_IER  (0x28) // Interrupt Enable
#define AT91_EMAC_IDR  (0x2c) // Interrupt Disable
#define AT91_EMAC_IMR  (0x30) // Interrupt Mask
#define AT91_EMAC_MAN  (0x34) // PHY Maintenance
#define AT91_EMAC_MAN_DATA_MASK  (0xffff <<  0) // Data to/from PHY
#define AT91_EMAC_MAN_CODE       (2      << 16) // Code
#define AT91_EMAC_MAN_REGA_MASK  (0x1f   << 18) // Register Address Mask
#define AT91_EMAC_MAN_REGA_SHIFT (18)           // Register Address Shift
#define AT91_EMAC_MAN_PHY_MASK   (0x1f   << 23) // PHY Address Mask
#define AT91_EMAC_MAN_PHY_SHIFT  (23)           // PHY Address Shift
#define AT91_EMAC_MAN_RD         (2      << 28) // Read operation
#define AT91_EMAC_MAN_WR         (1      << 28) // Write Operation
#define AT91_EMAC_MAN_HIGH       (1      << 30) // Must be set to 1
#define AT91_EMAC_FRA  (0x40) // Frames Transmitted OK
#define AT91_EMAC_SCOL (0x44) // Single Collision Frame
#define AT91_EMAC_MCOL (0x48) // Multiple Collision Frame
#define AT91_EMAC_OK   (0x4c) // Frames Received OK
#define AT91_EMAC_SEQE (0x50) // Frame Check Sequence Error
#define AT91_EMAC_ALE  (0x54) // Alignment Error
#define AT91_EMAC_DTR  (0x58) // Deferred Transmission Frame
#define AT91_EMAC_LCOL (0x5c) // Late Collision
#define AT91_EMAC_XCOL (0x60) // Excessive Collisions - ECAL!!
#define AT91_EMAC_CSE  (0x64) // Carrier Sense Error
#define AT91_EMAC_TUE  (0x68) // Transmit Underrun Error
#define AT91_EMAC_CDE  (0x6c) // Code Error
#define AT91_EMAC_ELR  (0x70) // Excessive Length
#define AT91_EMAC_RJB  (0x74) // Receiver Jabber 
#define AT91_EMAC_USF  (0x78) // Undersize Frame
#define AT91_EMAC_SQEE (0x7c) // SEQ Test Error
#define AT91_EMAC_DRFC (0x80) // Discarded RX Frame
#define AT91_EMAC_HSH  (0x90) // Hash Address High [63:21]
#define AT91_EMAC_HSL  (0x94) // Hash Address Low  [31:0]
#define AT91_EMAC_SA1L (0x98) // Specific Address 1 Low, First 4 bytes
#define AT91_EMAC_SA1H (0x9c) // Specific Address 1 High, Last 2 bytes
#define AT91_EMAC_SA2L (0xa0) // Specific Address 2 Low, First 4 bytes
#define AT91_EMAC_SA2H (0xa4) // Specific Address 2 High, Last 2 bytes
#define AT91_EMAC_SA3L (0xa8) // Specific Address 3 Low, First 4 bytes
#define AT91_EMAC_SA3H (0xac) // Specific Address 3 High, Last 2 bytes
#define AT91_EMAC_SA4L (0xb0) // Specific Address 4 Low, First 4 bytes
#define AT91_EMAC_SA4H (0xb4) // Specific Address 4 High, Last 2 bytes

// Receiver Buffer Descriptor
#define AT91_EMAC_RBD_ADDR 0x0  // Address to beginning of buffer
#define AT91_EMAC_RBD_ADDR_OWNER_EMAC (0 << 0) // EMAC owns receiver buffer
#define AT91_EMAC_RBD_ADDR_OWNER_SW   (1 << 0) // SW owns receiver buffer
#define AT91_EMAC_RBD_ADDR_WRAP       (1 << 1) // Last receiver buffer
#define AT91_EMAC_RBD_SR   0x1  // Buffer Status
#define AT91_EMAC_RBD_SR_LEN_MASK     (0xfff)   // Length of data
#define AT91_EMAC_RBD_SR_SOF          (1 << 14) // Start of Frame
#define AT91_EMAC_RBD_SR_EOF          (1 << 15) // End of Frame
#define AT91_EMAC_RBD_SR_CFI          (1 << 16) // Concatination Format Ind
#define AT91_EMAC_RDB_SR_VLAN_SHIFT   (17)      // VLAN priority tag 
#define AT91_EMAC_RDB_SR_VLAN_MASK    (7 << 17)
#define AT91_EMAC_RDB_SR_PRIORTY_TAG  (1 << 20) // Priority Tag Detected
#define AT91_EMAC_RDB_SR_VLAN_TAG     (1 << 21) // Priority Tag Detected
#define AT91_EMAC_RBD_SR_TYPE_ID      (1 << 22) // Type ID match
#define AT91_EMAC_RBD_SR_SA4M         (1 << 23) // Specific Address 4 match
#define AT91_EMAC_RBD_SR_SA3M         (1 << 24) // Specific Address 3 match
#define AT91_EMAC_RBD_SR_SA2M         (1 << 25) // Specific Address 2 match
#define AT91_EMAC_RBD_SR_SA1M         (1 << 26) // Specific Address 1 match
#define AT91_EMAC_RBD_SR_EXTNM        (1 << 28) // External Address match
#define AT91_EMAC_RBD_SR_UNICAST      (1 << 29) // Unicast hash match
#define AT91_EMAC_RBD_SR_MULTICAST    (1 << 30) // Multicast hash match
#define AT91_EMAC_RBD_SR_BROADCAST    (1 << 31) // Broadcast

// Transmit Buffer Descriptor
#define AT91_EMAC_TBD_ADDR 0x0  // Address to beginning of buffer
#define AT91_EMAC_TBD_SR   0x1  // Buffer Status
#define AT91_EMAC_TBD_SR_LEN_MASK     (0xfff)   // Length of data
#define AT91_EMAC_TBD_SR_EOF          (1 << 15) // End of Frame
#define AT91_EMAC_TBD_SR_NCRC         (1 << 16) // No CRC added by EMAC
#define AT91_EMAC_TBD_SR_EXHAUST      (1 << 27) // Buffers exhausted
#define AT91_EMAC_TBD_SR_TXUNDER      (1 << 28) // Transmit Underrun
#define AT91_EMAC_TBD_SR_RTRY         (1 << 29) // Retry limit exceeded
#define AT91_EMAC_TBD_SR_WRAP         (1 << 30) // Marks last descriptor
#define AT91_EMAC_TBD_SR_USED         (1 << 31) // Buffer used

#endif

//=============================================================================
// Two Wire Interface (TWI)

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_TWI
#define AT91_TWI 0xFFFB8000
#endif

#define AT91_TWI_CR   0x00 // Control
#define AT91_TWI_CR_START (1 << 0) // Send a Start
#define AT91_TWI_CR_STOP  (1 << 1) // Send a Stop
#define AT91_TWI_CR_MSEN  (1 << 2) // Master Transfer Enable
#define AT91_TWI_CR_MSDIS (1 << 3) // Master Transfer Disable
#define AT91_TWI_CR_SVEN  (1 << 4) // Slave Transfer Enable
#define AT91_TWI_CR_SDIS  (1 << 5) // Slave Transfer Disable
#define AT91_TWI_CR_SWRST (1 << 7) // Software Reset
#define AT91_TWI_MMR  0x04 // Master Mode
#define AT91_TWI_MMR_IADRZ_NO (0 <<  8) // Internal Device Address size 0Bytes
#define AT91_TWI_MMR_IADRZ_1  (1 <<  8) // Internal Device Address size 1Byte
#define AT91_TWI_MMR_IADRZ_2  (2 <<  8) // Internal Device Address size 2Bytes
#define AT91_TWI_MMR_IADRZ_3  (3 <<  8) // Internal Device Address size 3Bytes
#define AT91_TWI_MMR_MWRITE   (0 << 12) // Master Write
#define AT91_TWI_MMR_MREAD    (1 << 12) // Master Read
#define AT91_TWI_MMR_DADR_MASK  (0x3f << 16) // Device Address Mask
#define AT91_TWI_MMR_DADR_SHIFT (16)         // Device Address Shift
#define AT91_TWI_SMR  0x08 // Slave Mode
#define AT91_TWI_SMR_SADR_MASK  (0x3f << 16) // Slave Device Address Mask
#define AT91_TWI_SMR_SADR_SHIFT (16)         // Slave Device Address Shift
#define AT91_TWI_IADR 0x0C // Internal Address
#define AT91_TWI_CWGR 0x10 // Clock Waveform Generator
#define AT91_TWI_CWGR_CLDIV_MASK  (0xf <<  0) // Clock Low Divider Mask
#define AT91_TWI_CWGR_CLDIV_SHIFT (00)        // Clock Low Divider Shift
#define AT91_TWI_CWGR_CHDIV_MASK  (0xf <<  8) // Clock High Divider Mask
#define AT91_TWI_CWGR_CHDIV_SHIFT (08)        // Clock High Divider Shift
#define AT91_TWI_CWGR_CKDIV_MASK  (0x7 << 16) // Clock Divider Mask
#define AT91_TWI_CWGR_CKDIV_SHIFT (16)        // Clock Divider Shift
#define AT91_TWI_SR   0x20 // Status
#define AT91_TWI_SR_TXCOMP (1 << 0) // Transmission Completed
#define AT91_TWI_SR_RXRDY  (1 << 1) // Receiver Holding Register Ready
#define AT91_TWI_SR_TXRDY  (1 << 2) // Transmit Holding Register Ready
#define AT91_TWI_SR_SVREAD (1 << 3) // Slave Read
#define AT91_TWI_SR_SVACC  (1 << 4) // Slave Access
#define AT91_TWI_SR_GCACC  (1 << 5) // General Call Access
#define AT91_TWI_SR_OVRE   (1 << 6) // Overrun Error
#define AT91_TWI_SR_UNRE   (1 << 7) // Underrun Error
#define AT91_TWI_SR_NACK   (1 << 8) // Not Acknowledged
#define AT91_TWI_SR_ARBLST (1 << 9) // Arbitration Lost
#define AT91_TWI_IER  0x24 // Interrupt Enable
#define AT91_TWI_IDR  0x28 // Interrupt Disable
#define AT91_TWI_IMR  0x2C // Interrupt Mask
#define AT91_TWI_RHR  0x30 // Receiver Holding
#define AT91_TWI_THR  0x34 // Transmit Holding
#endif

//=============================================================================
// Analog to Digital Convertor (ADC)

#if defined(CYGHWR_HAL_ARM_AT91SAM7)

#ifndef AT91_ADC
#define AT91_ADC 0xFFFD8000
#endif

#define AT91_ADC_CR    0x00 // Control
#define AT91_ADC_CR_SWRST (1 << 0) // Software Reset
#define AT91_ADC_CR_START (1 << 1) // Start Conversion
#define AT91_ADC_MR    0x04 // Mode 
#define AT91_ADC_MR_TRGSEL_TIOA0  (0 << 1) // Trigger = TIAO0
#define AT91_ADC_MR_TRGSEL_TIOA1  (1 << 1) // Trigger = TIAO1
#define AT91_ADC_MR_TRGSEL_TIOA2  (2 << 1) // Trigger = TIAO2
#define AT91_ADC_MR_TRGSEL_TIOA3  (3 << 1) // Trigger = TIAO3
#define AT91_ADC_MR_TRGSEL_TIOA4  (4 << 1) // Trigger = TIAO4
#define AT91_ADC_MR_TRGSEL_TIOA5  (5 << 1) // Trigger = TIAO5
#define AT91_ADC_MR_TRGSEL_EXT    (6 << 1) // Trigger = External
#define AT91_ADC_MR_LOWREC_10BITS (0 << 4) // 10-bit Resolution
#define AT91_ADC_MR_LOWRES_8BITS  (1 << 4) // 8-bit resolution
#define AT91_ADC_MR_SLEEP_ON      (1 << 5) // Sleep mode on
#define AT91_ADC_MR_SLEEP_OFF     (0 << 5) // Sleep mode off
#define AT91_ADC_MR_PRESCAL_MASK  (0x3f << 8) // Prescale Mask
#define AT91_ADC_MR_PRESCAL_SHIFT (8)         // Prescale Shift
#define AT91_ADC_MR_STARTUP_MASK  (0x0f << 16) // Startup Time Mask
#define AT91_ADC_MR_STARTUP_SHIFT (16)         // Startup Time Mask
#define AT91_ADC_MR_SHTIM_MASK    (0x0f << 24) // Sample & Hold Time Mask
#define AT91_ADC_MR_SHTIM_SHIFT   (24)         // Sample & Hold Time Shift
#define AT91_ADC_CHER  0x10 // Channel Enable
#define AT91_ADC_CHER_CH0 (1 << 0) // Channel 0
#define AT91_ADC_CHER_CH1 (1 << 1) // Channel 1
#define AT91_ADC_CHER_CH2 (1 << 2) // Channel 2
#define AT91_ADC_CHER_CH3 (1 << 3) // Channel 3
#define AT91_ADC_CHER_CH4 (1 << 4) // Channel 4
#define AT91_ADC_CHER_CH5 (1 << 5) // Channel 5
#define AT91_ADC_CHER_CH6 (1 << 6) // Channel 6
#define AT91_ADC_CHER_CH7 (1 << 7) // Channel 7
#define AT91_ADC_CHDR  0x14 // Channel Disable
#define AT91_ADC_CHSR  0x18 // Channel Status
#define AT91_ADC_SR    0x1c // Status
#define AT91_ADC_CHSR_EOC0   (1 <<  0) // Channel 0 End of Conversion
#define AT91_ADC_CHSR_EOC1   (1 <<  1) // Channel 1 End of Conversion
#define AT91_ADC_CHSR_EOC2   (1 <<  2) // Channel 2 End of Conversion
#define AT91_ADC_CHSR_EOC3   (1 <<  3) // Channel 3 End of Conversion
#define AT91_ADC_CHSR_EOC4   (1 <<  4) // Channel 4 End of Conversion
#define AT91_ADC_CHSR_EOC5   (1 <<  5) // Channel 5 End of Conversion
#define AT91_ADC_CHSR_EOC6   (1 <<  6) // Channel 6 End of Conversion
#define AT91_ADC_CHSR_EOC7   (1 <<  7) // Channel 7 End of Conversion
#define AT91_ADC_CHSR_OVRE0  (1 <<  8) // Channel 0 Overrun Error
#define AT91_ADC_CHSR_OVRE1  (1 <<  9) // Channel 1 Overrun Error
#define AT91_ADC_CHSR_OVRE2  (1 << 10) // Channel 2 Overrun Error
#define AT91_ADC_CHSR_OVRE3  (1 << 11) // Channel 3 Overrun Error
#define AT91_ADC_CHSR_OVRE4  (1 << 12) // Channel 4 Overrun Error
#define AT91_ADC_CHSR_OVRE5  (1 << 13) // Channel 5 Overrun Error
#define AT91_ADC_CHSR_OVRE6  (1 << 14) // Channel 6 Overrun Error
#define AT91_ADC_CHSR_OVRE7  (1 << 15) // Channel 7 Overrun Error
#define AT91_ADC_CHSR_DRDY   (1 << 16) // Data Ready
#define AT91_ADC_CHSR_GOVER  (1 << 17) // General Overrun
#define AT91_ADC_CHSR_EDNRX  (1 << 18) // End of Receiver Transfer
#define AT91_ADC_CHSR_RXBUFF (1 << 19) // RXBUFFER Interrupt
#define AT91_ADC_LCDR  0x20 // Last Converted Data
#define AT91_ADC_IER   0x24 // Interrupt Enable
#define AT91_ADC_IDR   0x28 // Interrupt Disable
#define AT91_ADC_IMR   0x2c // Interrupt Mask
#define AT91_ADC_CDR0  0x30 // Channel Data 0
#define AT91_ADC_CDR1  0x34 // Channel Data 1
#define AT91_ADC_CDR2  0x38 // Channel Data 2
#define AT91_ADC_CDR3  0x3c // Channel Data 3
#define AT91_ADC_CDR4  0x40 // Channel Data 4
#define AT91_ADC_CDR5  0x44 // Channel Data 5
#define AT91_ADC_CDR6  0x48 // Channel Data 6
#define AT91_ADC_CDR7  0x4c // Channel Data 7
#define AT91_ADC_RPR  0x100 // Receive Pointer
#define AT91_ADC_RCR  0x104 // Receive Counter
#define AT91_ADC_TPR  0x108 // Transmit Pointer
#define AT91_ADC_TCR  0x10C // Transmit Counter
#define AT91_ADC_RNPR 0x110 // Receive Next Pointer
#define AT91_ADC_RNCR 0x114 // Receive Next Counter
#define AT91_ADC_TNPR 0x118 // Transmit Next Pointer
#define AT91_ADC_TNCR 0x11C // Transmit Next Counter
#define AT91_ADC_PTCR 0x120 // PDC Transfer Control
#define AT91_ADC_PTSR 0x124 // PDC Transfer Status

#endif

//=============================================================================
// Controller Area Network (CAN)

#if defined(CYGHWR_HAL_ARM_AT91SAM7X)

#ifndef AT91_CAN
#define AT91_CAN 0xFFFD8000
#endif

#define AT91_CAN_MR      0x000 // Mode
#define AT91_CAN_MR_CANEN  (1 << 0) // Enable
#define AT91_CAN_MR_LPM    (1 << 1) // Enable Low Power Mode
#define AT91_CAN_MR_ABM    (1 << 2) // Enable Autobaud/Listen mode
#define AT91_CAN_MR_OVL    (1 << 3) // Enable Overload Frame
#define AT91_CAN_MR_TEOF   (1 << 4) // Timestamp at End Of Trame
#define AT91_CAN_MR_TTM    (1 << 5) // Enable Time Triggered Mode
#define AT91_CAN_MR_TIMFRZ (1 << 6) // Enable Timer Freeze
#define AT91_CAN_MR_DRPT   (1 << 7) // Disable Repeat
#define AT91_CAN_IER     0x004 // Interrupt Enable
#define AT91_CAM_IER_MB0    (1 <<  0) // Mailbox 0
#define AT91_CAM_IER_MB1    (1 <<  1) // Mailbox 1
#define AT91_CAM_IER_MB2    (1 <<  2) // Mailbox 2
#define AT91_CAM_IER_MB3    (1 <<  3) // Mailbox 3
#define AT91_CAM_IER_MB4    (1 <<  4) // Mailbox 4
#define AT91_CAM_IER_MB5    (1 <<  5) // Mailbox 5
#define AT91_CAM_IER_MB6    (1 <<  6) // Mailbox 6
#define AT91_CAM_IER_MB7    (1 <<  7) // Mailbox 7
#define AT91_CAM_IER_ERRA   (1 << 16) // Error Active Mode
#define AT91_CAM_IER_WARN   (1 << 17) // Warning Limit
#define AT91_CAM_IER_ERRO   (1 << 18) // Error Passive Mode
#define AT91_CAM_IER_BOFF   (1 << 19) // Bus-Off Mode
#define AT91_CAM_IER_SLEEP  (1 << 20) // Sleep
#define AT91_CAM_IER_WAKEUP (1 << 21) // Wakeup
#define AT91_CAM_IER_TOVF   (1 << 22) // Timer Overflow
#define AT91_CAM_IER_TSTP   (1 << 23) // TimeStamp
#define AT91_CAM_IER_CERR   (1 << 24) // CRC Error
#define AT91_CAM_IER_SERR   (1 << 25) // Stuffing Error
#define AT91_CAM_IER_AERR   (1 << 26) // Acknowledgement Error
#define AT91_CAM_IER_FERR   (1 << 27) // Form Error
#define AT91_CAM_IER_BERR   (1 << 28) // Bit Error
#define AT91_CAN_IDR     0x008 // Interrupt Disable
#define AT91_CAN_IMR     0x00C // Interrupt Mask
#define AT91_CAN_SR      0x010 // Status
#define AT91_CAN_SR_RBSY   (1 << 29) // Receiver busy
#define AT91_CAM_SR_TBSY   (1 << 30) // Transmitter busy
#define AT91_CAM_IER_OVLSY (1 << 31) // Overload Busy
#define AT91_CAN_BR      0x014 // Baudrate
#define AT91_CAN_BR_PHASE1_MASK   (0x7 << 4)  // Phase 1 Segment mask
#define AT91_CAN_BR_PHASE1_SHIFT  (4)         // Phase 1 Segment shift
#define AT91_CAN_BR_PHASE2_MASK   (0x7 << 0)  // Phase 2 Segment mask
#define AT91_CAN_BR_PHASE2_SHIFT  (0)         // Phase 2 Segment shift
#define AT91_CAN_BR_PROPAG_MASK   (0x7 << 8)  // Programming Time Segment mask
#define AT91_CAN_BR_PROPAG_SHIFT  (8)         // Programming Time Segment shift
#define AT91_CAN_BR_SJW_MASK      (0x3 << 12) // Re-Sync jump width mask
#define AT91_CAN_BR_SJW_SHIFT     (12)        // Re-Sync jump width shift
#define AT91_CAN_BR_BRP_MASK      (0x7f << 16) // Baudrate Prescaler mask
#define AT91_CAN_BR_BRP_SHIFT     (16)         // Baudrate Prescaler mask
#define AT91_CAN_BR_SMP_ONCE      (0 << 24)     // Sampling once
#define AT91_CAN_BR_SMP_THRICE    (1 << 24)     // Sampling three times
#define AT91_CAN_TIM     0x018 // Timer
#define AT91_CAN_TIMESTP 0x01c // Timestamp
#define AT91_CAN_ECR     0x020 // Error Counter
#define AT91_CAN_ECR_REC_MASK  (0xf <<  0) // Receiver Error Counter mask
#define AT91_CAN_ECR_REC_SHIFT (00)        // Receiver Error Counter shift
#define AT91_CAN_ECR_TEC_MASK  (0xf << 16) // Transmit Error Counter mask
#define AT91_CAN_ECR_TEC_SHIFT (00)        // Transmit Error Counter shift
#define AT91_CAN_TCR     0x024 // Transfer Command
#define AT91_CAN_TCR_TIMRST (1 << 31) // Timer Reset
#define AT91_CAN_ACR     0x028 // Abort Command
#define AT91_CAN_MMR0    0x200 // Mailbox 0 Mode
#define AT91_CAN_MMR_PRIOR_MASK  (0xf << 16) // Priority Mask
#define AT91_CAN_MMR_PRIOR_SHIFT (16) // Priority Shift
#define AT91_CAN_MMR_MOT_DISABLED       (0 << 24) // Mailbox disabled
#define AT91_CAN_MMR_MOT_RECEPTION      (1 << 24) // Reception Mailbox
#define AT91_CAN_MMR_MOT_RECEPTION_OVER (2 << 24) // Reception with Overwrite
#define AT91_CAM_MMR_MOT_TRANSMIT       (3 << 24) // Transmit Mailbox
#define AT91_CAM_MMR_MOT_CONSUMER       (4 << 24) // Transmit Mailbox
#define AT91_CAM_MMR_MOT_PRODUCER       (5 << 24) // Transmit Mailbox
#define AT91_CAN_MAM0    0x204 // Mailbox 0 Acceptance Mask
#define AT91_CAM_MAM_MIDvB_MASK  (0x3ffff <<  0) // MIDvB mask
#define AT91_CAM_MAM_MIDvB_SHIFT (0)             // MIDvB shift
#define AT91_CAM_MAM_MIDvA_MASK  (0x7ff   << 18) // MIDvB mask
#define AT91_CAM_MAM_MIDvA_SHIFT (18)            // MIDvB shift
#define AT91_CAM_MAM_MIDE        (1 << 29)       // Identifier Version
#define AT91_CAN_MID0    0x208 // Mailbox 0 ID
#define AT91_CAN_MFID0   0x20C // Mailbox 0 Family ID
#define AT91_CAN_MSR0    0x210 // Mailbox 0 Status
#define AT91_CAM_MSR_MDLC_MASK  (0xf << 16) // Mailbox Data Length Code mask
#define AT91_CAM_MSR_MDLC_SHIFT (16)        // Mailbox Data Length Code shift
#define AT91_CAM_MSR_MRTR       (1 << 20)   // Mailbox Remote Tx Request
#define AT91_CAM_MSR_MABT       (1 << 22)   // Mailbox Abort
#define AT91_CAM_MSR_MRDY       (1 << 23)   // Mailbox Ready
#define AT91_CAM_MSR_MMI        (1 << 24)   // Mailbox Message Ignored
#define AT91_CAN_MDL0    0x214 // Mailbox 0 Data Low
#define AT91_CAN_MDH0    0x218 // Mailbox 0 Data High
#define AT91_CAN_MCR0    0x21c // Mailbox 0 Control
#define AT91_CAM_MCR_MDLC_MASK  (0xf << 16) // Mailbox Data Length Code mask
#define AT91_CAM_MCR_MDLC_SHIFT (16)        // Mailbox Data Length Code shift
#define AT91_CAM_MCR_MRTR       (1 << 20)   // Mailbox Remote Tx Request
#define AT91_CAM_MCR_MACR       (1 << 22)   // Mailbox Abort Request
#define AT91_CAM_MCR_MTCR       (1 << 23)   // Mailbox Transfer Command
#define AT91_CAN_MMR1    0x220 // Mailbox 1 Mode
#define AT91_CAN_MAM1    0x224 // Mailbox 1 Acceptance Mask
#define AT91_CAN_MID1    0x228 // Mailbox 1 ID
#define AT91_CAN_MFID1   0x22C // Mailbox 1 Family ID
#define AT91_CAN_MSR1    0x230 // Mailbox 1 Status
#define AT91_CAN_MDL1    0x234 // Mailbox 1 Data Low
#define AT91_CAN_MDH1    0x238 // Mailbox 1 Data High
#define AT91_CAN_MCR1    0x23c // Mailbox 1 Control

#endif


//=============================================================================
// FIQ interrupt vector which is shared by all HAL varients.

#define CYGNUM_HAL_INTERRUPT_FIQ 0

// Macros for access the GPIO lines and configuring peripheral pins

// Given a pin description, determine which PIO controller it is on
#define HAL_ARM_AT91_PIO_CTRL(_pin_)            \
  ((_pin_ >> 16) & 0xff)

// Given a pin description, determine which PIO bit controls this pin
#define HAL_ARM_AT91_PIO_BIT(_pin_)             \
  (1 << (_pin_ & 0xff))

// Evaluate to true if the pin is using peripheral A
#define HAL_ARM_AT91_PIO_A(_pin_)               \
  (((_pin_ >> 8) & 0xff) == 0)

// Configure a peripheral pin on a specific PIO controller.
#ifdef AT91_PIO_ASR
#define HAL_ARM_AT91_PIOX_CFG(_pin_, _nr_, _pio_base_)                  \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_PDR,                         \
                     HAL_ARM_AT91_PIO_BIT(_pin_));                      \
    if (HAL_ARM_AT91_PIO_A(_pin_)) {                                    \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_ASR,                       \
                       HAL_ARM_AT91_PIO_BIT(_pin_));                    \
    } else {                                                            \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_BSR,                       \
                       HAL_ARM_AT91_PIO_BIT(_pin_));                    \
    }                                                                   \
  }                                                                     \
  CYG_MACRO_END
#else // AT91_PIO_ASR
#define HAL_ARM_AT91_PIOX_CFG(_pin_, _nr_, _pio_base_)                  \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_PDR,                         \
                     HAL_ARM_AT91_PIO_BIT(_pin_));                      \
  }                                                                     \
  CYG_MACRO_END
#endif // !AT91_PIO_ASR

// Configure a GPIO pin direction on a specific PIO controller.
#define HAL_ARM_AT91_GPIOX_CFG_DIRECTION(_pin_, _dir_, _nr_, _pio_base_) \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_PER,                         \
                     HAL_ARM_AT91_PIO_BIT(_pin_));                      \
    if ((_dir_) == AT91_PIN_IN) {                                       \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_ODR,                       \
                       HAL_ARM_AT91_PIO_BIT(_pin_));                    \
    } else {                                                            \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_OER,                       \
                         HAL_ARM_AT91_PIO_BIT(_pin_));                  \
    }                                                                   \
  }                                                                     \
  CYG_MACRO_END

// Configure a GPIO pin pullup on a specific PIO controller.
#define HAL_ARM_AT91_GPIOX_CFG_PULLUP(_pin_, _enable_, _nr_, _pio_base_) \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    if (_enable_) {                                                     \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_PPUER,                     \
                       HAL_ARM_AT91_PIO_BIT(_pin_));                    \
    } else {                                                            \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_PPUDR,                     \
                         HAL_ARM_AT91_PIO_BIT(_pin_));                  \
    }                                                                   \
  }                                                                     \
  CYG_MACRO_END

// Set a GPIO pin on a specific PIO controller to generate interrupts
#define HAL_ARM_AT91_GPIOX_CFG_INTERRUPT(_pin_, _enable_, _nr_, _pio_base_) \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    if (_enable_) {                                                     \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_IER,                       \
                       HAL_ARM_AT91_PIO_BIT(_pin_));                    \
    } else {                                                            \
      HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_IDR,                       \
                         HAL_ARM_AT91_PIO_BIT(_pin_));                  \
    }                                                                   \
  }                                                                     \
  CYG_MACRO_END

// Set a GPIO pin on a specific PIO controller.
#define HAL_ARM_AT91_GPIOX_SET(_pin_, _nr_, _pio_base_) \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_SODR,                        \
                     HAL_ARM_AT91_PIO_BIT(_pin_));                      \
  }                                                                     \
  CYG_MACRO_END

// Reset a GPIO pin on a specific PIO controller.
#define HAL_ARM_AT91_GPIOX_RESET(_pin_, _nr_, _pio_base_) \
  CYG_MACRO_START                                                       \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_WRITE_UINT32((_pio_base_)+AT91_PIO_CODR,                        \
                     HAL_ARM_AT91_PIO_BIT(_pin_));                      \
  }                                                                     \
  CYG_MACRO_END

// Get a GPIO pin on a specific PIO controller.
#define HAL_ARM_AT91_GPIOX_GET(_pin_, _value_, _nr_, _pio_base_)        \
  CYG_MACRO_START                                                       \
  cyg_uint32 _pdsr_;                                                    \
  if (HAL_ARM_AT91_PIO_CTRL(_pin_) == (_nr_)) {                         \
    HAL_READ_UINT32((_pio_base_)+AT91_PIO_PDSR, _pdsr_);                \
    (_value_) = (_pdsr_ & HAL_ARM_AT91_PIO_BIT(_pin_) ? 1 : 0);         \
  }                                                                     \
  CYG_MACRO_END

#define AT91_PIN_IN  1
#define AT91_PIN_OUT 0
#define AT91_PIN_PULLUP_ENABLE  1
#define AT91_PIN_PULLUP_DISABLE 0
#define AT91_PIN_INTERRUPT_ENABLE  1
#define AT91_PIN_INTERRUPT_DISABLE 0

#ifndef AT91_PIOB
// Only one PIO controller

// Configure a peripheral pin for peripheral operation
#define HAL_ARM_AT91_PIO_CFG(_pin_)             \
  CYG_MACRO_START                               \
  HAL_ARM_AT91_PIOX_CFG(_pin_, 0, AT91_PIO);    \
  CYG_MACRO_END

// Configure a GPIO pin direction
#define HAL_ARM_AT91_GPIO_CFG_DIRECTION(_pin_, _dir_)            \
  CYG_MACRO_START                                                \
  HAL_ARM_AT91_GPIOX_CFG_DIRECTION(_pin_, _dir_, 0, AT91_PIO);   \
  CYG_MACRO_END

// Configure a GPIO pin pullup resistor
#define HAL_ARM_AT91_GPIO_CFG_PULLUP(_pin_, _enable_)               \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_CFG_PULLUP(_pin_, _enable_, 0, AT91_PIO);      \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_CFG_INTERRUPT(_pin_, _enable_)            \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_CFG_INTERRUPT(_pin_, _enable_, 0, AT91_PIO);   \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_SET(_pin_)                                \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_SET(_pin_, 0, AT91_PIO);                       \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_RESET(_pin_)                              \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_RESET(_pin_, 0, AT91_PIO);                     \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_GET(_pin_, _value_)                       \
  CYG_MACRO_START                                                    \
  HAL_ARM_AT91_GPIOX_GET(_pin_, _value_, 0, AT91_PIO);               \
  CYG_MACRO_END

#else // !AT91_PIOB
// Two PIO controllers

// Configure a peripheral pin for peripheral operation
#define HAL_ARM_AT91_PIO_CFG(_pin_)             \
  CYG_MACRO_START                               \
  HAL_ARM_AT91_PIOX_CFG(_pin_, 0, AT91_PIO);        \
  HAL_ARM_AT91_PIOX_CFG(_pin_, 1, AT91_PIOB);       \
CYG_MACRO_END

// Configure a GPIO pin direction
#define HAL_ARM_AT91_GPIO_CFG_DIRECTION(_pin_, _dir_)            \
  CYG_MACRO_START                                                \
  HAL_ARM_AT91_GPIOX_CFG_DIRECTION(_pin_, _dir_, 0, AT91_PIO);   \
  HAL_ARM_AT91_GPIOX_CFG_DIRECTION(_pin_, _dir_, 1, AT91_PIOB);  \
  CYG_MACRO_END

// Configure a GPIO pin pullup resistor
#define HAL_ARM_AT91_GPIO_CFG_PULLUP(_pin_, _enable_)               \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_CFG_PULLUP(_pin_, _enable_, 0, AT91_PIO);      \
  HAL_ARM_AT91_GPIOX_CFG_PULLUP(_pin_, _enable_, 1, AT91_PIOB);     \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_CFG_INTERRUPT(_pin_, _enable_)            \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_CFG_INTERRUPT(_pin_, _enable_, 0, AT91_PIO);   \
  HAL_ARM_AT91_GPIOX_CFG_INTERRUPT(_pin_, _enable_, 1, AT91_PIOB);  \
  CYG_MACRO_END

// Set a GPIO pin to 1
#define HAL_ARM_AT91_GPIO_SET(_pin_)                                \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_SET(_pin_, 0, AT91_PIO);                       \
  HAL_ARM_AT91_GPIOX_SET(_pin_, 1, AT91_PIOB);                      \
  CYG_MACRO_END

// Reset a GPIO pin to 0
#define HAL_ARM_AT91_GPIO_RESET(_pin_)                              \
  CYG_MACRO_START                                                   \
  HAL_ARM_AT91_GPIOX_RESET(_pin_, 0, AT91_PIO);                     \
  HAL_ARM_AT91_GPIOX_RESET(_pin_, 1, AT91_PIOB);                    \
  CYG_MACRO_END

// Configure a GPIO pin to generate interrupts
#define HAL_ARM_AT91_GPIO_GET(_pin_, _value_)                        \
  CYG_MACRO_START                                                    \
  HAL_ARM_AT91_GPIOX_GET(_pin_, _value_, 0, AT91_PIO);               \
  HAL_ARM_AT91_GPIOX_GET(_pin_, _value_, 1, AT91_PIOB);              \
  CYG_MACRO_END
#endif //!AT91_PIOB

// Put a GPIO pin to a given state
#define HAL_ARM_AT91_GPIO_PUT(_pin_, _state_)   \
  CYG_MACRO_START                               \
  if (_state_) {                                \
    HAL_ARM_AT91_GPIO_SET(_pin_);               \
  } else {                                      \
    HAL_ARM_AT91_GPIO_RESET(_pin_);             \
  }                                             \
  CYG_MACRO_END
   
//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
