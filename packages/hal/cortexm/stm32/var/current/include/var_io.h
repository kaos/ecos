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
#define CYGHWR_HAL_STM32_USB            0x40005C00
#define CYGHWR_HAL_STM32_USB_CAN_SRAM   0x40006000
#define CYGHWR_HAL_STM32_BXCAN          0x40006400
#define CYGHWR_HAL_STM32_BKP            0x40006C00
#define CYGHWR_HAL_STM32_PWR            0x40007000
#define CYGHWR_HAL_STM32_DAC            0x40007400
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
#define CYGHWR_HAL_STM32_SPI1           0x40013000
#define CYGHWR_HAL_STM32_TIM8           0x40013400
#define CYGHWR_HAL_STM32_UART1          0x40013800
#define CYGHWR_HAL_STM32_ADC3           0x40013C00
#define CYGHWR_HAL_STM32_SDIO           0x40018000
#define CYGHWR_HAL_STM32_DMA1           0x40020000
#define CYGHWR_HAL_STM32_DMA2           0x40020400
#define CYGHWR_HAL_STM32_RCC            0x40021000
#define CYGHWR_HAL_STM32_FLASH          0x40022000
#define CYGHWR_HAL_STM32_CRC            0x40023000

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
#define CYGHWR_HAL_STM32_RCC_CFGR               0x04
#define CYGHWR_HAL_STM32_RCC_CIR                0x08
#define CYGHWR_HAL_STM32_RCC_APB2RSTR           0x0C
#define CYGHWR_HAL_STM32_RCC_APB1RSTR           0x10
#define CYGHWR_HAL_STM32_RCC_AHBENR             0x14
#define CYGHWR_HAL_STM32_RCC_APB2ENR            0x18
#define CYGHWR_HAL_STM32_RCC_APB1ENR            0x1C
#define CYGHWR_HAL_STM32_RCC_BDCR               0x20
#define CYGHWR_HAL_STM32_RCC_CSR                0x24

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


#define CYGHWR_HAL_STM32_RCC_AHBENR_DMA1        BIT_(0)
#define CYGHWR_HAL_STM32_RCC_AHBENR_DMA2        BIT_(1)
#define CYGHWR_HAL_STM32_RCC_AHBENR_SRAM        BIT_(2)
#define CYGHWR_HAL_STM32_RCC_AHBENR_FLITF       BIT_(4)
#define CYGHWR_HAL_STM32_RCC_AHBENR_CRC         BIT_(6)
#define CYGHWR_HAL_STM32_RCC_AHBENR_FSMC        BIT_(8)
#define CYGHWR_HAL_STM32_RCC_AHBENR_SDIO        BIT_(10)

#define CYGHWR_HAL_STM32_RCC_APB2ENR_AFIO       BIT_(0)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPA       BIT_(2)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPB       BIT_(3)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPC       BIT_(4)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPD       BIT_(5)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPE       BIT_(6)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPF       BIT_(7)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_IOPG       BIT_(8)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC1       BIT_(9)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC2       BIT_(10)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM1       BIT_(11)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_SPI1       BIT_(12)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_TIM8       BIT_(13)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_UART1      BIT_(14)
#define CYGHWR_HAL_STM32_RCC_APB2ENR_ADC3       BIT_(15)


#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM2       BIT_(0)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM3       BIT_(1)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM4       BIT_(2)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM5       BIT_(3)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM6       BIT_(4)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_TIM7       BIT_(5)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_WWDG       BIT_(11)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI2       BIT_(14)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_SPI3       BIT_(15)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART2      BIT_(17)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART3      BIT_(18)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART4      BIT_(19)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_UART5      BIT_(20)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C1       BIT_(21)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_I2C2       BIT_(22)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_USB        BIT_(23)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_CAN        BIT_(25)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_BKP        BIT_(27)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_PWR        BIT_(28)
#define CYGHWR_HAL_STM32_RCC_APB1ENR_DAC        BIT_(29)

#define CYGHWR_HAL_STM32_RCC_CSR_LSION          BIT_(0)
#define CYGHWR_HAL_STM32_RCC_CSR_LSIRDY         BIT_(1)
#define CYGHWR_HAL_STM32_RCC_CSR_RMVF           BIT_(24)
#define CYGHWR_HAL_STM32_RCC_CSR_PINRSTF        BIT_(26)
#define CYGHWR_HAL_STM32_RCC_CSR_PORRSTF        BIT_(27)
#define CYGHWR_HAL_STM32_RCC_CSR_SFTRSTF        BIT_(28)
#define CYGHWR_HAL_STM32_RCC_CSR_IWDGRSTF       BIT_(29)
#define CYGHWR_HAL_STM32_RCC_CSR_WWDGRSTF       BIT_(30)
#define CYGHWR_HAL_STM32_RCC_CSR_LPWRRSTF       BIT_(31)

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

//=============================================================================
// Realtime Clock

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
// GPIO ports

#define CYGHWR_HAL_STM32_GPIO_CRL               0x00
#define CYGHWR_HAL_STM32_GPIO_CRH               0x04
#define CYGHWR_HAL_STM32_GPIO_IDR               0x08
#define CYGHWR_HAL_STM32_GPIO_ODR               0x0C
#define CYGHWR_HAL_STM32_GPIO_BSRR              0x10
#define CYGHWR_HAL_STM32_GPIO_BRR               0x14
#define CYGHWR_HAL_STM32_GPIO_LCKR              0x18

#define CYGHWR_HAL_STM32_GPIO_MODE_IN           VALUE_(0,0)     // Input mode
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ    VALUE_(0,1)     // Output mode, max 10MHz
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ     VALUE_(0,2)     // Output mode, max 2MHz
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ    VALUE_(0,3)     // Output mode, max 50MHz

#define CYGHWR_HAL_STM32_GPIO_CNF_AIN           VALUE_(2,0)     // Analog input
#define CYGHWR_HAL_STM32_GPIO_CNF_FIN           VALUE_(2,1)     // Floating input
#define CYGHWR_HAL_STM32_GPIO_CNF_PULL          VALUE_(2,2)     // Input with pull up/down
#define CYGHWR_HAL_STM32_GPIO_CNF_RESV          VALUE_(2,3)     // Reserved

#define CYGHWR_HAL_STM32_GPIO_CNF_GPOPP         VALUE_(2,0)     // GP output push/pull
#define CYGHWR_HAL_STM32_GPIO_CNF_GPOOD         VALUE_(2,1)     // GP output open drain
#define CYGHWR_HAL_STM32_GPIO_CNF_AOPP          VALUE_(2,2)     // Alt output push/pull
#define CYGHWR_HAL_STM32_GPIO_CNF_AOOD          VALUE_(2,3)     // Alt output open drain


// Alternative, more readable, config names
// Inputs
#define CYGHWR_HAL_STM32_GPIO_CNF_ANALOG        CYGHWR_HAL_STM32_GPIO_CNF_AIN
#define CYGHWR_HAL_STM32_GPIO_CNF_FLOATING      CYGHWR_HAL_STM32_GPIO_CNF_FIN
#define CYGHWR_HAL_STM32_GPIO_CNF_PULLDOWN      (CYGHWR_HAL_STM32_GPIO_CNF_PULL)
#define CYGHWR_HAL_STM32_GPIO_CNF_PULLUP        (CYGHWR_HAL_STM32_GPIO_CNF_PULL|CYGHWR_HAL_STM32_GPIO_PULLUP)
// Outputs
#define CYGHWR_HAL_STM32_GPIO_CNF_OUT_OPENDRAIN CYGHWR_HAL_STM32_GPIO_CNF_GPOOD
#define CYGHWR_HAL_STM32_GPIO_CNF_OUT_PUSHPULL  CYGHWR_HAL_STM32_GPIO_CNF_GPOPP
#define CYGHWR_HAL_STM32_GPIO_CNF_ALT_OPENDRAIN CYGHWR_HAL_STM32_GPIO_CNF_AOOD
#define CYGHWR_HAL_STM32_GPIO_CNF_ALT_PUSHPULL  CYGHWR_HAL_STM32_GPIO_CNF_AOPP


// This macro packs the port number, bit number, mode and
// configuration for a GPIO pin into a single word. The packing puts
// the mode and config in the ls 5 bits, the bit number in 16:20 and
// the offset of the GPIO port from GPIOA in bits 8:15. The port, mode
// and config are only specified using the last component of the names
// to keep definitions short.

#define CYGHWR_HAL_STM32_GPIO(__port, __bit, __mode, __cnf )            \
            ((CYGHWR_HAL_STM32_GPIO##__port - CYGHWR_HAL_STM32_GPIOA) | \
             (__bit<<16) |                                              \
             (CYGHWR_HAL_STM32_GPIO_MODE_##__mode) |                    \
             (CYGHWR_HAL_STM32_GPIO_CNF_##__cnf))

// Macros to extract encoded values
#define CYGHWR_HAL_STM32_GPIO_PORT(__pin)       (CYGHWR_HAL_STM32_GPIOA+((__pin)&0x0000FF00))
#define CYGHWR_HAL_STM32_GPIO_BIT(__pin)        (((__pin)>>16)&0x1F)
#define CYGHWR_HAL_STM32_GPIO_CFG(__pin)        ((__pin)&0xF)
#define CYGHWR_HAL_STM32_GPIO_PULLUP            BIT_(4)


#define CYGHWR_HAL_STM32_GPIO_NONE      (0xFFFFFFFF)

// Functions and macros to configure GPIO ports.

__externC void hal_stm32_gpio_set( cyg_uint32 pin );
__externC void hal_stm32_gpio_out( cyg_uint32 pin, int val );
__externC void hal_stm32_gpio_in ( cyg_uint32 pin, int *val );

#define CYGHWR_HAL_STM32_GPIO_SET(__pin ) hal_stm32_gpio_set( __pin )
#define CYGHWR_HAL_STM32_GPIO_OUT(__pin, __val ) hal_stm32_gpio_out( __pin, __val )
#define CYGHWR_HAL_STM32_GPIO_IN(__pin,  __val ) hal_stm32_gpio_in( __pin, __val )

//=============================================================================
// Alternate I/O configuration registers.

#define CYGHWR_HAL_STM32_AFIO_EVCR              0x00
#define CYGHWR_HAL_STM32_AFIO_MAPR              0x04
#define CYGHWR_HAL_STM32_AFIO_EXTICR1           0x08
#define CYGHWR_HAL_STM32_AFIO_EXTICR2           0x0C
#define CYGHWR_HAL_STM32_AFIO_EXTICR3           0x10
#define CYGHWR_HAL_STM32_AFIO_EXTICR4           0x14

// The following macro allows the four ETXICR registers to be indexed
// as CYGHWR_HAL_STM32_AFIO_EXTICR(1) to CYGHWR_HAL_STM32_AFIO_EXTICR(4)
#define CYGHWR_HAL_STM32_AFIO_EXTICR(__x)       (4*((__x)-1)+0x08)

#define CYGHWR_HAL_STM32_AFIO_EVCR_PIN(__x)     VALUE_(0,(__x))
#define CYGHWR_HAL_STM32_AFIO_EVCR_PORTA        VALUE_(4,0) 
#define CYGHWR_HAL_STM32_AFIO_EVCR_PORTB        VALUE_(4,1) 
#define CYGHWR_HAL_STM32_AFIO_EVCR_PORTC        VALUE_(4,2) 
#define CYGHWR_HAL_STM32_AFIO_EVCR_PORTD        VALUE_(4,3) 
#define CYGHWR_HAL_STM32_AFIO_EVCR_PORTE        VALUE_(4,4)
#define CYGHWR_HAL_STM32_AFIO_EVCR_EVOE         BIT_(7) 

#define CYGHWR_HAL_STM32_AFIO_MAPR_SPI1_RMP     BIT_(0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_I2C1_RMP     BIT_(1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_URT1_RMP     BIT_(2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_URT2_RMP     BIT_(3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_URT3_NO_RMP  VALUE_(4,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_URT3_P1_RMP  VALUE_(4,1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_URT3_FL_RMP  VALUE_(4,3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM1_NO_RMP  VALUE_(6,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM1_P1_RMP  VALUE_(6,1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM1_FL_RMP  VALUE_(6,3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM2_NO_RMP  VALUE_(8,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM2_P1_RMP  VALUE_(8,1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM2_P2_RMP  VALUE_(8,2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM2_FL_RMP  VALUE_(8,3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM3_NO_RMP  VALUE_(10,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM3_P2_RMP  VALUE_(10,2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM3_FL_RMP  VALUE_(10,3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM4_RMP     BIT_(12)

#define CYGHWR_HAL_STM32_AFIO_MAPR_CAN_NO_RMP   VALUE_(13,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_CAN_FL1_RMP  VALUE_(13,2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_CAN_FL2_RMP  VALUE_(13,3)

#define CYGHWR_HAL_STM32_AFIO_MAPR_PD01_RMP     BIT_(15)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM5CH4_RMP  BIT_(16)
#define CYGHWR_HAL_STM32_AFIO_MAPR_ADC1EINJ_RMP BIT_(17)
#define CYGHWR_HAL_STM32_AFIO_MAPR_ADC1EREG_RMP BIT_(18)
#define CYGHWR_HAL_STM32_AFIO_MAPR_ADC2EINJ_RMP BIT_(19)
#define CYGHWR_HAL_STM32_AFIO_MAPR_ADC2EREG_RMP BIT_(20)

#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_FULL     VALUE_(24,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_NORST    VALUE_(24,1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_SWDPEN   VALUE_(24,2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_SWDPDIS  VALUE_(24,4)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_MASK     VALUE_(24,7)

// The following macros are used to generate the bitfields for setting up
// external interrupts.  For example, CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTC(12)
// will generate the bitfield which when ORed into the EXTICR4 register will
// set up C12 as the external interrupt pin for the EXTI12 interrupt.
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTA(__x) VALUE_(4*((__x)&3),0)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTB(__x) VALUE_(4*((__x)&3),1)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTC(__x) VALUE_(4*((__x)&3),2)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTD(__x) VALUE_(4*((__x)&3),3)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTE(__x) VALUE_(4*((__x)&3),4)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTF(__x) VALUE_(4*((__x)&3),5)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_PORTG(__x) VALUE_(4*((__x)&3),6)
#define CYGHWR_HAL_STM32_AFIO_EXTICRX_MASK(__x)  VALUE_(4*((__x)&3),0xF)

//=============================================================================
// DMA controller register definitions.

#define CYGHWR_HAL_STM32_DMA_ISR                0x00
#define CYGHWR_HAL_STM32_DMA_IFCR               0x04

// The following macros allow access to the per-channel DMA registers, indexed
// by channel number.  Valid channel numbers are 1 to 7 for DMA1 and 1 to 5
// for DMA2.
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
#define CYGHWR_HAL_STM32_DMA_CCR_PLLOW          VALUE_(12,0)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMEDIUM       VALUE_(12,1)
#define CYGHWR_HAL_STM32_DMA_CCR_PLHIGH         VALUE_(12,2)
#define CYGHWR_HAL_STM32_DMA_CCR_PLMAX          VALUE_(12,3)
#define CYGHWR_HAL_STM32_DMA_CCR_MEM2MEM        BIT_(14)

//=============================================================================
// UARTs

#define CYGHWR_HAL_STM32_UART_SR                0x00
#define CYGHWR_HAL_STM32_UART_DR                0x04
#define CYGHWR_HAL_STM32_UART_BRR               0x08
#define CYGHWR_HAL_STM32_UART_CR1               0x0C
#define CYGHWR_HAL_STM32_UART_CR2               0x10
#define CYGHWR_HAL_STM32_UART_CR3               0x14
#define CYGHWR_HAL_STM32_UART_GTPR              0x18

// SR Bits

#define CYGHWR_HAL_STM32_UART_SR_PE             BIT_(0)
#define CYGHWR_HAL_STM32_UART_SR_FE             BIT_(1)
#define CYGHWR_HAL_STM32_UART_SR_NE             BIT_(2)
#define CYGHWR_HAL_STM32_UART_SR_ORE            BIT_(3)
#define CYGHWR_HAL_STM32_UART_SR_IDLE           BIT_(4)
#define CYGHWR_HAL_STM32_UART_SR_RXNE           BIT_(5)
#define CYGHWR_HAL_STM32_UART_SR_TC             BIT_(6)
#define CYGHWR_HAL_STM32_UART_SR_TXE            BIT_(7)
#define CYGHWR_HAL_STM32_UART_SR_LBD            BIT_(8)
#define CYGHWR_HAL_STM32_UART_SR_CTS            BIT_(9)

// BRR bits

#define CYGHWR_HAL_STM32_UART_DR_DIVF(__f)      VALUE_(0,__f)
#define CYGHWR_HAL_STM32_UART_DR_DIVM(__m)      VALUE_(4,__m)

// CR1 bits

#define CYGHWR_HAL_STM32_UART_CR1_SBK           BIT_(0)
#define CYGHWR_HAL_STM32_UART_CR1_RWU           BIT_(1)
#define CYGHWR_HAL_STM32_UART_CR1_RE            BIT_(2)
#define CYGHWR_HAL_STM32_UART_CR1_TE            BIT_(3)
#define CYGHWR_HAL_STM32_UART_CR1_IDLEIE        BIT_(4)
#define CYGHWR_HAL_STM32_UART_CR1_RXNEIE        BIT_(5)
#define CYGHWR_HAL_STM32_UART_CR1_TCIE          BIT_(6)
#define CYGHWR_HAL_STM32_UART_CR1_TXEIE         BIT_(7)
#define CYGHWR_HAL_STM32_UART_CR1_PEIE          BIT_(8)
#define CYGHWR_HAL_STM32_UART_CR1_PS_EVEN       0
#define CYGHWR_HAL_STM32_UART_CR1_PS_ODD        BIT_(9)
#define CYGHWR_HAL_STM32_UART_CR1_PCE           BIT_(10)
#define CYGHWR_HAL_STM32_UART_CR1_WAKE          BIT_(11)
#define CYGHWR_HAL_STM32_UART_CR1_M_8           0
#define CYGHWR_HAL_STM32_UART_CR1_M_9           BIT_(12)
#define CYGHWR_HAL_STM32_UART_CR1_UE            BIT_(13)

// CR2 bits

#define CYGHWR_HAL_STM32_UART_CR2_ADD(__a)      VALUE_(0,__a)
#define CYGHWR_HAL_STM32_UART_CR2_LBDL          BIT_(5)
#define CYGHWR_HAL_STM32_UART_CR2_LBDIE         BIT_(6)
#define CYGHWR_HAL_STM32_UART_CR2_LBCL          BIT_(8)
#define CYGHWR_HAL_STM32_UART_CR2_CPHA          BIT_(9)
#define CYGHWR_HAL_STM32_UART_CR2_CPOL          BIT_(10)
#define CYGHWR_HAL_STM32_UART_CR2_CLKEN         BIT_(11)
#define CYGHWR_HAL_STM32_UART_CR2_STOP_1        VALUE_(12,0)
#define CYGHWR_HAL_STM32_UART_CR2_STOP_0_5      VALUE_(12,1)
#define CYGHWR_HAL_STM32_UART_CR2_STOP_2        VALUE_(12,2)
#define CYGHWR_HAL_STM32_UART_CR2_STOP_1_5      VALUE_(12,3)
#define CYGHWR_HAL_STM32_UART_CR2_LINEN         BIT_(14)

// CR3 bits

#define CYGHWR_HAL_STM32_UART_CR3_EIE           BIT_(0)
#define CYGHWR_HAL_STM32_UART_CR3_IREN          BIT_(1)
#define CYGHWR_HAL_STM32_UART_CR3_IRLP          BIT_(2)
#define CYGHWR_HAL_STM32_UART_CR3_HDSEL         BIT_(3)
#define CYGHWR_HAL_STM32_UART_CR3_NACK          BIT_(4)
#define CYGHWR_HAL_STM32_UART_CR3_SCEN          BIT_(5)
#define CYGHWR_HAL_STM32_UART_CR3_DMAR          BIT_(6)
#define CYGHWR_HAL_STM32_UART_CR3_DMAT          BIT_(7)
#define CYGHWR_HAL_STM32_UART_CR3_RTSE          BIT_(8)
#define CYGHWR_HAL_STM32_UART_CR3_CTSE          BIT_(9)
#define CYGHWR_HAL_STM32_UART_CR3_CTSIE         BIT_(10)

// GTPR fields

#define CYGHWR_HAL_STM32_UART_GTPR_PSC(__p)     VALUE_(0,__p)
#define CYGHWR_HAL_STM32_UART_GTPR_GT(__g)      VALUE_(8,__g)

// UART GPIO pins

#define CYGHWR_HAL_STM32_UART1_RX               CYGHWR_HAL_STM32_GPIO( A, 10, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART1_TX               CYGHWR_HAL_STM32_GPIO( A,  9, OUT_50MHZ , ALT_PUSHPULL  )
#define CYGHWR_HAL_STM32_UART1_CTS              CYGHWR_HAL_STM32_GPIO( A, 11, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART1_RTS              CYGHWR_HAL_STM32_GPIO( A, 12, OUT_50MHZ , OUT_PUSHPULL  )

#define CYGHWR_HAL_STM32_UART2_RX               CYGHWR_HAL_STM32_GPIO( A,  3, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART2_TX               CYGHWR_HAL_STM32_GPIO( A,  2, OUT_50MHZ , ALT_PUSHPULL  )
#define CYGHWR_HAL_STM32_UART2_CTS              CYGHWR_HAL_STM32_GPIO( A,  0, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART2_RTS              CYGHWR_HAL_STM32_GPIO( A,  1, OUT_50MHZ , OUT_PUSHPULL  )

#define CYGHWR_HAL_STM32_UART3_RX               CYGHWR_HAL_STM32_GPIO( B, 11, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART3_TX               CYGHWR_HAL_STM32_GPIO( B, 10, OUT_50MHZ , ALT_PUSHPULL  )
#define CYGHWR_HAL_STM32_UART3_CTS              CYGHWR_HAL_STM32_GPIO( B, 13, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART3_RTS              CYGHWR_HAL_STM32_GPIO( B, 14, OUT_50MHZ , OUT_PUSHPULL  )

#define CYGHWR_HAL_STM32_UART4_RX               CYGHWR_HAL_STM32_GPIO( C, 11, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART4_TX               CYGHWR_HAL_STM32_GPIO( C, 10, OUT_50MHZ , ALT_PUSHPULL  )
#define CYGHWR_HAL_STM32_UART4_CTS              CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_UART4_RTS              CYGHWR_HAL_STM32_GPIO_NONE

#define CYGHWR_HAL_STM32_UART5_RX               CYGHWR_HAL_STM32_GPIO( C,  2, IN        , FLOATING      )
#define CYGHWR_HAL_STM32_UART5_TX               CYGHWR_HAL_STM32_GPIO( C, 12, OUT_50MHZ , ALT_PUSHPULL  )
#define CYGHWR_HAL_STM32_UART5_CTS              CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_UART5_RTS              CYGHWR_HAL_STM32_GPIO_NONE

#ifndef __ASSEMBLER__

__externC void hal_stm32_uart_setbaud( CYG_ADDRESS uart, cyg_uint32 baud );

#endif

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
#define CYGHWR_HAL_STM32_ADC_CR1_DUALMODE(__x)  VALUE_(16,(__x))
#define CYGHWR_HAL_STM32_ADC_CR1_JAWDEN         BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CR1_AWDEN          BIT_(23)

// CR2 fields

#define CYGHWR_HAL_STM32_ADC_CR2_ADON           BIT_(0)
#define CYGHWR_HAL_STM32_ADC_CR2_CONT           BIT_(1)
#define CYGHWR_HAL_STM32_ADC_CR2_CAL            BIT_(2)
#define CYGHWR_HAL_STM32_ADC_CR2_RSTCAL         BIT_(3)
#define CYGHWR_HAL_STM32_ADC_CR2_DMA            BIT_(8)
#define CYGHWR_HAL_STM32_ADC_CR2_ALIGN          BIT_(11)
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTSEL(__x)   VALUE_(12,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_JEXTTRIG       BIT_(15)
#define CYGHWR_HAL_STM32_ADC_CR2_EXTSEL(__x)    VALUE_(17,(__x))
#define CYGHWR_HAL_STM32_ADC_CR2_EXTTRIG        BIT_(20)
#define CYGHWR_HAL_STM32_ADC_CR2_JSWSTART       BIT_(21)
#define CYGHWR_HAL_STM32_ADC_CR2_SWSTART        BIT_(22)
#define CYGHWR_HAL_STM32_ADC_CR2_TSVREFE        BIT_(23)

// SMPRx fields

#define CYGHWR_HAL_STM32_ADC_SMPRx_SMP(__x, __y) VALUE_((__x) * 3, (__y))

// SQRx fields

#define CYGHWR_HAL_STM32_ADC_SQR1_L(__x)        VALUE_(20, (__x))
#define CYGHWR_HAL_STM32_ADC_SQRx_SQ(__x, __y)  VALUE_((__x) * 5, (__y))

// JSQR fields

#define CYGHWR_HAL_STM32_ADC_JSQR_SQ(__x, __y)  VALUE_((__x) * 5, (__y))

// ADC GPIO pins

#define CYGHWR_HAL_STM32_ADC123_IN0             CYGHWR_HAL_STM32_GPIO( A, 0,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN1             CYGHWR_HAL_STM32_GPIO( A, 1,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN2             CYGHWR_HAL_STM32_GPIO( A, 2,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN3             CYGHWR_HAL_STM32_GPIO( A, 3,  IN, ANALOG )

#define CYGHWR_HAL_STM32_ADC12_IN4              CYGHWR_HAL_STM32_GPIO( A, 4,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC12_IN5              CYGHWR_HAL_STM32_GPIO( A, 5,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC12_IN6              CYGHWR_HAL_STM32_GPIO( A, 6,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC12_IN7              CYGHWR_HAL_STM32_GPIO( A, 7,  IN, ANALOG )

#define CYGHWR_HAL_STM32_ADC12_IN8              CYGHWR_HAL_STM32_GPIO( B, 0,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC12_IN9              CYGHWR_HAL_STM32_GPIO( B, 1,  IN, ANALOG )

#define CYGHWR_HAL_STM32_ADC3_IN4               CYGHWR_HAL_STM32_GPIO( F, 6,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC3_IN5               CYGHWR_HAL_STM32_GPIO( F, 7,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC3_IN6               CYGHWR_HAL_STM32_GPIO( F, 8,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC3_IN7               CYGHWR_HAL_STM32_GPIO( F, 9,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC3_IN8               CYGHWR_HAL_STM32_GPIO( F, 10, IN, ANALOG )

#define CYGHWR_HAL_STM32_ADC123_IN10            CYGHWR_HAL_STM32_GPIO( C, 0,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN11            CYGHWR_HAL_STM32_GPIO( C, 1,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN12            CYGHWR_HAL_STM32_GPIO( C, 2,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC123_IN13            CYGHWR_HAL_STM32_GPIO( C, 3,  IN, ANALOG )

#define CYGHWR_HAL_STM32_ADC12_IN14             CYGHWR_HAL_STM32_GPIO( C, 4,  IN, ANALOG )
#define CYGHWR_HAL_STM32_ADC12_IN15             CYGHWR_HAL_STM32_GPIO( C, 5,  IN, ANALOG )

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
// Inputs 4 - 8 are already defined
#define CYGHWR_HAL_STM32_ADC3_IN9               CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_ADC3_IN10              CYGHWR_HAL_STM32_ADC123_IN10
#define CYGHWR_HAL_STM32_ADC3_IN11              CYGHWR_HAL_STM32_ADC123_IN11
#define CYGHWR_HAL_STM32_ADC3_IN12              CYGHWR_HAL_STM32_ADC123_IN12
#define CYGHWR_HAL_STM32_ADC3_IN13              CYGHWR_HAL_STM32_ADC123_IN13
#define CYGHWR_HAL_STM32_ADC3_IN14              CYGHWR_HAL_STM32_GPIO_NONE
#define CYGHWR_HAL_STM32_ADC3_IN15              CYGHWR_HAL_STM32_GPIO_NONE

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

//=============================================================================
// USB interface register definitions.

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

#ifndef __ASSEMBLER__

__externC cyg_uint32 hal_stm32_timer_clock( CYG_ADDRESS base );

#endif

//=============================================================================
// Flash controller

#define CYGHWR_HAL_STM32_FLASH_ACR              0x00
#define CYGHWR_HAL_STM32_FLASH_KEYR             0x04
#define CYGHWR_HAL_STM32_FLASH_OPTKEYR          0x08
#define CYGHWR_HAL_STM32_FLASH_SR               0x0C
#define CYGHWR_HAL_STM32_FLASH_CR               0x10
#define CYGHWR_HAL_STM32_FLASH_AR               0x14
#define CYGHWR_HAL_STM32_FLASH_OBR              0x1C
#define CYGHWR_HAL_STM32_FLASH_WRPR             0x20

// Key values

#define CYGHWR_HAL_STM32_FLASH_KEYR_KEY1        0x45670123
#define CYGHWR_HAL_STM32_FLASH_KEYR_KEY2        0xCDEF89AB

// ACR fields

#define CYGHWR_HAL_STM32_FLASH_ACR_LATENCY(__x) VALUE_(0,__x)
#define CYGHWR_HAL_STM32_FLASH_ACR_HLFCYA       BIT_(3)
#define CYGHWR_HAL_STM32_FLASH_ACR_PRFTBE       BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_ACR_PRFTBS       BIT_(5)

// SR fields

#define CYGHWR_HAL_STM32_FLASH_SR_BSY           BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_SR_PGERR         BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_SR_WRPRTERR      BIT_(4)
#define CYGHWR_HAL_STM32_FLASH_SR_EOP           BIT_(5)

// CR fields

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

// OBR fields

#define CYGHWR_HAL_STM32_FLASH_OBR_OPTERR       BIT_(0)
#define CYGHWR_HAL_STM32_FLASH_OBR_RDPRT        BIT_(1)
#define CYGHWR_HAL_STM32_FLASH_OBR_WDG_SW       BIT_(2)
#define CYGHWR_HAL_STM32_FLASH_OBR_nRST_STOP    BIT_(3)
#define CYGHWR_HAL_STM32_FLASH_OBR_nRST_STDBY   BIT_(4)

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

// CSR fields

#define CYGHWR_HAL_STM32_PWR_CSR_WUF            BIT_(0)
#define CYGHWR_HAL_STM32_PWR_CSR_SBF            BIT_(1)
#define CYGHWR_HAL_STM32_PWR_CSR_PVDO           BIT_(2)
#define CYGHWR_HAL_STM32_PWR_CSR_EWUP           BIT_(8)

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

#define CYGHWR_HAL_STM32_FSMC_PCR4              0xC0
#define CYGHWR_HAL_STM32_FSMC_SR4               0xC4
#define CYGHWR_HAL_STM32_FSMC_PMEM4             0xC8
#define CYGHWR_HAL_STM32_FSMC_PATT4             0xCC

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

//-----------------------------------------------------------------------------
// end of var_io.h
#endif // CYGONCE_HAL_VAR_IO_H
