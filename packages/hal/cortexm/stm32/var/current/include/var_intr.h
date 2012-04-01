#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H
//==========================================================================
//
//      var_intr.h
//
//      HAL Interrupt and clock assignments for STM32 variants
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-10-06
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for ST STM32 variants are
//               defined here.
//              
// Usage:        #include <cyg/hal/var_intr.h>
//               However applications should include using <cyg/hal/hal_intr.h>
//               instead to allow for platform overrides.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/plf_intr.h>

//==========================================================================


#define CYGNUM_HAL_INTERRUPT_WWDG               ( 0+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PVD                ( 1+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TAMPER             ( 2+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TAMP_STAMP         ( 2+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name - also timestamps
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_RTC_GLOBAL         ( 3+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGNUM_HAL_INTERRUPT_RTC_WKUP           ( 3+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#define CYGNUM_HAL_INTERRUPT_FLASH              ( 4+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_RCC                ( 5+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EXTI0              ( 6+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EXTI1              ( 7+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EXTI2              ( 8+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EXTI3              ( 9+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_EXTI4              (10+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA1_CH1           (11+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR0          (11+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH2           (12+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR1          (12+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH3           (13+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR2          (13+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH4           (14+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR3          (14+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH5           (15+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR4          (15+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH6           (16+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR5          (16+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA1_CH7           (17+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA1_STR6          (17+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_ADC1_2             (18+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 only has ADC1+2
#define CYGNUM_HAL_INTERRUPT_ADC1_2_3           (18+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has ADC1+2+3
#define CYGNUM_HAL_INTERRUPT_ADC                (18+CYGNUM_HAL_INTERRUPT_EXTERNAL) // More generic name
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_USB_HP             (19+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#define CYGNUM_HAL_INTERRUPT_CAN1_TX            (19+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_USB_LP             (20+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#define CYGNUM_HAL_INTERRUPT_CAN1_RX0           (20+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN1_RX1           (21+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN1_SCE           (22+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EXTI9_5            (23+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM1_BRK           (24+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM1_BRK_TIM9      (24+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM9
#define CYGNUM_HAL_INTERRUPT_TIM1_UP            (25+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM1_UP_TIM10      (25+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM10
#define CYGNUM_HAL_INTERRUPT_TIM1_TRG           (26+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM1_TRG_COM_TIM11 (26+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM11
#define CYGNUM_HAL_INTERRUPT_TIM1_CC            (27+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM2               (28+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM3               (29+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_TIM4               (30+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C1_EV            (31+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C1_EE            (32+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C2_EV            (33+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C2_EE            (34+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SPI1               (35+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SPI2               (36+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART1              (37+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART2              (38+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART3              (39+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_EXTI15_10          (40+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_RTC_ALARM          (41+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_USB_WAKEUP         (42+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGNUM_HAL_INTERRUPT_OTG_FS_WKUP        (42+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#ifndef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGNUM_HAL_INTERRUPT_TIM8_BRK           (43+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM8_UP            (44+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM8_TRG           (45+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM8_CC            (46+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_ADC3               (47+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#define CYGNUM_HAL_INTERRUPT_FSMC               (48+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SDIO               (49+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#else
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGNUM_HAL_INTERRUPT_TIM8_BRK_TIM12     (43+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM12
#define CYGNUM_HAL_INTERRUPT_TIM8_UP_TIM13      (44+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM13
#define CYGNUM_HAL_INTERRUPT_TIM8_TRG_COM_TIM14 (45+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because is also TIM13
#define CYGNUM_HAL_INTERRUPT_TIM8_CC            (46+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA1_STR7          (47+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_FSMC               (48+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SDIO               (49+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#endif
#endif

#define CYGNUM_HAL_INTERRUPT_TIM5               (50+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SPI3               (51+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART4              (52+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART5              (53+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM6               (54+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIM6_DAC           (54+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 name because also DAC1+2 underrun
#define CYGNUM_HAL_INTERRUPT_TIM7               (55+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA2_CH1           (56+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA2_STR0          (56+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA2_CH2           (57+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA2_STR1          (57+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_DMA2_CH3           (58+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA2_STR2          (58+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams

#ifndef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
# error Support does not yet exist for F2 or F4 without connectivity
#endif

#define CYGNUM_HAL_INTERRUPT_DMA2_CH4           (59+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA2_CH5           (CYGNUM_HAL_INTERRUPT_DMA2_CH4) // As per RM0008 datasheet 3.3.6 note

#define CYGNUM_HAL_INTERRUPT_NVIC_MAX           (59+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#else

#define CYGNUM_HAL_INTERRUPT_DMA2_CH4           (59+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA2_STR3          (59+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams

#define CYGNUM_HAL_INTERRUPT_DMA2_CH5           (60+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F1 has channels
#define CYGNUM_HAL_INTERRUPT_DMA2_STR4          (60+CYGNUM_HAL_INTERRUPT_EXTERNAL) // F2/F4 has streams
#define CYGNUM_HAL_INTERRUPT_ETH                (61+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ETH_WAKEUP         (62+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN2_TX            (63+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN2_RX0           (64+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN2_RX1           (65+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN2_SCE           (66+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGNUM_HAL_INTERRUPT_USB_FS             (67+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_NVIC_MAX           (67+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGNUM_HAL_INTERRUPT_OTG_FS             (67+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA2_STR5          (68+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA2_STR6          (69+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA2_STR7          (70+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART6              (71+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C3_EV            (72+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C3_ER            (73+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_OTG_HS_EP1_OUT     (74+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_OTG_HS_EP1_IN      (75+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_OTG_HS_WKUP        (76+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_OTG_HS             (77+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DCMI               (78+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CRYP               (79+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_HASH_RNG           (80+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
# define CYGNUM_HAL_INTERRUPT_HASH_FPU          (81+CYGNUM_HAL_INTERRUPT_EXTERNAL)

# define CYGNUM_HAL_INTERRUPT_NVIC_MAX          (81+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F2)
# define CYGNUM_HAL_INTERRUPT_NVIC_MAX          (80+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#else
# error "Support does not yet exist for this FAMILY_HIPERFORMANCE configuration"
#endif
#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#endif // ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY

#define CYGNUM_HAL_INTERRUPT_EXTI5              ( 1+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI6              ( 2+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI7              ( 3+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI8              ( 4+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI9              ( 5+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI10             ( 6+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI11             ( 7+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI12             ( 8+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI13             ( 9+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI14             (10+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#define CYGNUM_HAL_INTERRUPT_EXTI15             (11+CYGNUM_HAL_INTERRUPT_NVIC_MAX)

#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     CYGNUM_HAL_INTERRUPT_EXTI15
#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX + 1)

#define CYGNUM_HAL_VSR_MIN                     0
#ifndef CYGNUM_HAL_VSR_MAX
#define CYGNUM_HAL_VSR_MAX                     (CYGNUM_HAL_VECTOR_SYS_TICK+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#endif
#define CYGNUM_HAL_VSR_COUNT                   (CYGNUM_HAL_VSR_MAX+1)

//==========================================================================
// Interrupt mask and config for variant-specific devices

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                  \
    case CYGNUM_HAL_INTERRUPT_PVD:                                      \
        __v =  16;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_RTC_ALARM:                                \
        __v =  17;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_USB_WAKEUP:                               \
        __v =  18;                                                      \
        break;

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                  \
    case CYGNUM_HAL_INTERRUPT_PVD:                                      \
        __v =  16;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_RTC_ALARM:                                \
        __v =  17;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_OTG_FS_WKUP:                              \
        __v =  18;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_ETH_WAKEUP:                               \
        __v =  19;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_OTG_HS_WKUP:                              \
        __v =  20;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_TAMP_STAMP:                               \
        __v =  21;                                                      \
        break;                                                          \
    case CYGNUM_HAL_INTERRUPT_RTC_WKUP:                                 \
        __v =  22;                                                      \
        break;

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define HAL_VAR_INTERRUPT_MASK( __vector )                              \
{                                                                       \
    cyg_int32 __v = -1;                                                 \
                                                                        \
    switch( __vector )                                                  \
    {                                                                   \
    case CYGNUM_HAL_INTERRUPT_EXTI0...CYGNUM_HAL_INTERRUPT_EXTI4:       \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI0;                    \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI5...CYGNUM_HAL_INTERRUPT_EXTI9:       \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI10...CYGNUM_HAL_INTERRUPT_EXTI15:     \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                      \
    }                                                                   \
                                                                        \
    if( __v >= 0 )                                                      \
    {                                                                   \
        cyg_uint32 __imr;                                               \
        HAL_READ_UINT32( CYGHWR_HAL_STM32_EXTI+CYGHWR_HAL_STM32_EXTI_IMR, __imr ); \
        __imr &= ~CYGHWR_HAL_STM32_EXTI_BIT(__v);                       \
        HAL_WRITE_UINT32( CYGHWR_HAL_STM32_EXTI+CYGHWR_HAL_STM32_EXTI_IMR, __imr ); \
    }                                                                   \
}

#define HAL_VAR_INTERRUPT_UNMASK( __vector )                            \
{                                                                       \
    cyg_int32 __v = -1;                                                 \
                                                                        \
    switch( __vector )                                                  \
    {                                                                   \
    case CYGNUM_HAL_INTERRUPT_EXTI0...CYGNUM_HAL_INTERRUPT_EXTI4:       \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI0;                    \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI5...CYGNUM_HAL_INTERRUPT_EXTI9:       \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;                \
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SER(CYGNUM_HAL_INTERRUPT_EXTI9_5-CYGNUM_HAL_INTERRUPT_EXTERNAL), \
                          CYGARC_REG_NVIC_IBIT(CYGNUM_HAL_INTERRUPT_EXTI9_5-CYGNUM_HAL_INTERRUPT_EXTERNAL) ); \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI10...CYGNUM_HAL_INTERRUPT_EXTI15:     \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;                \
        HAL_WRITE_UINT32( CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_SER(CYGNUM_HAL_INTERRUPT_EXTI15_10-CYGNUM_HAL_INTERRUPT_EXTERNAL), \
                          CYGARC_REG_NVIC_IBIT(CYGNUM_HAL_INTERRUPT_EXTI15_10-CYGNUM_HAL_INTERRUPT_EXTERNAL) ); \
        break;                                                          \
                                                                        \
    HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                      \
    }                                                                   \
                                                                        \
    if( __v >= 0 )                                                      \
    {                                                                   \
        cyg_uint32 __imr;                                               \
        HAL_READ_UINT32( CYGHWR_HAL_STM32_EXTI+CYGHWR_HAL_STM32_EXTI_IMR, __imr ); \
        __imr |= CYGHWR_HAL_STM32_EXTI_BIT(__v);                        \
        HAL_WRITE_UINT32( CYGHWR_HAL_STM32_EXTI+CYGHWR_HAL_STM32_EXTI_IMR, __imr ); \
    }                                                                   \
}

#define HAL_VAR_INTERRUPT_SET_LEVEL( __vector, __level ) CYG_EMPTY_STATEMENT

#define HAL_VAR_INTERRUPT_ACKNOWLEDGE( __vector )                       \
{                                                                       \
    cyg_int32 __v = -1;                                                 \
                                                                        \
    switch( __vector )                                                  \
    {                                                                   \
    case CYGNUM_HAL_INTERRUPT_EXTI0...CYGNUM_HAL_INTERRUPT_EXTI4:       \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI0;                    \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI5...CYGNUM_HAL_INTERRUPT_EXTI9:       \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI10...CYGNUM_HAL_INTERRUPT_EXTI15:     \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                      \
    }                                                                   \
                                                                        \
    if( __v >= 0 )                                                      \
    {                                                                   \
        cyg_uint32 __bit = CYGHWR_HAL_STM32_EXTI_BIT(__v);              \
        HAL_WRITE_UINT32( CYGHWR_HAL_STM32_EXTI+CYGHWR_HAL_STM32_EXTI_PR, __bit ); \
    }                                                                   \
}

#define HAL_VAR_INTERRUPT_CONFIGURE( __vector, __level, __up )          \
{                                                                       \
    cyg_int32 __v = -1;                                                 \
                                                                        \
    switch( __vector )                                                  \
    {                                                                   \
    case CYGNUM_HAL_INTERRUPT_EXTI0...CYGNUM_HAL_INTERRUPT_EXTI4:       \
        __v = __vector - CYGNUM_HAL_INTERRUPT_EXTI0;                    \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI5...CYGNUM_HAL_INTERRUPT_EXTI9:       \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    case CYGNUM_HAL_INTERRUPT_EXTI10...CYGNUM_HAL_INTERRUPT_EXTI15:     \
        __v =  __vector - CYGNUM_HAL_INTERRUPT_EXTI5 + 5;               \
        break;                                                          \
                                                                        \
    HAL_VAR_PERIPH_EXTI_MAP_FAMILY                                      \
    }                                                                   \
                                                                        \
    if(( __v >= 0 ) && !(__level) )                                     \
    {                                                                   \
        cyg_uint32 __base = CYGHWR_HAL_STM32_EXTI;                      \
        cyg_uint32 __rtsr, __ftsr;                                      \
        cyg_uint32 __bit = CYGHWR_HAL_STM32_EXTI_BIT(__v);              \
        HAL_READ_UINT32( __base+CYGHWR_HAL_STM32_EXTI_RTSR, __rtsr );   \
        HAL_READ_UINT32( __base+CYGHWR_HAL_STM32_EXTI_FTSR, __ftsr );   \
        if( __up ) __rtsr |= __bit, __ftsr &= ~__bit;                   \
        else __ftsr |= __bit, __rtsr &= ~__bit;                         \
        HAL_WRITE_UINT32( __base+CYGHWR_HAL_STM32_EXTI_RTSR, __rtsr );  \
        HAL_WRITE_UINT32( __base+CYGHWR_HAL_STM32_EXTI_FTSR, __ftsr );  \
    }                                                                   \
}


//----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTR_H
// EOF var_intr.h
