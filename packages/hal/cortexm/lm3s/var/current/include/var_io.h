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
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):   ccoutand
// Date:        2011-01-18
// Purpose:     Stellaris Cortex-M3 variant specific registers
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_cortexm_lm3s.h>

#include <cyg/hal/plf_io.h>

//=============================================================================
// Peripherals

#define CYGHWR_HAL_LM3S_FLASH                    0x00000000
#define CYGHWR_HAL_LM3S_SRAM                     0x20000000

#define CYGHWR_HAL_LM3S_WDT0                     0x40000000
#define CYGHWR_HAL_LM3S_GPIOA                    0x40004000
#define CYGHWR_HAL_LM3S_GPIOB                    0x40005000
#define CYGHWR_HAL_LM3S_GPIOC                    0x40006000
#define CYGHWR_HAL_LM3S_GPIOD                    0x40007000
#define CYGHWR_HAL_LM3S_SSI0                     0x40008000
#define CYGHWR_HAL_LM3S_SSI1                     0x40009000
#define CYGHWR_HAL_LM3S_UART0                    0x4000C000
#define CYGHWR_HAL_LM3S_UART1                    0x4000D000
#define CYGHWR_HAL_LM3S_UART2                    0x4000E000
#define CYGHWR_HAL_LM3S_I2C_M0                   0x40020000
#define CYGHWR_HAL_LM3S_I2C_S0                   0x40020800
#define CYGHWR_HAL_LM3S_GPIOE                    0x40024000
#define CYGHWR_HAL_LM3S_GPIOF                    0x40025000
#define CYGHWR_HAL_LM3S_GPIOG                    0x40026000
#define CYGHWR_HAL_LM3S_GPIOH                    0x40027000
#define CYGHWR_HAL_LM3S_PWM                      0x40028000
#define CYGHWR_HAL_LM3S_QEI0                     0x4002C000
#define CYGHWR_HAL_LM3S_GPTIM0                   0x40030000
#define CYGHWR_HAL_LM3S_GPTIM1                   0x40031000
#define CYGHWR_HAL_LM3S_GPTIM2                   0x40032000
#define CYGHWR_HAL_LM3S_GPTIM3                   0x40033000
#define CYGHWR_HAL_LM3S_ADC0                     0x40038000
#define CYGHWR_HAL_LM3S_AC                       0x4003C000
#define CYGHWR_HAL_LM3S_CAN0                     0x40040000
#define CYGHWR_HAL_LM3S_ETH0                     0x40048000
#define CYGHWR_HAL_LM3S_FMC                      0x400FD000
#define CYGHWR_HAL_LM3S_SC                       0x400FE000

#define CYGHWR_HAL_LM3S_ITM                      0xE0000000
#define CYGHWR_HAL_LM3S_DWT                      0xE0001000
#define CYGHWR_HAL_LM3S_FPB                      0xE0002000
#define CYGHWR_HAL_LM3S_CORTEXM3                 0xE000E000
#define CYGHWR_HAL_LM3S_TPIU                     0xE0040000


//=============================================================================
// Device signature and ID registers

#define CYGHWR_HAL_LM3S_MCU_ID                   (CYGHWR_HAL_LM3S_CORTEXM3 + 0xD00)
#define CYGHWR_HAL_LM3S_MCU_ID_REV(__x)          ((__x)&0xF)
#define CYGHWR_HAL_LM3S_MCU_PART_NO(__x)         (((__x)>>4)&0x0FFF)
#define CYGHWR_HAL_LM3S_MCU_VAR_NO(__x)          (((__x)>>20)&0xF)


//=============================================================================
// System Control

#define CYGHWR_HAL_LM3S_SC_DID0                  0x000
#define CYGHWR_HAL_LM3S_SC_DID1                  0x004
#define CYGHWR_HAL_LM3S_SC_DIC0                  0x008
#define CYGHWR_HAL_LM3S_SC_DIC1                  0x010
#define CYGHWR_HAL_LM3S_SC_DIC2                  0x014
#define CYGHWR_HAL_LM3S_SC_DIC3                  0x018
#define CYGHWR_HAL_LM3S_SC_DIC4                  0x01c
#define CYGHWR_HAL_LM3S_SC_PBORCTL               0x030
#define CYGHWR_HAL_LM3S_SC_LDORCTL               0x034
#define CYGHWR_HAL_LM3S_SC_SRCR0                 0x040
#define CYGHWR_HAL_LM3S_SC_SRCR1                 0x044
#define CYGHWR_HAL_LM3S_SC_SRCR2                 0x048
#define CYGHWR_HAL_LM3S_SC_RIS                   0x050
#define CYGHWR_HAL_LM3S_SC_IMC                   0x054
#define CYGHWR_HAL_LM3S_SC_MISC                  0x058
#define CYGHWR_HAL_LM3S_SC_RESC                  0x05C
#define CYGHWR_HAL_LM3S_SC_RCC                   0x060
#define CYGHWR_HAL_LM3S_SC_PLLCFG                0x064
#define CYGHWR_HAL_LM3S_SC_RCGC0                 0x100
#define CYGHWR_HAL_LM3S_SC_RCGC1                 0x104
#define CYGHWR_HAL_LM3S_SC_RCGC2                 0x108
#define CYGHWR_HAL_LM3S_SC_SCGC0                 0x110
#define CYGHWR_HAL_LM3S_SC_SCGC1                 0x114
#define CYGHWR_HAL_LM3S_SC_SCGC2                 0x118
#define CYGHWR_HAL_LM3S_SC_DCGC0                 0x120
#define CYGHWR_HAL_LM3S_SC_DCGC1                 0x124
#define CYGHWR_HAL_LM3S_SC_DCGC2                 0x128
#define CYGHWR_HAL_LM3S_SC_FMPRE                 0x130
#define CYGHWR_HAL_LM3S_SC_FMPPE                 0x134
#define CYGHWR_HAL_LM3S_SC_USECRL                0x140
#define CYGHWR_HAL_LM3S_SC_DSLPCLKCFG            0x144
#define CYGHWR_HAL_LM3S_SC_CLKVCLR               0x150
#define CYGHWR_HAL_LM3S_SC_LDOARST               0x160

// PBORCTL bits
#define CYGHWR_HAL_LM3S_SC_PBORCTL_BORWT         BIT_(0)
#define CYGHWR_HAL_LM3S_SC_PBORCTL_BORIOR        BIT_(1)
#define CYGHWR_HAL_LM3S_SC_PBORCTL_BORTIM(__x)   VALUE_(2,__x)

// RIS bits
#define CYGHWR_HAL_LM3S_SC_RIS_PLLFRIS           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_RIS_BORRIS            BIT_(1)
#define CYGHWR_HAL_LM3S_SC_RIS_LDORIS            BIT_(2)
#define CYGHWR_HAL_LM3S_SC_RIS_MOFRIS            BIT_(3)
#define CYGHWR_HAL_LM3S_SC_RIS_IOFRIS            BIT_(4)
#define CYGHWR_HAL_LM3S_SC_RIS_CLFRIS            BIT_(5)
#define CYGHWR_HAL_LM3S_SC_RIS_PLLLRIS           BIT_(6)

// IMC bits
#define CYGHWR_HAL_LM3S_SC_IMC_PLLFIM            BIT_(0)
#define CYGHWR_HAL_LM3S_SC_IMC_BORRIM            BIT_(1)
#define CYGHWR_HAL_LM3S_SC_IMC_LDOIM             BIT_(2)
#define CYGHWR_HAL_LM3S_SC_IMC_MOFIM             BIT_(3)
#define CYGHWR_HAL_LM3S_SC_IMC_IOFIM             BIT_(4)
#define CYGHWR_HAL_LM3S_SC_IMC_CLIM              BIT_(5)
#define CYGHWR_HAL_LM3S_SC_IMC_PLLLFIM           BIT_(6)

// MISC bits
#define CYGHWR_HAL_LM3S_SC_MISC_BORMIS           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_MISC_LDOMIS           BIT_(2)
#define CYGHWR_HAL_LM3S_SC_MISC_MOFMIS           BIT_(3)
#define CYGHWR_HAL_LM3S_SC_MISC_IOFMIS           BIT_(4)
#define CYGHWR_HAL_LM3S_SC_MISC_CLMIS            BIT_(5)
#define CYGHWR_HAL_LM3S_SC_MISC_PLLLMIS          BIT_(6)

// RESC bits
#define CYGHWR_HAL_LM3S_SC_RESC_EXT              BIT_(0)
#define CYGHWR_HAL_LM3S_SC_RESC_POR              BIT_(1)
#define CYGHWR_HAL_LM3S_SC_RESC_BOR              BIT_(2)
#define CYGHWR_HAL_LM3S_SC_RESC_WDT              BIT_(3)
#define CYGHWR_HAL_LM3S_SC_RESC_SW               BIT_(4)
#define CYGHWR_HAL_LM3S_SC_RESC_LDO              BIT_(5)

// RCC bits
#define CYGHWR_HAL_LM3S_SC_RCC_MOSCDIS           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_RCC_IOSCDIS           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_RCC_MOSCVER           BIT_(2)
#define CYGHWR_HAL_LM3S_SC_RCC_IOSCVER           BIT_(3)
#define CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_MOSC       VALUE_(4,0)
#define CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_IOSC       VALUE_(4,1)
#define CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_IOSC_DIV4  VALUE_(4,2)
#define CYGHWR_HAL_LM3S_SC_RCC_OSCSRC_MASK       0x00000030
#define CYGHWR_HAL_LM3S_SC_RCC_XTAL(__x)         VALUE_(6,__x)
#define CYGHWR_HAL_LM3S_SC_RCC_XTAL_MASK         0x000003C0
#define CYGHWR_HAL_LM3S_SC_RCC_PLLVER            BIT_(10)
#define CYGHWR_HAL_LM3S_SC_RCC_BYPASS            BIT_(11)
#define CYGHWR_HAL_LM3S_SC_RCC_OEN               BIT_(12)
#define CYGHWR_HAL_LM3S_SC_RCC_PWRDN             BIT_(13)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV2       VALUE_(14, 0x0)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV4       VALUE_(14, 0x1)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV8       VALUE_(14, 0x2)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV16      VALUE_(14, 0x3)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV32      VALUE_(14, 0x4)
#define CYGHWR_HAL_LM3S_SC_RCC_PWMDIV_DIV64      VALUE_(14, 0x5)
#define CYGHWR_HAL_LM3S_SC_RCC_USEPWMDIV         BIT_(20)
#define CYGHWR_HAL_LM3S_SC_RCC_USESYSDIV         BIT_(22)
#define CYGHWR_HAL_LM3S_SC_RCC_SYSDIV(__x)       VALUE_(23, __x)
#define CYGHWR_HAL_LM3S_SC_RCC_SYSDIV_MASK       0x07800000
#define CYGHWR_HAL_LM3S_SC_RCC_ACG               BIT_(27)

// DSLPCLKCFG bits
#define CYGHWR_HAL_LM3S_SC_DSLPCLKCFG_IOSC       BIT_(0)

// CLKVCLR bits
#define CYGHWR_HAL_LM3S_SC_CLKVCLR_VERCLR        BIT_(0)

// LDOARST bits
#define CYGHWR_HAL_LM3S_SC_LDOARST_BIT           BIT_(0)

// RCGC0 bits
#define CYGHWR_HAL_LM3S_SC_RCGC0_WDT0            BIT_(3)
#define CYGHWR_HAL_LM3S_SC_RCGC0_MAXADCSPD_125K  VALUE_(8, 0x0)
#define CYGHWR_HAL_LM3S_SC_RCGC0_MAXADCSPD_250K  VALUE_(8, 0x1)
#define CYGHWR_HAL_LM3S_SC_RCGC0_MAXADCSPD_500K  VALUE_(8, 0x2)
#define CYGHWR_HAL_LM3S_SC_RCGC0_ADC0            BIT_(16)
#define CYGHWR_HAL_LM3S_SC_RCGC0_PWM0            BIT_(20)

// SCGC0 bits
#define CYGHWR_HAL_LM3S_SC_SCGC0_WDT0            BIT_(3)
#define CYGHWR_HAL_LM3S_SC_SCGC0_MAXADCSPD_125K  VALUE_(8, 0x0)
#define CYGHWR_HAL_LM3S_SC_SCGC0_MAXADCSPD_250K  VALUE_(8, 0x1)
#define CYGHWR_HAL_LM3S_SC_SCGC0_MAXADCSPD_500K  VALUE_(8, 0x2)
#define CYGHWR_HAL_LM3S_SC_SCGC0_ADC0            BIT_(16)
#define CYGHWR_HAL_LM3S_SC_SCGC0_PWM0            BIT_(20)

// DCGC0 bits
#define CYGHWR_HAL_LM3S_SC_DCGC0_WDT0            BIT_(3)
#define CYGHWR_HAL_LM3S_SC_DCGC0_ADC0            BIT_(16)
#define CYGHWR_HAL_LM3S_SC_DCGC0_PWM0            BIT_(20)

// RCGC1 bits
#define CYGHWR_HAL_LM3S_SC_RCGC1_UART0           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_RCGC1_UART1           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_RCGC1_SSI0            BIT_(4)
#define CYGHWR_HAL_LM3S_SC_RCGC1_I2C0            BIT_(12)
#define CYGHWR_HAL_LM3S_SC_RCGC1_TIMER0          BIT_(16)
#define CYGHWR_HAL_LM3S_SC_RCGC1_TIMER1          BIT_(17)
#define CYGHWR_HAL_LM3S_SC_RCGC1_TIMER2          BIT_(18)
#define CYGHWR_HAL_LM3S_SC_RCGC1_COMP0           BIT_(24)

// SCSG1 bits
#define CYGHWR_HAL_LM3S_SC_SCGC1_UART0           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_SCGC1_UART1           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_SCGC1_SSI0            BIT_(4)
#define CYGHWR_HAL_LM3S_SC_SCGC1_I2C0            BIT_(12)
#define CYGHWR_HAL_LM3S_SC_SCGC1_TIMER0          BIT_(16)
#define CYGHWR_HAL_LM3S_SC_SCGC1_TIMER1          BIT_(17)
#define CYGHWR_HAL_LM3S_SC_SCGC1_TIMER2          BIT_(18)
#define CYGHWR_HAL_LM3S_SC_SCGC1_COMP0           BIT_(24)

// DCSG1 bits
#define CYGHWR_HAL_LM3S_SC_DCGC1_UART0           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_DCGC1_UART1           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_DCGC1_SSI0            BIT_(4)
#define CYGHWR_HAL_LM3S_SC_DCGC1_I2C0            BIT_(12)
#define CYGHWR_HAL_LM3S_SC_DCGC1_TIMER0          BIT_(16)
#define CYGHWR_HAL_LM3S_SC_DCGC1_TIMER1          BIT_(17)
#define CYGHWR_HAL_LM3S_SC_DCGC1_TIMER2          BIT_(18)
#define CYGHWR_HAL_LM3S_SC_DCGC1_COMP0           BIT_(24)

// RCGC2 bits
#define CYGHWR_HAL_LM3S_SC_RCGC2_GPIOA           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_RCGC2_GPIOB           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_RCGC2_GPIOC           BIT_(2)
#define CYGHWR_HAL_LM3S_SC_RCGC2_GPIOD           BIT_(3)
#define CYGHWR_HAL_LM3S_SC_RCGC2_GPIOE           BIT_(4)

// SCSG2 bits
#define CYGHWR_HAL_LM3S_SC_SCGC2_GPIOA           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_SCGC2_GPIOB           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_SCGC2_GPIOC           BIT_(2)
#define CYGHWR_HAL_LM3S_SC_SCGC2_GPIOD           BIT_(3)
#define CYGHWR_HAL_LM3S_SC_SCGC2_GPIOE           BIT_(4)

// DCSG2 bits
#define CYGHWR_HAL_LM3S_SC_DCGC2_GPIOA           BIT_(0)
#define CYGHWR_HAL_LM3S_SC_DCGC2_GPIOB           BIT_(1)
#define CYGHWR_HAL_LM3S_SC_DCGC2_GPIOC           BIT_(2)
#define CYGHWR_HAL_LM3S_SC_DCGC2_GPIOD           BIT_(3)
#define CYGHWR_HAL_LM3S_SC_DCGC2_GPIOE           BIT_(4)

// Define peripheral
#define CYGHWR_HAL_LM3S_PERIPH_GC0               BIT_(31)
#define CYGHWR_HAL_LM3S_P_WDT0                   ( BIT_(31) | CYGHWR_HAL_LM3S_SC_SCGC0_WDT0 )
#define CYGHWR_HAL_LM3S_P_ADC0                   ( BIT_(31) | CYGHWR_HAL_LM3S_SC_SCGC0_ADC0 )
#define CYGHWR_HAL_LM3S_P_PWM                    ( BIT_(31) | CYGHWR_HAL_LM3S_SC_SCGC0_PWM0 )
#define CYGHWR_HAL_LM3S_PERIPH_GC1               BIT_(30)
#define CYGHWR_HAL_LM3S_P_UART0                  ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_UART0 )
#define CYGHWR_HAL_LM3S_P_UART1                  ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_UART1 )
#define CYGHWR_HAL_LM3S_P_SSI0                   ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_SSI0 )
#define CYGHWR_HAL_LM3S_P_I2C0                   ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_I2C0 )
#define CYGHWR_HAL_LM3S_P_TIMER0                 ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_TIMER0 )
#define CYGHWR_HAL_LM3S_P_TIMER1                 ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_TIMER1 )
#define CYGHWR_HAL_LM3S_P_TIMER2                 ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_TIMER2 )
#define CYGHWR_HAL_LM3S_P_COMP0                  ( BIT_(30) | CYGHWR_HAL_LM3S_SC_SCGC1_COMP0 )
#define CYGHWR_HAL_LM3S_PERIPH_GC2               BIT_(29)
#define CYGHWR_HAL_LM3S_P_GPIOA                  ( BIT_(29) | CYGHWR_HAL_LM3S_SC_RCGC2_GPIOA )
#define CYGHWR_HAL_LM3S_P_GPIOB                  ( BIT_(29) | CYGHWR_HAL_LM3S_SC_RCGC2_GPIOB )
#define CYGHWR_HAL_LM3S_P_GPIOC                  ( BIT_(29) | CYGHWR_HAL_LM3S_SC_RCGC2_GPIOC )
#define CYGHWR_HAL_LM3S_P_GPIOD                  ( BIT_(29) | CYGHWR_HAL_LM3S_SC_RCGC2_GPIOD )
#define CYGHWR_HAL_LM3S_P_GPIOE                  ( BIT_(29) | CYGHWR_HAL_LM3S_SC_RCGC2_GPIOE )

__externC void hal_lm3s_periph_set( cyg_uint32 periph, cyg_uint32 on_off );

#define CYGHWR_HAL_LM3S_PERIPH_SET(__periph, __on_off )  hal_lm3s_periph_set( __periph,  __on_off )

//=============================================================================
// Watchdog

#define CYGHWR_HAL_LM3S_WDT_LOAD                 0x000
#define CYGHWR_HAL_LM3S_WDT_VALUE                0x004
#define CYGHWR_HAL_LM3S_WDT_CTL                  0x008
#define CYGHWR_HAL_LM3S_WDT_ICR                  0x00c
#define CYGHWR_HAL_LM3S_WDT_RIS                  0x010
#define CYGHWR_HAL_LM3S_WDT_MIS                  0x014
#define CYGHWR_HAL_LM3S_WDT_TEST                 0x418
#define CYGHWR_HAL_LM3S_WDT_LOCK                 0xc00
#define CYGHWR_HAL_LM3S_WDT_PID4                 0xfd0
#define CYGHWR_HAL_LM3S_WDT_PID5                 0xfd4
#define CYGHWR_HAL_LM3S_WDT_PID6                 0xfd8
#define CYGHWR_HAL_LM3S_WDT_PID7                 0xfdc
#define CYGHWR_HAL_LM3S_WDT_PID0                 0xfe0
#define CYGHWR_HAL_LM3S_WDT_PID1                 0xfe4
#define CYGHWR_HAL_LM3S_WDT_PID2                 0xfe8
#define CYGHWR_HAL_LM3S_WDT_PID3                 0xfec
#define CYGHWR_HAL_LM3S_WDT_PCID0                0xff0
#define CYGHWR_HAL_LM3S_WDT_PCID1                0xff4
#define CYGHWR_HAL_LM3S_WDT_PCID2                0xff8
#define CYGHWR_HAL_LM3S_WDT_PCID3                0xffc


#define CYGHWR_HAL_LM3S_WDT_CTL_INTEN            BIT_(0)
#define CYGHWR_HAL_LM3S_WDT_CTL_RESEN            BIT_(1)

#define CYGHWR_HAL_LM3S_WDT_RIS_RIS              BIT_(0)

#define CYGHWR_HAL_LM3S_WDT_MIS_MIS              BIT_(0)

#define CYGHWR_HAL_LM3S_WDT_TEST_STALL           BIT_(8)

#define CYGHWR_HAL_LM3S_WDT_PID(__x)             ((__x)&0xFF)
#define CYGHWR_HAL_LM3S_WDT_PCID(__x)            ((__x)&0xFF)


//=============================================================================
// GPIO ports

#define CYGHWR_HAL_LM3S_GPIO_DATA                 0x000
#define CYGHWR_HAL_LM3S_GPIO_DIR                  0x400
#define CYGHWR_HAL_LM3S_GPIO_IS                   0x404
#define CYGHWR_HAL_LM3S_GPIO_IBE                  0x408
#define CYGHWR_HAL_LM3S_GPIO_IEV                  0x40c
#define CYGHWR_HAL_LM3S_GPIO_IM                   0x410
#define CYGHWR_HAL_LM3S_GPIO_RIS                  0x414
#define CYGHWR_HAL_LM3S_GPIO_MIS                  0x418
#define CYGHWR_HAL_LM3S_GPIO_ICR                  0x41c
#define CYGHWR_HAL_LM3S_GPIO_AFSEL                0x420
#define CYGHWR_HAL_LM3S_GPIO_DR2R                 0x500
#define CYGHWR_HAL_LM3S_GPIO_DR4R                 0x504
#define CYGHWR_HAL_LM3S_GPIO_DR8R                 0x508
#define CYGHWR_HAL_LM3S_GPIO_ODR                  0x50c
#define CYGHWR_HAL_LM3S_GPIO_PUR                  0x510
#define CYGHWR_HAL_LM3S_GPIO_PDR                  0x514
#define CYGHWR_HAL_LM3S_GPIO_SLR                  0x518
#define CYGHWR_HAL_LM3S_GPIO_DEN                  0x51c
#define CYGHWR_HAL_LM3S_GPIO_PID4                 0xfd0
#define CYGHWR_HAL_LM3S_GPIO_PID5                 0xfd4
#define CYGHWR_HAL_LM3S_GPIO_PID6                 0xfd8
#define CYGHWR_HAL_LM3S_GPIO_PID7                 0xfdc
#define CYGHWR_HAL_LM3S_GPIO_PID0                 0xfe0
#define CYGHWR_HAL_LM3S_GPIO_PID1                 0xfe4
#define CYGHWR_HAL_LM3S_GPIO_PID2                 0xfe8
#define CYGHWR_HAL_LM3S_GPIO_PID3                 0xfec
#define CYGHWR_HAL_LM3S_GPIO_PCID0                0xff0
#define CYGHWR_HAL_LM3S_GPIO_PCID1                0xff4
#define CYGHWR_HAL_LM3S_GPIO_PCID2                0xff8
#define CYGHWR_HAL_LM3S_GPIO_PCID3                0xffc

#define CYGHWR_HAL_LM3S_GPIO_DIR_IN               VALUE_(0,0)     // Input mode
#define CYGHWR_HAL_LM3S_GPIO_DIR_OUT              VALUE_(0,1)     // Output mode

#define CYGHWR_HAL_LM3S_GPIO_IS_EDGE_SENSE        VALUE_(0,0)     // Interrupt edge sensitive
#define CYGHWR_HAL_LM3S_GPIO_IS_LEV_SENSE         VALUE_(0,1)     // Interrupt level sensitive

#define CYGHWR_HAL_LM3S_GPIO_IBE_GPIOIEV          VALUE_(0,0)     // Interrupt control from GPIOIEV
#define CYGHWR_HAL_LM3S_GPIO_IBE_BOTH_EDGE        VALUE_(0,1)     // Both edges can trigger interrupt

#define CYGHWR_HAL_LM3S_GPIO_IEV_LOW              VALUE_(0,0)     // Falling edge or low triggers interrupt
#define CYGHWR_HAL_LM3S_GPIO_IEV_HIGH             VALUE_(0,1)     // Rising edge or low triggers interrupt

#define CYGHWR_HAL_LM3S_GPIO_IM_MASK              VALUE_(0,0)     // Interrupt is masked
#define CYGHWR_HAL_LM3S_GPIO_IM_UMASK             VALUE_(0,1)

// HAL definitions
#define CYGHWR_HAL_LM3S_GPIO_MODE_IN              VALUE_(0,0)     // Input mode
#define CYGHWR_HAL_LM3S_GPIO_MODE_OUT             VALUE_(0,1)     // Output mode
#define CYGHWR_HAL_LM3S_GPIO_MODE_PERIPH          VALUE_(0,2)     // Peripheral function mode

#define CYGHWR_HAL_LM3S_GPIO_STRENGTH_NONE        VALUE_(2,0)     // Strength not specified
#define CYGHWR_HAL_LM3S_GPIO_STRENGTH_2_MA        VALUE_(2,1)     // Strength 2 mA
#define CYGHWR_HAL_LM3S_GPIO_STRENGTH_4_MA        VALUE_(2,2)     // Strength 4 mA
#define CYGHWR_HAL_LM3S_GPIO_STRENGTH_8_MA        VALUE_(2,3)     // Strength 8 mA

#define CYGHWR_HAL_LM3S_GPIO_CNF_NONE             VALUE_(4,0)     // Configuration not specified
#define CYGHWR_HAL_LM3S_GPIO_CNF_PULLUP           VALUE_(4,1)     // Pull-up
#define CYGHWR_HAL_LM3S_GPIO_CNF_PULLDOWN         VALUE_(4,2)     // Pull-down
#define CYGHWR_HAL_LM3S_GPIO_CNF_OP               VALUE_(4,3)     // Open-drain
#define CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLUP        VALUE_(4,4)     // Open-drain / Pull-up
#define CYGHWR_HAL_LM3S_GPIO_CNF_OP_PULLDOWN      VALUE_(4,5)     // Open-drain / Pull-down
#define CYGHWR_HAL_LM3S_GPIO_CNF_AIN              VALUE_(4,6)     // Analog

#define CYGHWR_HAL_LM3S_GPIO_IRQ_DISABLE          VALUE_(9,0)     // Interrupt disable
#define CYGHWR_HAL_LM3S_GPIO_IRQ_FALLING_EDGE     VALUE_(9,1)     // Interrupt on falling edge
#define CYGHWR_HAL_LM3S_GPIO_IRQ_RISING_EDGE      VALUE_(9,2)     // Interrupt on rising edge
#define CYGHWR_HAL_LM3S_GPIO_IRQ_BOTH_EDGES       VALUE_(9,3)     // Interrupt on both edges
#define CYGHWR_HAL_LM3S_GPIO_IRQ_LOW_LEVEL        VALUE_(9,4)     // Interrupt on low level
#define CYGHWR_HAL_LM3S_GPIO_IRQ_HIGH_LEVEL       VALUE_(9,5)     // Interrupt on high level

// This macro packs the port number, bit number, mode, strength, irq and
// configuration for a GPIO pin into a single word.
// The packing puts:

#define CYGHWR_HAL_LM3S_GPIO(                                           \
    __port,                                                             \
    __bit,                                                              \
    __mode,                                                             \
    __strength,                                                         \
    __cnf,                                                              \
    __irq)                                                              \
(                                                                       \
        (CYGHWR_HAL_LM3S_GPIO ## __port - CYGHWR_HAL_LM3S_GPIOA) |      \
         (__bit << 24) |                                                \
         (CYGHWR_HAL_LM3S_GPIO_MODE_ ## __mode) |                       \
         (CYGHWR_HAL_LM3S_GPIO_STRENGTH_ ## __strength) |               \
         (CYGHWR_HAL_LM3S_GPIO_IRQ_ ## __irq) |                         \
         (CYGHWR_HAL_LM3S_GPIO_CNF_ ## __cnf)                           \
)

// Macros to extract encoded values
#define CYGHWR_HAL_LM3S_GPIO_PORT(__pin)       (CYGHWR_HAL_LM3S_GPIOA+((__pin)&0x000FF000))
#define CYGHWR_HAL_LM3S_GPIO_BIT(__pin)        (((__pin)>>24)&0x07)
#define CYGHWR_HAL_LM3S_GPIO_MODE(__pin)       ((__pin)&0x0003)
#define CYGHWR_HAL_LM3S_GPIO_STRENGTH(__pin)   ((__pin)&VALUE_(2,3))
#define CYGHWR_HAL_LM3S_GPIO_CFG(__pin)        ((__pin)&VALUE_(4,7))
#define CYGHWR_HAL_LM3S_GPIO_IRQ(__pin)        ((__pin)&VALUE_(9,7))

#define CYGHWR_HAL_LM3S_GPIO_NONE              (0xFFFFFFFF)


// Functions and macros to configure GPIO ports.

__externC void hal_lm3s_gpio_set( cyg_uint32 pin );
__externC void hal_lm3s_gpio_out( cyg_uint32 pin, int val );
__externC void hal_lm3s_gpio_in ( cyg_uint32 pin, int *val );

#define CYGHWR_HAL_LM3S_GPIO_SET(__pin )        hal_lm3s_gpio_set( __pin )
#define CYGHWR_HAL_LM3S_GPIO_OUT(__pin, __val ) hal_lm3s_gpio_out( __pin, __val )
#define CYGHWR_HAL_LM3S_GPIO_IN(__pin,  __val ) hal_lm3s_gpio_in( __pin, __val )


//=============================================================================
// UARTs

#define CYGHWR_HAL_LM3S_UART_DR                   0x000
#define CYGHWR_HAL_LM3S_UART_SR                   0x004
#define CYGHWR_HAL_LM3S_UART_FR                   0x018
#define CYGHWR_HAL_LM3S_UART_IBRD                 0x024
#define CYGHWR_HAL_LM3S_UART_FBRD                 0x028
#define CYGHWR_HAL_LM3S_UART_LCRH                 0x02c
#define CYGHWR_HAL_LM3S_UART_CTL                  0x030
#define CYGHWR_HAL_LM3S_UART_IFLS                 0x034
#define CYGHWR_HAL_LM3S_UART_IM                   0x038
#define CYGHWR_HAL_LM3S_UART_RIS                  0x03c
#define CYGHWR_HAL_LM3S_UART_MIS                  0x040
#define CYGHWR_HAL_LM3S_UART_ICR                  0x044
#define CYGHWR_HAL_LM3S_UART_PID4                 0xfd0
#define CYGHWR_HAL_LM3S_UART_PID5                 0xfd4
#define CYGHWR_HAL_LM3S_UART_PID6                 0xfd8
#define CYGHWR_HAL_LM3S_UART_PID7                 0xfdc
#define CYGHWR_HAL_LM3S_UART_PID0                 0xfe0
#define CYGHWR_HAL_LM3S_UART_PID1                 0xfe4
#define CYGHWR_HAL_LM3S_UART_PID2                 0xfe8
#define CYGHWR_HAL_LM3S_UART_PID3                 0xfec
#define CYGHWR_HAL_LM3S_UART_PCID0                0xff0
#define CYGHWR_HAL_LM3S_UART_PCID1                0xff4
#define CYGHWR_HAL_LM3S_UART_PCID2                0xff8
#define CYGHWR_HAL_LM3S_UART_PCID3                0xffc


// DR Bits
#define CYGHWR_HAL_LM3S_UART_DR_FE                BIT_(8)
#define CYGHWR_HAL_LM3S_UART_DR_PE                BIT_(9)
#define CYGHWR_HAL_LM3S_UART_DR_BE                BIT_(10)
#define CYGHWR_HAL_LM3S_UART_DR_OE                BIT_(11)

// SR Bits
#define CYGHWR_HAL_LM3S_UART_SR_FE                BIT_(0)
#define CYGHWR_HAL_LM3S_UART_SR_PE                BIT_(1)
#define CYGHWR_HAL_LM3S_UART_SR_BE                BIT_(2)
#define CYGHWR_HAL_LM3S_UART_SR_OE                BIT_(3)

// FR bits
#define CYGHWR_HAL_LM3S_UART_FR_BUSY              BIT_(3)
#define CYGHWR_HAL_LM3S_UART_FR_RXFE              BIT_(4)
#define CYGHWR_HAL_LM3S_UART_FR_TXFF              BIT_(5)
#define CYGHWR_HAL_LM3S_UART_FR_RXFF              BIT_(6)
#define CYGHWR_HAL_LM3S_UART_FR_TXFE              BIT_(7)

// LCRH bits
#define CYGHWR_HAL_LM3S_UART_LCRH_BRK             BIT_(0)
#define CYGHWR_HAL_LM3S_UART_LCRH_PEN             BIT_(1)
#define CYGHWR_HAL_LM3S_UART_LCRH_EPS             BIT_(2)
#define CYGHWR_HAL_LM3S_UART_LCRH_STP2            BIT_(3)
#define CYGHWR_HAL_LM3S_UART_LCRH_FEN             BIT_(4)
#define CYGHWR_HAL_LM3S_UART_LCRH_WLEN(__x)       VALUE_(5,__x)
#define CYGHWR_HAL_LM3S_UART_LCRH_WLEN_MASK       0x00000060
#define CYGHWR_HAL_LM3S_UART_LCRH_SPS             BIT_(7)

// CTL bits
#define CYGHWR_HAL_LM3S_UART_CTL_UARTEN           BIT_(0)
#define CYGHWR_HAL_LM3S_UART_CTL_LBE              BIT_(7)
#define CYGHWR_HAL_LM3S_UART_CTL_TXE              BIT_(8)
#define CYGHWR_HAL_LM3S_UART_CTL_RXE              BIT_(9)

// IFLS bits
#define CYGHWR_HAL_LM3S_UART_IFLS_RXIFLSEL(__x)   VALUE_(0,__x)
#define CYGHWR_HAL_LM3S_UART_IFLS_RXIFLSEL_MASK   0x00000007
#define CYGHWR_HAL_LM3S_UART_IFLS_TXIFLSEL(__x)   VALUE_(3,__x)
#define CYGHWR_HAL_LM3S_UART_IFLS_TXIFLSEL_MASK   0x00000038

// IM bits
#define CYGHWR_HAL_LM3S_UART_IM_RXIM              BIT_(4)
#define CYGHWR_HAL_LM3S_UART_IM_TXIM              BIT_(5)
#define CYGHWR_HAL_LM3S_UART_IM_RTIM              BIT_(6)
#define CYGHWR_HAL_LM3S_UART_IM_FEIM              BIT_(7)
#define CYGHWR_HAL_LM3S_UART_IM_PEIM              BIT_(8)
#define CYGHWR_HAL_LM3S_UART_IM_BEIM              BIT_(9)
#define CYGHWR_HAL_LM3S_UART_IM_OEIM              BIT_(10)

// RIS bits
#define CYGHWR_HAL_LM3S_UART_RIS_RXRIS            BIT_(4)
#define CYGHWR_HAL_LM3S_UART_RIS_TXRIS            BIT_(5)
#define CYGHWR_HAL_LM3S_UART_RIS_RTRIS            BIT_(6)
#define CYGHWR_HAL_LM3S_UART_RIS_FERIS            BIT_(7)
#define CYGHWR_HAL_LM3S_UART_RIS_PERIS            BIT_(8)
#define CYGHWR_HAL_LM3S_UART_RIS_BERIS            BIT_(9)
#define CYGHWR_HAL_LM3S_UART_RIS_OERIS            BIT_(10)

// MIS bits
#define CYGHWR_HAL_LM3S_UART_MIS_RXMIS            BIT_(4)
#define CYGHWR_HAL_LM3S_UART_MIS_TXMIS            BIT_(5)
#define CYGHWR_HAL_LM3S_UART_MIS_RTMIS            BIT_(6)
#define CYGHWR_HAL_LM3S_UART_MIS_FEMIS            BIT_(7)
#define CYGHWR_HAL_LM3S_UART_MIS_PEMIS            BIT_(8)
#define CYGHWR_HAL_LM3S_UART_MIS_BEMIS            BIT_(9)
#define CYGHWR_HAL_LM3S_UART_MIS_OEMIS            BIT_(10)

// ICR bits
#define CYGHWR_HAL_LM3S_UART_ICR_RXIC             BIT_(4)
#define CYGHWR_HAL_LM3S_UART_ICR_TXIC             BIT_(5)
#define CYGHWR_HAL_LM3S_UART_ICR_RTIC             BIT_(6)
#define CYGHWR_HAL_LM3S_UART_ICR_FEIC             BIT_(7)
#define CYGHWR_HAL_LM3S_UART_ICR_PEIC             BIT_(8)
#define CYGHWR_HAL_LM3S_UART_ICR_BEIC             BIT_(9)
#define CYGHWR_HAL_LM3S_UART_ICR_OEIC             BIT_(10)

// UARTO shall be connected to PortA 0/1 on all device
#ifndef CYGHWR_HAL_LM3S_UART0_TX
# define CYGHWR_HAL_LM3S_UART0_TX                 CYGHWR_HAL_LM3S_GPIO( A, 0, PERIPH, NONE, NONE, DISABLE )
#endif
#ifndef CYGHWR_HAL_LM3S_UART0_RX
# define CYGHWR_HAL_LM3S_UART0_RX                 CYGHWR_HAL_LM3S_GPIO( A, 1, PERIPH, NONE, NONE, DISABLE )
#endif
#ifndef CYGHWR_HAL_LM3S_P_UART0_GPIO
# define CYGHWR_HAL_LM3S_P_UART0_GPIO             CYGHWR_HAL_LM3S_P_GPIOA
#endif

#ifndef __ASSEMBLER__

__externC void hal_lm3s_uart_setbaud( CYG_ADDRESS uart, cyg_uint32 baud );

#endif

//=============================================================================
// ADCs

#ifdef CYGHWR_HAL_LM3S_ADC0_CHAN

#define CYGHWR_HAL_LM3S_ADC_ACTSS                 0x0
#define CYGHWR_HAL_LM3S_ADC_RIS                   0x4
#define CYGHWR_HAL_LM3S_ADC_IMR                   0x8
#define CYGHWR_HAL_LM3S_ADC_ISCR                  0xc
#define CYGHWR_HAL_LM3S_ADC_OSR                   0x10
#define CYGHWR_HAL_LM3S_ADC_EMUX                  0x14
#define CYGHWR_HAL_LM3S_ADC_USR                   0x18
#define CYGHWR_HAL_LM3S_ADC_SSPRI                 0x20
#define CYGHWR_HAL_LM3S_ADC_PSSI                  0x28
#define CYGHWR_HAL_LM3S_ADC_SAC                   0x30
#define CYGHWR_HAL_LM3S_ADC_SS_MUX0               0x40
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0               0x44
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO0              0x48
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO0_SR           0x4c
#define CYGHWR_HAL_LM3S_ADC_SS_MUX1               0x60
#define CYGHWR_HAL_LM3S_ADC_SS_CTL1               0x64
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO1              0x68
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO1_SR           0x6c
#define CYGHWR_HAL_LM3S_ADC_SS_MUX2               0x80
#define CYGHWR_HAL_LM3S_ADC_SS_CTL2               0x84
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO2              0x88
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO2_SR           0x8c
#define CYGHWR_HAL_LM3S_ADC_SS_MUX3               0xa0
#define CYGHWR_HAL_LM3S_ADC_SS_CTL3               0xa4
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO3              0xa8
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO3_SR           0xac
#define CYGHWR_HAL_LM3S_ADC_TMLB                  0x100

#define CYGHWR_HAL_LM3S_ADC_SAMPLE_SIZE           10

// Active Sample Sequencer
#define CYGHWR_HAL_LM3S_ADC_ACTSS_ASEN(__x)       VALUE_(((__x)&3),1)

// Raw Interrupt Status
#define CYGHWR_HAL_LM3S_ADC_RIS_INR(__x)          VALUE_(((__x)&3),1)

// Interrupt Mask Register
#define CYGHWR_HAL_LM3S_ADC_IMR_MASK(__x)         VALUE_(((__x)&3),1)

// Interrupt Status and Clear Register
#define CYGHWR_HAL_LM3S_ADC_ISCR_IN(__x)          VALUE_(((__x)&3),1)

// Overflow Status Register
#define CYGHWR_HAL_LM3S_ADC_OSR_OV(__x)           VALUE_(((__x)&3),1)

// Event Multiplexer Select
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_CTRL(__x)     VALUE_((((__x)&3)<<2),0)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_AC0(__x)      VALUE_((((__x)&3)<<2),1)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_EXT(__x)      VALUE_((((__x)&3)<<2),4)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_TIMER(__x)    VALUE_((((__x)&3)<<2),5)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_PWM0(__x)     VALUE_((((__x)&3)<<2),6)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_PWM1(__x)     VALUE_((((__x)&3)<<2),7)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_PWM2(__x)     VALUE_((((__x)&3)<<2),8)
#define CYGHWR_HAL_LM3S_ADC_EMUX_EM_ALWS(__x)     VALUE_((((__x)&3)<<2),15)

// Underflow Status
#define CYGHWR_HAL_LM3S_ADC_USR_UV(__x)           VALUE_(((__x)&3),1)

// Sample Sequence Priority
#define CYGHWR_HAL_LM3S_ADC_SSPRI_SS(__x, __y)    VALUE_((((__x)&3)<<2),(__y)&3)

// Processor Sample Sequence Initiate
#define CYGHWR_HAL_LM3S_ADC_PSSI_SS(__x)          VALUE_(((__x)&3),1)

// Sample Averaging Control
#define CYGHWR_HAL_LM3S_ADC_SAC_NO_OVER           0x0
#define CYGHWR_HAL_LM3S_ADC_SAC_2X                0x1
#define CYGHWR_HAL_LM3S_ADC_SAC_4X                0x2
#define CYGHWR_HAL_LM3S_ADC_SAC_8X                0x3
#define CYGHWR_HAL_LM3S_ADC_SAC_16X               0x4
#define CYGHWR_HAL_LM3S_ADC_SAC_32X               0x5
#define CYGHWR_HAL_LM3S_ADC_SAC_64X               0x6

// Sample Sequence Input Multiplexer Select 0
#define CYGHWR_HAL_LM3S_ADC_SS_MUX0_V(_p_, _x_)   VALUE_((((_x_)&3)<<2),_p_)
#define CYGHWR_HAL_LM3S_ADC_SS_MUX0_M(_x_)        VALUE_((((_x_)&3)<<2), 0x3)

// Sample Sequence Control 0
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D0            BIT_(0)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END0          BIT_(1)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE0           BIT_(2)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS0           BIT_(3)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D1            BIT_(4)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END1          BIT_(5)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE1           BIT_(6)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS1           BIT_(7)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D2            BIT_(8)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END2          BIT_(9)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE2           BIT_(10)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS2           BIT_(11)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D3            BIT_(12)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END3          BIT_(13)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE3           BIT_(14)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS3           BIT_(15)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D4            BIT_(16)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END4          BIT_(17)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE4           BIT_(18)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS4           BIT_(19)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D5            BIT_(20)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END5          BIT_(21)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE5           BIT_(22)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS5           BIT_(23)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D6            BIT_(24)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END6          BIT_(25)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE6           BIT_(26)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS6           BIT_(27)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D7            BIT_(28)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END7          BIT_(29)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE7           BIT_(30)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS7           BIT_(31)

#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_MASK(_x_)     VALUE_((((_x_)&3)<<2), 0x15)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_D(_x_)        VALUE_((((_x_)&3)<<2), 0x1)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_END(_x_)      VALUE_((((_x_)&3)<<2), 0x2)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_IE(_x_)       VALUE_((((_x_)&3)<<2), 0x4)
#define CYGHWR_HAL_LM3S_ADC_SS_CTL0_TS(_x_)       VALUE_((((_x_)&3)<<2), 0x8)

// Sequence FIFO Status
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_TPTR       0x0
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_HPTR       0x0
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_EMPTY      BIT_(8)
#define CYGHWR_HAL_LM3S_ADC_SS_FIFO_SR_FULL       BIT_(12)

#endif // CYGHWR_HAL_LM3S_ADC0_CHAN


//=============================================================================
// SSI interface register definitions.

#ifdef CYGHWR_HAL_LM3S_SSI_CHAN

#define CYGHWR_HAL_LM3S_SSI_CR0                   0x000
#define CYGHWR_HAL_LM3S_SSI_CR1                   0x004
#define CYGHWR_HAL_LM3S_SSI_DR                    0x008
#define CYGHWR_HAL_LM3S_SSI_SR                    0x00C
#define CYGHWR_HAL_LM3S_SSI_CPSR                  0x010
#define CYGHWR_HAL_LM3S_SSI_IM                    0x014
#define CYGHWR_HAL_LM3S_SSI_RIS                   0x018
#define CYGHWR_HAL_LM3S_SSI_MIS                   0x01c
#define CYGHWR_HAL_LM3S_SSI_ICR                   0x020
#define CYGHWR_HAL_LM3S_SSI_PID4                  0xfd0
#define CYGHWR_HAL_LM3S_SSI_PID5                  0xfd4
#define CYGHWR_HAL_LM3S_SSI_PID6                  0xfd8
#define CYGHWR_HAL_LM3S_SSI_PID7                  0xfdc
#define CYGHWR_HAL_LM3S_SSI_PID0                  0xfe0
#define CYGHWR_HAL_LM3S_SSI_PID1                  0xfe4
#define CYGHWR_HAL_LM3S_SSI_PID2                  0xfe8
#define CYGHWR_HAL_LM3S_SSI_PID3                  0xfec
#define CYGHWR_HAL_LM3S_SSI_PCID0                 0xff0
#define CYGHWR_HAL_LM3S_SSI_PCID1                 0xff4
#define CYGHWR_HAL_LM3S_SSI_PCID2                 0xff8
#define CYGHWR_HAL_LM3S_SSI_PCID3                 0xffc


// CR0 bits
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_4BIT          VALUE_(0, 0x3)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_5BIT          VALUE_(0, 0x4)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_6BIT          VALUE_(0, 0x5)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_7BIT          VALUE_(0, 0x6)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_8BIT          VALUE_(0, 0x7)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_9BIT          VALUE_(0, 0x8)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_10BIT         VALUE_(0, 0x9)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_11BIT         VALUE_(0, 0xa)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_12BIT         VALUE_(0, 0xb)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_13BIT         VALUE_(0, 0xc)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_14BIT         VALUE_(0, 0xd)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_15BIT         VALUE_(0, 0xe)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_16BIT         VALUE_(0, 0xf)
#define CYGHWR_HAL_LM3S_SSI_CR0_DSS_MASK          0x0000000f
#define CYGHWR_HAL_LM3S_SSI_CR0_FRF_FSPIFF        VALUE_(4, 0x0)
#define CYGHWR_HAL_LM3S_SSI_CR0_FRF_TISSFF        VALUE_(4, 0x1)
#define CYGHWR_HAL_LM3S_SSI_CR0_FRF_MFF           VALUE_(4, 0x2)
#define CYGHWR_HAL_LM3S_SSI_CR0_FRF_MASK          0x00000030
#define CYGHWR_HAL_LM3S_SSI_CR0_SPO               BIT_(6)
#define CYGHWR_HAL_LM3S_SSI_CR0_SPH               BIT_(7)
#define CYGHWR_HAL_LM3S_SSI_CR0_SCR(__x)          VALUE_(8, ((__x)&0xff))

// CR1 bits
#define CYGHWR_HAL_LM3S_SSI_CR1_LBM               BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_CR1_SSE               BIT_(1)
#define CYGHWR_HAL_LM3S_SSI_CR1_MS                BIT_(2)
#define CYGHWR_HAL_LM3S_SSI_CR1_SOD               BIT_(3)

// SR bits
#define CYGHWR_HAL_LM3S_SSI_SR_TFE                BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_SR_TNF                BIT_(1)
#define CYGHWR_HAL_LM3S_SSI_SR_RNE                BIT_(2)
#define CYGHWR_HAL_LM3S_SSI_SR_RFF                BIT_(3)
#define CYGHWR_HAL_LM3S_SSI_SR_BSY                BIT_(4)

// IM bits
#define CYGHWR_HAL_LM3S_SSI_IM_RORIM              BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_IM_RTIM               BIT_(1)
#define CYGHWR_HAL_LM3S_SSI_IM_RXIM               BIT_(2)
#define CYGHWR_HAL_LM3S_SSI_IM_TXIM               BIT_(3)

// RIS bits
#define CYGHWR_HAL_LM3S_SSI_RIS_RORRIS            BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_RIS_RTRIS             BIT_(1)
#define CYGHWR_HAL_LM3S_SSI_RIS_RXRIS             BIT_(2)
#define CYGHWR_HAL_LM3S_SSI_RIS_TXRIS             BIT_(3)

// MIS bits
#define CYGHWR_HAL_LM3S_SSI_MIS_RORMIS            BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_MIS_RTMIS             BIT_(1)
#define CYGHWR_HAL_LM3S_SSI_MIS_RXMIS             BIT_(2)
#define CYGHWR_HAL_LM3S_SSI_MIS_TXMIS             BIT_(3)

// ICR bits
#define CYGHWR_HAL_LM3S_SSI_ICR_RORIC             BIT_(0)
#define CYGHWR_HAL_LM3S_SSI_ICR_RTIC              BIT_(1)

#endif // CYGHWR_HAL_LM3S_SSI_CHAN

//=============================================================================
// PWM register definitions.

#ifdef CYGHWR_HAL_LM3S_PWM_CHAN

#define CYGHWR_HAL_LM3S_PWM_CTL                   0x000
#define CYGHWR_HAL_LM3S_PWM_SYNC                  0x004
#define CYGHWR_HAL_LM3S_PWM_ENABLE                0x008
#define CYGHWR_HAL_LM3S_PWM_INVERT                0x00c
#define CYGHWR_HAL_LM3S_PWM_FAULT                 0x010
#define CYGHWR_HAL_LM3S_PWM_INTEN                 0x014
#define CYGHWR_HAL_LM3S_PWM_RIS                   0x018
#define CYGHWR_HAL_LM3S_PWM_ISC                   0x01c
#define CYGHWR_HAL_LM3S_PWM_STATUS                0x020

#define CYGHWR_HAL_LM3S_PWM0_CTL                  0x040
#define CYGHWR_HAL_LM3S_PWM0_INTEN                0x044
#define CYGHWR_HAL_LM3S_PWM0_RIS                  0x048
#define CYGHWR_HAL_LM3S_PWM0_ISC                  0x04c
#define CYGHWR_HAL_LM3S_PWM0_LOAD                 0x050
#define CYGHWR_HAL_LM3S_PWM0_COUNT                0x054
#define CYGHWR_HAL_LM3S_PWM0_CMPA                 0x058
#define CYGHWR_HAL_LM3S_PWM0_CMPB                 0x05c
#define CYGHWR_HAL_LM3S_PWM0_GENA                 0x060
#define CYGHWR_HAL_LM3S_PWM0_GENB                 0x064
#define CYGHWR_HAL_LM3S_PWM0_DBCTL                0x068
#define CYGHWR_HAL_LM3S_PWM0_DBRISE               0x06c
#define CYGHWR_HAL_LM3S_PWM0_DBFALL               0x070

#define CYGHWR_HAL_LM3S_PWM1_CTL                  0x080
#define CYGHWR_HAL_LM3S_PWM1_INTEN                0x084
#define CYGHWR_HAL_LM3S_PWM1_RIS                  0x088
#define CYGHWR_HAL_LM3S_PWM1_ISC                  0x08c
#define CYGHWR_HAL_LM3S_PWM1_LOAD                 0x090
#define CYGHWR_HAL_LM3S_PWM1_COUNT                0x094
#define CYGHWR_HAL_LM3S_PWM1_CMPA                 0x098
#define CYGHWR_HAL_LM3S_PWM1_CMPB                 0x09c
#define CYGHWR_HAL_LM3S_PWM1_GENA                 0x0a0
#define CYGHWR_HAL_LM3S_PWM1_GENB                 0x0a4
#define CYGHWR_HAL_LM3S_PWM1_DBCTL                0x0a8
#define CYGHWR_HAL_LM3S_PWM1_DBRISE               0x0ac
#define CYGHWR_HAL_LM3S_PWM1_DBFALL               0x0b0

#define CYGHWR_HAL_LM3S_PWM2_CTL                  0x0c0
#define CYGHWR_HAL_LM3S_PWM2_INTEN                0x0c4
#define CYGHWR_HAL_LM3S_PWM2_RIS                  0x0c8
#define CYGHWR_HAL_LM3S_PWM2_ISC                  0x0cc
#define CYGHWR_HAL_LM3S_PWM2_LOAD                 0x0d0
#define CYGHWR_HAL_LM3S_PWM2_COUNT                0x0d4
#define CYGHWR_HAL_LM3S_PWM2_CMPA                 0x0d8
#define CYGHWR_HAL_LM3S_PWM2_CMPB                 0x0dc
#define CYGHWR_HAL_LM3S_PWM2_GENA                 0x0e0
#define CYGHWR_HAL_LM3S_PWM2_GENB                 0x0e4
#define CYGHWR_HAL_LM3S_PWM2_DBCTL                0x0e8
#define CYGHWR_HAL_LM3S_PWM2_DBRISE               0x0ec
#define CYGHWR_HAL_LM3S_PWM2_DBFALL               0x0f0


// CTL bits
#define CYGHWR_HAL_LM3S_PWM_CTL_GSYNC0            BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_CTL_GSYNC1            BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_CTL_GSYNC2            BIT_(2)

// SYNC bits
#define CYGHWR_HAL_LM3S_PWM_SYNC_SYNC0            BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_SYNC_SYNC1            BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_SYNC_SYNC2            BIT_(2)

// ENABLE bits
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM0           BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM1           BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM2           BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM3           BIT_(3)
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM4           BIT_(4)
#define CYGHWR_HAL_LM3S_PWM_ENABLE_PWM5           BIT_(5)

// INVERT bits
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM0           BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM1           BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM2           BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM3           BIT_(3)
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM4           BIT_(4)
#define CYGHWR_HAL_LM3S_PWM_INVERT_PWM5           BIT_(5)

// FAULT bits
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM0            BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM1            BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM2            BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM3            BIT_(3)
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM4            BIT_(4)
#define CYGHWR_HAL_LM3S_PWM_FAULT_PWM5            BIT_(5)

// INTEN bits
#define CYGHWR_HAL_LM3S_PWM_INTEN_PWM0            BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_INTEN_PWM1            BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_INTEN_PWM2            BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_INTEN_FAULT           BIT_(16)

// RIS bits
#define CYGHWR_HAL_LM3S_PWM_RIS_PWM0              BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_RIS_PWM1              BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_RIS_PWM2              BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_RIS_FAULT             BIT_(16)

// ISC bits
#define CYGHWR_HAL_LM3S_PWM_ISC_PWM0              BIT_(0)
#define CYGHWR_HAL_LM3S_PWM_ISC_PWM1              BIT_(1)
#define CYGHWR_HAL_LM3S_PWM_ISC_PWM2              BIT_(2)
#define CYGHWR_HAL_LM3S_PWM_ISC_FAULT             BIT_(16)

// STATUS bits
#define CYGHWR_HAL_LM3S_PWM_STATUS_FAULT          BIT_(0)

// PWMx CTL Bits
#define CYGHWR_HAL_LM3S_PWMx_CTL_ENABLE           BIT_(0)
#define CYGHWR_HAL_LM3S_PWMx_CTL_MODE             BIT_(1)
#define CYGHWR_HAL_LM3S_PWMx_CTL_DEBUG            BIT_(2)
#define CYGHWR_HAL_LM3S_PWMx_CTL_LOAD_UPD         BIT_(3)
#define CYGHWR_HAL_LM3S_PWMx_CTL_CMPA_UPD         BIT_(4)
#define CYGHWR_HAL_LM3S_PWMx_CTL_CMPB_UPD         BIT_(5)

// PWMx INTEN Bits
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CNT_ZERO       BIT_(0)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CNT_LOAD       BIT_(1)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CMPA_UP        BIT_(2)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CMPA_DOWN      BIT_(3)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CMPB_UP        BIT_(4)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_CMPB_DOWN      BIT_(5)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CNT_ZERO  BIT_(8)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CNT_LOAD  BIT_(9)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CMPA_UP   BIT_(10)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CMPA_DOWN BIT_(11)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CMPB_UP   BIT_(12)
#define CYGHWR_HAL_LM3S_PWMx_INTEN_TRIG_CMPB_DOWN BIT_(13)

// PWMx RIS Bits
#define CYGHWR_HAL_LM3S_PWMx_RIS_CNT_ZERO         BIT_(0)
#define CYGHWR_HAL_LM3S_PWMx_RIS_CNT_LOAD         BIT_(1)
#define CYGHWR_HAL_LM3S_PWMx_RIS_CMPA_UP          BIT_(2)
#define CYGHWR_HAL_LM3S_PWMx_RIS_CMPA_DOWN        BIT_(3)
#define CYGHWR_HAL_LM3S_PWMx_RIS_CMPB_UP          BIT_(4)
#define CYGHWR_HAL_LM3S_PWMx_RIS_CMPB_DOWN        BIT_(5)

// PWMx ISC Bits
#define CYGHWR_HAL_LM3S_PWMx_ISC_CNT_ZERO         BIT_(0)
#define CYGHWR_HAL_LM3S_PWMx_ISC_CNT_LOAD         BIT_(1)
#define CYGHWR_HAL_LM3S_PWMx_ISC_CMPA_UP          BIT_(2)
#define CYGHWR_HAL_LM3S_PWMx_ISC_CMPA_DOWN        BIT_(3)
#define CYGHWR_HAL_LM3S_PWMx_ISC_CMPB_UP          BIT_(4)
#define CYGHWR_HAL_LM3S_PWMx_ISC_CMPB_DOWN        BIT_(5)

// PWMx GENA Bits
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_ZERO_NONE   VALUE_(0, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_ZERO_INVS   VALUE_(0, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_ZERO_SET0   VALUE_(0, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_ZERO_SET1   VALUE_(0, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_ZERO_MASK   0x00000003
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_LOAD_NONE   VALUE_(2, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_LOAD_INVS   VALUE_(2, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_LOAD_SET0   VALUE_(2, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_LOAD_SET1   VALUE_(2, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_ACT_LOAD_MASK   0x0000000c
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_UP_NONE    VALUE_(4, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_UP_INVS    VALUE_(4, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_UP_SET0    VALUE_(4, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_UP_SET1    VALUE_(4, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_UP_MASK    0x00000030
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_DOWN_NONE  VALUE_(6, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_DOWN_INVS  VALUE_(6, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_DOWN_SET0  VALUE_(6, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_DOWN_SET1  VALUE_(6, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPA_DOWN_MASK  0x000000c0
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_UP_NONE    VALUE_(8, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_UP_INVS    VALUE_(8, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_UP_SET0    VALUE_(8, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_UP_SET1    VALUE_(8, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_UP_MASK    0x00000300
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_DOWN_NONE  VALUE_(10, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_DOWN_INVS  VALUE_(10, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_DOWN_SET0  VALUE_(10, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_DOWN_SET1  VALUE_(10, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENA_CMPB_DOWN_MASK  0x00000c00

// PWMx GENB Bits
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_ZERO_NONE   VALUE_(0, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_ZERO_INVS   VALUE_(0, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_ZERO_SET0   VALUE_(0, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_ZERO_SET1   VALUE_(0, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_ZERO_MASK   0x00000003
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_LOAD_NONE   VALUE_(2, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_LOAD_INVS   VALUE_(2, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_LOAD_SET0   VALUE_(2, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_LOAD_SET1   VALUE_(2, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_ACT_LOAD_MASK   0x0000000c
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_UP_NONE    VALUE_(4, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_UP_INVS    VALUE_(4, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_UP_SET0    VALUE_(4, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_UP_SET1    VALUE_(4, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_UP_MASK    0x00000030
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_DOWN_NONE  VALUE_(6, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_DOWN_INVS  VALUE_(6, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_DOWN_SET0  VALUE_(6, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_DOWN_SET1  VALUE_(6, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPA_DOWN_MASK  0x000000c0
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_UP_NONE    VALUE_(8, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_UP_INVS    VALUE_(8, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_UP_SET0    VALUE_(8, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_UP_SET1    VALUE_(8, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_UP_MASK    0x00000300
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_DOWN_NONE  VALUE_(10, 0x0)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_DOWN_INVS  VALUE_(10, 0x1)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_DOWN_SET0  VALUE_(10, 0x2)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_DOWN_SET1  VALUE_(10, 0x3)
#define CYGHWR_HAL_LM3S_PWMx_GENB_CMPB_DOWN_MASK  0x00000c00

// PWMx DBCTL Bits
#define CYGHWR_HAL_LM3S_PWMx_DBCTL_ENABLE         BIT_(0)

#endif // CYGHWR_HAL_LM3S_PWM_CHAN


//=============================================================================
// Global Timers register definitions.

#define CYGHWR_HAL_LM3S_GPTIM_CFG                 0x000
#define CYGHWR_HAL_LM3S_GPTIM_TAMR                0x004
#define CYGHWR_HAL_LM3S_GPTIM_TBMR                0x008
#define CYGHWR_HAL_LM3S_GPTIM_CTL                 0x00c
#define CYGHWR_HAL_LM3S_GPTIM_IMR                 0x018
#define CYGHWR_HAL_LM3S_GPTIM_RIS                 0x01C
#define CYGHWR_HAL_LM3S_GPTIM_MIS                 0x020
#define CYGHWR_HAL_LM3S_GPTIM_ICR                 0x024
#define CYGHWR_HAL_LM3S_GPTIM_TAILR               0x028
#define CYGHWR_HAL_LM3S_GPTIM_TBILR               0x02c
#define CYGHWR_HAL_LM3S_GPTIM_TAMATCHR            0x030
#define CYGHWR_HAL_LM3S_GPTIM_TBMATCHR            0x034
#define CYGHWR_HAL_LM3S_GPTIM_TAPR                0x038
#define CYGHWR_HAL_LM3S_GPTIM_TBPR                0x03c
#define CYGHWR_HAL_LM3S_GPTIM_TAPMR               0x040
#define CYGHWR_HAL_LM3S_GPTIM_TBPMR               0x044
#define CYGHWR_HAL_LM3S_GPTIM_TAR                 0x048
#define CYGHWR_HAL_LM3S_GPTIM_TBR                 0x04c

// CFG bits
#define CYGHWR_HAL_LM3S_GPTIM_CFG_32BIT           VALUE_(0, 0x0)
#define CYGHWR_HAL_LM3S_GPTIM_CFG_32BIT_RTC       VALUE_(0, 0x1)
#define CYGHWR_HAL_LM3S_GPTIM_CFG_16BIT           VALUE_(0, 0x4)
#define CYGHWR_HAL_LM3S_GPTIM_CFG_MASK            0x00000007

// TAMR bits
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_ONE_SHOT       VALUE_(0, 0x1)
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_PERIODIC       VALUE_(0, 0x2)
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_CAPTURE        VALUE_(0, 0x3)
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_MASK           0x00000003
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_TACMR          BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_TAMR_TAAMS          BIT_(3)

// TBMR bits
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_ONE_SHOT       VALUE_(0, 0x1)
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_PERIODIC       VALUE_(0, 0x2)
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_CAPTURE        VALUE_(0, 0x3)
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_MASK           0x00000003
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_TBCMR          BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_TBMR_TBAMS          BIT_(3)

// CTL bits
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAEN            BIT_(0)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TASTALL         BIT_(1)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAEVENT_POS     VALUE_(2, 0x0)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAEVENT_NEG     VALUE_(2, 0x1)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAEVENT_BOTH    VALUE_(2, 0x3)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_RTCEN           BIT_(4)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAOTE           BIT_(5)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TAPWML          BIT_(6)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBEN            BIT_(8)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBSTALL         BIT_(9)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBEVENT_POS     VALUE_(10, 0x0)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBEVENT_NEG     VALUE_(10, 0x1)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBEVENT_BOTH    VALUE_(10, 0x3)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBOTE           BIT_(13)
#define CYGHWR_HAL_LM3S_GPTIM_CTL_TBPWML          BIT_(14)

// IMR bits
#define CYGHWR_HAL_LM3S_GPTIM_IMR_TATOIM          BIT_(0)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_CAMIM           BIT_(1)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_CAEIM           BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_RTCIM           BIT_(3)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_TBTOIM          BIT_(8)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_CBMIM           BIT_(9)
#define CYGHWR_HAL_LM3S_GPTIM_IMR_CBEIM           BIT_(10)

// RIS bits
#define CYGHWR_HAL_LM3S_GPTIM_RIS_TATORIS         BIT_(0)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_CAMRIS          BIT_(1)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_CAERIS          BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_RTCRIS          BIT_(3)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_TBTORIS         BIT_(8)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_CBMRIS          BIT_(9)
#define CYGHWR_HAL_LM3S_GPTIM_RIS_CBERIS          BIT_(10)

// MIS bits
#define CYGHWR_HAL_LM3S_GPTIM_MIS_TATOMIS         BIT_(0)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_CAMMIS          BIT_(1)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_CAEMIS          BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_RTCMIS          BIT_(3)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_TBTOMIS         BIT_(8)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_CBMMIS          BIT_(9)
#define CYGHWR_HAL_LM3S_GPTIM_MIS_CBEMIS          BIT_(10)

// ICR bits
#define CYGHWR_HAL_LM3S_GPTIM_ICR_TATOCINT        BIT_(0)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_CAMCINT         BIT_(1)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_CAECINT         BIT_(2)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_RTCCINT         BIT_(3)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_TBTOCINT        BIT_(8)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_CBMCINT         BIT_(9)
#define CYGHWR_HAL_LM3S_GPTIM_ICR_CBECINT         BIT_(10)

cyg_uint32 hal_lm3s_timer_clock( void );

#ifndef __ASSEMBLER__

__externC cyg_uint32 hal_stellaris_lm3s_timer_clock( CYG_ADDRESS base );

#endif


//=============================================================================
// I2C register definitions.

#ifdef CYGHWR_HAL_LM3S_I2C_CHAN

#define CYGHWR_HAL_LM3S_I2C_MSA                   0x000
#define CYGHWR_HAL_LM3S_I2C_MCS                   0x004
#define CYGHWR_HAL_LM3S_I2C_MDR                   0x008
#define CYGHWR_HAL_LM3S_I2C_MTPR                  0x00c
#define CYGHWR_HAL_LM3S_I2C_MIMR                  0x010
#define CYGHWR_HAL_LM3S_I2C_MRIS                  0x014
#define CYGHWR_HAL_LM3S_I2C_MMIS                  0x018
#define CYGHWR_HAL_LM3S_I2C_MICR                  0x01c
#define CYGHWR_HAL_LM3S_I2C_MCR                   0x020

#define CYGHWR_HAL_LM3S_I2C_SOAR                  0x000
#define CYGHWR_HAL_LM3S_I2C_SCSR                  0x004
#define CYGHWR_HAL_LM3S_I2C_SDR                   0x008
#define CYGHWR_HAL_LM3S_I2C_SIMR                  0x00c
#define CYGHWR_HAL_LM3S_I2C_SRIS                  0x010
#define CYGHWR_HAL_LM3S_I2C_SMIS                  0x014
#define CYGHWR_HAL_LM3S_I2C_SICR                  0x018

// MSA bits
#define CYGHWR_HAL_LM3S_I2C_MSA_RS                BIT_(0)
#define CYGHWR_HAL_LM3S_I2C_MSA_SA(__x)           VALUE_(1, __x)

// MCS bits
#define CYGHWR_HAL_LM3S_I2C_MCS_BUSY              BIT_(0)
#define CYGHWR_HAL_LM3S_I2C_MCS_ERR               BIT_(1)
#define CYGHWR_HAL_LM3S_I2C_MCS_ADRACK            BIT_(2)
#define CYGHWR_HAL_LM3S_I2C_MCS_DATACK            BIT_(3)
#define CYGHWR_HAL_LM3S_I2C_MCS_ARBLST            BIT_(4)
#define CYGHWR_HAL_LM3S_I2C_MCS_IDLE              BIT_(5)
#define CYGHWR_HAL_LM3S_I2C_MCS_BUSBSY            BIT_(6)

#define CYGHWR_HAL_LM3S_I2C_MCS_RUN               BIT_(0)
#define CYGHWR_HAL_LM3S_I2C_MCS_START             BIT_(1)
#define CYGHWR_HAL_LM3S_I2C_MCS_STOP              BIT_(2)
#define CYGHWR_HAL_LM3S_I2C_MCS_ACK               BIT_(3)

// MIMR bits
#define CYGHWR_HAL_LM3S_I2C_MIMR_IM               BIT_(0)

// MRIS bits
#define CYGHWR_HAL_LM3S_I2C_MRIS_RIS              BIT_(0)

// MMIS bits
#define CYGHWR_HAL_LM3S_I2C_MMIS_MIS              BIT_(0)

// MICR bits
#define CYGHWR_HAL_LM3S_I2C_MICR_IC               BIT_(0)

// MCR bits
#define CYGHWR_HAL_LM3S_I2C_MCR_LPBK              BIT_(0)
#define CYGHWR_HAL_LM3S_I2C_MCR_MFE               BIT_(4)
#define CYGHWR_HAL_LM3S_I2C_MCR_SFE               BIT_(5)

// SCSR bits
#define CYGHWR_HAL_LM3S_I2C_SCSR_READ_RREQ        BIT_(0)
#define CYGHWR_HAL_LM3S_I2C_SCSR_READ_TREQ        BIT_(1)
#define CYGHWR_HAL_LM3S_I2C_SCSR_READ_FBR         BIT_(2)

#define CYGHWR_HAL_LM3S_I2C_SCSR_WRITE_DA         BIT_(0)

// SIMR bits
#define CYGHWR_HAL_LM3S_I2C_SIMR_DATAIM           BIT_(0)

// SRIS bits
#define CYGHWR_HAL_LM3S_I2C_SRIS_DATARIS          BIT_(0)

// SMIS bits
#define CYGHWR_HAL_LM3S_I2C_SMIS_DATAMIS          BIT_(0)

// SICR bits
#define CYGHWR_HAL_LM3S_I2C_SICR_DATAMIC          BIT_(0)

__externC cyg_uint32 hal_lm3s_i2c_clock( void );

#endif // CYGHWR_HAL_LM3S_I2C_CHAN


//=============================================================================
// AC register definitions.

#ifdef CYGHWR_HAL_LM3S_AC_CHAN

#define CYGHWR_HAL_LM3S_AC_MIS                    0x000
#define CYGHWR_HAL_LM3S_AC_RIS                    0x004
#define CYGHWR_HAL_LM3S_AC_INTEN                  0x008
#define CYGHWR_HAL_LM3S_AC_REFCTL                 0x010
#define CYGHWR_HAL_LM3S_AC_STAT0                  0x020
#define CYGHWR_HAL_LM3S_AC_CTL0                   0x024
#define CYGHWR_HAL_LM3S_AC_STAT1                  0x040
#define CYGHWR_HAL_LM3S_AC_CTL1                   0x044
#define CYGHWR_HAL_LM3S_AC_STAT2                  0x060
#define CYGHWR_HAL_LM3S_AC_CTL2                   0x064

// MIS bits
#define CYGHWR_HAL_LM3S_AC_MIS_IN0                BIT_(0)
#define CYGHWR_HAL_LM3S_AC_MIS_IN1                BIT_(1)
#define CYGHWR_HAL_LM3S_AC_MIS_IN2                BIT_(2)

// RIS bits
#define CYGHWR_HAL_LM3S_AC_RIS_IN0                BIT_(0)
#define CYGHWR_HAL_LM3S_AC_RIS_IN1                BIT_(1)
#define CYGHWR_HAL_LM3S_AC_RIS_IN2                BIT_(2)

// INTEN bits
#define CYGHWR_HAL_LM3S_AC_INTEN_IN0              BIT_(0)
#define CYGHWR_HAL_LM3S_AC_INTEN_IN1              BIT_(1)
#define CYGHWR_HAL_LM3S_AC_INTEN_IN2              BIT_(2)

// REFCTL bits
#define CYGHWR_HAL_LM3S_AC_REFCTL_RNG             BIT_(8)
#define CYGHWR_HAL_LM3S_AC_REFCTL_EN              BIT_(9)

// STAT0, STAT1 and STAT2 bits
#define CYGHWR_HAL_LM3S_AC_STATx_OVAL             BIT_(1)

// CTL0, CTL1 anf CTL2 bits
#define CYGHWR_HAL_LM3S_AC_CTLx_CINV              BIT_(1)
#define CYGHWR_HAL_LM3S_AC_CTLx_ISEN_LEV_SENSE    VALUE_(2, 0x0)
#define CYGHWR_HAL_LM3S_AC_CTLx_ISEN_FALL_EDGE    VALUE_(2, 0x1)
#define CYGHWR_HAL_LM3S_AC_CTLx_ISEN_RISE_EDGE    VALUE_(2, 0x2)
#define CYGHWR_HAL_LM3S_AC_CTLx_ISEN_BOTH_EDGE    VALUE_(2, 0x3)
#define CYGHWR_HAL_LM3S_AC_CTLx_ISLVAL            BIT_(4)
#define CYGHWR_HAL_LM3S_AC_CTLx_ASRCP_PIN_VAL     VALUE_(9, 0x0)
#define CYGHWR_HAL_LM3S_AC_CTLx_ASRCP_PIN_VAL_C0  VALUE_(9, 0x1)
#define CYGHWR_HAL_LM3S_AC_CTLx_ASRCP_IVOLTREF    VALUE_(9, 0x2)

#endif // CYGHWR_HAL_LM3S_AC_CHAN


//=============================================================================
// QEI register definitions.

#ifdef CYGHWR_HAL_LM3S_QEI_CHAN

#define CYGHWR_HAL_LM3S_QEI_CTL                   0x000
#define CYGHWR_HAL_LM3S_QEI_STAT                  0x004
#define CYGHWR_HAL_LM3S_QEI_POS                   0x008
#define CYGHWR_HAL_LM3S_QEI_MAXPOS                0x00c
#define CYGHWR_HAL_LM3S_QEI_LOAD                  0x010
#define CYGHWR_HAL_LM3S_QEI_TIME                  0x014
#define CYGHWR_HAL_LM3S_QEI_COUNT                 0x018
#define CYGHWR_HAL_LM3S_QEI_SPEED                 0x01c
#define CYGHWR_HAL_LM3S_QEI_INTEN                 0x020
#define CYGHWR_HAL_LM3S_QEI_RIS                   0x024
#define CYGHWR_HAL_LM3S_QEI_ISC                   0x028

// CTL bits
#define CYGHWR_HAL_LM3S_QEI_CTL_EN                BIT_(0)
#define CYGHWR_HAL_LM3S_QEI_CTL_SWAP              BIT_(1)
#define CYGHWR_HAL_LM3S_QEI_CTL_SIGMODE           BIT_(2)
#define CYGHWR_HAL_LM3S_QEI_CTL_CAPMODE           BIT_(3)
#define CYGHWR_HAL_LM3S_QEI_CTL_RESMODE           BIT_(4)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELEN             BIT_(5)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_1          VALUE_(6, 0x0)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_2          VALUE_(6, 0x1)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_4          VALUE_(6, 0x2)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_8          VALUE_(6, 0x3)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_16         VALUE_(6, 0x4)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_32         VALUE_(6, 0x5)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_64         VALUE_(6, 0x6)
#define CYGHWR_HAL_LM3S_QEI_CTL_VELDIV_128        VALUE_(6, 0x7)
#define CYGHWR_HAL_LM3S_QEI_CTL_INVA              BIT_(9)
#define CYGHWR_HAL_LM3S_QEI_CTL_INVB              BIT_(10)
#define CYGHWR_HAL_LM3S_QEI_CTL_INVI              BIT_(11)
#define CYGHWR_HAL_LM3S_QEI_CTL_SATLLEN           BIT_(12)

// STAT bits
#define CYGHWR_HAL_LM3S_QEI_STAT_ERR              BIT_(0)
#define CYGHWR_HAL_LM3S_QEI_STAT_DIR              BIT_(1)

// INTEN bits
#define CYGHWR_HAL_LM3S_QEI_INTEN_INTINDEX        BIT_(0)
#define CYGHWR_HAL_LM3S_QEI_INTEN_INTTIMER        BIT_(1)
#define CYGHWR_HAL_LM3S_QEI_INTEN_INTDIR          BIT_(2)
#define CYGHWR_HAL_LM3S_QEI_INTEN_INTERROR        BIT_(3)

// RIS bits
#define CYGHWR_HAL_LM3S_QEI_RIS_INTINDEX          BIT_(0)
#define CYGHWR_HAL_LM3S_QEI_RIS_INTTIMER          BIT_(1)
#define CYGHWR_HAL_LM3S_QEI_RIS_INTDIR            BIT_(2)
#define CYGHWR_HAL_LM3S_QEI_RIS_INTERROR          BIT_(3)

// ISC bits
#define CYGHWR_HAL_LM3S_QEI_ISC_INTINDEX          BIT_(0)
#define CYGHWR_HAL_LM3S_QEI_ISC_INTTIMER          BIT_(1)
#define CYGHWR_HAL_LM3S_QEI_ISC_INTDIR            BIT_(2)
#define CYGHWR_HAL_LM3S_QEI_ISC_INTERROR          BIT_(3)

#endif // CYGHWR_HAL_LM3S_QEI_CHAN


//=============================================================================
// Flash controller

#define CYGHWR_HAL_LM3S_FMC_FMA                   0x000
#define CYGHWR_HAL_LM3S_FMC_FMD                   0x004
#define CYGHWR_HAL_LM3S_FMC_FMC                   0x008
#define CYGHWR_HAL_LM3S_FMC_FCRIS                 0x00c
#define CYGHWR_HAL_LM3S_FMC_FCIM                  0x010
#define CYGHWR_HAL_LM3S_FMC_FCMISC                0x014

// Key value
#define CYGHWR_HAL_LM3S_FMC_WRKEY                 0xA4420000

// FMC bits
#define CYGHWR_HAL_LM3S_FMC_FMC_WRITE             BIT_(0)
#define CYGHWR_HAL_LM3S_FMC_FMC_ERASE             BIT_(1)
#define CYGHWR_HAL_LM3S_FMC_FMC_MERASE            BIT_(2)
#define CYGHWR_HAL_LM3S_FMC_FMC_COMT              BIT_(3)

// RIS bits
#define CYGHWR_HAL_LM3S_FMC_FCRIS_ARIS            BIT_(0)
#define CYGHWR_HAL_LM3S_FMC_FCRIS_PRIS            BIT_(1)

// IM bits
#define CYGHWR_HAL_LM3S_FMC_FCIM_AMASK            BIT_(0)
#define CYGHWR_HAL_LM3S_FMC_FCIM_PMASK            BIT_(1)

// MISC bits
#define CYGHWR_HAL_LM3S_FMC_FCMISC_AMASK          BIT_(0)
#define CYGHWR_HAL_LM3S_FMC_FCMISC_PMASK          BIT_(0)


//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_IO_H
// EOF of var_io.h
