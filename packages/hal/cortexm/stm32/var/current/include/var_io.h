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
// Author(s):   nickg
// Date:        2008-07-30
// Purpose:     STM32 variant specific registers
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_cortexm_stm32.h>

#include <cyg/hal/plf_io.h>

//=============================================================================
// Peripherals

#define CYGHWR_HAL_STM32_TIM2           0x40000000
#define CYGHWR_HAL_STM32_TIM3           0x40000400
#define CYGHWR_HAL_STM32_TIM4           0x40000800
#define CYGHWR_HAL_STM32_TIM5           0x40000C00
#define CYGHWR_HAL_STM32_TIM6           0x40001000
#define CYGHWR_HAL_STM32_TIM7           0x40001400
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_TIM12          0x40001800
#define CYGHWR_HAL_STM32_TIM13          0x40001C00
#define CYGHWR_HAL_STM32_TIM14          0x40002000
#endif
#define CYGHWR_HAL_STM32_RTC            0x40002800
#define CYGHWR_HAL_STM32_WWDG           0x40002C00
#define CYGHWR_HAL_STM32_IWDG           0x40003000
#define CYGHWR_HAL_STM32_SPI2           0x40003800
#define CYGHWR_HAL_STM32_SPI3           0x40003C00
#define CYGHWR_HAL_STM32_UART2          0x40004400
#define CYGHWR_HAL_STM32_UART3          0x40004800
#define CYGHWR_HAL_STM32_UART4          0x40004C00
#define CYGHWR_HAL_STM32_UART5          0x40005000
#define CYGHWR_HAL_STM32_I2C1           0x40005400
#define CYGHWR_HAL_STM32_I2C2           0x40005800
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_USB            0x40005C00
#define CYGHWR_HAL_STM32_USB_CAN_SRAM   0x40006000
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_I2C3           0x40005C00
#endif
#define CYGHWR_HAL_STM32_BXCAN1         0x40006400
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_BXCAN2         0x40006800
#endif
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_BKP            0x40006C00
#endif
#define CYGHWR_HAL_STM32_PWR            0x40007000
#define CYGHWR_HAL_STM32_DAC            0x40007400
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_AFIO           0x40010000
#define CYGHWR_HAL_STM32_EXTI           0x40010400
#define CYGHWR_HAL_STM32_GPIOA          0x40010800
#define CYGHWR_HAL_STM32_GPIOB          0x40010C00
#define CYGHWR_HAL_STM32_GPIOC          0x40011000
#define CYGHWR_HAL_STM32_GPIOD          0x40011400
#define CYGHWR_HAL_STM32_GPIOE          0x40011800
#define CYGHWR_HAL_STM32_GPIOF          0x40011C00
#define CYGHWR_HAL_STM32_GPIOG          0x40012000
#define CYGHWR_HAL_STM32_ADC1           0x40012400
#define CYGHWR_HAL_STM32_ADC2           0x40012800
#define CYGHWR_HAL_STM32_TIM1           0x40012C00
#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_TIM1           0x40010000
#define CYGHWR_HAL_STM32_PWM1           0x40010000
#define CYGHWR_HAL_STM32_TIM8           0x40010400
#define CYGHWR_HAL_STM32_PWM2           0x40010400
#define CYGHWR_HAL_STM32_UART1          0x40011000
#define CYGHWR_HAL_STM32_UART6          0x40014000
#define CYGHWR_HAL_STM32_ADC1           0x40012000
#define CYGHWR_HAL_STM32_ADC2           CYGHWR_HAL_STM32_ADC1 + 0x0100
#define CYGHWR_HAL_STM32_ADC3           CYGHWR_HAL_STM32_ADC1 + 0x0200
#define CYGHWR_HAL_STM32_ADC_COMMON     CYGHWR_HAL_STM32_ADC1 + 0x0300
#define CYGHWR_HAL_STM32_SDIO           0x40012C00
#endif
#define CYGHWR_HAL_STM32_SPI1           0x40013000
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_TIM8           0x40013400
#define CYGHWR_HAL_STM32_UART1          0x40013800
#define CYGHWR_HAL_STM32_ADC3           0x40013C00
#define CYGHWR_HAL_STM32_SDIO           0x40018000
#define CYGHWR_HAL_STM32_DMA1           0x40020000
#define CYGHWR_HAL_STM32_DMA2           0x40020400
#define CYGHWR_HAL_STM32_RCC            0x40021000
#define CYGHWR_HAL_STM32_FLASH          0x40022000
#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_SYSCFG         0x40013800
#define CYGHWR_HAL_STM32_EXTI           0x40013C00
#define CYGHWR_HAL_STM32_TIM9           0x40014000
#define CYGHWR_HAL_STM32_TIM10          0x40014400
#define CYGHWR_HAL_STM32_TIM11          0x40014800
#define CYGHWR_HAL_STM32_GPIOA          0x40020000
#define CYGHWR_HAL_STM32_GPIOB          0x40020400
#define CYGHWR_HAL_STM32_GPIOC          0x40020800
#define CYGHWR_HAL_STM32_GPIOD          0x40020C00
#define CYGHWR_HAL_STM32_GPIOE          0x40021000
#define CYGHWR_HAL_STM32_GPIOF          0x40021400
#define CYGHWR_HAL_STM32_GPIOG          0x40021800
#define CYGHWR_HAL_STM32_GPIOH          0x40021C00
#define CYGHWR_HAL_STM32_GPIOI          0x40022000
#endif
#define CYGHWR_HAL_STM32_CRC            0x40023000
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC            0x40023800
#define CYGHWR_HAL_STM32_FLASH          0x40023C00
#define CYGHWR_HAL_STM32_BKPSRAM        0x40024000
#define CYGHWR_HAL_STM32_DMA1           0x40026000
#define CYGHWR_HAL_STM32_DMA2           0x40026400
#endif
#define CYGHWR_HAL_STM32_ETH            0x40028000
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_USB_OTG_HS     0x40040000
#endif
#define CYGHWR_HAL_STM32_USB_OTG_FS     0x50000000
#define CYGHWR_HAL_STM32_OTG            CYGHWR_HAL_STM32_USB_OTG_FS // compatibility define. Deprecated.
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_DCMI           0x50050000
#define CYGHWR_HAL_STM32_CRYP           0x50060000
#define CYGHWR_HAL_STM32_HASH           0x50060400
#define CYGHWR_HAL_STM32_RNG            0x50060800
#endif
#define CYGHWR_HAL_STM32_FSMC           0xA0000000

//=============================================================================
// Device signature and ID registers

#define CYGHWR_HAL_STM32_DEV_SIG                0x1FFFF7E0
#define CYGHWR_HAL_STM32_DEV_SIG_RSIZE(__s)     (((__s)>>16)&0xFFFF)
#define CYGHWR_HAL_STM32_DEV_SIG_FSIZE(__s)     ((__s)&0xFFFF)

#define CYGHWR_HAL_STM32_MCU_ID                 0xe0042000
#define CYGHWR_HAL_STM32_MCU_ID_DEV(__x)        ((__x)&0xFFF)
#define CYGHWR_HAL_STM32_MCU_ID_DEV_MEDIUM      0x410
#define CYGHWR_HAL_STM32_MCU_ID_DEV_HIGH        0x414
#define CYGHWR_HAL_STM32_MCU_ID_REV(__x)        (((__x)>>16)&0xFFFF)

//=============================================================================
// RCC
//
// Not all registers are described here

#define CYGHWR_HAL_STM32_RCC_CR                 0x00
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_RCC_CFGR               0x04
#define CYGHWR_HAL_STM32_RCC_CIR                0x08
#define CYGHWR_HAL_STM32_RCC_APB2RSTR           0x0C
#define CYGHWR_HAL_STM32_RCC_APB1RSTR           0x10
#define CYGHWR_HAL_STM32_RCC_AHBENR             0x14
#define CYGHWR_HAL_STM32_RCC_APB2ENR            0x18
#define CYGHWR_HAL_STM32_RCC_APB1ENR            0x1C
#define CYGHWR_HAL_STM32_RCC_BDCR               0x20
#define CYGHWR_HAL_STM32_RCC_CSR                0x24
# ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_RCC_AHBRSTR            0x28
#define CYGHWR_HAL_STM32_RCC_CFGR2              0x2C
# endif
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR            0x04
#define CYGHWR_HAL_STM32_RCC_CFGR               0x08
#define CYGHWR_HAL_STM32_RCC_CIR                0x0C
#define CYGHWR_HAL_STM32_RCC_AHB1RSTR           0x10
#define CYGHWR_HAL_STM32_RCC_AHB2RSTR           0x14
#define CYGHWR_HAL_STM32_RCC_AHB3RSTR           0x18
#define CYGHWR_HAL_STM32_RCC_APB1RSTR           0x20
#define CYGHWR_HAL_STM32_RCC_APB2RSTR           0x24
#define CYGHWR_HAL_STM32_RCC_AHB1ENR            0x30
#define CYGHWR_HAL_STM32_RCC_AHB2ENR            0x34
#define CYGHWR_HAL_STM32_RCC_AHB3ENR            0x38
#define CYGHWR_HAL_STM32_RCC_APB1ENR            0x40
#define CYGHWR_HAL_STM32_RCC_APB2ENR            0x44
#define CYGHWR_HAL_STM32_RCC_AHB1LPENR          0x50
#define CYGHWR_HAL_STM32_RCC_AHB2LPENR          0x54
#define CYGHWR_HAL_STM32_RCC_AHB3LPENR          0x58
#define CYGHWR_HAL_STM32_RCC_APB1LPENR          0x60
#define CYGHWR_HAL_STM32_RCC_APB2LPENR          0x64
#define CYGHWR_HAL_STM32_RCC_BDCR               0x70
#define CYGHWR_HAL_STM32_RCC_CSR                0x74
#endif

#define CYGHWR_HAL_STM32_RCC_CR_HSION           BIT_(0)
#define CYGHWR_HAL_STM32_RCC_CR_HSIRDY          BIT_(1)
#define CYGHWR_HAL_STM32_RCC_CR_HSITRIM         MASK_(3,5)
#define CYGHWR_HAL_STM32_RCC_CR_HSICAL          MASK_(8,8)
#define CYGHWR_HAL_STM32_RCC_CR_HSEON           BIT_(16)
#define CYGHWR_HAL_STM32_RCC_CR_HSERDY          BIT_(17)
#define CYGHWR_HAL_STM32_RCC_CR_HSEBYP          BIT_(18)
#define CYGHWR_HAL_STM32_RCC_CR_CSSON           BIT_(19)
#define CYGHWR_HAL_STM32_RCC_CR_PLLON           BIT_(24)
#define CYGHWR_HAL_STM32_RCC_CR_PLLRDY          BIT_(25)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC_CR_PLLI2SON        BIT_(26)
#define CYGHWR_HAL_STM32_RCC_CR_PLLI2SRDY       BIT_(27)
#endif

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLM(__m)  VALUE_(0,__m)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLM_MASK  MASK_(0,6)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLN(__n)  VALUE_(6,__n)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLN_MASK  MASK_(6,9)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP(__p)  VALUE_(16,((__p)>>1)-1 )
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP_2     VALUE_(16,0)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP_4     VALUE_(16,1)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP_6     VALUE_(16,2)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLP_8     VALUE_(16,3)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLSRC_HSI VALUE_(22,0)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLSRC_HSE VALUE_(22,1)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLQ(__q)  VALUE_(24,__q)
#define CYGHWR_HAL_STM32_RCC_PLLCFGR_PLLQ_MASK  MASK_(24,4)
#endif

#define CYGHWR_HAL_STM32_RCC_CFGR_SW_HSI        VALUE_(0,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_SW_HSE        VALUE_(0,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_SW_PLL        VALUE_(0,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_SW_XXX        VALUE_(0,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_SWS_HSI       VALUE_(2,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_SWS_HSE       VALUE_(2,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_SWS_PLL       VALUE_(2,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_SWS_XXX       VALUE_(2,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_1        VALUE_(4,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_2        VALUE_(4,8)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_4        VALUE_(4,9)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_8        VALUE_(4,10)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_16       VALUE_(4,11)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_64       VALUE_(4,12)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_128      VALUE_(4,13)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_256      VALUE_(4,14)
#define CYGHWR_HAL_STM32_RCC_CFGR_HPRE_512      VALUE_(4,15)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_1       VALUE_(8,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_2       VALUE_(8,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_4       VALUE_(8,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_8       VALUE_(8,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_16      VALUE_(8,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_1       VALUE_(11,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_2       VALUE_(11,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_4       VALUE_(11,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_8       VALUE_(11,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_16      VALUE_(11,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_2      VALUE_(14,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_4      VALUE_(14,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_6      VALUE_(14,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_8      VALUE_(14,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_ADCPRE_XXX    VALUE_(14,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_PLLSRC_HSI    0
#define CYGHWR_HAL_STM32_RCC_CFGR_PLLSRC_HSE    BIT_(16)
#define CYGHWR_HAL_STM32_RCC_CFGR_PLLXTPRE      BIT_(17)
#define CYGHWR_HAL_STM32_RCC_CFGR_PLLMUL(__x)   VALUE_(18,(__x)-2)
#define CYGHWR_HAL_STM32_RCC_CFGR_USBPRE        BIT_(22)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_NONE      VALUE_(24,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_SYSCLK    VALUE_(24,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_HSI       VALUE_(24,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_HSE       VALUE_(24,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_PLL       VALUE_(24,7)
# ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_PLL2      VALUE_(24,8)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_PLL3_HALF VALUE_(24,9)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_XT1       VALUE_(24,10)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO_PLL3      VALUE_(24,11)
# endif
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_1       VALUE_(10,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_2       VALUE_(10,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_4       VALUE_(10,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_8       VALUE_(10,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE1_16      VALUE_(10,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_1       VALUE_(13,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_2       VALUE_(13,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_4       VALUE_(13,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_8       VALUE_(13,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_PPRE2_16      VALUE_(13,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_RTCPRE(__x)   VALUE_(16,__x)
#define CYGHWR_HAL_STM32_RCC_CFGR_RTCPRE_MASK   MASK_(16,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1_MASK     MASK_(21,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1_HSI      VALUE_(21,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1_LSE      VALUE_(21,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1_HSE      VALUE_(21,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1_PLL      VALUE_(21,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_I2SSRC_PLLI2S VALUE_(23,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_I2SSRC_EXT    VALUE_(23,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_MASK  MASK_(24,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_1     VALUE_(24,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_2     VALUE_(24,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_3     VALUE_(24,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_4     VALUE_(24,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_5     VALUE_(24,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_MASK  MASK_(27,3)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_1     VALUE_(27,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_2     VALUE_(27,4)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_3     VALUE_(27,5)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_4     VALUE_(27,6)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2PRE_5     VALUE_(27,7)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2_MASK     MASK_(30,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2_SYSCLK   VALUE_(30,0)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2_PLLI2S   VALUE_(30,1)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2_HSE      VALUE_(30,2)
#define CYGHWR_HAL_STM32_RCC_CFGR_MCO2_PLL      VALUE_(30,3)
#endif

#define CYGHWR_HAL_STM32_RCC_AHBENR_DMA1        (0)
#define CYGHWR_HAL_STM32_RCC_AHBENR_DMA2        (1)
#define CYGHWR_HAL_STM32_RCC_AHBENR_SRAM        (2)
#define CYGHWR_HAL_STM32_RCC_AHBENR_FLITF       (4)
#define CYGHWR_HAL_STM32_RCC_AHBENR_CRC         (6)
#define CYGHWR_HAL_STM32_RCC_AHBENR_FSMC        (8)
#define CYGHWR_HAL_STM32_RCC_AHBENR_SDIO        (10)
#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_RCC_AHBENR_OTGFS       (12)
#define CYGHWR_HAL_STM32_RCC_AHBENR_ETHMAC      (14)
#define CYGHWR_HAL_STM32_RCC_AHBENR_ETHMACTX    (15)
#define CYGHWR_HAL_STM32_RCC_AHBENR_ETHMACRX    (16)
#endif

// Note that the following are bit numbers, not masks. They should
// either be used with the CYGHWR_HAL_STM32_CLOCK() macro or used to
// shift a 1, perhaps using the BIT_() macro.
//
// Note that in the F2/F4 families, the bit positions in the LP registers are
// the same.

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_RCC_APB2ENR_AFIO       (0)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPA       (2)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPB       (3)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPC       (4)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPD       (5)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPE       (6)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPF       (7)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPG       (8)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC1       (9)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC2       (10)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM1       (11)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_SPI1       (12)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM8       (13)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_UART1      (14)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC3       (15)


#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM2       (0)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM3       (1)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM4       (2)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM5       (3)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM6       (4)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM7       (5)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_WWDG       (11)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI2       (14)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI3       (15)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART2      (17)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART3      (18)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART4      (19)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART5      (20)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C1       (21)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C2       (22)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_USB        (23)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_CAN1       (25)
#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_RCC_APB1ENR_CAN2       (26)
#endif
#define CYGHWR_HAL_STM32_RCC_APB1ENR_BKP        (27)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_PWR        (28)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_DAC        (29)

#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_RCC_CFGR2_PREDIV1(__x)     VALUE_(0,__x)
#define CYGHWR_HAL_STM32_RCC_CFGR2_PREDIV2(__x)     VALUE_(4,__x)
#define CYGHWR_HAL_STM32_RCC_CFGR2_PLL2MUL(__x)     VALUE_(8,__x)
#define CYGHWR_HAL_STM32_RCC_CFGR2_PLL3MUL(__x)     VALUE_(12,__x)
#define CYGHWR_HAL_STM32_RCC_CFGR2_PREDIV1SRC_HSE   0
#define CYGHWR_HAL_STM32_RCC_CFGR2_PREDIV1SRC_PLL2  BIT_(16)
#define CYGHWR_HAL_STM32_RCC_CFGR2_I2S2SRC_SYSCLK   0
#define CYGHWR_HAL_STM32_RCC_CFGR2_I2S2SRC_PLL3     BIT_(17)
#define CYGHWR_HAL_STM32_RCC_CFGR2_I2S3SRC_SYSCLK   0
#define CYGHWR_HAL_STM32_RCC_CFGR2_I2S3SRC_PLL3     BIT_(18)
#endif

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOA       (0)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOB       (1)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOC       (2)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOD       (3)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOE       (4)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOF       (5)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOG       (6)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOH       (7)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_GPIOI       (8)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_CRC        (12)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_BKPSRAM    (18)
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_CCMDATARAMEN (20)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_DMA1       (21)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_DMA2       (22)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_ETHMAC     (25)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_ETHMACTX   (26)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_ETHMACRX   (27)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_ETHMACPTP  (28)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_OTGHS      (29)
#define CYGHWR_HAL_STM32_RCC_AHB1ENR_OTGHSULPI  (30)

#define CYGHWR_HAL_STM32_RCC_AHB2ENR_DCMI        (0)
#define CYGHWR_HAL_STM32_RCC_AHB2ENR_CRYP        (4)
#define CYGHWR_HAL_STM32_RCC_AHB2ENR_HASH        (5)
#define CYGHWR_HAL_STM32_RCC_AHB2ENR_RNG         (6)
#define CYGHWR_HAL_STM32_RCC_AHB2ENR_OTGFS       (7)

#define CYGHWR_HAL_STM32_RCC_AHB3ENR_FSMC        (0)

#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM2        (0)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM3        (1)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM4        (2)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM5        (3)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM6        (4)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM7        (5)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM12       (6)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM13       (7)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM14       (8)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_WWDG       (11)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI2       (14)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI3       (15)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART2      (17)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART3      (18)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART4      (19)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART5      (20)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C1       (21)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C2       (22)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C3       (23)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_CAN1       (25)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_CAN2       (26)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_PWR        (28)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_DAC        (29)

#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM1        (0)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM8        (1)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_UART1       (4)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_UART6       (5)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC1        (8)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC2        (9)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC3       (10)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_SDIO       (11)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_SPI1       (12)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_SYSCFG     (14)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM9       (16)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM10      (17)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM11      (18)

#endif

// The following encodes the control register and clock bit number
// into a 32 bit descriptor.
#define CYGHWR_HAL_STM32_CLOCK( __reg, __pin )                  \
            ((CYGHWR_HAL_STM32_RCC_##__reg##ENR) |              \
             ((CYGHWR_HAL_STM32_RCC_##__reg##ENR_##__pin)<<16))

// Macros to extract encoded values.
#define CYGHWR_HAL_STM32_CLOCK_REG( __desc ) ((__desc)&0xFF)
#define CYGHWR_HAL_STM32_CLOCK_PIN( __desc ) (((__desc)>>16)&0xFF)

// Functions and macros to enable/disable clocks.

__externC void hal_stm32_clock_enable( cyg_uint32 desc );
__externC void hal_stm32_clock_disable( cyg_uint32 desc );

#define CYGHWR_HAL_STM32_CLOCK_ENABLE( __desc ) hal_stm32_clock_enable( __desc )
#define CYGHWR_HAL_STM32_CLOCK_DISABLE( __desc ) hal_stm32_clock_disable( __desc )


#define CYGHWR_HAL_STM32_RCC_BDCR_LSEON         BIT_(0)
#define CYGHWR_HAL_STM32_RCC_BDCR_LSERDY        BIT_(1)
#define CYGHWR_HAL_STM32_RCC_BDCR_LSEBYP        BIT_(2)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_NO     VALUE_(8,0)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_LSE    VALUE_(8,1)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_LSI    VALUE_(8,2)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_HSE    VALUE_(8,3)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCSEL_XXX    VALUE_(8,3)
#define CYGHWR_HAL_STM32_RCC_BDCR_RTCEN         BIT_(15)
#define CYGHWR_HAL_STM32_RCC_BDCR_BDRST         BIT_(16)

#define CYGHWR_HAL_STM32_RCC_CSR_LSION          BIT_(0)
#define CYGHWR_HAL_STM32_RCC_CSR_LSIRDY         BIT_(1)
#define CYGHWR_HAL_STM32_RCC_CSR_RMVF           BIT_(24)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_RCC_CSR_BORRSTF        BIT_(25)
#endif
#define CYGHWR_HAL_STM32_RCC_CSR_PINRSTF        BIT_(26)
#define CYGHWR_HAL_STM32_RCC_CSR_PORRSTF        BIT_(27)
#define CYGHWR_HAL_STM32_RCC_CSR_SFTRSTF        BIT_(28)
#define CYGHWR_HAL_STM32_RCC_CSR_IWDGRSTF       BIT_(29)
#define CYGHWR_HAL_STM32_RCC_CSR_WWDGRSTF       BIT_(30)
#define CYGHWR_HAL_STM32_RCC_CSR_LPWRRSTF       BIT_(31)


// Miscellaneous clock control bits

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_CLOCK_BKP              CYGHWR_HAL_STM32_CLOCK( APB1, BKP )
#endif

#define CYGHWR_HAL_STM32_CLOCK_PWR              CYGHWR_HAL_STM32_CLOCK( APB1, PWR )

//=============================================================================
// Realtime Clock

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_RTC_CRH                0x00
#define CYGHWR_HAL_STM32_RTC_CRL                0x04
#define CYGHWR_HAL_STM32_RTC_PRLH               0x08
#define CYGHWR_HAL_STM32_RTC_PRLL               0x0C
#define CYGHWR_HAL_STM32_RTC_DIVH               0x10
#define CYGHWR_HAL_STM32_RTC_DIVL               0x14
#define CYGHWR_HAL_STM32_RTC_CNTH               0x18
#define CYGHWR_HAL_STM32_RTC_CNTL               0x1C
#define CYGHWR_HAL_STM32_RTC_ALRH               0x20
#define CYGHWR_HAL_STM32_RTC_ALRL               0x24

// CRH fields

#define CYGHWR_HAL_STM32_RTC_CRH_SECIE          BIT_(0)
#define CYGHWR_HAL_STM32_RTC_CRH_ALRIE          BIT_(1)
#define CYGHWR_HAL_STM32_RTC_CRH_OWIE           BIT_(2)

// CRL fields

#define CYGHWR_HAL_STM32_RTC_CRL_SECF           BIT_(0)
#define CYGHWR_HAL_STM32_RTC_CRL_ALRF           BIT_(1)
#define CYGHWR_HAL_STM32_RTC_CRL_OWF            BIT_(2)
#define CYGHWR_HAL_STM32_RTC_CRL_RSF            BIT_(3)
#define CYGHWR_HAL_STM32_RTC_CRL_CNF            BIT_(4)
#define CYGHWR_HAL_STM32_RTC_CRL_RTOFF          BIT_(5)

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_RTC_TR                 0x00
#define CYGHWR_HAL_STM32_RTC_DR                 0x04
#define CYGHWR_HAL_STM32_RTC_CR                 0x08
#define CYGHWR_HAL_STM32_RTC_ISR                0x0C
#define CYGHWR_HAL_STM32_RTC_PRER               0x10
#define CYGHWR_HAL_STM32_RTC_WUTR               0x14
#define CYGHWR_HAL_STM32_RTC_CALIBR             0x18
#define CYGHWR_HAL_STM32_RTC_ALRMAR             0x1C
#define CYGHWR_HAL_STM32_RTC_ALRMBR             0x20
#define CYGHWR_HAL_STM32_RTC_WPR                0x24
#define CYGHWR_HAL_STM32_RTC_TSTR               0x30
#define CYGHWR_HAL_STM32_RTC_TSDR               0x34
#define CYGHWR_HAL_STM32_RTC_TAFCR              0x40
#define CYGHWR_HAL_STM32_RTC_BKxR(_x)           (0x50 + 4*(_x))

#define CYGHWR_HAL_STM32_RTC_TR_SU              MASK_(0,4)
#define CYGHWR_HAL_STM32_RTC_TR_SU_SHIFT        0
#define CYGHWR_HAL_STM32_RTC_TR_ST              MASK_(4,3)
#define CYGHWR_HAL_STM32_RTC_TR_ST_SHIFT        4
#define CYGHWR_HAL_STM32_RTC_TR_MNU             MASK_(8,4)
#define CYGHWR_HAL_STM32_RTC_TR_MNU_SHIFT       8
#define CYGHWR_HAL_STM32_RTC_TR_MNT             MASK_(12,3)
#define CYGHWR_HAL_STM32_RTC_TR_MNT_SHIFT       12
#define CYGHWR_HAL_STM32_RTC_TR_HU              MASK_(16,4)
#define CYGHWR_HAL_STM32_RTC_TR_HU_SHIFT        16
#define CYGHWR_HAL_STM32_RTC_TR_HT              MASK_(20,2)
#define CYGHWR_HAL_STM32_RTC_TR_HT_SHIFT        20
#define CYGHWR_HAL_STM32_RTC_TR_AM              VALUE_(22, 0)
#define CYGHWR_HAL_STM32_RTC_TR_PM              VALUE_(22, 1)

#define CYGHWR_HAL_STM32_RTC_DR_DU              MASK_(0,4)
#define CYGHWR_HAL_STM32_RTC_DR_DU_SHIFT        0
#define CYGHWR_HAL_STM32_RTC_DR_DT              MASK_(4,2)
#define CYGHWR_HAL_STM32_RTC_DR_DT_SHIFT        4
#define CYGHWR_HAL_STM32_RTC_DR_MU              MASK_(8,4)
#define CYGHWR_HAL_STM32_RTC_DR_MU_SHIFT        8
#define CYGHWR_HAL_STM32_RTC_DR_MT              BIT_(12)
#define CYGHWR_HAL_STM32_RTC_DR_MT_SHIFT        12
#define CYGHWR_HAL_STM32_RTC_DR_WDU_MON         VALUE_(13,1)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_TUE         VALUE_(13,2)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_WED         VALUE_(13,3)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_THU         VALUE_(13,4)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_FRI         VALUE_(13,5)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_SAT         VALUE_(13,6)
#define CYGHWR_HAL_STM32_RTC_DR_WDU_SUN         VALUE_(13,7)
#define CYGHWR_HAL_STM32_RTC_DR_YU              MASK_(16,4)
#define CYGHWR_HAL_STM32_RTC_DR_YU_SHIFT        16
#define CYGHWR_HAL_STM32_RTC_DR_YT              MASK_(20,4)
#define CYGHWR_HAL_STM32_RTC_DR_YT_SHIFT        20

#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_16      VALUE_(0,0)
#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_8       VALUE_(0,1)
#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_4       VALUE_(0,2)
#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_2       VALUE_(0,3)
#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_CK_SPRE VALUE_(0,4)
#define CYGHWR_HAL_STM32_RTC_CR_WUCKSEL_CK_SPRE_PLUS_216 VALUE_(0,6)
#define CYGHWR_HAL_STM32_RTC_CR_TSEDGE          BIT_(3)
#define CYGHWR_HAL_STM32_RTC_CR_REFCKON         BIT_(4)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_RTC_CR_BYPSHAD         BIT_(5)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
#define CYGHWR_HAL_STM32_RTC_CR_FMT             BIT_(6)
#define CYGHWR_HAL_STM32_RTC_CR_DCE             BIT_(7)
#define CYGHWR_HAL_STM32_RTC_CR_ALRAE           BIT_(8)
#define CYGHWR_HAL_STM32_RTC_CR_ALRBE           BIT_(9)
#define CYGHWR_HAL_STM32_RTC_CR_WUTE            BIT_(10)
#define CYGHWR_HAL_STM32_RTC_CR_TSE             BIT_(11)
#define CYGHWR_HAL_STM32_RTC_CR_ALRAIE          BIT_(12)
#define CYGHWR_HAL_STM32_RTC_CR_ALRBIE          BIT_(13)
#define CYGHWR_HAL_STM32_RTC_CR_WUTIE           BIT_(14)
#define CYGHWR_HAL_STM32_RTC_CR_TSIE            BIT_(15)
#define CYGHWR_HAL_STM32_RTC_CR_ADD1H           BIT_(16)
#define CYGHWR_HAL_STM32_RTC_CR_SUB1H           BIT_(17)
#define CYGHWR_HAL_STM32_RTC_CR_BKP             BIT_(18)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_RTC_CR_COSEL           BIT_(19)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
#define CYGHWR_HAL_STM32_RTC_CR_POL             BIT_(20)
#define CYGHWR_HAL_STM32_RTC_CR_OSEL_DISABLE    VALUE_(21, 0)
#define CYGHWR_HAL_STM32_RTC_CR_OSEL_ALRAOE     VALUE_(21, 1)
#define CYGHWR_HAL_STM32_RTC_CR_OSEL_ALRBOE     VALUE_(21, 2)
#define CYGHWR_HAL_STM32_RTC_CR_OSEL_WUOE       VALUE_(21, 3)
#define CYGHWR_HAL_STM32_RTC_CR_OSEL_COE        BIT_(23)

#define CYGHWR_HAL_STM32_RTC_ISR_ALRAWF         BIT_(0)
#define CYGHWR_HAL_STM32_RTC_ISR_ALRBWF         BIT_(1)
#define CYGHWR_HAL_STM32_RTC_ISR_WUTWF          BIT_(2)
#define CYGHWR_HAL_STM32_RTC_ISR_INITS          BIT_(4)
#define CYGHWR_HAL_STM32_RTC_ISR_RSF            BIT_(5)
#define CYGHWR_HAL_STM32_RTC_ISR_INITF          BIT_(6)
#define CYGHWR_HAL_STM32_RTC_ISR_INIT           BIT_(7)
#define CYGHWR_HAL_STM32_RTC_ISR_ALRAF          BIT_(8)
#define CYGHWR_HAL_STM32_RTC_ISR_ALRBF          BIT_(9)
#define CYGHWR_HAL_STM32_RTC_ISR_WUTF           BIT_(10)
#define CYGHWR_HAL_STM32_RTC_ISR_TSF            BIT_(11)
#define CYGHWR_HAL_STM32_RTC_ISR_TSOVF          BIT_(12)
#define CYGHWR_HAL_STM32_RTC_ISR_TAMP1F         BIT_(13)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F2)
#define CYGHWR_HAL_STM32_RTC_PRER_PREDIV_S      MASK_(0,13)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_RTC_ISR_SHPF           BIT_(3)
#define CYGHWR_HAL_STM32_RTC_ISR_TAMP2F         BIT_(14)
#define CYGHWR_HAL_STM32_RTC_ISR_RECALPF        BIT_(16)
#define CYGHWR_HAL_STM32_RTC_PRER_PREDIV_S      MASK_(0,15)
#endif
#define CYGHWR_HAL_STM32_RTC_PRER_PREDIV_A      MASK_(16,7)

// RTC_WUTR defines omitted
// RTC_CALIBR defines omitted
// RTC_ALRMAR defines omitted
// RTC_ALRMBR defines omitted

#define CYGHWR_HAL_STM32_RTC_WPR_KEY            MASK_(0,8)
#define CYGHWR_HAL_STM32_RTC_WPR_KEY1           0xCA
#define CYGHWR_HAL_STM32_RTC_WPR_KEY2           0x53

// RTC_TSTR defines omitted, but layout identical to RTC_TR
// RTC_TSDR defines omitted, but layout identical to RTC_DR except for omission of year fields
// RTC_TAFCR defines omitted
// No relevant RTC_BKPxR defines.

// RCC clock is selected within wallclock driver, so no define here.

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)


//=============================================================================
// System configuration controller - F2 and F4 only

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
// Register offsets
#define CYGHWR_HAL_STM32_SYSCFG_MEMRMP          0x00
#define CYGHWR_HAL_STM32_SYSCFG_PMC             0x04
#define CYGHWR_HAL_STM32_SYSCFG_EXTICR1         0x08
#define CYGHWR_HAL_STM32_SYSCFG_EXTICR2         0x0C
#define CYGHWR_HAL_STM32_SYSCFG_EXTICR3         0x10
#define CYGHWR_HAL_STM32_SYSCFG_EXTICR4         0x14
#define CYGHWR_HAL_STM32_SYSCFG_CMPCR           0x20

// Register definitions

#define CYGHWR_HAL_STM32_SYSCFG_MEMRMP_MAINFLASH VALUE_(0,0)
#define CYGHWR_HAL_STM32_SYSCFG_MEMRMP_SYSFLASH  VALUE_(0,1)
#define CYGHWR_HAL_STM32_SYSCFG_MEMRMP_FSMC1     VALUE_(0,2)
#define CYGHWR_HAL_STM32_SYSCFG_MEMRMP_SRAM      VALUE_(0,3)

#define CYGHWR_HAL_STM32_SYSCFG_PMC_MII          VALUE_(23,0)
#define CYGHWR_HAL_STM32_SYSCFG_PMC_RMII         VALUE_(23,1)

// FIXME: The below EXTI bits should be merged with the F1 defines in
// var_io_pins.h to provide a common interface

// The following macro allows the four EXTICR registers to be indexed
// as CYGHWR_HAL_STM32_SYSCFG_EXTICR(1) to CYGHWR_HAL_STM32_SYSCFG_EXTICR(4)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICR(__x)      (4*((__x)-1)+0x08)

// The following macros are used to generate the bitfields for setting up
// external interrupts.  For example, CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTC(12)
// will generate the bitfield which when ORed into the EXTICR4 register will
// set up C12 as the external interrupt pin for the EXTI12 interrupt.
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTA(__x) VALUE_(4*((__x)&3),0)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTB(__x) VALUE_(4*((__x)&3),1)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTC(__x) VALUE_(4*((__x)&3),2)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTD(__x) VALUE_(4*((__x)&3),3)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTE(__x) VALUE_(4*((__x)&3),4)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTF(__x) VALUE_(4*((__x)&3),5)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTG(__x) VALUE_(4*((__x)&3),6)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_PORTH(__x) VALUE_(4*((__x)&3),7)
#define CYGHWR_HAL_STM32_SYSCFG_EXTICRX_MASK(__x)  VALUE_(4*((__x)&3),0xF)

#define CYGHWR_HAL_STM32_SYSCFG_CMPCR_CMP_DIS      VALUE_(0,0)
#define CYGHWR_HAL_STM32_SYSCFG_CMPCR_CMP_ENA      VALUE_(0,1)
#define CYGHWR_HAL_STM32_SYSCFG_CMPCR_READY        BIT_(8)

// SYSCFG clock control

#define CYGHWR_HAL_STM32_SYSCFG_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB2, SYSCFG )

#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)


//=============================================================================
// External interrupt controller

#define CYGHWR_HAL_STM32_EXTI_IMR               0x00
#define CYGHWR_HAL_STM32_EXTI_EMR               0x04
#define CYGHWR_HAL_STM32_EXTI_RTSR              0x08
#define CYGHWR_HAL_STM32_EXTI_FTSR              0x0C
#define CYGHWR_HAL_STM32_EXTI_SWIER             0x10
#define CYGHWR_HAL_STM32_EXTI_PR                0x14

#define CYGHWR_HAL_STM32_EXTI_BIT(__b)          BIT_(__b)

//=============================================================================
// GPIO ports and pin configuration. Include separate header file for this
// to avoid this header getting unmanageable.
#include <cyg/hal/var_io_pins.h>

//=============================================================================
// DMA controller register definitions.

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_DMA_ISR                0x00
#define CYGHWR_HAL_STM32_DMA_IFCR               0x04

#define CYGHWR_HAL_STM32_DMA_ISR_REG(__chan)    CYGHWR_HAL_STM32_DMA_ISR
#define CYGHWR_HAL_STM32_DMA_IFCR_REG(__chan)   CYGHWR_HAL_STM32_DMA_IFCR

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_DMA_LISR               0x00
#define CYGHWR_HAL_STM32_DMA_HISR               0x04
#define CYGHWR_HAL_STM32_DMA_LIFCR              0x08
#define CYGHWR_HAL_STM32_DMA_HIFCR              0x0C

#define CYGHWR_HAL_STM32_DMA_ISR_REG(__stream)  ((__stream)>3 ?               \
                                                 CYGHWR_HAL_STM32_DMA_HISR :  \
                                                 CYGHWR_HAL_STM32_DMA_LISR)
#define CYGHWR_HAL_STM32_DMA_IFCR_REG(__stream) ((__stream)>3 ?               \
                                                 CYGHWR_HAL_STM32_DMA_HIFCR : \
                                                 CYGHWR_HAL_STM32_DMA_LIFCR)

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)


// The following macros allow access to the per-channel DMA registers, indexed
// by channel number. For F1 parts there is no concept of a stream - each entry
// has a fixed relationship to the corresponding channel.
// Valid channel/stream numbers are 1 to 7 for DMA1 and 1 to 5 for DMA2 on F1
// parts, 1 to 7 for DMA2 streams on F2/F4 parts.
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_DMA_CCR(__x)           (0x14*(__x)-0x0C)
#define CYGHWR_HAL_STM32_DMA_CNDTR(__x)         (0x14*(__x)-0x08)
#define CYGHWR_HAL_STM32_DMA_CPAR(__x)          (0x14*(__x)-0x04)
#define CYGHWR_HAL_STM32_DMA_CMAR(__x)          (0x14*(__x))

#define CYGHWR_HAL_STM32_DMA_ISR_GIF(__x)       BIT_(4*(__x)-4)
#define CYGHWR_HAL_STM32_DMA_ISR_TCIF(__x)      BIT_(4*(__x)-3)
#define CYGHWR_HAL_STM32_DMA_ISR_HTIF(__x)      BIT_(4*(__x)-2)
#define CYGHWR_HAL_STM32_DMA_ISR_TEIF(__x)      BIT_(4*(__x)-1)
#define CYGHWR_HAL_STM32_DMA_ISR_MASK(__x)      VALUE_(4*(__x)-4,0xF)

#define CYGHWR_HAL_STM32_DMA_IFCR_CGIF(__x)     BIT_(4*(__x)-4)
#define CYGHWR_HAL_STM32_DMA_IFCR_CTCIF(__x)    BIT_(4*(__x)-3)
#define CYGHWR_HAL_STM32_DMA_IFCR_CHTIF(__x)    BIT_(4*(__x)-2)
#define CYGHWR_HAL_STM32_DMA_IFCR_CTEIF(__x)    BIT_(4*(__x)-1)
#define CYGHWR_HAL_STM32_DMA_IFCR_MASK(__x)     VALUE_(4*(__x)-4,0xF)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_DMA_SCR(__x)           (0x18*(__x)+0x10)
#define CYGHWR_HAL_STM32_DMA_SNDTR(__x)         (0x18*(__x)+0x14)
#define CYGHWR_HAL_STM32_DMA_SPAR(__x)          (0x18*(__x)+0x18)
#define CYGHWR_HAL_STM32_DMA_SM0AR(__x)         (0x18*(__x)+0x1C)
#define CYGHWR_HAL_STM32_DMA_SM1AR(__x)         (0x18*(__x)+0x20)
#define CYGHWR_HAL_STM32_DMA_SFCR(__x)          (0x18*(__x)+0x24)

// For now at least we implement an identity mapping between
// streams and channels.
#define CYGHWR_HAL_STM32_DMA_CCR(__x)           CYGHWR_HAL_STM32_DMA_SCR(__x)
#define CYGHWR_HAL_STM32_DMA_CNDTR(__x)         CYGHWR_HAL_STM32_DMA_SNDTR(__x)
#define CYGHWR_HAL_STM32_DMA_CPAR(__x)          CYGHWR_HAL_STM32_DMA_SPAR(__x)
#define CYGHWR_HAL_STM32_DMA_CMAR(__x)          CYGHWR_HAL_STM32_DMA_SM0AR(__x)
#define CYGHWR_HAL_STM32_DMA_CM0AR(__x)         CYGHWR_HAL_STM32_DMA_SM0AR(__x)
#define CYGHWR_HAL_STM32_DMA_CM1AR(__x)         CYGHWR_HAL_STM32_DMA_SM1AR(__x)

// This selects which region of an isr register to use for a stream
#define CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x)  ( ((__x)&0x3) == 3 ? 22 : \
                                               ((__x)&0x3) == 2 ? 16 : \
                                               ((__x)&0x3) == 1 ? 6 : 0 )
    
#define CYGHWR_HAL_STM32_DMA_ISR_FEIF(__x)      BIT_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x) )
#define CYGHWR_HAL_STM32_DMA_ISR_DMEIF(__x)     BIT_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x) + 2 )
#define CYGHWR_HAL_STM32_DMA_ISR_TEIF(__x)      BIT_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x) + 3 )
#define CYGHWR_HAL_STM32_DMA_ISR_HTIF(__x)      BIT_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x) + 4 )
#define CYGHWR_HAL_STM32_DMA_ISR_TCIF(__x)      BIT_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x) + 5 )

#define CYGHWR_HAL_STM32_DMA_ISR_MASK(__x)      VALUE_( CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x), 0x3f )

// This selects which region of an ifcr register to use for a stream.
// Happens to be laid out the same as the isr, so reuse.
#define CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x)    CYGHWR_HAL_STM32_DMA_ISR_SHIFT(__x)

#define CYGHWR_HAL_STM32_DMA_IFCR_CFEIF(__x)    BIT_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x) )
#define CYGHWR_HAL_STM32_DMA_IFCR_CDMEIF(__x)   BIT_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x) + 2 )
#define CYGHWR_HAL_STM32_DMA_IFCR_CTEIF(__x)    BIT_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x) + 3 )
#define CYGHWR_HAL_STM32_DMA_IFCR_CHTIF(__x)    BIT_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x) + 4 )
#define CYGHWR_HAL_STM32_DMA_IFCR_CTCIF(__x)    BIT_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x) + 5 )
#define CYGHWR_HAL_STM32_DMA_IFCR_MASK(__x)     VALUE_( CYGHWR_HAL_STM32_DMA_IFCR_SHIFT(__x), 0x3f )

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)


#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_DMA_CCR_EN             BIT_(0)
#define CYGHWR_HAL_STM32_DMA_CCR_TCIE           BIT_(1)
#define CYGHWR_HAL_STM32_DMA_CCR_HTIE           BIT_(2)
#define CYGHWR_HAL_STM32_DMA_CCR_TEIE           BIT_(3)
#define CYGHWR_HAL_STM32_DMA_CCR_DIR            BIT_(4)
#define CYGHWR_HAL_STM32_DMA_CCR_CIRC           BIT_(5)
#define CYGHWR_HAL_STM32_DMA_CCR_PINC           BIT_(6)
#define CYGHWR_HAL_STM32_DMA_CCR_MINC           BIT_(7)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE8         VALUE_(8,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE16        VALUE_(8,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE32        VALUE_(8,2)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE8         VALUE_(10,0)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE16        VALUE_(10,1)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE32        VALUE_(10,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PL(__x)        VALUE_(12,__x)
#define CYGHWR_HAL_STM32_DMA_CCR_PLLOW          VALUE_(12,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMEDIUM       VALUE_(12,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PLHIGH         VALUE_(12,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMAX          VALUE_(12,3)
#define CYGHWR_HAL_STM32_DMA_CCR_MEM2MEM        BIT_(14)

// F2/F4 compatibility combinations to control transfer source/dest
#define CYGHWR_HAL_STM32_DMA_CCR_P2MEM          0
#define CYGHWR_HAL_STM32_DMA_CCR_MEM2P          CYGHWR_HAL_STM32_DMA_CCR_DIR

// Clock enable bits

#define CYGHWR_HAL_STM32_DMA1_CLOCK             CYGHWR_HAL_STM32_CLOCK( AHB, DMA1 )
#define CYGHWR_HAL_STM32_DMA2_CLOCK             CYGHWR_HAL_STM32_CLOCK( AHB, DMA2 )

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_DMA_CCR_EN             BIT_(0)
#define CYGHWR_HAL_STM32_DMA_CCR_DMEIE          BIT_(1)
#define CYGHWR_HAL_STM32_DMA_CCR_TEIE           BIT_(2)
#define CYGHWR_HAL_STM32_DMA_CCR_HTIE           BIT_(3)
#define CYGHWR_HAL_STM32_DMA_CCR_TCIE           BIT_(4)
#define CYGHWR_HAL_STM32_DMA_CCR_PFCTRL         BIT_(5)
#define CYGHWR_HAL_STM32_DMA_CCR_P2MEM          VALUE_(6,0)
#define CYGHWR_HAL_STM32_DMA_CCR_MEM2P          VALUE_(6,1)
#define CYGHWR_HAL_STM32_DMA_CCR_MEM2MEM        VALUE_(6,2)

#define CYGHWR_HAL_STM32_DMA_CCR_CIRC           BIT_(8)
#define CYGHWR_HAL_STM32_DMA_CCR_PINC           BIT_(9)
#define CYGHWR_HAL_STM32_DMA_CCR_MINC           BIT_(10)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE8         VALUE_(11,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE16        VALUE_(11,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PSIZE32        VALUE_(11,2)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE8         VALUE_(13,0)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE16        VALUE_(13,1)
#define CYGHWR_HAL_STM32_DMA_CCR_MSIZE32        VALUE_(13,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PINCOS         BIT_(15)
#define CYGHWR_HAL_STM32_DMA_CCR_PL(__x)        VALUE_(16,__x)
#define CYGHWR_HAL_STM32_DMA_CCR_PLLOW          VALUE_(16,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMEDIUM       VALUE_(16,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PLHIGH         VALUE_(16,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMAX          VALUE_(16,3)
#define CYGHWR_HAL_STM32_DMA_CCR_DBM            BIT_(18)
#define CYGHWR_HAL_STM32_DMA_CCR_CT             BIT_(19)
#define CYGHWR_HAL_STM32_DMA_CCR_PBURST1        VALUE_(21,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PBURST4        VALUE_(21,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PBURST8        VALUE_(21,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PBURST16       VALUE_(21,3)
#define CYGHWR_HAL_STM32_DMA_CCR_MBURST1        VALUE_(23,0)
#define CYGHWR_HAL_STM32_DMA_CCR_MBURST4        VALUE_(23,1)
#define CYGHWR_HAL_STM32_DMA_CCR_MBURST8        VALUE_(23,2)
#define CYGHWR_HAL_STM32_DMA_CCR_MBURST16       VALUE_(23,3)
#define CYGHWR_HAL_STM32_DMA_CCR_CHSEL(__x)     VALUE_(25,__x)

#define CYGHWR_HAL_STM32_DMA_FCR_FTH_QUARTER    VALUE_(0,0) // FIFO threshold selection
#define CYGHWR_HAL_STM32_DMA_FCR_FTH_HALF       VALUE_(0,1)
#define CYGHWR_HAL_STM32_DMA_FCR_FTH_3QUARTER   VALUE_(0,2)
#define CYGHWR_HAL_STM32_DMA_FCR_FTH_FULL       VALUE_(0,3)
#define CYGHWR_HAL_STM32_DMA_FCR_DMDIS          BIT_(2)
#define CYGHWR_HAL_STM32_DMA_FCR_FS_LTQUARTER   VALUE_(3,0) // LT==less than
#define CYGHWR_HAL_STM32_DMA_FCR_FS_LTHALF      VALUE_(3,1)
#define CYGHWR_HAL_STM32_DMA_FCR_FS_LT3QUARTER  VALUE_(3,2)
#define CYGHWR_HAL_STM32_DMA_FCR_FS_LTFULL      VALUE_(3,3)
#define CYGHWR_HAL_STM32_DMA_FCR_FS_EMPTY       VALUE_(3,4)
#define CYGHWR_HAL_STM32_DMA_FCR_FS_FULL        VALUE_(3,5)
#define CYGHWR_HAL_STM32_DMA_FCR_FEIE           BIT_(7)

// Clock enable bits

#define CYGHWR_HAL_STM32_DMA1_CLOCK             CYGHWR_HAL_STM32_CLOCK( AHB1, DMA1 )
#define CYGHWR_HAL_STM32_DMA2_CLOCK             CYGHWR_HAL_STM32_CLOCK( AHB1, DMA2 )

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

//=============================================================================
// UARTs
// Include separate header file for this to avoid this header getting unmanageable.

#include <cyg/hal/var_io_usart.h>

//=============================================================================
// ADCs

#define CYGHWR_HAL_STM32_ADC_SR                 0x00
#define CYGHWR_HAL_STM32_ADC_CR1                0x04
#define CYGHWR_HAL_STM32_ADC_CR2                0x08
#define CYGHWR_HAL_STM32_ADC_SMPR1              0x0C
#define CYGHWR_HAL_STM32_ADC_SMPR2              0x10
#define CYGHWR_HAL_STM32_ADC_JOFR(__x)          0x14 + ((__x) << 2)
#define CYGHWR_HAL_STM32_ADC_HTR                0x24
#define CYGHWR_HAL_STM32_ADC_LTR                0x28
#define CYGHWR_HAL_STM32_ADC_SQR1               0x2C
#define CYGHWR_HAL_STM32_ADC_SQR2               0x30
#define CYGHWR_HAL_STM32_ADC_SQR3               0x34
#define CYGHWR_HAL_STM32_ADC_JSQR               0x38
#define CYGHWR_HAL_STM32_ADC_JDR(__x)           0x3C + ((__x) << 2)
#define CYGHWR_HAL_STM32_ADC_DR                 0x4C

// SR fields

#define CYGHWR_HAL_STM32_ADC_SR_AWD             BIT_(0)
#define CYGHWR_HAL_STM32_ADC_SR_EOC             BIT_(1)
#define CYGHWR_HAL_STM32_ADC_SR_JEOC            BIT_(2)
#define CYGHWR_HAL_STM32_ADC_SR_JSTRT           BIT_(3)
#define CYGHWR_HAL_STM32_ADC_SR_STRT            BIT_(4)

// CR1 fields

#define CYGHWR_HAL_STM32_ADC_CR1_AWDCH(__x)     VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_ADC_CR1_EOCIE          BIT_(5)
#define CYGHWR_HAL_STM32_ADC_CR1_AWDIE          BIT_(6)
#define CYGHWR_HAL_STM32_ADC_CR1_JEOCIE         BIT_(7)
#define CYGHWR_HAL_STM32_ADC_CR1_SCAN           BIT_(8)
#define CYGHWR_HAL_STM32_ADC_CR1_AWDSGL         BIT_(9)
#define CYGHWR_HAL_STM32_ADC_CR1_JAUTO          BIT_(10)
#define CYGHWR_HAL_STM32_ADC_CR1_DISCEN         BIT_(11)
#define CYGHWR_HAL_STM32_ADC_CR1_JDISCEN        BIT_(12)
#define CYGHWR_HAL_STM32_ADC_CR1_DISCNUM(__x)   VALUE_(13,(__x))
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_ADC_CR1_DUALMODE(__x)  VALUE_(16,(__x))
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1
#define CYGHWR_HAL_STM32_ADC_CR1_JAWDEN         BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CR1_AWDEN          BIT_(23)
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC_CR1_OVRIE          BIT_(26)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE



// CR2 fields

#define CYGHWR_HAL_STM32_ADC_CR2_ADON           BIT_(0)
#define CYGHWR_HAL_STM32_ADC_CR2_CONT           BIT_(1)
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_ADC_CR2_CAL            BIT_(2)
#define CYGHWR_HAL_STM32_ADC_CR2_RSTCAL         BIT_(3)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1
#define CYGHWR_HAL_STM32_ADC_CR2_DMA            BIT_(8)
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC_CR2_DDS            BIT_(9)
#define CYGHWR_HAL_STM32_ADC_CR2_EOCS           BIT_(10)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE

#define CYGHWR_HAL_STM32_ADC_CR2_ALIGN          BIT_(11)
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTSEL(__x)   VALUE_(12,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTTRIG       BIT_(15)
#define CYGHWR_HAL_STM32_ADC_CR2_EXTSEL(__x)    VALUE_(17,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_EXTTRIG        BIT_(20)
#define CYGHWR_HAL_STM32_ADC_CR2_JSWSTART       BIT_(21)
#define CYGHWR_HAL_STM32_ADC_CR2_SWSTART        BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CR2_TSVREFE        BIT_(23)
#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTSEL(__x)   VALUE_(16,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTEN(__x)    VALUE_(20,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_JSWSTART       BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CR2_EXTSEL(__x)    VALUE_(24,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_EXTEN(__x)     VALUE_(28,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_SWSTART        BIT_(30)
#endif

#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
// On F1 devices ADC1 and ADC3 have different external event sets for regular groups
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM1_CC1       (0)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM1_CC2       (1)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM1_CC3       (2)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM2_CC2       (3)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM3_TRGO      (4)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM4_CC4       (5)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_TIM8_TRGO      (6) // For high- and XL-density devices
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC1_SWSTART        (7)

#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM3_CC1       (0)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM2_CC3       (1)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM1_CC3       (2)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM8_CC1       (3)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM8_TRGO      (4)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM5_CC1       (5)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_TIM5_CC3       (6)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_ADC3_SWSTART        (7)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1

#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM1_CC1    (0)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM1_CC2    (1)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM1_CC3    (2)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM2_CC2    (3)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM2_CC3    (4)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM2_CC4    (5)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM2_TRGO   (6)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM3_CC1    (7)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM3_TRGO   (8)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM4_CC4    (9)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM5_CC1    (10)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM5_CC2    (11)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM5_CC3    (12)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM8_CC1    (13)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_TIM8_TRGO   (14)
#define CYGHWR_HAL_STM32_ADC_EXTSEL_EXTI        (15) // line 11

#define CYGHWR_HAL_STM32_ADC_EXTEN_DISABLED     (0)
#define CYGHWR_HAL_STM32_ADC_EXTEN_EDGE_RISE    (1)
#define CYGHWR_HAL_STM32_ADC_EXTEN_EDGE_FALL    (2)
#define CYGHWR_HAL_STM32_ADC_EXTEN_EDGE_BOTH    (3)

#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE

// SMPRx fields

#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
// F1 : SMPR1 17..10 : SMPR2 9..0
#define CYGHWR_HAL_STM32_ADC_SMPR1_NUM_CHANNELS (8)
#define CYGHWR_HAL_STM32_ADC_SMPR2_NUM_CHANNELS (10)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1
#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
// F2/F4 : SMPR1 18..10 : SMPR2 9..0
#define CYGHWR_HAL_STM32_ADC_SMPR1_NUM_CHANNELS (9)
#define CYGHWR_HAL_STM32_ADC_SMPR2_NUM_CHANNELS (10)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE

#define CYGHWR_HAL_STM32_ADC_SMPRx_SMP(__x, __y) VALUE_((__x) * 3, (__y))

// SQRx fields

#define CYGHWR_HAL_STM32_ADC_SQR1_L(__x)        VALUE_(20, (__x))
#define CYGHWR_HAL_STM32_ADC_SQRx_SQ(__x, __y)  VALUE_((__x) * 5, (__y))

// JSQR fields

#define CYGHWR_HAL_STM32_ADC_JSQR_SQ(__x, __y)  VALUE_((__x) * 5, (__y))

// ADC GPIO pins

// F1/F2/F4 GPIO inputs have 16 channels (0..15).
// ADC1 has extra internal sources, which for F1 and HIPERFORMANCE (F2/F4)
// devices respectively have 18 (0..17) and 19 (0..18) available sources.

// Internal (non-GPIO) channels (ADC1 only):
// - ADC1_IN16 temperature      F1/F2/F4
// - ADC1_IN17 Vrefint          F1/F2/F4
// - ADC1_IN18 Vbat             F2/F4

#define CYGHWR_HAL_STM32_ADC123_IN0             CYGHWR_HAL_STM32_PIN_ANALOG( A, 0 )
#define CYGHWR_HAL_STM32_ADC123_IN1             CYGHWR_HAL_STM32_PIN_ANALOG( A, 1 )
#define CYGHWR_HAL_STM32_ADC123_IN2             CYGHWR_HAL_STM32_PIN_ANALOG( A, 2 )
#define CYGHWR_HAL_STM32_ADC123_IN3             CYGHWR_HAL_STM32_PIN_ANALOG( A, 3 )

#define CYGHWR_HAL_STM32_ADC12_IN4              CYGHWR_HAL_STM32_PIN_ANALOG( A, 4 )
#define CYGHWR_HAL_STM32_ADC12_IN5              CYGHWR_HAL_STM32_PIN_ANALOG( A, 5 )
#define CYGHWR_HAL_STM32_ADC12_IN6              CYGHWR_HAL_STM32_PIN_ANALOG( A, 6 )
#define CYGHWR_HAL_STM32_ADC12_IN7              CYGHWR_HAL_STM32_PIN_ANALOG( A, 7 )

#define CYGHWR_HAL_STM32_ADC12_IN8              CYGHWR_HAL_STM32_PIN_ANALOG( B, 0 )
#define CYGHWR_HAL_STM32_ADC12_IN9              CYGHWR_HAL_STM32_PIN_ANALOG( B, 1 )

#define CYGHWR_HAL_STM32_ADC3_IN4               CYGHWR_HAL_STM32_PIN_ANALOG( F, 6 )
#define CYGHWR_HAL_STM32_ADC3_IN5               CYGHWR_HAL_STM32_PIN_ANALOG( F, 7 )
#define CYGHWR_HAL_STM32_ADC3_IN6               CYGHWR_HAL_STM32_PIN_ANALOG( F, 8 )
#define CYGHWR_HAL_STM32_ADC3_IN7               CYGHWR_HAL_STM32_PIN_ANALOG( F, 9 )
#define CYGHWR_HAL_STM32_ADC3_IN8               CYGHWR_HAL_STM32_PIN_ANALOG( F, 10 )

#define CYGHWR_HAL_STM32_ADC123_IN10            CYGHWR_HAL_STM32_PIN_ANALOG( C, 0 )
#define CYGHWR_HAL_STM32_ADC123_IN11            CYGHWR_HAL_STM32_PIN_ANALOG( C, 1 )
#define CYGHWR_HAL_STM32_ADC123_IN12            CYGHWR_HAL_STM32_PIN_ANALOG( C, 2 )
#define CYGHWR_HAL_STM32_ADC123_IN13            CYGHWR_HAL_STM32_PIN_ANALOG( C, 3 )

#define CYGHWR_HAL_STM32_ADC12_IN14             CYGHWR_HAL_STM32_PIN_ANALOG( C, 4 )
#define CYGHWR_HAL_STM32_ADC12_IN15             CYGHWR_HAL_STM32_PIN_ANALOG( C, 5)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
// Following ADC3 channels not-mapped on F1 devices
#define CYGHWR_HAL_STM32_ADC3_IN9        CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_ADC3_IN14       CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_ADC3_IN15       CYGHWR_HAL_STM32_GPIO_NONE
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC3_IN9        CYGHWR_HAL_STM32_PIN_ANALOG( F, 3 )
#define CYGHWR_HAL_STM32_ADC3_IN14       CYGHWR_HAL_STM32_PIN_ANALOG( F, 4 )
#define CYGHWR_HAL_STM32_ADC3_IN15       CYGHWR_HAL_STM32_PIN_ANALOG( F, 5 )
#endif

// ADC1 GPIO pin aliases

#define CYGHWR_HAL_STM32_ADC1_IN0               CYGHWR_HAL_STM32_ADC123_IN0
#define CYGHWR_HAL_STM32_ADC1_IN1               CYGHWR_HAL_STM32_ADC123_IN1
#define CYGHWR_HAL_STM32_ADC1_IN2               CYGHWR_HAL_STM32_ADC123_IN2
#define CYGHWR_HAL_STM32_ADC1_IN3               CYGHWR_HAL_STM32_ADC123_IN3
#define CYGHWR_HAL_STM32_ADC1_IN4               CYGHWR_HAL_STM32_ADC12_IN4
#define CYGHWR_HAL_STM32_ADC1_IN5               CYGHWR_HAL_STM32_ADC12_IN5
#define CYGHWR_HAL_STM32_ADC1_IN6               CYGHWR_HAL_STM32_ADC12_IN6
#define CYGHWR_HAL_STM32_ADC1_IN7               CYGHWR_HAL_STM32_ADC12_IN7
#define CYGHWR_HAL_STM32_ADC1_IN8               CYGHWR_HAL_STM32_ADC12_IN8
#define CYGHWR_HAL_STM32_ADC1_IN9               CYGHWR_HAL_STM32_ADC12_IN9
#define CYGHWR_HAL_STM32_ADC1_IN10              CYGHWR_HAL_STM32_ADC123_IN10
#define CYGHWR_HAL_STM32_ADC1_IN11              CYGHWR_HAL_STM32_ADC123_IN11
#define CYGHWR_HAL_STM32_ADC1_IN12              CYGHWR_HAL_STM32_ADC123_IN12
#define CYGHWR_HAL_STM32_ADC1_IN13              CYGHWR_HAL_STM32_ADC123_IN13
#define CYGHWR_HAL_STM32_ADC1_IN14              CYGHWR_HAL_STM32_ADC12_IN14
#define CYGHWR_HAL_STM32_ADC1_IN15              CYGHWR_HAL_STM32_ADC12_IN15

// ADC2 GPIO pin aliases

#define CYGHWR_HAL_STM32_ADC2_IN0               CYGHWR_HAL_STM32_ADC123_IN0
#define CYGHWR_HAL_STM32_ADC2_IN1               CYGHWR_HAL_STM32_ADC123_IN1
#define CYGHWR_HAL_STM32_ADC2_IN2               CYGHWR_HAL_STM32_ADC123_IN2
#define CYGHWR_HAL_STM32_ADC2_IN3               CYGHWR_HAL_STM32_ADC123_IN3
#define CYGHWR_HAL_STM32_ADC2_IN4               CYGHWR_HAL_STM32_ADC12_IN4
#define CYGHWR_HAL_STM32_ADC2_IN5               CYGHWR_HAL_STM32_ADC12_IN5
#define CYGHWR_HAL_STM32_ADC2_IN6               CYGHWR_HAL_STM32_ADC12_IN6
#define CYGHWR_HAL_STM32_ADC2_IN7               CYGHWR_HAL_STM32_ADC12_IN7
#define CYGHWR_HAL_STM32_ADC2_IN8               CYGHWR_HAL_STM32_ADC12_IN8
#define CYGHWR_HAL_STM32_ADC2_IN9               CYGHWR_HAL_STM32_ADC12_IN9
#define CYGHWR_HAL_STM32_ADC2_IN10              CYGHWR_HAL_STM32_ADC123_IN10
#define CYGHWR_HAL_STM32_ADC2_IN11              CYGHWR_HAL_STM32_ADC123_IN11
#define CYGHWR_HAL_STM32_ADC2_IN12              CYGHWR_HAL_STM32_ADC123_IN12
#define CYGHWR_HAL_STM32_ADC2_IN13              CYGHWR_HAL_STM32_ADC123_IN13
#define CYGHWR_HAL_STM32_ADC2_IN14              CYGHWR_HAL_STM32_ADC12_IN14
#define CYGHWR_HAL_STM32_ADC2_IN15              CYGHWR_HAL_STM32_ADC12_IN15

// ADC3 GPIO pin aliases

#define CYGHWR_HAL_STM32_ADC3_IN0               CYGHWR_HAL_STM32_ADC123_IN0
#define CYGHWR_HAL_STM32_ADC3_IN1               CYGHWR_HAL_STM32_ADC123_IN1
#define CYGHWR_HAL_STM32_ADC3_IN2               CYGHWR_HAL_STM32_ADC123_IN2
#define CYGHWR_HAL_STM32_ADC3_IN3               CYGHWR_HAL_STM32_ADC123_IN3
// Inputs 4 - 9 are already defined
#define CYGHWR_HAL_STM32_ADC3_IN10              CYGHWR_HAL_STM32_ADC123_IN10
#define CYGHWR_HAL_STM32_ADC3_IN11              CYGHWR_HAL_STM32_ADC123_IN11
#define CYGHWR_HAL_STM32_ADC3_IN12              CYGHWR_HAL_STM32_ADC123_IN12
#define CYGHWR_HAL_STM32_ADC3_IN13              CYGHWR_HAL_STM32_ADC123_IN13
// Inputs 14 - 15 are already defined

// ADC Clock control pins

#define CYGHWR_HAL_STM32_ADC1_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, ADC1 )
#define CYGHWR_HAL_STM32_ADC2_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, ADC2 )
#define CYGHWR_HAL_STM32_ADC3_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, ADC3 )

// F2/F4 only: Common configuration registers based from CYGHWR_HAL_STM32_ADC_COMMON
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ADC_CSR  0x00
#define CYGHWR_HAL_STM32_ADC_CCR  0x04
#define CYGHWR_HAL_STM32_ADC_CDR  0x08

// CSR
#define CYGHWR_HAL_STM32_ADC_CSR_AWD            BIT_(0)
#define CYGHWR_HAL_STM32_ADC_CSR_EOC            BIT_(1)
#define CYGHWR_HAL_STM32_ADC_CSR_JEOC           BIT_(2)
#define CYGHWR_HAL_STM32_ADC_CSR_JSTRT          BIT_(3)
#define CYGHWR_HAL_STM32_ADC_CSR_STRT           BIT_(4)
#define CYGHWR_HAL_STM32_ADC_CSR_OVR            BIT_(5)

#define CYGHWR_HAL_STM32_ADC1_CSR_AWD            CYGHWR_HAL_STM32_ADC_CSR_AWD
#define CYGHWR_HAL_STM32_ADC1_CSR_EOC            CYGHWR_HAL_STM32_ADC_CSR_EOC
#define CYGHWR_HAL_STM32_ADC1_CSR_JEOC           CYGHWR_HAL_STM32_ADC_CSR_JEOC
#define CYGHWR_HAL_STM32_ADC1_CSR_JSTRT          CYGHWR_HAL_STM32_ADC_CSR_JSTRT
#define CYGHWR_HAL_STM32_ADC1_CSR_STRT           CYGHWR_HAL_STM32_ADC_CSR_STRT
#define CYGHWR_HAL_STM32_ADC1_CSR_OVR            CYGHWR_HAL_STM32_ADC_CSR_OVR

#define CYGHWR_HAL_STM32_ADC2_CSR_AWD            (CYGHWR_HAL_STM32_ADC_CSR_AWD << 8)
#define CYGHWR_HAL_STM32_ADC2_CSR_EOC            (CYGHWR_HAL_STM32_ADC_CSR_EOC << 8)
#define CYGHWR_HAL_STM32_ADC2_CSR_JEOC           (CYGHWR_HAL_STM32_ADC_CSR_JEOC << 8)
#define CYGHWR_HAL_STM32_ADC2_CSR_JSTRT          (CYGHWR_HAL_STM32_ADC_CSR_JSTRT << 8)
#define CYGHWR_HAL_STM32_ADC2_CSR_STRT           (CYGHWR_HAL_STM32_ADC_CSR_STRT << 8)
#define CYGHWR_HAL_STM32_ADC2_CSR_OVR            (CYGHWR_HAL_STM32_ADC_CSR_OVR << 8)

#define CYGHWR_HAL_STM32_ADC3_CSR_AWD            (CYGHWR_HAL_STM32_ADC_CSR_AWD << 16)
#define CYGHWR_HAL_STM32_ADC3_CSR_EOC            (CYGHWR_HAL_STM32_ADC_CSR_EOC << 16)
#define CYGHWR_HAL_STM32_ADC3_CSR_JEOC           (CYGHWR_HAL_STM32_ADC_CSR_JEOC << 16)
#define CYGHWR_HAL_STM32_ADC3_CSR_JSTRT          (CYGHWR_HAL_STM32_ADC_CSR_JSTRT << 16)
#define CYGHWR_HAL_STM32_ADC3_CSR_STRT           (CYGHWR_HAL_STM32_ADC_CSR_STRT << 16)
#define CYGHWR_HAL_STM32_ADC3_CSR_OVR            (CYGHWR_HAL_STM32_ADC_CSR_OVR << 16)

#define CYGHWR_HAL_STM32_ADC_CSR_ADC1(__csr) ((__csr) >> 0)
#define CYGHWR_HAL_STM32_ADC_CSR_ADC2(__csr) ((__csr) >> 8)
#define CYGHWR_HAL_STM32_ADC_CSR_ADC3(__csr) ((__csr) >> 16)

// CCR
#define CYGHWR_HAL_STM32_ADC_CCR_MULTI_XXX     VALUE_(0,0x1F)
#define CYGHWR_HAL_STM32_ADC_CCR_DELAY_XXX     VALUE_(8,0xF)
#define CYGHWR_HAL_STM32_ADC_CCR_DDS           BIT_(13)
#define CYGHWR_HAL_STM32_ADC_CCR_DMA_XXX       VALUE_(14,0x3)
#define CYGHWR_HAL_STM32_ADC_CCR_ADCPRE_2      VALUE_(16,0x0)
#define CYGHWR_HAL_STM32_ADC_CCR_ADCPRE_4      VALUE_(16,0x1)
#define CYGHWR_HAL_STM32_ADC_CCR_ADCPRE_6      VALUE_(16,0x2)
#define CYGHWR_HAL_STM32_ADC_CCR_ADCPRE_8      VALUE_(16,0x3)
#define CYGHWR_HAL_STM32_ADC_CCR_ADCPRE_XXX    VALUE_(16,0x3)
#define CYGHWR_HAL_STM32_ADC_CCR_VBATE         BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CCR_TSVREFE       BIT_(23)

// CDR
#define CYGHWR_HAL_STM32_ADC_CDR_DATA1_XXX    VALUE_(0,0xFFFF)
#define CYGHWR_HAL_STM32_ADC_CDR_DATA2_XXX    VALUE_(16,0xFFFF)
#endif

//=============================================================================
// SPI interface register definitions.

#define CYGHWR_HAL_STM32_SPI_CR1                0x00
#define CYGHWR_HAL_STM32_SPI_CR2                0x04
#define CYGHWR_HAL_STM32_SPI_SR                 0x08
#define CYGHWR_HAL_STM32_SPI_DR                 0x0C
#define CYGHWR_HAL_STM32_SPI_CRCPR              0x10
#define CYGHWR_HAL_STM32_SPI_RXCRCR             0x14
#define CYGHWR_HAL_STM32_SPI_TXCRCR             0x18
#define CYGHWR_HAL_STM32_SPI_I2SCFGR            0x1C
#define CYGHWR_HAL_STM32_SPI_I2SPR              0x20

#define CYGHWR_HAL_STM32_SPI_CR1_CPHA           BIT_(0)
#define CYGHWR_HAL_STM32_SPI_CR1_CPOL           BIT_(1)
#define CYGHWR_HAL_STM32_SPI_CR1_MSTR           BIT_(2)
#define CYGHWR_HAL_STM32_SPI_CR1_BR(__x)        VALUE_(3,(__x))
#define CYGHWR_HAL_STM32_SPI_CR1_SPE            BIT_(6)
#define CYGHWR_HAL_STM32_SPI_CR1_LSBFIRST       BIT_(7)
#define CYGHWR_HAL_STM32_SPI_CR1_SSI            BIT_(8)
#define CYGHWR_HAL_STM32_SPI_CR1_SSM            BIT_(9)
#define CYGHWR_HAL_STM32_SPI_CR1_RXONLY         BIT_(10)
#define CYGHWR_HAL_STM32_SPI_CR1_DFF            BIT_(11)
#define CYGHWR_HAL_STM32_SPI_CR1_CRCNEXT        BIT_(12)
#define CYGHWR_HAL_STM32_SPI_CR1_CRCEN          BIT_(13)
#define CYGHWR_HAL_STM32_SPI_CR1_BIDIOE         BIT_(14)
#define CYGHWR_HAL_STM32_SPI_CR1_BIDIMODE       BIT_(15)

#define CYGHWR_HAL_STM32_SPI_CR2_RXDMAEN        BIT_(0)
#define CYGHWR_HAL_STM32_SPI_CR2_TXDMAEN        BIT_(1)
#define CYGHWR_HAL_STM32_SPI_CR2_SSOE           BIT_(2)
#define CYGHWR_HAL_STM32_SPI_CR2_ERRIE          BIT_(5)
#define CYGHWR_HAL_STM32_SPI_CR2_RXNEIE         BIT_(6)
#define CYGHWR_HAL_STM32_SPI_CR2_TXEIE          BIT_(7)

#define CYGHWR_HAL_STM32_SPI_SR_RXNE            BIT_(0)
#define CYGHWR_HAL_STM32_SPI_SR_TXE             BIT_(1)
#define CYGHWR_HAL_STM32_SPI_SR_CHSIDE          BIT_(2)
#define CYGHWR_HAL_STM32_SPI_SR_UDR             BIT_(3)
#define CYGHWR_HAL_STM32_SPI_SR_CRCERR          BIT_(4)
#define CYGHWR_HAL_STM32_SPI_SR_MODF            BIT_(5)
#define CYGHWR_HAL_STM32_SPI_SR_OVR             BIT_(6)
#define CYGHWR_HAL_STM32_SPI_SR_BSY             BIT_(7)

#define CYGHWR_HAL_STM32_SPI_I2SCFGR_CHLEN      BIT_(0)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_DATLEN16   VALUE_(1,0)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_DATLEN24   VALUE_(1,1)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_DATLEN32   VALUE_(1,2)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_CKPOL      BIT_(3)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SSTDPHL  VALUE_(4,0)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SSTDMSB  VALUE_(4,1)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SSTDLSB  VALUE_(4,2)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SSTDPCM  VALUE_(4,3)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_PCMSYNC    BIT_(7)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SCFGST   VALUE_(8,0)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SCFGSR   VALUE_(8,1)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SCFGMT   VALUE_(8,2)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SCFGMR   VALUE_(8,3)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2SE       BIT_(10)
#define CYGHWR_HAL_STM32_SPI_I2SCFGR_I2MOD      BIT_(11)

#define CYGHWR_HAL_STM32_SPI_I2SPR_I2SDIV(__x)  VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_SPI_I2SPR_ODD          BIT_(8)
#define CYGHWR_HAL_STM32_SPI_I2SPR_MCKOE        BIT_(9)

// Clock control definitions for each SPI bus

#define CYGHWR_HAL_STM32_SPI1_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, SPI1 )
#define CYGHWR_HAL_STM32_SPI2_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, SPI2 )
#define CYGHWR_HAL_STM32_SPI3_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, SPI3 )

//=============================================================================
// I2C busses

#define CYGHWR_HAL_STM32_I2C_CR1                0x00
#define CYGHWR_HAL_STM32_I2C_CR2                0x04
#define CYGHWR_HAL_STM32_I2C_OAR1               0x08
#define CYGHWR_HAL_STM32_I2C_OAR2               0x0C
#define CYGHWR_HAL_STM32_I2C_DR                 0x10
#define CYGHWR_HAL_STM32_I2C_SR1                0x14
#define CYGHWR_HAL_STM32_I2C_SR2                0x18
#define CYGHWR_HAL_STM32_I2C_CCR                0x1C
#define CYGHWR_HAL_STM32_I2C_TRISE              0x20

#define CYGHWR_HAL_STM32_I2C_CR1_PE             BIT_(0)
#define CYGHWR_HAL_STM32_I2C_CR1_SMBUS          BIT_(1)
#define CYGHWR_HAL_STM32_I2C_CR1_SMBTYPE        BIT_(3)
#define CYGHWR_HAL_STM32_I2C_CR1_ENARP          BIT_(4)
#define CYGHWR_HAL_STM32_I2C_CR1_ENPEC          BIT_(5)
#define CYGHWR_HAL_STM32_I2C_CR1_ENGC           BIT_(6)
#define CYGHWR_HAL_STM32_I2C_CR1_NOSTRETCH      BIT_(7)
#define CYGHWR_HAL_STM32_I2C_CR1_START          BIT_(8)
#define CYGHWR_HAL_STM32_I2C_CR1_STOP           BIT_(9)
#define CYGHWR_HAL_STM32_I2C_CR1_ACK            BIT_(10)
#define CYGHWR_HAL_STM32_I2C_CR1_POS            BIT_(11)
#define CYGHWR_HAL_STM32_I2C_CR1_PEC            BIT_(12)
#define CYGHWR_HAL_STM32_I2C_CR1_ALERT          BIT_(13)
#define CYGHWR_HAL_STM32_I2C_CR1_SWRST          BIT_(15)


#define CYGHWR_HAL_STM32_I2C_CR2_FREQ(__x)      VALUE_(0,__x)
#define CYGHWR_HAL_STM32_I2C_CR2_FREQ_MASK      MASK_(0,6)
#define CYGHWR_HAL_STM32_I2C_CR2_ITERREN        BIT_(8)
#define CYGHWR_HAL_STM32_I2C_CR2_ITEVTEN        BIT_(9)
#define CYGHWR_HAL_STM32_I2C_CR2_ITBUFEN        BIT_(10)
#define CYGHWR_HAL_STM32_I2C_CR2_DMAEN          BIT_(11)
#define CYGHWR_HAL_STM32_I2C_CR2_LAST           BIT_(12)

// OAR1 and OAR2 omitted, we only support master mode

#define CYGHWR_HAL_STM32_I2C_SR1_SB             BIT_(0)
#define CYGHWR_HAL_STM32_I2C_SR1_ADDR           BIT_(1)
#define CYGHWR_HAL_STM32_I2C_SR1_BTF            BIT_(2)
#define CYGHWR_HAL_STM32_I2C_SR1_ADD10          BIT_(3)
#define CYGHWR_HAL_STM32_I2C_SR1_STOPF          BIT_(4)
#define CYGHWR_HAL_STM32_I2C_SR1_RxNE           BIT_(6)
#define CYGHWR_HAL_STM32_I2C_SR1_TxE            BIT_(7)
#define CYGHWR_HAL_STM32_I2C_SR1_BERR           BIT_(8)
#define CYGHWR_HAL_STM32_I2C_SR1_ARLO           BIT_(9)
#define CYGHWR_HAL_STM32_I2C_SR1_AF             BIT_(10)
#define CYGHWR_HAL_STM32_I2C_SR1_OVR            BIT_(11)
#define CYGHWR_HAL_STM32_I2C_SR1_PECERR         BIT_(12)
#define CYGHWR_HAL_STM32_I2C_SR1_TIMEOUT        BIT_(14)
#define CYGHWR_HAL_STM32_I2C_SR1_SMBALERT       BIT_(15)


#define CYGHWR_HAL_STM32_I2C_SR2_MSL            BIT_(0)
#define CYGHWR_HAL_STM32_I2C_SR2_BUSY           BIT_(1)
#define CYGHWR_HAL_STM32_I2C_SR2_TRA            BIT_(2)
#define CYGHWR_HAL_STM32_I2C_SR2_GENCALL        BIT_(4)
#define CYGHWR_HAL_STM32_I2C_SR2_SMBDEFAULT     BIT_(5)
#define CYGHWR_HAL_STM32_I2C_SR2_SMBHOST        BIT_(6)
#define CYGHWR_HAL_STM32_I2C_SR2_DUALF          BIT_(7)
#define CYGHWR_HAL_STM32_I2C_SR2_PEC            MASK_(7,8)

#define CYGHWR_HAL_STM32_I2C_CCR_CCR(__x)       VALUE_(0,__x)
#define CYGHWR_HAL_STM32_I2C_CCR_CCR_MASK       MASK_(0,12)
#define CYGHWR_HAL_STM32_I2C_CCR_DUTY_2         0
#define CYGHWR_HAL_STM32_I2C_CCR_DUTY_16_9      BIT_(14)
#define CYGHWR_HAL_STM32_I2C_CCR_STD            0
#define CYGHWR_HAL_STM32_I2C_CCR_FAST           BIT_(15)

#define CYGHWR_HAL_STM32_I2C_TRISE_VAL(__x)     VALUE_(0,__x)
#define CYGHWR_HAL_STM32_I2C_TRISE_MASK         MASK_(0,6)

// Clock control definitions for each I2C bus

#define CYGHWR_HAL_STM32_I2C1_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, I2C1 )
#define CYGHWR_HAL_STM32_I2C2_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, I2C2 )


//=============================================================================
// USB interface register definitions.

#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_USB_EP0R               0x00 
#define CYGHWR_HAL_STM32_USB_EP1R               0x04 
#define CYGHWR_HAL_STM32_USB_EP2R               0x08 
#define CYGHWR_HAL_STM32_USB_EP3R               0x0C 
#define CYGHWR_HAL_STM32_USB_EP4R               0x10 
#define CYGHWR_HAL_STM32_USB_EP5R               0x14 
#define CYGHWR_HAL_STM32_USB_EP6R               0x18 
#define CYGHWR_HAL_STM32_USB_EP7R               0x1C 

#define CYGHWR_HAL_STM32_USB_CNTR               0x40
#define CYGHWR_HAL_STM32_USB_ISTR               0x44
#define CYGHWR_HAL_STM32_USB_FNR                0x48
#define CYGHWR_HAL_STM32_USB_DADDR              0x4C
#define CYGHWR_HAL_STM32_USB_BTABLE             0x50

// The following macro allows the USB endpoint registers to be indexed as
// CYGHWR_HAL_STM32_USB_EPXR(0) to CYGHWR_HAL_STM32_USB_EPXR(7).
#define CYGHWR_HAL_STM32_USB_EPXR(__x)          ((__x)*4)

#define CYGHWR_HAL_STM32_USB_EPXR_EA(__x)       VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_USB_EPXR_STATTX_DIS    VALUE_(4,0)
#define CYGHWR_HAL_STM32_USB_EPXR_STATTX_STALL  VALUE_(4,1)
#define CYGHWR_HAL_STM32_USB_EPXR_STATTX_NAK    VALUE_(4,2)
#define CYGHWR_HAL_STM32_USB_EPXR_STATTX_VALID  VALUE_(4,3)
#define CYGHWR_HAL_STM32_USB_EPXR_STATTX_MASK   VALUE_(4,3)
#define CYGHWR_HAL_STM32_USB_EPXR_DTOGTX        BIT_(6)
#define CYGHWR_HAL_STM32_USB_EPXR_SWBUFRX       BIT_(6)
#define CYGHWR_HAL_STM32_USB_EPXR_CTRTX         BIT_(7)
#define CYGHWR_HAL_STM32_USB_EPXR_EPKIND        BIT_(8)
#define CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_BULK   VALUE_(9,0)
#define CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_CTRL   VALUE_(9,1)
#define CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_ISO    VALUE_(9,2)
#define CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_INTR   VALUE_(9,3)
#define CYGHWR_HAL_STM32_USB_EPXR_EPTYPE_MASK   VALUE_(9,3)
#define CYGHWR_HAL_STM32_USB_EPXR_SETUP         BIT_(11)
#define CYGHWR_HAL_STM32_USB_EPXR_STATRX_DIS    VALUE_(12,0)
#define CYGHWR_HAL_STM32_USB_EPXR_STATRX_STALL  VALUE_(12,1)
#define CYGHWR_HAL_STM32_USB_EPXR_STATRX_NAK    VALUE_(12,2)
#define CYGHWR_HAL_STM32_USB_EPXR_STATRX_VALID  VALUE_(12,3)
#define CYGHWR_HAL_STM32_USB_EPXR_STATRX_MASK   VALUE_(12,3)
#define CYGHWR_HAL_STM32_USB_EPXR_DTOGRX        BIT_(14)
#define CYGHWR_HAL_STM32_USB_EPXR_SWBUFTX       BIT_(14)
#define CYGHWR_HAL_STM32_USB_EPXR_CTRRX         BIT_(15)

#define CYGHWR_HAL_STM32_USB_CNTR_FRES          BIT_(0)
#define CYGHWR_HAL_STM32_USB_CNTR_PDWN          BIT_(1)
#define CYGHWR_HAL_STM32_USB_CNTR_LPMODE        BIT_(2)
#define CYGHWR_HAL_STM32_USB_CNTR_FSUSP         BIT_(3)
#define CYGHWR_HAL_STM32_USB_CNTR_RESUME        BIT_(4)
#define CYGHWR_HAL_STM32_USB_CNTR_ESOFM         BIT_(8)
#define CYGHWR_HAL_STM32_USB_CNTR_SOFM          BIT_(9)
#define CYGHWR_HAL_STM32_USB_CNTR_RESETM        BIT_(10)
#define CYGHWR_HAL_STM32_USB_CNTR_SUSPM         BIT_(11)
#define CYGHWR_HAL_STM32_USB_CNTR_WKUPM         BIT_(12)
#define CYGHWR_HAL_STM32_USB_CNTR_ERRM          BIT_(13)
#define CYGHWR_HAL_STM32_USB_CNTR_PMAOVRM       BIT_(14)
#define CYGHWR_HAL_STM32_USB_CNTR_CTRM          BIT_(15)

#define CYGHWR_HAL_STM32_USB_ISTR_EPID(__x)     VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_USB_ISTR_EPID_MASK     MASK_(0,4)
#define CYGHWR_HAL_STM32_USB_ISTR_DIR           BIT_(4)
#define CYGHWR_HAL_STM32_USB_ISTR_ESOF          BIT_(8)
#define CYGHWR_HAL_STM32_USB_ISTR_SOF           BIT_(9)
#define CYGHWR_HAL_STM32_USB_ISTR_RESET         BIT_(10)
#define CYGHWR_HAL_STM32_USB_ISTR_SUSP          BIT_(11)
#define CYGHWR_HAL_STM32_USB_ISTR_WKUP          BIT_(12)
#define CYGHWR_HAL_STM32_USB_ISTR_ERR           BIT_(13)
#define CYGHWR_HAL_STM32_USB_ISTR_PMAOVR        BIT_(14)
#define CYGHWR_HAL_STM32_USB_ISTR_CTR           BIT_(15)

#define CYGHWR_HAL_STM32_USB_FNR_FN_MASK        MASK_(0,11)
#define CYGHWR_HAL_STM32_USB_FNR_LSOF_LSOF0     VALUE_(11,0)
#define CYGHWR_HAL_STM32_USB_FNR_LSOF_LSOF1     VALUE_(11,1)
#define CYGHWR_HAL_STM32_USB_FNR_LSOF_LSOF2     VALUE_(11,2)
#define CYGHWR_HAL_STM32_USB_FNR_LSOF_LSOFN     VALUE_(11,3)
#define CYGHWR_HAL_STM32_USB_FNR_LSOF_MASK      MASK_(11,2)
#define CYGHWR_HAL_STM32_USB_FNR_LCK            BIT_(13)
#define CYGHWR_HAL_STM32_USB_FNR_RXDM           BIT_(14)
#define CYGHWR_HAL_STM32_USB_FNR_RXDP           BIT_(15)

#define CYGHWR_HAL_STM32_USB_DADDR_ADD(__x)     VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_USB_DADDR_EF           BIT_(7)

#endif // if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

// USB in F2/F4 parts is completely different. Definitions will be provided when implemented.

//=============================================================================
// Timers
//
// This currently only defines the basic registers and functionality
// common to all timers.

#define CYGHWR_HAL_STM32_TIM_CR1                0x00
#define CYGHWR_HAL_STM32_TIM_CR2                0x04
#define CYGHWR_HAL_STM32_TIM_DIER               0x0C
#define CYGHWR_HAL_STM32_TIM_SR                 0x10
#define CYGHWR_HAL_STM32_TIM_EGR                0x14
#define CYGHWR_HAL_STM32_TIM_CCMR1              0x18
#define CYGHWR_HAL_STM32_TIM_CCMR2              0x1C
#define CYGHWR_HAL_STM32_TIM_CCER               0x20
#define CYGHWR_HAL_STM32_TIM_CNT                0x24
#define CYGHWR_HAL_STM32_TIM_PSC                0x28
#define CYGHWR_HAL_STM32_TIM_ARR                0x2C
#define CYGHWR_HAL_STM32_TIM_CCR1               0x34
#define CYGHWR_HAL_STM32_TIM_CCR2               0x38
#define CYGHWR_HAL_STM32_TIM_CCR3               0x3C
#define CYGHWR_HAL_STM32_TIM_CCR4               0x40

#define CYGHWR_HAL_STM32_TIM_CR1_CEN            BIT_(0)
#define CYGHWR_HAL_STM32_TIM_CR1_UDIS           BIT_(1)
#define CYGHWR_HAL_STM32_TIM_CR1_URS            BIT_(2)
#define CYGHWR_HAL_STM32_TIM_CR1_OPM            BIT_(3)
#define CYGHWR_HAL_STM32_TIM_CR1_DIR            BIT_(4)
#define CYGHWR_HAL_STM32_TIM_CR1_ARPE           BIT_(7)
#define CYGHWR_HAL_STM32_TIM_CR1_CKD_1          VALUE_(8,0)
#define CYGHWR_HAL_STM32_TIM_CR1_CKD_2          VALUE_(8,1)
#define CYGHWR_HAL_STM32_TIM_CR1_CKD_4          VALUE_(8,2)
#define CYGHWR_HAL_STM32_TIM_CR1_CKD_XXX        VALUE_(8,3)

#define CYGHWR_HAL_STM32_TIM_CR2_MMS_RESET      VALUE_(4,0)
#define CYGHWR_HAL_STM32_TIM_CR2_MMS_ENABLE     VALUE_(4,1)
#define CYGHWR_HAL_STM32_TIM_CR2_MMS_UPDATE     VALUE_(4,2)

#define CYGHWR_HAL_STM32_TIM_DIER_UIE           BIT_(0)
#define CYGHWR_HAL_STM32_TIM_DIER_UDE           BIT_(8)

#define CYGHWR_HAL_STM32_TIM_SR_UIF             BIT_(0)

#define CYGHWR_HAL_STM32_TIM_EGR_UG             BIT_(0)

// Clock control pins
#define CYGHWR_HAL_STM32_TIM1_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, TIM1 )
#define CYGHWR_HAL_STM32_TIM2_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM2 )
#define CYGHWR_HAL_STM32_TIM3_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM3 )
#define CYGHWR_HAL_STM32_TIM4_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM4 )
#define CYGHWR_HAL_STM32_TIM5_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM5 )
#define CYGHWR_HAL_STM32_TIM6_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM6 )
#define CYGHWR_HAL_STM32_TIM7_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, TIM7 )
#define CYGHWR_HAL_STM32_TIM8_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, TIM8 )
#if 0
#define CYGHWR_HAL_STM32_TIM9_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, TIM9 )
#define CYGHWR_HAL_STM32_TIM10_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB2, TIM10 )
#define CYGHWR_HAL_STM32_TIM11_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB2, TIM11 )
#define CYGHWR_HAL_STM32_TIM12_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB1, TIM12 )
#define CYGHWR_HAL_STM32_TIM13_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB1, TIM13 )
#define CYGHWR_HAL_STM32_TIM14_CLOCK            CYGHWR_HAL_STM32_CLOCK( APB1, TIM14 )
#endif

#ifndef __ASSEMBLER__

__externC cyg_uint32 hal_stm32_timer_clock( CYG_ADDRESS base );

#endif

//=============================================================================
// Independent Watchdog

#define CYGHWR_HAL_STM32_IWDG_KR                0x00
#define CYGHWR_HAL_STM32_IWDG_PR                0x04
#define CYGHWR_HAL_STM32_IWDG_RLR               0x08
#define CYGHWR_HAL_STM32_IWDG_SR                0x0C

#define CYGHWR_HAL_STM32_IWDG_KR_RESET          0xAAAA
#define CYGHWR_HAL_STM32_IWDG_KR_ACCESS         0x5555
#define CYGHWR_HAL_STM32_IWDG_KR_START          0xCCCC

#define CYGHWR_HAL_STM32_IWDG_PR_4              0
#define CYGHWR_HAL_STM32_IWDG_PR_8              1
#define CYGHWR_HAL_STM32_IWDG_PR_16             2
#define CYGHWR_HAL_STM32_IWDG_PR_32             3
#define CYGHWR_HAL_STM32_IWDG_PR_64             4
#define CYGHWR_HAL_STM32_IWDG_PR_128            5
#define CYGHWR_HAL_STM32_IWDG_PR_256            6

#define CYGHWR_HAL_STM32_IWDG_SR_PVU            BIT_(0)
#define CYGHWR_HAL_STM32_IWDG_SR_RVU            BIT_(1)

// Clock control

//#define CYGHWR_HAL_STM32_IWDG_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB1, IWDG )


//=============================================================================
// Flash controller

#define CYGHWR_HAL_STM32_FLASH_ACR              0x00
#define CYGHWR_HAL_STM32_FLASH_KEYR             0x04
#define CYGHWR_HAL_STM32_FLASH_OPTKEYR          0x08
#define CYGHWR_HAL_STM32_FLASH_SR               0x0C
#define CYGHWR_HAL_STM32_FLASH_CR               0x10
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_FLASH_AR               0x14
#define CYGHWR_HAL_STM32_FLASH_OBR              0x1C
#define CYGHWR_HAL_STM32_FLASH_WRPR             0x20
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_FLASH_OPTCR            0x14
#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// Key values

#define CYGHWR_HAL_STM32_FLASH_KEYR_KEY1        0x45670123
#define CYGHWR_HAL_STM32_FLASH_KEYR_KEY2        0xCDEF89AB

// ACR fields

#define CYGHWR_HAL_STM32_FLASH_ACR_LATENCY(__x) VALUE_(0,__x)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_FLASH_ACR_HLFCYA       BIT_(3)
#define CYGHWR_HAL_STM32_FLASH_ACR_PRFTBE       BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_ACR_PRFTBS       BIT_(5)

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_FLASH_ACR_PRFTEN       BIT_(8)
#define CYGHWR_HAL_STM32_FLASH_ACR_ICEN         BIT_(9)
#define CYGHWR_HAL_STM32_FLASH_ACR_DCEN         BIT_(10)
#define CYGHWR_HAL_STM32_FLASH_ACR_ICRST        BIT_(11)
#define CYGHWR_HAL_STM32_FLASH_ACR_DCRST        BIT_(12)

#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// SR fields

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_FLASH_SR_BSY           BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_SR_PGERR         BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_SR_WRPRTERR      BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_SR_EOP           BIT_(5)

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_FLASH_SR_EOP           BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_SR_OPERR         BIT_(1)
#define CYGHWR_HAL_STM32_FLASH_SR_WRPERR        BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_SR_PGAERR        BIT_(5)
#define CYGHWR_HAL_STM32_FLASH_SR_PGPERR        BIT_(6)
#define CYGHWR_HAL_STM32_FLASH_SR_PGSERR        BIT_(7)
#define CYGHWR_HAL_STM32_FLASH_SR_BSY           BIT_(16)

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// CR fields

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_FLASH_CR_PG            BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_CR_PER           BIT_(1)
#define CYGHWR_HAL_STM32_FLASH_CR_MER           BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_CR_OPTPG         BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_CR_OPTER         BIT_(5)
#define CYGHWR_HAL_STM32_FLASH_CR_STRT          BIT_(6)
#define CYGHWR_HAL_STM32_FLASH_CR_LOCK          BIT_(7)
#define CYGHWR_HAL_STM32_FLASH_CR_OPTWRE        BIT_(9)
#define CYGHWR_HAL_STM32_FLASH_CR_ERRIE         BIT_(10)
#define CYGHWR_HAL_STM32_FLASH_CR_EOPIE         BIT_(12)

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_FLASH_CR_PG            BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_CR_SER           BIT_(1)
#define CYGHWR_HAL_STM32_FLASH_CR_MER           BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_CR_SNB(__x)      (((__x)&0xf) << 3)
#define CYGHWR_HAL_STM32_FLASH_CR_SNB_MASK      MASK_(3,4)
#define CYGHWR_HAL_STM32_FLASH_CR_PSIZE(__x)    ( (__x) == 8 ? VALUE_(8,0) : \
                                                  (__x) == 16 ? VALUE_(8,1) : \
                                                  (__x) == 32 ? VALUE_(8,2) : \
                                                  VALUE_(8,3) )
#define CYGHWR_HAL_STM32_FLASH_CR_PSIZE_8       VALUE_(8,0)
#define CYGHWR_HAL_STM32_FLASH_CR_PSIZE_16      VALUE_(8,1)
#define CYGHWR_HAL_STM32_FLASH_CR_PSIZE_32      VALUE_(8,2)
#define CYGHWR_HAL_STM32_FLASH_CR_PSIZE_64      VALUE_(8,3)
#define CYGHWR_HAL_STM32_FLASH_CR_STRT          BIT_(16)
#define CYGHWR_HAL_STM32_FLASH_CR_EOPIE         BIT_(24)
#define CYGHWR_HAL_STM32_FLASH_CR_ERRIE         BIT_(25)
#define CYGHWR_HAL_STM32_FLASH_CR_LOCK          BIT_(31)

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// OBR fields
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_FLASH_OBR_OPTERR       BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_OBR_RDPRT        BIT_(1)
#define CYGHWR_HAL_STM32_FLASH_OBR_WDG_SW       BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_OBR_nRST_STOP    BIT_(3)
#define CYGHWR_HAL_STM32_FLASH_OBR_nRST_STDBY   BIT_(4)
#endif

// F2/F4 FLASH_OPTCR not defined as our flash driver doesn't use it.

//=============================================================================
// Power control

#define CYGHWR_HAL_STM32_PWR_CR                 0x00
#define CYGHWR_HAL_STM32_PWR_CSR                0x04

// CR fields

#define CYGHWR_HAL_STM32_PWR_CR_LPDS            BIT_(0)
#define CYGHWR_HAL_STM32_PWR_CR_PDDS            BIT_(1)
#define CYGHWR_HAL_STM32_PWR_CR_CWUF            BIT_(2)
#define CYGHWR_HAL_STM32_PWR_CR_CSBF            BIT_(3)
#define CYGHWR_HAL_STM32_PWR_CR_PVDE            BIT_(4)
#define CYGHWR_HAL_STM32_PWR_CR_PLS_XXX         VALUE_(5,7)
#define CYGHWR_HAL_STM32_PWR_CR_DBP             BIT_(8)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_PWR_CR_FPDS            BIT_(9)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_PWR_CR_VOS             BIT_(14)
#endif // (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE

// CSR fields

#define CYGHWR_HAL_STM32_PWR_CSR_WUF            BIT_(0)
#define CYGHWR_HAL_STM32_PWR_CSR_SBF            BIT_(1)
#define CYGHWR_HAL_STM32_PWR_CSR_PVDO           BIT_(2)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_PWR_CSR_BRR            BIT_(3)
#endif
#define CYGHWR_HAL_STM32_PWR_CSR_EWUP           BIT_(8)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_PWR_CSR_BRE            BIT_(9)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
#define CYGHWR_HAL_STM32_PWR_CSR_VOSRDY         BIT_(14)
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4
#endif // CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE

// Functions and macros to reset the backup domain as well as
// enable/disable backup domain write protection.

#ifndef __ASSEMBLER__

__externC void hal_stm32_bd_protect( int protect );

#endif

#define CYGHWR_HAL_STM32_BD_RESET()                                         \
    CYG_MACRO_START                                                         \
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_RCC_BDCR,        \
                     CYGHWR_HAL_STM32_RCC_BDCR_BDRST);                      \
    HAL_WRITE_UINT32(CYGHWR_HAL_STM32_RCC+CYGHWR_HAL_STM32_RCC_BDCR, 0);    \
    CYG_MACRO_END

#define CYGHWR_HAL_STM32_BD_PROTECT(__protect )                         \
    hal_stm32_bd_protect( __protect )

//=============================================================================
// FSMC
//
// These registers are usually set up in hal_system_init() using direct
// binary values. Hence we don't define all the fields here (of which
// there are many).

#define CYGHWR_HAL_STM32_FSMC_BCR1              0x00
#define CYGHWR_HAL_STM32_FSMC_BTR1              0x04
#define CYGHWR_HAL_STM32_FSMC_BCR2              0x08
#define CYGHWR_HAL_STM32_FSMC_BTR2              0x0C
#define CYGHWR_HAL_STM32_FSMC_BCR3              0x10
#define CYGHWR_HAL_STM32_FSMC_BTR3              0x14
#define CYGHWR_HAL_STM32_FSMC_BCR4              0x18
#define CYGHWR_HAL_STM32_FSMC_BTR4              0x1C

#define CYGHWR_HAL_STM32_FSMC_BWTR1             0x104
#define CYGHWR_HAL_STM32_FSMC_BWTR2             0x10C
#define CYGHWR_HAL_STM32_FSMC_BWTR3             0x114
#define CYGHWR_HAL_STM32_FSMC_BWTR4             0x11C

#define CYGHWR_HAL_STM32_FSMC_PCR2              0x60
#define CYGHWR_HAL_STM32_FSMC_SR2               0x64
#define CYGHWR_HAL_STM32_FSMC_PMEM2             0x68
#define CYGHWR_HAL_STM32_FSMC_PATT2             0x6C
#define CYGHWR_HAL_STM32_FSMC_ECCR2             0x74

#define CYGHWR_HAL_STM32_FSMC_PCR3              0x80
#define CYGHWR_HAL_STM32_FSMC_SR3               0x84
#define CYGHWR_HAL_STM32_FSMC_PMEM3             0x88
#define CYGHWR_HAL_STM32_FSMC_PATT3             0x8C
#define CYGHWR_HAL_STM32_FSMC_ECCR3             0x94

#define CYGHWR_HAL_STM32_FSMC_PCR4              0xA0
#define CYGHWR_HAL_STM32_FSMC_SR4               0xA4
#define CYGHWR_HAL_STM32_FSMC_PMEM4             0xA8
#define CYGHWR_HAL_STM32_FSMC_PATT4             0xAC

#define CYGHWR_HAL_STM32_FSMC_PIO4              0xB0


#define CYGHWR_HAL_STM32_FSMC_BANK2_BASE        0x70000000
#define CYGHWR_HAL_STM32_FSMC_BANK3_BASE        0x80000000
#define CYGHWR_HAL_STM32_FSMC_BANK4_BASE        0x90000000

#define CYGHWR_HAL_STM32_FSMC_BANK_CMD          0x10000
#define CYGHWR_HAL_STM32_FSMC_BANK_ADDR         0x20000

// PCR fields

#define CYGHWR_HAL_STM32_FSMC_PCR_PWAITEN       BIT_(1)
#define CYGHWR_HAL_STM32_FSMC_PCR_PBKEN         BIT_(2)
#define CYGHWR_HAL_STM32_FSMC_PCR_PTYP_NAND     BIT_(3)
#define CYGHWR_HAL_STM32_FSMC_PCR_PWID_8        VALUE_(4,0)
#define CYGHWR_HAL_STM32_FSMC_PCR_PWID_16       VALUE_(4,1)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCEN         BIT_(6)
// FIXME: I don't see where ADLOW comes from? It's not in F1, F2 or F4. -Jifl
#define CYGHWR_HAL_STM32_FSMC_PCR_ADLOW         BIT_(8)
#define CYGHWR_HAL_STM32_FSMC_PCR_TCLR(__x)     VALUE_(9,__x)
#define CYGHWR_HAL_STM32_FSMC_PCR_TAR(__x)      VALUE_(13,__x)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_256     VALUE_(17,0)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_512     VALUE_(17,1)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_1024    VALUE_(17,2)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_2048    VALUE_(17,3)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_4096    VALUE_(17,4)
#define CYGHWR_HAL_STM32_FSMC_PCR_ECCPS_8192    VALUE_(17,5)

// SR fields

#define CYGHWR_HAL_STM32_FSMC_SR_IRS            BIT_(0)
#define CYGHWR_HAL_STM32_FSMC_SR_ILS            BIT_(1)
#define CYGHWR_HAL_STM32_FSMC_SR_IFS            BIT_(2)
#define CYGHWR_HAL_STM32_FSMC_SR_IREN           BIT_(3)
#define CYGHWR_HAL_STM32_FSMC_SR_ILEN           BIT_(4)
#define CYGHWR_HAL_STM32_FSMC_SR_IFEN           BIT_(5)
#define CYGHWR_HAL_STM32_FSMC_SR_FEMPT          BIT_(6)

//=============================================================================
// Ethernet MAC
// Include separate header file for this to avoid this header getting unmanageable.

#include <cyg/hal/var_io_eth.h>

//==========================================================================

#if (defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103RC) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103VC) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103ZC) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103RD) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103VD) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103ZD) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103RE) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103VE) || \
     defined(CYGHWR_HAL_CORTEXM_STM32_F1_F103ZE))
// NOTE: From ST document ES0104 (STM32F101xC/D/E and STM32F103xC/D/E)
// errata section 2.6.9 we cannot use FSMC and I2C1 at the same time.
// For I2C1 support we are limited to on-chip SRAM/Flash execution and
// must ensure that FSMC is disabled.
# if (defined(CYG_HAL_STARTUP_SRAM) || \
      defined(CYG_HAL_STARTUP_ROM) || \
      defined(CYG_HAL_STARTUP_JTAG))
#  define HAL_AARDVARK_CHECK_I2C( _i2cdev_ )                                   \
     CYG_MACRO_START                                                           \
     if ((_i2cdev_)->i2c_bus == &hal_stm32_i2c_bus1) {                         \
         CYGHWR_HAL_STM32_CLOCK_DISABLE( CYGHWR_HAL_STM32_CLOCK( AHB, FSMC) ); \
     }                                                                         \
     CYG_MACRO_END
# else // on this CPU we cannot use I2C1 since FSMC needed for the CYG_HAL_STARTUP type
#  define HAL_AARDVARK_CHECK_I2C( _i2cdev_ )                                  \
     CYG_MACRO_START                                                          \
     if ((_i2cdev_)->i2c_bus == &hal_stm32_i2c_bus1) {                        \
         CYG_TEST_FAIL_FINISH("Invalid CYG_HAL_STARTUP for I2C1 operations"); \
     }                                                                        \
     CYG_MACRO_END
# endif
#endif

//==========================================================================

#endif // CYGONCE_HAL_VAR_IO_H
//-----------------------------------------------------------------------------
// end of var_io.h
