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
// Contributors:jskov, gthomas, tkoeller, tdrury, nickg, asl
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

// macro could be different from target to target (i.e jtst)
#ifndef AT91_US_BAUD
#define AT91_US_BAUD(baud) ((CYGNUM_HAL_ARM_AT91_CLOCK_SPEED/(8*(baud))+1)/2)
#endif
//=============================================================================
// PIO

#ifndef AT91_PIO
#define AT91_PIO      0xFFFF0000
#endif

#define AT91_PIO_PER  0x00  // PIO enable
#define AT91_PIO_PDR  0x04  // PIO disable
#define AT91_PIO_PSR  0x08  // PIO status

#if defined(CYGHWR_HAL_ARM_AT91_M55800A)

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

#elif defined (CYGHWR_HAL_ARM_AT91SAM7S)
#include <pkgconf/hal_arm_at91sam7s.h>

// PIOA
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
#define AT91_PIO_PSR_TF       0x00008000 // Transmit Frame Sync
#define AT91_PIO_PSR_TK       0x00010000 // Transmit Clock
#define AT91_PIO_PSR_TD       0x00020000 // Transmit Data
#define AT91_PIO_PSR_RD       0x00040000 // Receive Data
#define AT91_PIO_PSR_RK       0x00080000 // Receive Clock
#define AT91_PIO_PSR_RF       0x00100000 // Receive Frame Sync
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
#define AT01_PIO_PSR_NPCS1    x800000000 // SPI Chip Select 1
#endif // !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s64)

// PIOB
#define AT91_PIO_PSR_TIOA0     0x00000001 // Timer/Counter 0 IO Line A
#define AT91_PIO_PSR_TIOB0     0x00000002 // Timer/Counter 0 IO Line B
#define AT91_PIO_PSR_SCK0      0x00000004 // USART 0 Serial Clock
#define AT91_PIO_PSR_NPCS3     0x00000008 // SPI Chip Select 3
#define AT91_PIO_PSR_TCLK0     0x00000010 // Timer/Counter 0 Clock Input
#define AT91_PIO_PSR_NPCS3X    0x00000020 // SPI Chip Select 3 (again)
#define AT91_PIO_PSR_PCK0      0x00000040 // Programmable Clock Output 0
#define AT91_PIO_PSR_PWM3      0x00000080 // Pulse Width Modulation #3
#define AT91_PIO_PSR_ADTRG     0x00000100 // ADC Trigger
#define AT91_PIO_PSR_NPCS1     0x00000200 // SPI Chip Select 1
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

#else

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

#ifdef CYGHWR_HAL_ARM_AT91SAM7S
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
#endif // CYGHWR_HAL_ARM_AT91SAM7S
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

#ifdef CYGHWR_HAL_ARM_AT91SAM7S
#define AT91_AIC_DCR    0x138 // Debug Control Register
#define AT91_AIC_FFER   0x140 // Fast Forcing Enable Register
#define AT91_AIC_FFDR   0x144 // Fast Forcing Enable Register
#define AT91_AIC_FFSR   0x148 // Fast Forcing Enable Register
#endif 

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
      defined(CYGHWR_HAL_ARM_AT91SAM7S)

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
#else

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
#define AT91_SPI_SR_TDRE       0x00000002        // Transmit Data Register Empty
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
 
#if defined(CYGHWR_HAL_ARM_AT91_M55800A)
 
#define AT91_SPI_PIO         AT91_PIOA
#define AT91_SPI_PIO_NPCS(x) (((x)&0x0F)<<26)
#endif

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)
 
#define AT91_SPI_PIO         AT91_PIOA
#define AT91_SPI_PIO_NPCS(x)                   \
   ( (x & (1 << 0) ? AT91_PIO_PSR_NPCS0 : 0) | \
     (x & (1 << 1) ? AT91_PIO_PSR_NPCS1 : 0) | \
     (x & (1 << 2) ? AT91_PIO_PSR_NPCS2 : 0) | \
     (x & (1 << 3) ? AT91_PIO_PSR_NPCS3 : 0))
#endif

//=============================================================================
// Watchdog Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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
#endif //CYGHWR_HAL_ARM_AT91SAM7S 

//=============================================================================
// Reset Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

#ifndef AT91_PITC
#define AT91_PITC 0xfffffd30
#endif

#define AT91_PITC_PIMR 0x00  // Period Interval Mode Register
#define AT91_PITC_PIMR_PITEN  (1 << 24) // Periodic Interval Timer Enable
#define AT91_PITC_PIMR_PITIEN (1 << 25) // Periodic Interval Timer Interrupt Enable
#define AT91_PITC_PISR 0x04  // Period Interval Status Register
#define AT91_PITC_PISR_PITS   (1 << 0)  // Periodic Interval Timer Status
#define AT91_PITC_PIVR 0x08  // Period Interval Status Register
#define AT91_PITC_PIIR 0x0C  // Period Interval Image Register
#endif

//=============================================================================
// Real Time Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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
// Real Time Timer Controller

#if defined(CYGHWR_HAL_ARM_AT91SAM7S)

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
#define AT91_UDP_CSR_EPTYPE_CTRL     (0 << 9) // Control
#define AT91_UDP_CSR_EPTYPE_ISO_OUT  (1 << 9) // Isochronous OUT
#define AT91_UDP_CSR_EPTYPE_BULK_OUT (2 << 9) // Bulk OUT
#define AT91_UDP_CSR_EPTYPE_INT_OUT  (3 << 9) // Interrupt OUT
#define AT91_UDP_CSR_EPTYPE_ISO_IN   (5 << 9) // Isochronous IN
#define AT91_UDP_CSR_EPTYPE_BULK_IN  (6 << 9) // Bulk IN
#define AT91_UDP_CSR_EPTYPE_INT_IN   (6 << 9) // Interrupt IN
#define AT91_UDP_CSR_DTGLE       (1 << 11) // Data Toggle
#define AT91_UDP_CSR_EPEDS       (1 << 15) // Endpoint Enable Disable
#define AT91_UDP_FDR        0x50  // Endpoint FIFO Data
#define AT91_UDP_TXVC       0x74  // Transceiver Control
#define AT91_UDP_TXVC_TXVDIS     (1 <<  8) // Disable Transceiver
#define AT91_UDP_TXVC_PUON       (1 <<  9) // Pull-up ON
#endif

//=============================================================================
// FIQ interrupt vector which is shared by all HAL varients.

#define CYGNUM_HAL_INTERRUPT_FIQ 0
//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
