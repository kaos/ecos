#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H
//=============================================================================
//
//      var_io.h
//
//      Variant specific registers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  
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
// Author(s):     ilijak
// Original data: Uwe Kindler ( LPC24XX port )
// Date:          2010-12-22
// Purpose:       LPC17XX variant specific registers
// Description:
// Usage:         #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_cortexm_lpc17xx.h>

#include <cyg/hal/plf_io.h>

//=============================================================================
// Peripherals

//=============================================================================
// Cortex-M architecture register

// VTOR setting
#ifndef CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM
#define CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM               BIT_(28)
#endif

//---------------------------------------------------------------------------
// Utilize LPC17xx flash between startup vectors and 0x2fc
// for misc funtions.
#ifdef CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION
# define CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR \
         CYGBLD_ATTRIB_SECTION(".lpc17xx_misc")
#else
# define CYGOPT_HAL_LPC17XX_MISC_FLASH_SECTION_ATTR
#endif

__externC const cyg_uint32* hal_lpc17xx_crp_p(void);

// LPC System Control Block
#define CYGHWR_HAL_LPC17XX_REG_SCB_BASE                 0x400FC000

// Flash accelerator
#define CYGHWR_HAL_LPC17XX_REG_FLASHCFG                 0x0000
#define CYGHWR_HAL_LPC17XX_REG_FLTSET(__tim)            (__tim << 12)
#define CYGHWR_HAL_LPC17XX_REG_FLTIM20MHZ               0x0
#define CYGHWR_HAL_LPC17XX_REG_FLTIM40MHZ               0x1
#define CYGHWR_HAL_LPC17XX_REG_FLTIM60MHZ               0x2
#define CYGHWR_HAL_LPC17XX_REG_FLTIM80MHZ               0x3
#define CYGHWR_HAL_LPC17XX_REG_FLTIM100MHZ              0x4
#define CYGHWR_HAL_LPC17XX_REG_FLTIM120MHZ              0x4
#define CYGHWR_HAL_LPC17XX_REG_FLTIMSAFE                0x5
#define CYGHWR_HAL_LPC17XX_REG_FLTIM_MASK               CYGHWR_HAL_LPC17XX_REG_FLTSET(0x0f)

// PLL. Registers are offsets from base of this subsystem
#define CYGHWR_HAL_LPC17XX_REG_PLL0CON                  0x0080
#define CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLE              (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_PLLCON_PLLC              (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_PLL0CFG                  0x0084
#define CYGHWR_HAL_LPC17XX_REG_PLL0STAT                 0x0088
#define CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLLE            (1<<24)
#define CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLLC            (1<<25)
#define CYGHWR_HAL_LPC17XX_REG_PLL0STAT_PLOCK           (1<<26)
#define CYGHWR_HAL_LPC17XX_REG_PLL0FEED                 0x008C

#define CYGHWR_HAL_LPC17XX_REG_PLL1CON                  0x00A0
#define CYGHWR_HAL_LPC17XX_REG_PLL1CFG                  0x00A4
#define CYGHWR_HAL_LPC17XX_REG_PLL1STAT                 0x00A8
#define CYGHWR_HAL_LPC17XX_REG_PLL1STAT_PLLE            (1<<8)
#define CYGHWR_HAL_LPC17XX_REG_PLL1STAT_PLLC            (1<<9)
#define CYGHWR_HAL_LPC17XX_REG_PLL1STAT_PLOCK           (1<<10)
#define CYGHWR_HAL_LPC17XX_REG_PLL1FEED                 0x00AC

// Clock source selection register
#define CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL                0x010C
#define CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL_IRC            0x00
#define CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL_MAIN           0x01
#define CYGHWR_HAL_LPC17XX_REG_CLKSRCSEL_RTC            0x10

#define CYGHWR_HAL_LPC17XX_REG_CCLKCFG                  0x0104
#define CYGHWR_HAL_LPC17XX_REG_USBCLKCFG                0x0108
/* #define CYGHWR_HAL_LPC17XX_REG_IRCTRIM                  0x01A4 */
#define CYGHWR_HAL_LPC17XX_REG_PCLKSEL0                 0x01A8
#define CYGHWR_HAL_LPC17XX_REG_PCLKSEL1                 0x01AC
#define CYGHWR_HAL_LPC17XX_REG_INTWAKE                  0x0144

// Power Control
#define CYGHWR_HAL_LPC17XX_REG_PCON                     0x00C0
#define CYGHWR_HAL_LPC17XX_REG_PCON_IDL                 (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_PCON_PD                  (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_PCONP                    0x00C4
#define CYGHWR_HAL_LPC17XX_REG_PCONP_TIM0               (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_TIM1               (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_URT0               (1<<3)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_URT1               (1<<4)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_PWM0               (1<<5)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_PWM1               (1<<6)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_I2C0               (1<<7)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_SPI                (1<<8)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_RTC                (1<<9)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_SSP1               (1<<10)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_EMC                (1<<11)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_AD                 (1<<12)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_CAN1               (1<<13)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_CAN2               (1<<14)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_I2C1               (1<<19)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_LCD                (1<<20)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_SSP0               (1<<21)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_TIM2               (1<<22)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_TIM3               (1<<23)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_URT2               (1<<24)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_URT3               (1<<25)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_I2C2               (1<<26)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_I2S                (1<<27)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_SD                 (1<<28)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_DMA                (1<<29)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_ENET               (1<<30)
#define CYGHWR_HAL_LPC17XX_REG_PCONP_USB                (1<<31)

// Utility
#define CYGHWR_HAL_LPC17XX_REG_CLKOUTCFG                0x01C8

// System control and status register
#define CYGHWR_HAL_LPC17XX_REG_SCS                      0x01A0
#define CYGHWR_HAL_LPC17XX_REG_SCS_OSCEN                0x20
#define CYGHWR_HAL_LPC17XX_REG_SCS_OSCSTAT              0x40


//=============================================================================
// Pin Connect Block (PIN)

#define CYGHWR_HAL_LPC17XX_REG_PIN_BASE                  0x4002C000

#define CYGHWR_HAL_LPC17XX_REG_PINSEL0                   0x000
#define CYGHWR_HAL_LPC17XX_REG_PINSEL1                   0x004
#define CYGHWR_HAL_LPC17XX_REG_PINSEL2                   0x008
#define CYGHWR_HAL_LPC17XX_REG_PINSEL3                   0x00C
#define CYGHWR_HAL_LPC17XX_REG_PINSEL4                   0x010
#define CYGHWR_HAL_LPC17XX_REG_PINSEL5                   0x014
#define CYGHWR_HAL_LPC17XX_REG_PINSEL6                   0x018
#define CYGHWR_HAL_LPC17XX_REG_PINSEL7                   0x01C
#define CYGHWR_HAL_LPC17XX_REG_PINSEL8                   0x020
#define CYGHWR_HAL_LPC17XX_REG_PINSEL9                   0x024
#define CYGHWR_HAL_LPC17XX_REG_PINSEL10                  0x028
#define CYGHWR_HAL_LPC17XX_REG_PINSEL11                  0x02C

#define CYGHWR_HAL_LPC17XX_REG_PINMODE0                  0x040
#define CYGHWR_HAL_LPC17XX_REG_PINMODE1                  0x044
#define CYGHWR_HAL_LPC17XX_REG_PINMODE2                  0x048
#define CYGHWR_HAL_LPC17XX_REG_PINMODE3                  0x04C
#define CYGHWR_HAL_LPC17XX_REG_PINMODE4                  0x050
#define CYGHWR_HAL_LPC17XX_REG_PINMODE5                  0x054
#define CYGHWR_HAL_LPC17XX_REG_PINMODE6                  0x058
#define CYGHWR_HAL_LPC17XX_REG_PINMODE7                  0x05C
#define CYGHWR_HAL_LPC17XX_REG_PINMODE8                  0x060
#define CYGHWR_HAL_LPC17XX_REG_PINMODE9                  0x064

#define CYGHWR_HAL_LPC17XX_PIN_SET(_reg, _func) \
            HAL_WRITE_UINT32(CYGHWR_HAL_LPC17XX_REG_PIN_BASE + _reg, _func)

#define CYGHWR_HAL_LPC17XX_PIN_GET(_reg, _dst) \
            HAL_READ_UINT32(CYGHWR_HAL_LPC17XX_REG_PIN_BASE + _reg, _dst)

//=============================================================================
// UARTs (Ux)

#define CYGHWR_HAL_LPC17XX_REG_UART0_BASE                0x4000C000
#define CYGHWR_HAL_LPC17XX_REG_UART1_BASE                0x40010000
#define CYGHWR_HAL_LPC17XX_REG_UART2_BASE                0x40098000
#define CYGHWR_HAL_LPC17XX_REG_UART3_BASE                0x4009C000

// Registers are offsets from base for each UART
#define CYGHWR_HAL_LPC17XX_REG_UxRBR                     0x0000 // DLAB=0 read
#define CYGHWR_HAL_LPC17XX_REG_UxTHR                     0x0000 // DLAB=0 write
#define CYGHWR_HAL_LPC17XX_REG_UxDLL                     0x0000 // DLAB=1 r/w
#define CYGHWR_HAL_LPC17XX_REG_UxIER                     0x0004 // DLAB=0
#define CYGHWR_HAL_LPC17XX_REG_UxIER_RXDATA_INT          (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_UxIER_THRE_INT            (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_UxIER_RXLS_INT            (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_U1IER_RXMS_INT            (1<<3) // U1 only
#define CYGHWR_HAL_LPC17XX_REG_UxDLM                     0x0004 // DLAB=1

#define CYGHWR_HAL_LPC17XX_REG_UxIIR                     0x0008 // read
#define CYGHWR_HAL_LPC17XX_REG_UxIIR_IIR0                (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_UxIIR_IIR1                (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_UxIIR_IIR2                (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_UxIIR_IIR3                (1<<3)
#define CYGHWR_HAL_LPC17XX_REG_UxIIR_FIFOS               (0xB0)

#define CYGHWR_HAL_LPC17XX_REG_UxFCR                     0x0008 // write
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_FIFO_ENA            (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_FIFO_RESET       (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_TX_FIFO_RESET       (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_TRIGGER_0        (0x00)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_TRIGGER_1        (0x40)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_TRIGGER_2        (0x80)
#define CYGHWR_HAL_LPC17XX_REG_UxFCR_RX_TRIGGER_3        (0xB0)

#define CYGHWR_HAL_LPC17XX_REG_UxLCR                     0x000C
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_WORD_LENGTH_5       (0x00)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_WORD_LENGTH_6       (0x01)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_WORD_LENGTH_7       (0x02)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_WORD_LENGTH_8       (0x03)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_STOP_1              (0x00)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_STOP_2              (0x04)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_PARITY_ENA          (0x08)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_PARITY_ODD          (0x00)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_PARITY_EVEN         (0x10)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_PARITY_ONE          (0x20)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_PARITY_ZERO         (0x30)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_BREAK_ENA           (0x40)
#define CYGHWR_HAL_LPC17XX_REG_UxLCR_DLAB                (0x80)

// Modem Control Register is UART1 only
#define CYGHWR_HAL_LPC17XX_REG_U1MCR                     0x0010
#define CYGHWR_HAL_LPC17XX_REG_U1MCR_DTR                 (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_U1MCR_RTS                 (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_U1MCR_LOOPBACK            (1<<4)

#define CYGHWR_HAL_LPC17XX_REG_UxLSR                     0x0014
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_RDR                 (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_OE                  (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_PE                  (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_FE                  (1<<3)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_BI                  (1<<4)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_THRE                (1<<5)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_TEMT                (1<<6)
#define CYGHWR_HAL_LPC17XX_REG_UxLSR_RX_FIFO_ERR         (1<<7)

// Modem Status Register is UART1 only
#define CYGHWR_HAL_LPC17XX_REG_U1MSR                     0x0018
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_DCTS                (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_DDSR                (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_RI_FALL             (1<<2)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_DDCD                (1<<3)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_CTS                 (1<<4)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_DSR                 (1<<5)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_RI                  (1<<6)
#define CYGHWR_HAL_LPC17XX_REG_U1MSR_DCD                 (1<<7)

#define CYGHWR_HAL_LPC17XX_REG_UxSCR                     0x001C
#define CYGHWR_HAL_LPC17XX_REG_UxACR                     0x0020
#define CYGHWR_HAL_LPC17XX_REG_U3ICR                     0x0024
#define CYGHWR_HAL_LPC17XX_REG_UxFDR                     0x0028
#define CYGHWR_HAL_LPC17XX_REG_UxTER                     0x0030

// RTC
#define CYGHWR_HAL_LPC17XX_REG_RTC_BASE                   0x40024000
#define CYGARC_HAL_LPC2XXX_REG_RTC_BASE  CYGHWR_HAL_LPC17XX_REG_RTC_BASE

// Registers are offsets from base of this subsystem
#define CYGHWR_HAL_LPC17XX_REG_RTC_ILR                    0x0000
#define CYGHWR_HAL_LPC17XX_REG_RTC_ILR_CIF                (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_RTC_ILR_ALF                (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_RTC_CTC                    0x0004
#define CYGHWR_HAL_LPC17XX_REG_RTC_CCR                    0x0008
#define CYGHWR_HAL_LPC17XX_REG_RTC_CCR_CLKEN              (1<<0)
#define CYGHWR_HAL_LPC17XX_REG_RTC_CCR_CTCRST             (1<<1)
#define CYGHWR_HAL_LPC17XX_REG_RTC_CIIR                   0x000C
#define CYGHWR_HAL_LPC17XX_REG_RTC_AMR                    0x0010
#define CYGHWR_HAL_LPC17XX_REG_RTC_CTIME0                 0x0014
#define CYGHWR_HAL_LPC17XX_REG_RTC_CTIME1                 0x0018
#define CYGHWR_HAL_LPC17XX_REG_RTC_CTIME2                 0x001C
#define CYGHWR_HAL_LPC17XX_REG_RTC_SEC                    0x0020
#define CYGHWR_HAL_LPC17XX_REG_RTC_MIN                    0x0024
#define CYGHWR_HAL_LPC17XX_REG_RTC_HOUR                   0x0028
#define CYGHWR_HAL_LPC17XX_REG_RTC_DOM                    0x002C
#define CYGHWR_HAL_LPC17XX_REG_RTC_DOW                    0x0030
#define CYGHWR_HAL_LPC17XX_REG_RTC_DOY                    0x0034
#define CYGHWR_HAL_LPC17XX_REG_RTC_MONTH                  0x0038
#define CYGHWR_HAL_LPC17XX_REG_RTC_YEAR                   0x003C
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALSEC                  0x0060
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALMIN                  0x0064
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALHOUR                 0x0068
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALDOM                  0x006C
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALDOW                  0x0070
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALDOY                  0x0074
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALMON                  0x0078
#define CYGHWR_HAL_LPC17XX_REG_RTC_ALYEAR                 0x007C
#define CYGHWR_HAL_LPC17XX_REG_RTC_PREINT                 0x0080
#define CYGHWR_HAL_LPC17XX_REG_RTC_PREFRAC                0x0084

// Ethernet (EMAC)
#define CYGHWR_HAL_LPC17XX_REG_EMAC_BASE                0x50000000

// End Peripherals

#ifndef __ASSEMBLER__

__externC void hal_plf_uart_setbaud( CYG_ADDRESS uart, cyg_uint32 baud );

//-----------------------------------------------------------------------------
// Configure pin function
//
__externC void  hal_set_pin_function(cyg_uint8 port, cyg_uint8 pin,
                                     cyg_uint8 function);

//-----------------------------------------------------------------------------
// Function to enable/disable power for certain peripheral
// Use PCONP identifiers from below
//
externC void hal_lpc_set_power(cyg_uint8 pconp_id, int on);


//-----------------------------------------------------------------------------
// Identifiers for power control, hal_get_pclk()
//
#define CYNUM_HAL_LPC17XX_PCONP_TIMER0 1
#define CYNUM_HAL_LPC17XX_PCONP_TIMER1 2
#define CYNUM_HAL_LPC17XX_PCONP_UART0  3
#define CYNUM_HAL_LPC17XX_PCONP_UART1  4
#define CYNUM_HAL_LPC17XX_PCONP_PWM0   5
#define CYNUM_HAL_LPC17XX_PCONP_PWM1   6
#define CYNUM_HAL_LPC17XX_PCONP_I2C0   7
#define CYNUM_HAL_LPC17XX_PCONP_SPI    8
#define CYNUM_HAL_LPC17XX_PCONP_RTC    9
#define CYNUM_HAL_LPC17XX_PCONP_SSP1   10
#define CYNUM_HAL_LPC17XX_PCONP_EMC    11
#define CYNUM_HAL_LPC17XX_PCONP_ADC    12
#define CYNUM_HAL_LPC17XX_PCONP_CAN1   13
#define CYNUM_HAL_LPC17XX_PCONP_CAN2   14
#define CYNUM_HAL_LPC17XX_PCONP_I2C1   19
#define CYNUM_HAL_LPC17XX_PCONP_LCD    20
#define CYNUM_HAL_LPC17XX_PCONP_SSP0   21
#define CYNUM_HAL_LPC17XX_PCONP_TIMER2 22
#define CYNUM_HAL_LPC17XX_PCONP_TIMER3 23
#define CYNUM_HAL_LPC17XX_PCONP_UART2  24
#define CYNUM_HAL_LPC17XX_PCONP_UART3  25
#define CYNUM_HAL_LPC17XX_PCONP_I2C2   26
#define CYNUM_HAL_LPC17XX_PCONP_I2S    27
#define CYNUM_HAL_LPC17XX_PCONP_SDC    28
#define CYNUM_HAL_LPC17XX_PCONP_GPDMA  29
#define CYNUM_HAL_LPC17XX_PCONP_ENET   30
#define CYNUM_HAL_LPC17XX_PCONP_USB    31

#endif // ifndef __ASSEMBLER__


//-----------------------------------------------------------------------------
// LPC2xxx compatibility block
// These definitions enable reusing of compatible LPC2xxx devs.

// UART
#define CYGARC_HAL_LPC24XX_REG_UART0_BASE CYGHWR_HAL_LPC17XX_REG_UART0_BASE
#define CYGARC_HAL_LPC24XX_REG_UART1_BASE CYGHWR_HAL_LPC17XX_REG_UART1_BASE
#define CYGARC_HAL_LPC24XX_REG_UART2_BASE CYGHWR_HAL_LPC17XX_REG_UART2_BASE
#define CYGARC_HAL_LPC24XX_REG_UART3_BASE CYGHWR_HAL_LPC17XX_REG_UART3_BASE

// RTC
#define CYGARC_HAL_LPC2XXX_REG_RTC_BASE  CYGHWR_HAL_LPC17XX_REG_RTC_BASE

// Ethernet (EMAC)
#define CYGARC_HAL_LPC2XXX_REG_EMAC_BASE CYGHWR_HAL_LPC17XX_REG_EMAC_BASE

// System Control Block
#define CYGARC_HAL_LPC24XX_REG_SCB_BASE      CYGHWR_HAL_LPC17XX_REG_SCB_BASE

// Power Control
#define CYGARC_HAL_LPC24XX_REG_PCONP         CYGHWR_HAL_LPC17XX_REG_PCONP
#define CYGARC_HAL_LPC24XX_REG_PCONP_ENET    CYGHWR_HAL_LPC17XX_REG_PCONP_ENET

// Pin Connect Block (PIN)
#define CYGARC_HAL_LPC24XX_REG_PIN_BASE      CYGHWR_HAL_LPC17XX_REG_PIN_BASE
#define CYGARC_HAL_LPC24XX_REG_PINSEL2       CYGHWR_HAL_LPC17XX_REG_PINSEL2
#define CYGARC_HAL_LPC24XX_REG_PINSEL3       CYGHWR_HAL_LPC17XX_REG_PINSEL3

// End of LPC2xxx device compatibiliy block.

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_IO_H
// End of var_io.h
