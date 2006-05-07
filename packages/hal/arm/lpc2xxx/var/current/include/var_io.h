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
// Author(s):   jlarmour
// Contributors:
// Date:        2004-07-23
// Purpose:     Philips LPC2xxx variant specific registers
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_arm_lpc2xxx.h>  // variant chip model selection.
#include <cyg/hal/plf_io.h>

//=============================================================================
// Watchdog (WD)

#define CYGARC_HAL_LPC2XXX_REG_WD_BASE                   0xE0000000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_WDMOD                     0x0000
#define CYGARC_HAL_LPC2XXX_REG_WDMOD_WDEN                (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_WDMOD_WDRESET             (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_WDMOD_WDTOF               (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_WDMOD_WDINT               (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_WDTC                      0x0004
#define CYGARC_HAL_LPC2XXX_REG_WDFEED                    0x0008
#define CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC1             0xAA
#define CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC2             0x55
#define CYGARC_HAL_LPC2XXX_REG_WDTV                      0x000C

//=============================================================================
// Timers (Tx)

#define CYGARC_HAL_LPC2XXX_REG_TIMER0_BASE               0xE0004000
#define CYGARC_HAL_LPC2XXX_REG_TIMER1_BASE               0xE0008000

// Registers are offsets from base for each timer
#define CYGARC_HAL_LPC2XXX_REG_TxIR                      0x0000
#define CYGARC_HAL_LPC2XXX_REG_TxIR_MR0                  (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_MR1                  (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_MR2                  (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_MR3                  (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_CR0                  (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_CR1                  (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_CR2                  (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_TxIR_CR3                  (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_TxTCR                     0x0004
#define CYGARC_HAL_LPC2XXX_REG_TxTCR_CTR_ENABLE          (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_TxTCR_CTR_RESET           (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_TxTC                      0x0008
#define CYGARC_HAL_LPC2XXX_REG_TxPR                      0x000C
#define CYGARC_HAL_LPC2XXX_REG_TxPC                      0x0010
#define CYGARC_HAL_LPC2XXX_REG_TxMCR                     0x0014
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_INT             (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_RESET           (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR0_STOP            (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR1_INT             (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR1_RESET           (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR1_STOP            (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR2_INT             (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR2_RESET           (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR2_STOP            (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR3_INT             (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR3_RESET           (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_TxMCR_MR3_STOP            (1<<11)
#define CYGARC_HAL_LPC2XXX_REG_TxMR0                     0x0018
#define CYGARC_HAL_LPC2XXX_REG_TxMR1                     0x001C
#define CYGARC_HAL_LPC2XXX_REG_TxMR2                     0x0020
#define CYGARC_HAL_LPC2XXX_REG_TxMR3                     0x0024
#define CYGARC_HAL_LPC2XXX_REG_TxCCR                     0x0028
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR0_RISE        (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR0_FALL        (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR0             (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR1_RISE        (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR1_FALL        (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR1             (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR2_RISE        (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR2_FALL        (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR2             (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR3_RISE        (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR3_FALL        (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_TxCCR_INT_CR3             (1<<11)
#define CYGARC_HAL_LPC2XXX_REG_TxCR0                     0x002C
#define CYGARC_HAL_LPC2XXX_REG_TxCR1                     0x0030
#define CYGARC_HAL_LPC2XXX_REG_TxCR2                     0x0034
#define CYGARC_HAL_LPC2XXX_REG_TxCR3                     0x0038
#define CYGARC_HAL_LPC2XXX_REG_TxEMR                     0x003C
#define CYGARC_HAL_LPC2XXX_REG_TxEMR_EM0                 (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_TxEMR_EM1                 (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_TxEMR_EM2                 (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_TxEMR_EM3                 (1<<3)

//=============================================================================
// UARTs (Ux)

#define CYGARC_HAL_LPC2XXX_REG_UART0_BASE                0xE000C000
#define CYGARC_HAL_LPC2XXX_REG_UART1_BASE                0xE0010000

// Registers are offsets from base for each UART
#define CYGARC_HAL_LPC2XXX_REG_UxRBR                     0x0000 // DLAB=0 read
#define CYGARC_HAL_LPC2XXX_REG_UxTHR                     0x0000 // DLAB=0 write
#define CYGARC_HAL_LPC2XXX_REG_UxDLL                     0x0000 // DLAB=1 r/w
#define CYGARC_HAL_LPC2XXX_REG_UxIER                     0x0004 // DLAB=0
#define CYGARC_HAL_LPC2XXX_REG_UxIER_RXDATA_INT          (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_UxIER_THRE_INT            (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_UxIER_RXLS_INT            (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_U1IER_RXMS_INT            (1<<3) // U1 only
#define CYGARC_HAL_LPC2XXX_REG_UxDLM                     0x0004 // DLAB=1

#define CYGARC_HAL_LPC2XXX_REG_UxIIR                     0x0008 // read
#define CYGARC_HAL_LPC2XXX_REG_UxIIR_IIR0                (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_UxIIR_IIR1                (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_UxIIR_IIR2                (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_UxIIR_IIR3                (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_UxIIR_FIFOS               (0xB0)

#define CYGARC_HAL_LPC2XXX_REG_UxFCR                     0x0008 // write
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_FIFO_ENA            (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_RX_FIFO_RESET       (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_TX_FIFO_RESET       (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_RX_TRIGGER_0        (0x00)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_RX_TRIGGER_1        (0x40)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_RX_TRIGGER_2        (0x80)
#define CYGARC_HAL_LPC2XXX_REG_UxFCR_RX_TRIGGER_3        (0xB0)

#define CYGARC_HAL_LPC2XXX_REG_UxLCR                     0x000C
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_WORD_LENGTH_5       (0x00)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_WORD_LENGTH_6       (0x01)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_WORD_LENGTH_7       (0x02)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_WORD_LENGTH_8       (0x03)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_STOP_1              (0x00)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_STOP_2              (0x04)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_PARITY_ENA          (0x08)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_PARITY_ODD          (0x00)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_PARITY_EVEN         (0x10)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_PARITY_ONE          (0x20)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_PARITY_ZERO         (0x30)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_BREAK_ENA           (0x40)
#define CYGARC_HAL_LPC2XXX_REG_UxLCR_DLAB                (0x80)


// Modem Control Register is UART1 only
#define CYGARC_HAL_LPC2XXX_REG_U1MCR                     0x0010
#define CYGARC_HAL_LPC2XXX_REG_U1MCR_DTR                 (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_U1MCR_RTS                 (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_U1MCR_LOOPBACK            (1<<4)

#define CYGARC_HAL_LPC2XXX_REG_UxLSR                     0x0014
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_RDR                 (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_OE                  (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_PE                  (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_FE                  (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_BI                  (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_THRE                (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_TEMT                (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_UxLSR_RX_FIFO_ERR         (1<<7)

// Modem Status Register is UART1 only
#define CYGARC_HAL_LPC2XXX_REG_U1MSR                     0x0018
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_DCTS                (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_DDSR                (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_RI_FALL             (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_DDCD                (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_CTS                 (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_DSR                 (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_RI                  (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_U1MSR_DCD                 (1<<7)

#define CYGARC_HAL_LPC2XXX_REG_UxSCR                     0x001C

//=============================================================================
// Pulse Width Modulator (PWM)

#define CYGARC_HAL_LPC2XXX_REG_PWM_BASE                  0xE0014000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_PWMIR                     0x0000
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR0_INT             (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR1_INT             (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR2_INT             (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR3_INT             (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR4_INT             (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR5_INT             (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_PWMIR_MR6_INT             (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_PWMTCR                    0x0004
#define CYGARC_HAL_LPC2XXX_REG_PWMTCR_CTR_ENA            (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PWMTCR_CTR_RESET          (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PWMTCR_PWM_ENA            (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PWMTC                     0x0008
#define CYGARC_HAL_LPC2XXX_REG_PWMPR                     0x000C
#define CYGARC_HAL_LPC2XXX_REG_PWMPC                     0x0010
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR                    0x0014
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR0_INT            (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR0_RESET          (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR0_STOP           (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR1_INT            (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR1_RESET          (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR1_STOP           (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR2_INT            (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR2_RESET          (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR2_STOP           (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR3_INT            (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR3_RESET          (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR3_STOP           (1<<11)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR4_INT            (1<<12)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR4_RESET          (1<<13)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR4_STOP           (1<<14)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR5_INT            (1<<15)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR5_RESET          (1<<16)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR5_STOP           (1<<17)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR6_INT            (1<<18)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR6_RESET          (1<<19)
#define CYGARC_HAL_LPC2XXX_REG_PWMMCR_MR6_STOP           (1<<20)
#define CYGARC_HAL_LPC2XXX_REG_PWMMR0                    0x0018
#define CYGARC_HAL_LPC2XXX_REG_PWMMR1                    0x001C
#define CYGARC_HAL_LPC2XXX_REG_PWMMR2                    0x0020
#define CYGARC_HAL_LPC2XXX_REG_PWMMR3                    0x0024
#define CYGARC_HAL_LPC2XXX_REG_PWMMR4                    0x0040
#define CYGARC_HAL_LPC2XXX_REG_PWMMR5                    0x0044
#define CYGARC_HAL_LPC2XXX_REG_PWMMR6                    0x0048
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR                   0x004C
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL1              (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL2              (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL3              (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL4              (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL5              (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_SEL6              (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA1              (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA2              (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA3              (1<<11)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA4              (1<<12)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA5              (1<<13)
#define CYGARC_HAL_LPC2XXX_REG_PWMMPCR_ENA6              (1<<14)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER                    0x0050
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M0_ENA             (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M1_ENA             (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M2_ENA             (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M3_ENA             (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M4_ENA             (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M5_ENA             (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_PWMLER_M6_ENA             (1<<6)

//=============================================================================
// I2C (I2)

#define CYGARC_HAL_LPC2XXX_REG_I2_BASE                   0xE001C000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET                  0x0000
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET_AA               (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET_SI               (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET_STO              (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET_STA              (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_I2CONSET_I2EN             (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_I2STAT                    0x0004
#define CYGARC_HAL_LPC2XXX_REG_I2STAT_SHIFT              3
#define CYGARC_HAL_LPC2XXX_REG_I2DAT                     0x0008
#define CYGARC_HAL_LPC2XXX_REG_I2ADR                     0x000C
#define CYGARC_HAL_LPC2XXX_REG_I2ADR_GC                  (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_I2SCLH                    0x0010
#define CYGARC_HAL_LPC2XXX_REG_I2SCLL                    0x0014
#define CYGARC_HAL_LPC2XXX_REG_I2CONCLR                  0x0018
#define CYGARC_HAL_LPC2XXX_REG_I2CONCLR_AAC              (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_I2CONCLR_SIC              (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_I2CONCLR_STAC             (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_I2CONCLR_I2ENC            (1<<6)

//=============================================================================
// SPI (S)

#define CYGARC_HAL_LPC2XXX_REG_SPI0_BASE                  0xE0020000
#define CYGARC_HAL_LPC2XXX_REG_SPI1_BASE                  0xE0030000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR                   0x0000
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR_CPHA              (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR_CPOL              (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR_MSTR              (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR_LSBF              (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCR_SPIE              (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR                   0x0004
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR_ABRT              (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR_MODF              (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR_ROVR              (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR_WCOL              (1<<6)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPSR_SPIF              (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPDR                   0x0008
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPCCR                  0x000C
#define CYGARC_HAL_LPC2XXX_REG_SPI_SPINT                  0x001C


//=============================================================================
// RTC

#define CYGARC_HAL_LPC2XXX_REG_RTC_BASE                   0xE0024000

// Registers are offsets from base of this subsystem

#define CYGARC_HAL_LPC2XXX_REG_RTC_ILR                    0x0000
#define CYGARC_HAL_LPC2XXX_REG_RTC_ILR_CIF                (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_RTC_ILR_ALF                (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_RTC_CTC                    0x0004
#define CYGARC_HAL_LPC2XXX_REG_RTC_CCR                    0x0008
#define CYGARC_HAL_LPC2XXX_REG_RTC_CCR_CLKEN              (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_RTC_CCR_CTCRST             (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_RTC_CIIR                   0x000C
#define CYGARC_HAL_LPC2XXX_REG_RTC_AMR                    0x0010
#define CYGARC_HAL_LPC2XXX_REG_RTC_CTIME0                 0x0014
#define CYGARC_HAL_LPC2XXX_REG_RTC_CTIME1                 0x0018
#define CYGARC_HAL_LPC2XXX_REG_RTC_CTIME2                 0x001C
#define CYGARC_HAL_LPC2XXX_REG_RTC_SEC                    0x0020
#define CYGARC_HAL_LPC2XXX_REG_RTC_MIN                    0x0024
#define CYGARC_HAL_LPC2XXX_REG_RTC_HOUR                   0x0028
#define CYGARC_HAL_LPC2XXX_REG_RTC_DOM                    0x002C
#define CYGARC_HAL_LPC2XXX_REG_RTC_DOW                    0x0030
#define CYGARC_HAL_LPC2XXX_REG_RTC_DOY                    0x0034
#define CYGARC_HAL_LPC2XXX_REG_RTC_MONTH                  0x0038
#define CYGARC_HAL_LPC2XXX_REG_RTC_YEAR                   0x003C
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALSEC                  0x0060
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALMIN                  0x0064
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALHOUR                 0x0068
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALDOM                  0x006C
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALDOW                  0x0070
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALDOY                  0x0074
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALMON                  0x0078
#define CYGARC_HAL_LPC2XXX_REG_RTC_ALYEAR                 0x007C
#define CYGARC_HAL_LPC2XXX_REG_RTC_PREINT                 0x0080
#define CYGARC_HAL_LPC2XXX_REG_RTC_PREFRAC                0x0084

//=============================================================================
// GPIO (IO)

#define CYGARC_HAL_LPC2XXX_REG_IO_BASE                   0xE0028000

#if defined(CYGHWR_HAL_ARM_LPC2XXX_FAMILY_LPC210X)

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_IOPIN                     0x000
#define CYGARC_HAL_LPC2XXX_REG_IOSET                     0x004
#define CYGARC_HAL_LPC2XXX_REG_IODIR                     0x008
#define CYGARC_HAL_LPC2XXX_REG_IOCLR                     0x00C

#else

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_IO0PIN                    0x000
#define CYGARC_HAL_LPC2XXX_REG_IO0SET                    0x004
#define CYGARC_HAL_LPC2XXX_REG_IO0DIR                    0x008
#define CYGARC_HAL_LPC2XXX_REG_IO0CLR                    0x00C

#define CYGARC_HAL_LPC2XXX_REG_IO1PIN                    0x010
#define CYGARC_HAL_LPC2XXX_REG_IO1SET                    0x014
#define CYGARC_HAL_LPC2XXX_REG_IO1DIR                    0x018
#define CYGARC_HAL_LPC2XXX_REG_IO1CLR                    0x01C

#define CYGARC_HAL_LPC2XXX_REG_IO2PIN                    0x020
#define CYGARC_HAL_LPC2XXX_REG_IO2SET                    0x024
#define CYGARC_HAL_LPC2XXX_REG_IO2DIR                    0x028
#define CYGARC_HAL_LPC2XXX_REG_IO2CLR                    0x02C

#define CYGARC_HAL_LPC2XXX_REG_IO3PIN                    0x030
#define CYGARC_HAL_LPC2XXX_REG_IO3SET                    0x034
#define CYGARC_HAL_LPC2XXX_REG_IO3DIR                    0x038
#define CYGARC_HAL_LPC2XXX_REG_IO3CLR                    0x03C

#endif

//=============================================================================
// Pin Connect Block (PIN)

#define CYGARC_HAL_LPC2XXX_REG_PIN_BASE                  0xE002C000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_PINSEL0                   0x000
#define CYGARC_HAL_LPC2XXX_REG_PINSEL1                   0x004
#define CYGARC_HAL_LPC2XXX_REG_PINSEL2                   0x014

//=============================================================================
// ADC (AD)

#define CYGARC_HAL_LPC2XXX_REG_AD_BASE                  0xE0034000

// Registers are offsets from base of this subsystem
#define CYGARC_HAL_LPC2XXX_REG_ADCR                     0x0000
#define CYGARC_HAL_LPC2XXX_REG_ADCR_BURST               (1<<16)
#define CYGARC_HAL_LPC2XXX_REG_ADCR_PDN                 (1<<21)
#define CYGARC_HAL_LPC2XXX_REG_ADCR_EDGE                (1<<27)
#define CYGARC_HAL_LPC2XXX_REG_ADDR                     0x0004
#define CYGARC_HAL_LPC2XXX_REG_ADDR_OVERRUN             (1<<30)
#define CYGARC_HAL_LPC2XXX_REG_ADDR_DONE                (1<<31)

//=============================================================================
// System Control Block

#define CYGARC_HAL_LPC2XXX_REG_SCB_BASE                 0xE01FC000

// Registers are offsets from base of this subsystem

// Memory accelerator module
#define CYGARC_HAL_LPC2XXX_REG_MAMCR                    0x0000
#define CYGARC_HAL_LPC2XXX_REG_MAMCR_DISABLED           0x00
#define CYGARC_HAL_LPC2XXX_REG_MAMCR_PARTIAL            0x01
#define CYGARC_HAL_LPC2XXX_REG_MAMCR_FULL               0x02
#define CYGARC_HAL_LPC2XXX_REG_MAMTIM                   0x0004

// Memory mapping control
#define CYGARC_HAL_LPC2XXX_REG_MEMMAP                   0x0040

// PLL
#define CYGARC_HAL_LPC2XXX_REG_PLLCON                   0x0080
#define CYGARC_HAL_LPC2XXX_REG_PLLCON_PLLE              (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PLLCON_PLLC              (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PLLCFG                   0x0084
#define CYGARC_HAL_LPC2XXX_REG_PLLSTAT                  0x0088
#define CYGARC_HAL_LPC2XXX_REG_PLLSTAT_PLLE              0x100      // (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_PLLSTAT_PLLC              0x200      // (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_PLLSTAT_PLOCK             0x400      // (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_PLLFEED                  0x008C

// Power Control
#define CYGARC_HAL_LPC2XXX_REG_PCON                     0x00C0
#define CYGARC_HAL_LPC2XXX_REG_PCON_IDL                 (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_PCON_PD                  (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PCONP                    0x00C4
#define CYGARC_HAL_LPC2XXX_REG_PCONP_TIM0               (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_TIM1               (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_URT0               (1<<3)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_URT1               (1<<4)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_PWM0               (1<<5)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_I2C                (1<<7)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_SPI0               (1<<8)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_RTC                (1<<9)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_SPI1               (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_PCONP_AD                 (1<<12)

// VPB Divider
#define CYGARC_HAL_LPC2XXX_REG_VPBDIV                   0x0100

// External interrupt inputs
#define CYGARC_HAL_LPC2XXX_REG_EXTINT                   0x0140
#define CYGARC_HAL_LPC2XXX_REG_EXTWAKE                  0x0144
#define CYGARC_HAL_LPC2XXX_REG_EXTMODE                  0x0148
#define CYGARC_HAL_LPC2XXX_REG_EXTPOLAR                 0x014C

#define CYGARC_HAL_LPC2XXX_REG_EXTxxx_INT0              (1<<0)
#define CYGARC_HAL_LPC2XXX_REG_EXTxxx_INT1              (1<<1)
#define CYGARC_HAL_LPC2XXX_REG_EXTxxx_INT2              (1<<2)
#define CYGARC_HAL_LPC2XXX_REG_EXTxxx_INT3              (1<<3)


//=============================================================================
// External Memory Controller

#if defined(CYGHWR_HAL_ARM_LPC2XXX_FAMILY_LPC22XX)

#define CYGARC_HAL_LPC2XXX_REG_BCFG0                    0xFFE00000
#define CYGARC_HAL_LPC2XXX_REG_BCFG1                    0xFFE00004
#define CYGARC_HAL_LPC2XXX_REG_BCFG2                    0xFFE00008
#define CYGARC_HAL_LPC2XXX_REG_BCFG3                    0xFFE0000C

#define CYGARC_HAL_LPC2XXX_REG_BCFGx_RBLE               (1<<10)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_BUSERR             (1<<24)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_WPERR              (1<<25)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_WP                 (1<<26)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_BM                 (1<<27)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_MW_8BIT            (0x00000000)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_MW_16BIT           (0x10000000)
#define CYGARC_HAL_LPC2XXX_REG_BCFGx_MW_32BIT           (0x20000000)

#endif

//=============================================================================
// Vectored Interrupt Controller (VIC)

#define CYGARC_HAL_LPC2XXX_REG_VIC_BASE                 0xFFFFF000

// Registers are offsets from base of this subsystem

#define CYGARC_HAL_LPC2XXX_REG_VICIRQSTAT               0x0000
#define CYGARC_HAL_LPC2XXX_REG_VICFIQSTAT               0x0004
#define CYGARC_HAL_LPC2XXX_REG_VICRAWINTR               0x0008
#define CYGARC_HAL_LPC2XXX_REG_VICINTSELECT             0x000C
#define CYGARC_HAL_LPC2XXX_REG_VICINTENABLE             0x0010
#define CYGARC_HAL_LPC2XXX_REG_VICINTENCLEAR            0x0014
#define CYGARC_HAL_LPC2XXX_REG_VICSOFTINT               0x0018
#define CYGARC_HAL_LPC2XXX_REG_VICSOFTINTCLEAR          0x001C
#define CYGARC_HAL_LPC2XXX_REG_VICPROTECTION            0x0020
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR              0x0030
#define CYGARC_HAL_LPC2XXX_REG_VICDEFVECTADDR           0x0034

#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR0             0x0100
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR1             0x0104
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR2             0x0108
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR3             0x010C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR4             0x0110
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR5             0x0114
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR6             0x0118
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR7             0x011C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR8             0x0120
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR9             0x0124
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR10            0x0128
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR11            0x012C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR12            0x0130
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR13            0x0134
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR14            0x0138
#define CYGARC_HAL_LPC2XXX_REG_VICVECTADDR15            0x013C

#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL0             0x0200
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL1             0x0204
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL2             0x0208
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL3             0x020C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL4             0x0210
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL5             0x0214
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL6             0x0218
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL7             0x021C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL8             0x0220
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL9             0x0224
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL10            0x0228
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL11            0x022C
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL12            0x0230
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL13            0x0234
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL14            0x0238
#define CYGARC_HAL_LPC2XXX_REG_VICVECTCNTL15            0x023C


//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
