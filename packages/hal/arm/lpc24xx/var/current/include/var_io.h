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
// Author(s):   Uwe Kindler
// Contributors:
// Date:        2008-07-05
// Purpose:     NXP LPC24xx variant specific registers
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_arm_lpc24xx.h>  // variant chip model selection.
#include <cyg/hal/plf_io.h>

//=============================================================================
// Watchdog (WD)
#define CYGARC_HAL_LPC24XX_REG_WD_BASE                   0xE0000000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_WDMOD                     0x0000
#define CYGARC_HAL_LPC24XX_REG_WDMOD_WDEN                (1<<0)
#define CYGARC_HAL_LPC24XX_REG_WDMOD_WDRESET             (1<<1)
#define CYGARC_HAL_LPC24XX_REG_WDMOD_WDTOF               (1<<2)
#define CYGARC_HAL_LPC24XX_REG_WDMOD_WDINT               (1<<3)
#define CYGARC_HAL_LPC24XX_REG_WDTC                      0x0004
#define CYGARC_HAL_LPC24XX_REG_WDFEED                    0x0008
#define CYGARC_HAL_LPC24XX_REG_WDFEED_MAGIC1             0xAA
#define CYGARC_HAL_LPC24XX_REG_WDFEED_MAGIC2             0x55
#define CYGARC_HAL_LPC24XX_REG_WDTV                      0x000C


//=============================================================================
// Timers (Tx)

#define CYGARC_HAL_LPC24XX_REG_TIMER0_BASE               0xE0004000
#define CYGARC_HAL_LPC24XX_REG_TIMER1_BASE               0xE0008000
#define CYGARC_HAL_LPC24XX_REG_TIMER2_BASE               0xE0070000
#define CYGARC_HAL_LPC24XX_REG_TIMER3_BASE               0xE0074000

// Registers are offsets from base for each timer
#define CYGARC_HAL_LPC24XX_REG_TxIR                      0x0000
#define CYGARC_HAL_LPC24XX_REG_TxIR_MR0                  (1<<0)
#define CYGARC_HAL_LPC24XX_REG_TxIR_MR1                  (1<<1)
#define CYGARC_HAL_LPC24XX_REG_TxIR_MR2                  (1<<2)
#define CYGARC_HAL_LPC24XX_REG_TxIR_MR3                  (1<<3)
#define CYGARC_HAL_LPC24XX_REG_TxIR_CR0                  (1<<4)
#define CYGARC_HAL_LPC24XX_REG_TxIR_CR1                  (1<<5)
#define CYGARC_HAL_LPC24XX_REG_TxIR_CR2                  (1<<6)
#define CYGARC_HAL_LPC24XX_REG_TxIR_CR3                  (1<<7)
#define CYGARC_HAL_LPC24XX_REG_TxTCR                     0x0004
#define CYGARC_HAL_LPC24XX_REG_TxTCR_CTR_ENABLE          (1<<0)
#define CYGARC_HAL_LPC24XX_REG_TxTCR_CTR_RESET           (1<<1)
#define CYGARC_HAL_LPC24XX_REG_TxTC                      0x0008
#define CYGARC_HAL_LPC24XX_REG_TxPR                      0x000C
#define CYGARC_HAL_LPC24XX_REG_TxPC                      0x0010
#define CYGARC_HAL_LPC24XX_REG_TxMCR                     0x0014
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_INT             (1<<0)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_RESET           (1<<1)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR0_STOP            (1<<2)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR1_INT             (1<<3)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR1_RESET           (1<<4)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR1_STOP            (1<<5)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR2_INT             (1<<6)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR2_RESET           (1<<7)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR2_STOP            (1<<8)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR3_INT             (1<<9)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR3_RESET           (1<<10)
#define CYGARC_HAL_LPC24XX_REG_TxMCR_MR3_STOP            (1<<11)
#define CYGARC_HAL_LPC24XX_REG_TxMR0                     0x0018
#define CYGARC_HAL_LPC24XX_REG_TxMR1                     0x001C
#define CYGARC_HAL_LPC24XX_REG_TxMR2                     0x0020
#define CYGARC_HAL_LPC24XX_REG_TxMR3                     0x0024
#define CYGARC_HAL_LPC24XX_REG_TxCCR                     0x0028
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR0_RISE        (1<<0)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR0_FALL        (1<<1)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR0             (1<<2)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR1_RISE        (1<<3)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR1_FALL        (1<<4)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR1             (1<<5)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR2_RISE        (1<<6)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR2_FALL        (1<<7)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR2             (1<<8)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR3_RISE        (1<<9)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR3_FALL        (1<<10)
#define CYGARC_HAL_LPC24XX_REG_TxCCR_INT_CR3             (1<<11)
#define CYGARC_HAL_LPC24XX_REG_TxCR0                     0x002C
#define CYGARC_HAL_LPC24XX_REG_TxCR1                     0x0030
#define CYGARC_HAL_LPC24XX_REG_TxCR2                     0x0034
#define CYGARC_HAL_LPC24XX_REG_TxCR3                     0x0038
#define CYGARC_HAL_LPC24XX_REG_TxEMR                     0x003C
#define CYGARC_HAL_LPC24XX_REG_TxEMR_EM0                 (1<<0)
#define CYGARC_HAL_LPC24XX_REG_TxEMR_EM1                 (1<<1)
#define CYGARC_HAL_LPC24XX_REG_TxEMR_EM2                 (1<<2)
#define CYGARC_HAL_LPC24XX_REG_TxEMR_EM3                 (1<<3)

//=============================================================================
// UARTs (Ux)

#define CYGARC_HAL_LPC24XX_REG_UART0_BASE                0xE000C000
#define CYGARC_HAL_LPC24XX_REG_UART1_BASE                0xE0010000
#define CYGARC_HAL_LPC24XX_REG_UART2_BASE                0xE0078000
#define CYGARC_HAL_LPC24XX_REG_UART3_BASE                0xE007C000

// Registers are offsets from base for each UART
#define CYGARC_HAL_LPC24XX_REG_UxRBR                     0x0000 // DLAB=0 read
#define CYGARC_HAL_LPC24XX_REG_UxTHR                     0x0000 // DLAB=0 write
#define CYGARC_HAL_LPC24XX_REG_UxDLL                     0x0000 // DLAB=1 r/w
#define CYGARC_HAL_LPC24XX_REG_UxIER                     0x0004 // DLAB=0
#define CYGARC_HAL_LPC24XX_REG_UxIER_RXDATA_INT          (1<<0)
#define CYGARC_HAL_LPC24XX_REG_UxIER_THRE_INT            (1<<1)
#define CYGARC_HAL_LPC24XX_REG_UxIER_RXLS_INT            (1<<2)
#define CYGARC_HAL_LPC24XX_REG_U1IER_RXMS_INT            (1<<3) // U1 only
#define CYGARC_HAL_LPC24XX_REG_UxDLM                     0x0004 // DLAB=1

#define CYGARC_HAL_LPC24XX_REG_UxIIR                     0x0008 // read
#define CYGARC_HAL_LPC24XX_REG_UxIIR_IIR0                (1<<0)
#define CYGARC_HAL_LPC24XX_REG_UxIIR_IIR1                (1<<1)
#define CYGARC_HAL_LPC24XX_REG_UxIIR_IIR2                (1<<2)
#define CYGARC_HAL_LPC24XX_REG_UxIIR_IIR3                (1<<3)
#define CYGARC_HAL_LPC24XX_REG_UxIIR_FIFOS               (0xB0)

#define CYGARC_HAL_LPC24XX_REG_UxFCR                     0x0008 // write
#define CYGARC_HAL_LPC24XX_REG_UxFCR_FIFO_ENA            (1<<0)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_RX_FIFO_RESET       (1<<1)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_TX_FIFO_RESET       (1<<2)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_RX_TRIGGER_0        (0x00)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_RX_TRIGGER_1        (0x40)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_RX_TRIGGER_2        (0x80)
#define CYGARC_HAL_LPC24XX_REG_UxFCR_RX_TRIGGER_3        (0xB0)

#define CYGARC_HAL_LPC24XX_REG_UxLCR                     0x000C
#define CYGARC_HAL_LPC24XX_REG_UxLCR_WORD_LENGTH_5       (0x00)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_WORD_LENGTH_6       (0x01)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_WORD_LENGTH_7       (0x02)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_WORD_LENGTH_8       (0x03)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_STOP_1              (0x00)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_STOP_2              (0x04)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_PARITY_ENA          (0x08)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_PARITY_ODD          (0x00)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_PARITY_EVEN         (0x10)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_PARITY_ONE          (0x20)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_PARITY_ZERO         (0x30)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_BREAK_ENA           (0x40)
#define CYGARC_HAL_LPC24XX_REG_UxLCR_DLAB                (0x80)


// Modem Control Register is UART1 only
#define CYGARC_HAL_LPC24XX_REG_U1MCR                     0x0010
#define CYGARC_HAL_LPC24XX_REG_U1MCR_DTR                 (1<<0)
#define CYGARC_HAL_LPC24XX_REG_U1MCR_RTS                 (1<<1)
#define CYGARC_HAL_LPC24XX_REG_U1MCR_LOOPBACK            (1<<4)

#define CYGARC_HAL_LPC24XX_REG_UxLSR                     0x0014
#define CYGARC_HAL_LPC24XX_REG_UxLSR_RDR                 (1<<0)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_OE                  (1<<1)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_PE                  (1<<2)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_FE                  (1<<3)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_BI                  (1<<4)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_THRE                (1<<5)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_TEMT                (1<<6)
#define CYGARC_HAL_LPC24XX_REG_UxLSR_RX_FIFO_ERR         (1<<7)

// Modem Status Register is UART1 only
#define CYGARC_HAL_LPC24XX_REG_U1MSR                     0x0018
#define CYGARC_HAL_LPC24XX_REG_U1MSR_DCTS                (1<<0)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_DDSR                (1<<1)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_RI_FALL             (1<<2)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_DDCD                (1<<3)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_CTS                 (1<<4)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_DSR                 (1<<5)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_RI                  (1<<6)
#define CYGARC_HAL_LPC24XX_REG_U1MSR_DCD                 (1<<7)

#define CYGARC_HAL_LPC24XX_REG_UxSCR                     0x001C
#define CYGARC_HAL_LPC24XX_REG_UxACR                     0x0020
#define CYGARC_HAL_LPC24XX_REG_U3ICR                     0x0024
#define CYGARC_HAL_LPC24XX_REG_UxFDR                     0x0028
#define CYCARC_HAL_LPC24XX_REG_UxTER                     0x0030


//=============================================================================
// Pulse Width Modulator (PWM)

#define CYGARC_HAL_LPC24XX_REG_PWM0_BASE                 0xE0014000
#define CYGARC_HAL_LPC24XX_REG_PWM1_BASE                 0xE0018000


// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_PWMIR                     0x0000
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR0_INT             (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR1_INT             (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR2_INT             (1<<2)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR3_INT             (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR4_INT             (1<<8)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR5_INT             (1<<9)
#define CYGARC_HAL_LPC24XX_REG_PWMIR_MR6_INT             (1<<10)
#define CYGARC_HAL_LPC24XX_REG_PWMTCR                    0x0004
#define CYGARC_HAL_LPC24XX_REG_PWMTCR_CTR_ENA            (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PWMTCR_CTR_RESET          (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PWMTCR_PWM_ENA            (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PWMTC                     0x0008
#define CYGARC_HAL_LPC24XX_REG_PWMPR                     0x000C
#define CYGARC_HAL_LPC24XX_REG_PWMPC                     0x0010
#define CYGARC_HAL_LPC24XX_REG_PWMMCR                    0x0014
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR0_INT            (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR0_RESET          (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR0_STOP           (1<<2)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR1_INT            (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR1_RESET          (1<<4)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR1_STOP           (1<<5)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR2_INT            (1<<6)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR2_RESET          (1<<7)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR2_STOP           (1<<8)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR3_INT            (1<<9)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR3_RESET          (1<<10)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR3_STOP           (1<<11)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR4_INT            (1<<12)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR4_RESET          (1<<13)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR4_STOP           (1<<14)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR5_INT            (1<<15)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR5_RESET          (1<<16)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR5_STOP           (1<<17)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR6_INT            (1<<18)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR6_RESET          (1<<19)
#define CYGARC_HAL_LPC24XX_REG_PWMMCR_MR6_STOP           (1<<20)
#define CYGARC_HAL_LPC24XX_REG_PWMMR0                    0x0018
#define CYGARC_HAL_LPC24XX_REG_PWMMR1                    0x001C
#define CYGARC_HAL_LPC24XX_REG_PWMMR2                    0x0020
#define CYGARC_HAL_LPC24XX_REG_PWMMR3                    0x0024
#define CYGARC_HAL_LPC24XX_REG_PWMMR4                    0x0040
#define CYGARC_HAL_LPC24XX_REG_PWMMR5                    0x0044
#define CYGARC_HAL_LPC24XX_REG_PWMMR6                    0x0048
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR                   0x004C
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL1              (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL2              (1<<2)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL3              (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL4              (1<<4)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL5              (1<<5)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_SEL6              (1<<6)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA1              (1<<9)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA2              (1<<10)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA3              (1<<11)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA4              (1<<12)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA5              (1<<13)
#define CYGARC_HAL_LPC24XX_REG_PWMMPCR_ENA6              (1<<14)
#define CYGARC_HAL_LPC24XX_REG_PWMLER                    0x0050
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M0_ENA             (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M1_ENA             (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M2_ENA             (1<<2)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M3_ENA             (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M4_ENA             (1<<4)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M5_ENA             (1<<5)
#define CYGARC_HAL_LPC24XX_REG_PWMLER_M6_ENA             (1<<6)

#define CYGARC_HAL_LPC24XX_REG_PWMCTCR                   0x0070

//=============================================================================
// I2C (I2)

#define CYGARC_HAL_LPC24XX_REG_I2C0_BASE                 0xE001C000
#define CYGARC_HAL_LPC24XX_REG_I2C1_BASE                 0xE005C000
#define CYGARC_HAL_LPC24XX_REG_I2C2_BASE                 0xE0080000


// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_I2CONSET                  0x0000
#define CYGARC_HAL_LPC24XX_REG_I2CONSET_AA               (1<<2)
#define CYGARC_HAL_LPC24XX_REG_I2CONSET_SI               (1<<3)
#define CYGARC_HAL_LPC24XX_REG_I2CONSET_STO              (1<<4)
#define CYGARC_HAL_LPC24XX_REG_I2CONSET_STA              (1<<5)
#define CYGARC_HAL_LPC24XX_REG_I2CONSET_I2EN             (1<<6)
#define CYGARC_HAL_LPC24XX_REG_I2STAT                    0x0004
#define CYGARC_HAL_LPC24XX_REG_I2STAT_SHIFT              3
#define CYGARC_HAL_LPC24XX_REG_I2DAT                     0x0008
#define CYGARC_HAL_LPC24XX_REG_I2ADR                     0x000C
#define CYGARC_HAL_LPC24XX_REG_I2ADR_GC                  (1<<0)
#define CYGARC_HAL_LPC24XX_REG_I2SCLH                    0x0010
#define CYGARC_HAL_LPC24XX_REG_I2SCLL                    0x0014
#define CYGARC_HAL_LPC24XX_REG_I2CONCLR                  0x0018
#define CYGARC_HAL_LPC24XX_REG_I2CONCLR_AAC              (1<<2)
#define CYGARC_HAL_LPC24XX_REG_I2CONCLR_SIC              (1<<3)
#define CYGARC_HAL_LPC24XX_REG_I2CONCLR_STAC             (1<<5)
#define CYGARC_HAL_LPC24XX_REG_I2CONCLR_I2ENC            (1<<6)

//=============================================================================
// SPI (S)

#define CYGARC_HAL_LPC24XX_REG_SPI0_BASE                  0xE0020000
#define CYGARC_HAL_LPC24XX_REG_SPI1_BASE                  0xE0030000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR                   0x0000
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR_CPHA              (1<<3)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR_CPOL              (1<<4)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR_MSTR              (1<<5)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR_LSBF              (1<<6)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCR_SPIE              (1<<7)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR                   0x0004
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR_ABRT              (1<<3)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR_MODF              (1<<4)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR_ROVR              (1<<5)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR_WCOL              (1<<6)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPSR_SPIF              (1<<7)
#define CYGARC_HAL_LPC24XX_REG_SPI_SPDR                   0x0008
#define CYGARC_HAL_LPC24XX_REG_SPI_SPCCR                  0x000C
#define CYGARC_HAL_LPC24XX_REG_SPI_SPINT                  0x001C


//=============================================================================
// RTC

#define CYGARC_HAL_LPC24XX_REG_RTC_BASE                   0xE0024000
#define CYGARC_HAL_LPC2XXX_REG_RTC_BASE                   0xE0024000 


// Registers are offsets from base of this subsystem

#define CYGARC_HAL_LPC24XX_REG_RTC_ILR                    0x0000
#define CYGARC_HAL_LPC24XX_REG_RTC_ILR_CIF                (1<<0)
#define CYGARC_HAL_LPC24XX_REG_RTC_ILR_ALF                (1<<1)
#define CYGARC_HAL_LPC24XX_REG_RTC_CTC                    0x0004
#define CYGARC_HAL_LPC24XX_REG_RTC_CCR                    0x0008
#define CYGARC_HAL_LPC24XX_REG_RTC_CCR_CLKEN              (1<<0)
#define CYGARC_HAL_LPC24XX_REG_RTC_CCR_CTCRST             (1<<1)
#define CYGARC_HAL_LPC24XX_REG_RTC_CIIR                   0x000C
#define CYGARC_HAL_LPC24XX_REG_RTC_AMR                    0x0010
#define CYGARC_HAL_LPC24XX_REG_RTC_CTIME0                 0x0014
#define CYGARC_HAL_LPC24XX_REG_RTC_CTIME1                 0x0018
#define CYGARC_HAL_LPC24XX_REG_RTC_CTIME2                 0x001C
#define CYGARC_HAL_LPC24XX_REG_RTC_SEC                    0x0020
#define CYGARC_HAL_LPC24XX_REG_RTC_MIN                    0x0024
#define CYGARC_HAL_LPC24XX_REG_RTC_HOUR                   0x0028
#define CYGARC_HAL_LPC24XX_REG_RTC_DOM                    0x002C
#define CYGARC_HAL_LPC24XX_REG_RTC_DOW                    0x0030
#define CYGARC_HAL_LPC24XX_REG_RTC_DOY                    0x0034
#define CYGARC_HAL_LPC24XX_REG_RTC_MONTH                  0x0038
#define CYGARC_HAL_LPC24XX_REG_RTC_YEAR                   0x003C
#define CYGARC_HAL_LPC24XX_REG_RTC_ALSEC                  0x0060
#define CYGARC_HAL_LPC24XX_REG_RTC_ALMIN                  0x0064
#define CYGARC_HAL_LPC24XX_REG_RTC_ALHOUR                 0x0068
#define CYGARC_HAL_LPC24XX_REG_RTC_ALDOM                  0x006C
#define CYGARC_HAL_LPC24XX_REG_RTC_ALDOW                  0x0070
#define CYGARC_HAL_LPC24XX_REG_RTC_ALDOY                  0x0074
#define CYGARC_HAL_LPC24XX_REG_RTC_ALMON                  0x0078
#define CYGARC_HAL_LPC24XX_REG_RTC_ALYEAR                 0x007C
#define CYGARC_HAL_LPC24XX_REG_RTC_PREINT                 0x0080
#define CYGARC_HAL_LPC24XX_REG_RTC_PREFRAC                0x0084

//=============================================================================
// GPIO (IO)

#define CYGARC_HAL_LPC24XX_REG_IO_BASE                   0xE0028000
#define CYGARC_HAL_LPC24XX_REG_FIO_BASE                  0x3FFFC000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_IO0PIN                    0x000
#define CYGARC_HAL_LPC24XX_REG_IO0SET                    0x004
#define CYGARC_HAL_LPC24XX_REG_IO0DIR                    0x008
#define CYGARC_HAL_LPC24XX_REG_IO0CLR                    0x00C

#define CYGARC_HAL_LPC24XX_REG_IO1PIN                    0x010
#define CYGARC_HAL_LPC24XX_REG_IO1SET                    0x014
#define CYGARC_HAL_LPC24XX_REG_IO1DIR                    0x018
#define CYGARC_HAL_LPC24XX_REG_IO1CLR                    0x01C

#define CYGARC_HAL_LPC24XX_REG_FIO0DIR                   0x0000
#define CYGARC_HAL_LPC24XX_REG_FIO1DIR                   0x0020
#define CYGARC_HAL_LPC24XX_REG_FIO2DIR                   0x0040
#define CYGARC_HAL_LPC24XX_REG_FIO3DIR                   0x0050
#define CYGARC_HAL_LPC24XX_REG_FIO4DIR                   0x0080

#define CYGARC_HAL_LPC24XX_REG_FIO0SET                   0x0018
#define CYGARC_HAL_LPC24XX_REG_FIO1SET                   0x0038
#define CYGARC_HAL_LPC24XX_REG_FIO2SET                   0x0058
#define CYGARC_HAL_LPC24XX_REG_FIO3SET                   0x0078
#define CYGARC_HAL_LPC24XX_REG_FIO4SET                   0x0098

#define CYGARC_HAL_LPC24XX_REG_FIO0CLR                   0x001C
#define CYGARC_HAL_LPC24XX_REG_FIO1CLR                   0x003C
#define CYGARC_HAL_LPC24XX_REG_FIO2CLR                   0x005C
#define CYGARC_HAL_LPC24XX_REG_FIO3CLR                   0x007C
#define CYGARC_HAL_LPC24XX_REG_FIO4CLR                   0x009C

#define CYGARC_HAL_LPC24XX_REG_FIO0PIN                   0x0014
#define CYGARC_HAL_LPC24XX_REG_FIO1PIN                   0x0034
#define CYGARC_HAL_LPC24XX_REG_FIO2PIN                   0x0054
#define CYGARC_HAL_LPC24XX_REG_FIO3PIN                   0x0074
#define CYGARC_HAL_LPC24XX_REG_FIO4PIN                   0x0094

#define CYGARC_HAL_LPC24XX_REG_FIO0MASK                  0x0010
#define CYGARC_HAL_LPC24XX_REG_FIO1MASK                  0x0030
#define CYGARC_HAL_LPC24XX_REG_FIO2MASK                  0x0050
#define CYGARC_HAL_LPC24XX_REG_FIO3MASK                  0x0070
#define CYGARC_HAL_LPC24XX_REG_FIO4MASK                  0x0090



//=============================================================================
// Pin Connect Block (PIN)

#define CYGARC_HAL_LPC24XX_REG_PIN_BASE                  0xE002C000

#define CYGARC_HAL_LPC24XX_REG_PINSEL0                   0x000
#define CYGARC_HAL_LPC24XX_REG_PINSEL1                   0x004
#define CYGARC_HAL_LPC24XX_REG_PINSEL2                   0x008
#define CYGARC_HAL_LPC24XX_REG_PINSEL3                   0x00C
#define CYGARC_HAL_LPC24XX_REG_PINSEL4                   0x010
#define CYGARC_HAL_LPC24XX_REG_PINSEL5                   0x014
#define CYGARC_HAL_LPC24XX_REG_PINSEL6                   0x018
#define CYGARC_HAL_LPC24XX_REG_PINSEL7                   0x01C
#define CYGARC_HAL_LPC24XX_REG_PINSEL8                   0x020
#define CYGARC_HAL_LPC24XX_REG_PINSEL9                   0x024
#define CYGARC_HAL_LPC24XX_REG_PINSEL10                  0x028
#define CYGARC_HAL_LPC24XX_REG_PINSEL11                  0x02C

#define CYGARC_HAL_LPC24XX_REG_PINMODE0                  0x040
#define CYGARC_HAL_LPC24XX_REG_PINMODE1                  0x044
#define CYGARC_HAL_LPC24XX_REG_PINMODE2                  0x048
#define CYGARC_HAL_LPC24XX_REG_PINMODE3                  0x04C
#define CYGARC_HAL_LPC24XX_REG_PINMODE4                  0x050
#define CYGARC_HAL_LPC24XX_REG_PINMODE5                  0x054
#define CYGARC_HAL_LPC24XX_REG_PINMODE6                  0x058
#define CYGARC_HAL_LPC24XX_REG_PINMODE7                  0x05C
#define CYGARC_HAL_LPC24XX_REG_PINMODE8                  0x060
#define CYGARC_HAL_LPC24XX_REG_PINMODE9                  0x064


//=============================================================================
// SSP - Synchronous Serial Port
#define CYGARC_HAL_LPC24XX_REG_SSP0_BASE                 0xE0068000
#define CYGARC_HAL_LPC24XX_REG_SSP1_BASE                 0xE0030000

#define CYGARC_HAL_LPC24XX_REG_SSP_CR0                   0x0000
#define CYGARC_HAL_LPC24XX_REG_SSP_CR1                   0x0004
#define CYGARC_HAL_LPC24XX_REG_SSP_DR                    0x0008
#define CYGARC_HAL_LPC24XX_REG_SSP_SR                    0x000C
#define CYGARC_HAL_LPC24XX_REG_SSP_CPSR                  0x0010
#define CYGARC_HAL_LPC24XX_REG_SSP_IMSC                  0x0014
#define CYGARC_HAL_LPC24XX_REG_SSP_RIS                   0x0018
#define CYGARC_HAL_LPC24XX_REG_SSP_MIS                   0x001C
#define CYGARC_HAL_LPC24XX_REG_SSP_ICR                   0x0020
#define CYGARC_HAL_LPC24XX_REG_SSP_DMACR                 0x0024


//=============================================================================
// ADC (AD)
#define CYGARC_HAL_LPC24XX_REG_AD_BASE                  0xE0034000
#define CYGARC_HAL_LPC2XXX_REG_AD_BASE CYGARC_HAL_LPC24XX_REG_AD_BASE

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC24XX_REG_ADCR                     0x0000
#define CYGARC_HAL_LPC24XX_REG_ADCR_BURST               (1<<16)
#define CYGARC_HAL_LPC24XX_REG_ADCR_PDN                 (1<<21)
#define CYGARC_HAL_LPC24XX_REG_ADCR_EDGE                (1<<27)
#define CYGARC_HAL_LPC24XX_REG_ADGDR                    0x0004
#define CYGARC_HAL_LPC24XX_REG_ADSTAT                   0x0030
#define CYGARC_HAL_LPC24XX_REG_ADINTEN                  0x000C
#define CYGARC_HAL_LPC24XX_REG_ADDR0                    0x0010
#define CYGARC_HAL_LPC24XX_REG_ADDR1                    0x0018
#define CYGARC_HAL_LPC24XX_REG_ADDR2                    0x0018
#define CYGARC_HAL_LPC24XX_REG_ADDR3                    0x001C
#define CYGARC_HAL_LPC24XX_REG_ADDR4                    0x0020
#define CYGARC_HAL_LPC24XX_REG_ADDR5                    0x0024
#define CYGARC_HAL_LPC24XX_REG_ADDR6                    0x0028
#define CYGARC_HAL_LPC24XX_REG_ADDR7                    0x002C


//=============================================================================
// CAN
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_RAM         0xE0038000
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_BASE        0xE003C000
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_AFMR        0x0000
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_SFF_sa      0x0004
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_SFF_GRP_sa  0x0008
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_EFF_sa      0x000C
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_EFF_GRP_sa  0x0010
#define CYGARC_HAL_LPC24XX_REG_CAN_ACCFILT_END         0x0014

#define CYGARC_HAL_LPC24XX_REG_CAN_COMMON_BASE         0xE0040000
#define CYGARC_HAL_LPC24XX_REG_CAN_TxSR                0x0000
#define CYGARC_HAL_LPC24XX_REG_CAN_RxSR                0x0004
#define CYGARC_HAL_LPC24XX_REG_CAN_MSR                 0x0008

#define CYGARC_HAL_LPC24XX_REG_CAN0_BASE               0xE0044000
#define CYGARC_HAL_LPC24XX_REG_CAN1_BASE               0xE0048000
#define CYCARC_HAL_LPC24XX_REG_CANx_MOD                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_CMR                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_GSR                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_ICR                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_IER                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_BTR                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_EWL                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_SR                 0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RFS                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RID                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RDA                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RDB                0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RFI1               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TID1               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDA1               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDB1               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RFI2               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TID2               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDA2               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDB2               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_RFI3               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TID3               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDA3               0x0000
#define CYCARC_HAL_LPC24XX_REG_CANx_TDB3               0x0000


//=============================================================================
// DAC
#define CYGARC_HAL_LPC24XX_REG_DAC_BASE                0xE006C000


//=============================================================================
// Battery RAM
#define CYGARC_HAL_LPC24XX_REG_BATTERY_RAM             0xE0084000


//=============================================================================
// I2S
#define CYGARC_HAL_LPC24XX_REG_I2S_BASE                0xE0088000

#define CYGARC_HAL_LPC24XX_REG_I2S_DAO                 0x0000
#define CYGARC_HAL_LPC24XX_REG_I2S_DAI                 0x0004
#define CYGARC_HAL_LPC24XX_REG_I2S_TXFIFO              0x0008
#define CYGARC_HAL_LPC24XX_REG_I2S_RXFIFO              0x000C
#define CYGARC_HAL_LPC24XX_REG_I2S_STATE               0x0010
#define CYGARC_HAL_LPC24XX_REG_I2S_DMA1                0x0014
#define CYGARC_HAL_LPC24XX_REG_I2S_DMA2                0x0018
#define CYGARC_HAL_LPC24XX_REG_I2S_IRQ                 0x001C
#define CYGARC_HAL_LPC24XX_REG_I2S_TXRATE              0x0020
#define CYGARC_HAL_LPC24XX_REG_I2S_RXRATE              0x0024



//=============================================================================
// SD/MMC Card Interface
#define CYGARC_HAL_LPC24XX_REG_SD_MMC_BASE             0xE008C000


//=============================================================================
// System Control Block

#define CYGARC_HAL_LPC24XX_REG_SCB_BASE                 0xE01FC000

// Registers are offsets from base of this subsystem

// Memory accelerator module
#define CYGARC_HAL_LPC24XX_REG_MAMCR                    0x0000
#define CYGARC_HAL_LPC24XX_REG_MAMCR_DISABLED           0x00
#define CYGARC_HAL_LPC24XX_REG_MAMCR_PARTIAL            0x01
#define CYGARC_HAL_LPC24XX_REG_MAMCR_FULL               0x02
#define CYGARC_HAL_LPC24XX_REG_MAMTIM                   0x0004

// Memory mapping control
#define CYGARC_HAL_LPC24XX_REG_MEMMAP                   0x0040

// PLL
#define CYGARC_HAL_LPC24XX_REG_PLLCON                   0x0080
#define CYGARC_HAL_LPC24XX_REG_PLLCON_PLLE              (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PLLCON_PLLC              (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PLLCFG                   0x0084
#define CYGARC_HAL_LPC24XX_REG_PLLSTAT                  0x0088
#define CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLLE             (1<<24)
#define CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLLC             (1<<25)
#define CYGARC_HAL_LPC24XX_REG_PLLSTAT_PLOCK            (1<<26)
#define CYGARC_HAL_LPC24XX_REG_PLLFEED                  0x008C

// Power Control
#define CYGARC_HAL_LPC24XX_REG_PCON                     0x00C0
#define CYGARC_HAL_LPC24XX_REG_PCON_IDL                 (1<<0)
#define CYGARC_HAL_LPC24XX_REG_PCON_PD                  (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PCONP                    0x00C4
#define CYGARC_HAL_LPC24XX_REG_PCONP_TIM0               (1<<1)
#define CYGARC_HAL_LPC24XX_REG_PCONP_TIM1               (1<<2)
#define CYGARC_HAL_LPC24XX_REG_PCONP_URT0               (1<<3)
#define CYGARC_HAL_LPC24XX_REG_PCONP_URT1               (1<<4)
#define CYGARC_HAL_LPC24XX_REG_PCONP_PWM0               (1<<5)
#define CYGARC_HAL_LPC24XX_REG_PCONP_PWM1               (1<<6)
#define CYGARC_HAL_LPC24XX_REG_PCONP_I2C0               (1<<7)
#define CYGARC_HAL_LPC24XX_REG_PCONP_SPI                (1<<8)
#define CYGARC_HAL_LPC24XX_REG_PCONP_RTC                (1<<9)
#define CYGARC_HAL_LPC24XX_REG_PCONP_SSP1               (1<<10)
#define CYGARC_HAL_LPC24XX_REG_PCONP_EMC                (1<<11)
#define CYGARC_HAL_LPC24XX_REG_PCONP_AD                 (1<<12)
#define CYGARC_HAL_LPC24XX_REG_PCONP_CAN1               (1<<13)
#define CYGARC_HAL_LPC24XX_REG_PCONP_CAN2               (1<<14)
#define CYGARC_HAL_LPC24XX_REG_PCONP_I2C1               (1<<19)
#define CYGARC_HAL_LPC24XX_REG_PCONP_LCD                (1<<20)
#define CYGARC_HAL_LPC24XX_REG_PCONP_SSP0               (1<<21)
#define CYGARC_HAL_LPC24XX_REG_PCONP_TIM2               (1<<22)
#define CYGARC_HAL_LPC24XX_REG_PCONP_TIM3               (1<<23)
#define CYGARC_HAL_LPC24XX_REG_PCONP_URT2               (1<<24)
#define CYGARC_HAL_LPC24XX_REG_PCONP_URT3               (1<<25)
#define CYGARC_HAL_LPC24XX_REG_PCONP_I2C2               (1<<26)
#define CYGARC_HAL_LPC24XX_REG_PCONP_I2S                (1<<27)
#define CYGARC_HAL_LPC24XX_REG_PCONP_SD                 (1<<28)
#define CYGARC_HAL_LPC24XX_REG_PCONP_DMA                (1<<29)
#define CYGARC_HAL_LPC24XX_REG_PCONP_ENET               (1<<30)
#define CYGARC_HAL_LPC24XX_REG_PCONP_USB                (1<<31)

// External interrupt inputs
#define CYGARC_HAL_LPC24XX_REG_EXTINT                   0x0140
#define CYGARC_HAL_LPC24XX_REG_EXTMODE                  0x0148
#define CYGARC_HAL_LPC24XX_REG_EXTPOLAR                 0x014C

#define CYGARC_HAL_LPC24XX_REG_EXTxxx_INT0              (1<<0)
#define CYGARC_HAL_LPC24XX_REG_EXTxxx_INT1              (1<<1)
#define CYGARC_HAL_LPC24XX_REG_EXTxxx_INT2              (1<<2)
#define CYGARC_HAL_LPC24XX_REG_EXTxxx_INT3              (1<<3)

// Reset source identification register
#define CYGARC_HAL_LPC24XX_REG_RSID                     0x0180
#define CYGARC_HAL_LPC24XX_REG_RSID_POR                 (1<<0)
#define CYGARC_HAL_LPC24XX_REG_RSID_EXTR                (1<<1)
#define CYGARC_HAL_LPC24XX_REG_RSID_WDTR                (1<<2)
#define CYGARC_HAL_LPC24XX_REG_RSID_BODR                (1<<3)

// System control and status register
#define CYGARC_HAL_LPC24XX_REG_SCS                      0x01A0
#define CYGARC_HAL_LPC24XX_REG_SCS_OSCEN                0x20
#define CYGARC_HAL_LPC24XX_REG_SCS_OSCSTAT              0x40

// Clock source selection register
#define CYGARC_HAL_LPC24XX_REG_CLKSRCSEL                0x010C
#define CYGARC_HAL_LPC24XX_REG_CLKSRCSEL_IRC            0x00 
#define CYGARC_HAL_LPC24XX_REG_CLKSRCSEL_MAIN           0x01
#define CYGARC_HAL_LPC24XX_REG_CLKSRCSEL_RTC            0x10

#define CYGARC_HAL_LPC24XX_REG_CCLKCFG                  0x0104
#define CYGARC_HAL_LPC24XX_REG_USBCLKCFG                0x0108
#define CYGARC_HAL_LPC24XX_REG_IRCTRIM                  0x01A4
#define CYGARC_HAL_LPC24XX_REG_PCLKSEL0                 0x01A8 
#define CYGARC_HAL_LPC24XX_REG_PCLKSEL1                 0x01AC
#define CYGARC_HAL_LPC24XX_REG_INTWAKE                  0x0144


//=============================================================================
// External Memory Controller
#define CYGARC_HAL_LPC24XX_REG_EMC_BASE                 0xFFE08000

#define CYGARC_HAL_LPC24XX_REG_EMC_CTRL                  0x0000
#define CYGARC_HAL_LPC24XX_REG_EMC_CTRL_EN              (1 << 0)
#define CYGARC_HAL_LPC24XX_REG_EMC_CTRL_ADDRMIRR        (1 << 1)
#define CYGARC_HAL_LPC24XX_REG_EMC_CTRL_LOWPOW          (1 << 2)
#define CYGARC_HAL_LPC24XX_REG_EMC_STATUS               0x0004
#define CYGARC_HAL_LPC24XX_REG_EMC_CONFIG               0x0008
#define CYGARC_HAL_LPC24XX_REG_EMCD_CONTROL             0x0020
#define CYGARC_HAL_LPC24XX_REG_EMCD_REFRESH             0x0024
#define CYGARC_HAL_LPC24XX_REG_EMCD_RDCFG               0x0028
#define CYGARC_HAL_LPC24XX_REG_EMCD_RP                  0x0030
#define CYGARC_HAL_LPC24XX_REG_EMCD_RAS                 0x0034
#define CYGARC_HAL_LPC24XX_REG_EMCD_SREX                0x0038
#define CYGARC_HAL_LPC24XX_REG_EMCD_APR                 0x003C
#define CYGARC_HAL_LPC24XX_REG_EMCD_DAL                 0x0040
#define CYGARC_HAL_LPC24XX_REG_EMCD_WR                  0x0044
#define CYGARC_HAL_LPC24XX_REG_EMCD_RC                  0x0048
#define CYGARC_HAL_LPC24XX_REG_EMCD_RFC                 0x004C
#define CYGARC_HAL_LPC24XX_REG_EMCD_XSR                 0x0050
#define CYGARC_HAL_LPC24XX_REG_EMCD_RRD                 0x0054
#define CYGARC_HAL_LPC24XX_REG_EMCD_MRD                 0x0058
#define CYGARC_HAL_LPC24XX_REG_EMCS_EXT_WAIT            0x0080

#define CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG0             0x0100
#define CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS0             0x0104
#define CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG1             0x0120
#define CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS1             0x0124
#define CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG2             0x0140
#define CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS2             0x0144
#define CYGARC_HAL_LPC24XX_REG_EMCD_CONFIG3             0x0160
#define CYGARC_HAL_LPC24XX_REG_EMCD_RASCAS3             0x0164

#define CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG0             0x0200
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN0           0x0204
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN0           0x0208
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD0             0x020C
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE0           0x0210
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR0             0x0214
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN0           0x0218

#define CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG1             0x0220
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN1           0x0224
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN1           0x0228
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD1             0x022C
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE1           0x0230
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR1             0x0234
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN1           0x0238

#define CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG2             0x0240
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN2           0x0244
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN2           0x0248
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD2             0x024C
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE2           0x0250
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR2             0x0254
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN2           0x0258

#define CYGARC_HAL_LPC24XX_REG_EMCS_CONFIG3             0x0260
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITW_EN3           0x0264
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITO_EN3           0x0268
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITRD3             0x026C
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITPAGE3           0x0270
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITWR3             0x0274
#define CYGARC_HAL_LPC24XX_REG_EMCS_WAITTURN3           0x0278


//=============================================================================
// Vectored Interrupt Controller (VIC)

#define CYGARC_HAL_LPC24XX_REG_VIC_BASE                 0xFFFFF000

// Registers are offsets from base of this subsystem

#define CYGARC_HAL_LPC24XX_REG_VICIRQSTAT               0x000
#define CYGARC_HAL_LPC24XX_REG_VICFIQSTAT               0x004
#define CYGARC_HAL_LPC24XX_REG_VICRAWINTR               0x008
#define CYGARC_HAL_LPC24XX_REG_VICINTSELECT             0x00C
#define CYGARC_HAL_LPC24XX_REG_VICINTENABLE             0x010
#define CYGARC_HAL_LPC24XX_REG_VICINTENCLEAR            0x014
#define CYGARC_HAL_LPC24XX_REG_VICSOFTINT               0x018
#define CYGARC_HAL_LPC24XX_REG_VICSOFTINTCLEAR          0x01C
#define CYGARC_HAL_LPC24XX_REG_VICPROTECTION            0x020
#define CYGARC_HAL_LPC24XX_REG_VICSWPRIOMASK            0x020

#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR0             0x100
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR1             0x104
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR2             0x108
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR3             0x10C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR4             0x110
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR5             0x114
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR6             0x118
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR7             0x11C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR8             0x120
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR9             0x124
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR10            0x128
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR11            0x12C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR12            0x130
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR13            0x134
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR14            0x138
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR15            0x13C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR16            0x140
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR17            0x144
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR18            0x148
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR19            0x14C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR20            0x150
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR21            0x154
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR22            0x158
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR23            0x15C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR24            0x160
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR25            0x164
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR26            0x168
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR27            0x16C
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR28            0x170
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR29            0x174
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR30            0x178
#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR31            0x17C

#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO0             0x200
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO1             0x204
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO2             0x208
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO3             0x20C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO4             0x210
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO5             0x214
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO6             0x218
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO7             0x21C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO8             0x220
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO9             0x224
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO10            0x228
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO11            0x22C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO12            0x230
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO13            0x234
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO14            0x238
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO15            0x23C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO16            0x240
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO17            0x244
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO18            0x248
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO19            0x24C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO20            0x250
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO21            0x254
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO22            0x258
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO23            0x25C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO24            0x260
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO25            0x264
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO26            0x268
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO27            0x26C
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO28            0x270
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO29            0x274
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO30            0x278
#define CYGARC_HAL_LPC24XX_REG_VICVECTPRIO31            0x27C


#define CYGARC_HAL_LPC24XX_REG_VICVECTADDR              0xF00


//=============================================================================
// Ethernet (EMAC)
#define CYGARC_HAL_LPC2XXX_REG_EMAC_BASE                0xFFE00000

//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
