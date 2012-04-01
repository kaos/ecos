#ifndef CYGONCE_HAL_VAR_IO_PINS_H
#define CYGONCE_HAL_VAR_IO_PINS_H
//=============================================================================
//
//      var_io_pins.h
//
//      Pin configuration and GPIO definitions
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.                        
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
// Author(s):   nickg, jlarmour
// Date:        2011-11-29
// Purpose:     STM32 variant GPIO and pin configuration specific registers
// Description: 
// Usage:       Do not include this header file directly. Instead:
//              #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#ifndef CYGONCE_HAL_VAR_IO_H
# error Do not include var_io_pins.h directly, use var_io.h
#endif

//=============================================================================
// GPIO ports - common manifests

#define CYGHWR_HAL_STM32_GPIO_OSPEED_NA (0) // Convenience define for ease of pin definitions (for F1 actually marks MODE as INPUT)

//=============================================================================
// GPIO ports - F1 family

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
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

#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_LOW (CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_MED (CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_FAST (CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_HIGH (CYGHWR_HAL_STM32_GPIO_MODE_OUT_FAST) // F1 limited to 50MHz

// The following allows compatible specification of speed with other parts
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_AT_LEAST(__mhz)  ( ((__mhz) <= 2) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ :   \
                                                        ((__mhz) <= 10) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ : \
                                                        ((__mhz) <= 50) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ : \
                                                          CYGHWR_HAL_STM32_GPIO_MODE_OUT_HIGH )

#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_AT_MOST(__mhz)  ( ((__mhz) < 10) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ :   \
                                                       ((__mhz) < 50) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ :  \
                                                       ((__mhz) < 100) ? CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ : \
                                                         CYGHWR_HAL_STM32_GPIO_MODE_OUT_HIGH )

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

// We treat the CNF and MODE fields as a single field to simplify the hardware register access. The CNFMODE fields are split across
// two registers (CRL/CRH) so the passed __pin needs to be in the range 0..7
#define CYGHWR_HAL_STM32_GPIO_CNFMODE_VAL(__pin, __cnfmode) ((__cnfmode) << ((__pin)<<2))
#define CYGHWR_HAL_STM32_GPIO_CNFMODE_SET(__pin, __cnfmode, __reg) ((__reg) &= ~MASK_((__pin<<2),4), \
                                                              (__reg) |= CYGHWR_HAL_STM32_GPIO_CNFMODE_VAL(__pin, __cnfmode))

// Macros to extract encoded values
#define CYGHWR_HAL_STM32_GPIO_PORT(__pin)       (CYGHWR_HAL_STM32_GPIOA+((__pin)&0x0000FF00))
#define CYGHWR_HAL_STM32_GPIO_BIT(__pin)        (((__pin)>>16)&0x1F)
#define CYGHWR_HAL_STM32_GPIO_CFG(__pin)        ((__pin)&0xF)
#define CYGHWR_HAL_STM32_GPIO_PULLUP            BIT_(4)

//=============================================================================
// Alternate I/O configuration registers.

#define CYGHWR_HAL_STM32_AFIO_EVCR              0x00
#define CYGHWR_HAL_STM32_AFIO_MAPR              0x04
#define CYGHWR_HAL_STM32_AFIO_EXTICR1           0x08
#define CYGHWR_HAL_STM32_AFIO_EXTICR2           0x0C
#define CYGHWR_HAL_STM32_AFIO_EXTICR3           0x10
#define CYGHWR_HAL_STM32_AFIO_EXTICR4           0x14

// The following macro allows the four EXTICR registers to be indexed
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

#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_AFIO_MAPR_ETH_RMP      BIT_(21)
#define CYGHWR_HAL_STM32_AFIO_MAPR_CAN2_RMP     BIT_(22)
#define CYGHWR_HAL_STM32_AFIO_MAPR_ETH_RMII     BIT_(23)
#endif

#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_FULL     VALUE_(24,0)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_NORST    VALUE_(24,1)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_SWDPEN   VALUE_(24,2)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_SWDPDIS  VALUE_(24,4)
#define CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_MASK     VALUE_(24,7)

#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY
#define CYGHWR_HAL_STM32_AFIO_MAPR_SPI3_RMP     BIT_(28)
#define CYGHWR_HAL_STM32_AFIO_MAPR_TIM2ITR1_RMP BIT_(29)
#define CYGHWR_HAL_STM32_AFIO_MAPR_PTP_PPS_RMP  BIT_(30)
#endif

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

// AFIO clock control

#define CYGHWR_HAL_STM32_AFIO_CLOCK             CYGHWR_HAL_STM32_CLOCK( APB2, AFIO )

#endif // if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
//=============================================================================
// GPIO ports - F2/F4 family

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// GPIO Register offsets.
#define CYGHWR_HAL_STM32_GPIO_MODER                   0x00
#define CYGHWR_HAL_STM32_GPIO_OTYPER                  0x04
#define CYGHWR_HAL_STM32_GPIO_OSPEEDR                 0x08
#define CYGHWR_HAL_STM32_GPIO_PUPDR                   0x0C
#define CYGHWR_HAL_STM32_GPIO_IDR                     0x10
#define CYGHWR_HAL_STM32_GPIO_ODR                     0x14
#define CYGHWR_HAL_STM32_GPIO_BSRR                    0x18
#define CYGHWR_HAL_STM32_GPIO_LCKR                    0x1C
#define CYGHWR_HAL_STM32_GPIO_AFRL                    0x20
#define CYGHWR_HAL_STM32_GPIO_AFRH                    0x24

// A helper macro just to allow access to a particular register
#define CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, __offset)  ((volatile cyg_uint32 *)( ((char*)__portbaseaddr) + __offset ))

// GPIO port mode register. 
#define CYGHWR_HAL_STM32_GPIO_MODE_GPIO_IN            (0)
#define CYGHWR_HAL_STM32_GPIO_MODE_GPIO_OUT           (1)
#define CYGHWR_HAL_STM32_GPIO_MODE_ALTFN              (2)
#define CYGHWR_HAL_STM32_GPIO_MODE_ANALOG             (3)
#define CYGHWR_HAL_STM32_GPIO_MODE_VAL(__pin, __mode) ((__mode) << ((__pin)<<1))
#define CYGHWR_HAL_STM32_GPIO_MODE_SET(__pin, __mode, __reg) ((__reg) &= ~MASK_((__pin<<1),2), \
                                                              (__reg) |= CYGHWR_HAL_STM32_GPIO_MODE_VAL(__pin, __mode))

// GPIO port output type register.
#define CYGHWR_HAL_STM32_GPIO_OTYPE_PUSHPULL          (0)
#define CYGHWR_HAL_STM32_GPIO_OTYPE_OPENDRAIN         (1)
#define CYGHWR_HAL_STM32_GPIO_OTYPE_VAL(__pin, __otype)   VALUE_(__pin,__otype)
#define CYGHWR_HAL_STM32_GPIO_OTYPE_SET(__pin, __otype, __reg)   ((__reg) &= ~BIT_(__pin), \
                                                                  (__reg) |= CYGHWR_HAL_STM32_GPIO_OTYPE_VAL(__pin, __otype))
#define CYGHWR_HAL_STM32_GPIO_OTYPE_NA                (0) // Convenience define for ease of pin definitions

// GPIO port output speed register.
#define CYGHWR_HAL_STM32_GPIO_OSPEED_LOW              (0)  // 2MHz
#define CYGHWR_HAL_STM32_GPIO_OSPEED_2MHZ             (0)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_MED              (1)  // 25MHz
#define CYGHWR_HAL_STM32_GPIO_OSPEED_25MHZ            (1)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_FAST             (2)  // 50MHz
#define CYGHWR_HAL_STM32_GPIO_OSPEED_50MHZ            (2)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_HIGH             (3)  // 100MHZ on 30pF, 80MHz on 15pF
#define CYGHWR_HAL_STM32_GPIO_OSPEED_BAL(__pin, __speed)  ((__speed) << ((__pin)<<1))
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SET(__pin, __speed, __reg)  ((__reg) &= ~MASK_((__pin<<1),2), \
                                                                  (__reg) |= CYGHWR_HAL_STM32_GPIO_OSPEED_BAL(__pin, __speed))

// The following allows compatible specification of speed with other parts
// which have different speed ranges e.g. F1
#define CYGHWR_HAL_STM32_GPIO_OSPEED_AT_LEAST(__mhz)  ( ((__mhz) <= 2) ? CYGHWR_HAL_STM32_GPIO_OSPEED_2MHZ :   \
                                                        ((__mhz) <= 25) ? CYGHWR_HAL_STM32_GPIO_OSPEED_25MHZ : \
                                                        ((__mhz) <= 50) ? CYGHWR_HAL_STM32_GPIO_OSPEED_50MHZ : \
                                                        CYGHWR_HAL_STM32_GPIO_OSPEED_HIGH )
  
#define CYGHWR_HAL_STM32_GPIO_OSPEED_AT_MOST(__mhz)  ( ((__mhz) < 25) ? CYGHWR_HAL_STM32_GPIO_OSPEED_2MHZ :   \
                                                       ((__mhz) < 50) ? CYGHWR_HAL_STM32_GPIO_OSPEED_25MHZ :  \
                                                       ((__mhz) < 100) ? CYGHWR_HAL_STM32_GPIO_OSPEED_50MHZ : \
                                                       CYGHWR_HAL_STM32_GPIO_OSPEED_HIGH )

// GPIO port pull-up/pull-down register.
#define CYGHWR_HAL_STM32_GPIO_PUPD_NONE               (0)
#define CYGHWR_HAL_STM32_GPIO_PUPD_PULLUP             (1)
#define CYGHWR_HAL_STM32_GPIO_PUPD_PULLDOWN           (2)
#define CYGHWR_HAL_STM32_GPIO_PUPD_VAL(__pin, __pupd) ((__pupd) << ((__pin)<<1))
#define CYGHWR_HAL_STM32_GPIO_PUPD_SET(__pin, __pupd, __reg) ((__reg) &= ~MASK_(((__pin)<<1),2), \
                                                              (__reg) |= CYGHWR_HAL_STM32_GPIO_PUPD_VAL(__pin, __pupd))

// GPIO port input data register.
#define CYGHWR_HAL_STM32_GPIO_IDR_GET(__portbaseaddr, __pin, __val) \
  ((__val) = ( *CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, CYGHWR_HAL_STM32_GPIO_IDR) >> (__pin)) & 1)

// GPIO port output data register.
// Don't encourage setting it here. Use GPIO_BSRR instead for that.
#define CYGHWR_HAL_STM32_GPIO_ODR_GET(__portbaseaddr, __pin, __val) \
  ((__val) = ( *CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, CYGHWR_HAL_STM32_GPIO_ODR) >> (__pin)) & 1)

// GPIO port bit set/reset register.
#define CYGHWR_HAL_STM32_GPIO_BSRR_SET(__portbaseaddr, __pin, __val) \
  ( *CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, CYGHWR_HAL_STM32_GPIO_BSRR) = (__val)?(1<<(__pin)):(1<<((__pin)+16)))

// GPIO port configuration lock register.
#define CYGHWR_HAL_STM32_GPIO_LCKR_LCKK               BIT_(16)
#define CYGHWR_HAL_STM32_GPIO_LCKR_LCK(__pin)         BIT_((__pin))

// GPIO alternate function low register.
#define CYGHWR_HAL_STM32_GPIO_AFRL0                   MASK_(0,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL1                   MASK_(4,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL2                   MASK_(8,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL3                   MASK_(12,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL4                   MASK_(16,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL5                   MASK_(20,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL6                   MASK_(24,4)
#define CYGHWR_HAL_STM32_GPIO_AFRL7                   MASK_(28,4)
// GPIO alternate function high register.
#define CYGHWR_HAL_STM32_GPIO_AFRH8                   MASK_(0,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH9                   MASK_(4,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH10                  MASK_(8,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH11                  MASK_(12,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH12                  MASK_(16,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH13                  MASK_(20,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH14                  MASK_(24,4)
#define CYGHWR_HAL_STM32_GPIO_AFRH15                  MASK_(28,4)

// Set alternate function. We try to keep this as a macro as most times the
// arguments will be constant so can easily be collapsed substantially by the
// compiler.
// Note, this is not interrupt-safe, unavoidably. Provide your own protection
// if that's needed, although in general this will happen at startup time.
#define CYGHWR_HAL_STM32_GPIO_AFR_SET(__portbaseaddr, __pin, __func)    \
  CYG_MACRO_START                                                       \
  cyg_uint32 __cur_afr, __mask;                                         \
  volatile cyg_uint32 *__reg;                                           \
  cyg_uint8 __reg_pin = (__pin);                                        \
  if (__pin < 8) {                                                      \
    __reg = CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, CYGHWR_HAL_STM32_GPIO_AFRL); \
  } else {                                                              \
    __reg = CYGHWR_HAL_STM32_GPIO_REG(__portbaseaddr, CYGHWR_HAL_STM32_GPIO_AFRH); \
    __reg_pin -= 8;                                                     \
  }                                                                     \
  HAL_READ_UINT32( __reg, __cur_afr );                                  \
  __mask = 0xf << (__reg_pin<<2);                                       \
  __cur_afr &= ~__mask;                                                 \
  __mask = (__func) << (__reg_pin<<2);                                  \
  __cur_afr |= __mask;                                                  \
  HAL_WRITE_UINT32( __reg, __cur_afr );                                 \
  CYG_MACRO_END


// This macro packs the port number, bit number, mode and
// configuration for a GPIO pin into a single word. The packing puts
// the GPIO bank at bits 16:19, the pin at bits 12:15, the mode (i.e. function)
// at bits 10:11, for ALTFN mode the specific mapping at bits 6:9, pushpull(0)
// or open drain(1) at bit 5, pull-up(1) pull-down (2) or neither(0) at
// bits 3:4, and speed at bits 0:2 (low, med, fast, high).
// To keep definitions short, we simplify most of the arguments so they
// can be passed in with only their last components.

// FIXME: This should be renamed to something like CYGHWR_HAL_STM32_PIN(... when
// bringing F1 into line with this way of declaring pins.

#define CYGHWR_HAL_STM32_GPIO(__port, __bit, __mode, __af, __ppod, __pupd, __speed) \
  ( ((CYGHWR_HAL_STM32_GPIO##__port - CYGHWR_HAL_STM32_GPIOA) << 6 ) |	\
    (__bit << 12) |							\
    (CYGHWR_HAL_STM32_GPIO_MODE_##__mode << 10) |			\
    (__af << 6) |                                                       \
    (CYGHWR_HAL_STM32_GPIO_OTYPE_##__ppod << 5) |			\
    (CYGHWR_HAL_STM32_GPIO_PUPD_##__pupd << 3) |			\
    (CYGHWR_HAL_STM32_GPIO_OSPEED_##__speed) )

// Macros to extract encoded values
#define CYGHWR_HAL_STM32_GPIO_PORT(__pin)       (CYGHWR_HAL_STM32_GPIOA+(((__pin)&0xF0000)>>6))
#define CYGHWR_HAL_STM32_GPIO_BIT(__pin)        (((__pin)>>12)&0xF)
#define CYGHWR_HAL_STM32_GPIO_MODE(__pin)       (((__pin)>>10)&0x3)
#define CYGHWR_HAL_STM32_GPIO_AF(__pin)         (((__pin)>>6)&0xF)
#define CYGHWR_HAL_STM32_GPIO_OPENDRAIN(__pin)  (((__pin)>>5)&0x1)
#define CYGHWR_HAL_STM32_GPIO_PULLUPDOWN(__pin) (((__pin)>>3)&0x3)
#define CYGHWR_HAL_STM32_GPIO_SPEED(__pin)       ((__pin)&0x7)

#endif  //if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

//=============================================================================

#define CYGHWR_HAL_STM32_GPIO_NONE      (0xFFFFFFFF)

// Functions and macros to configure GPIO ports.

__externC void hal_stm32_gpio_set( cyg_uint32 pin );
__externC void hal_stm32_gpio_out( cyg_uint32 pin, int val );
__externC void hal_stm32_gpio_in ( cyg_uint32 pin, int *val );

#define CYGHWR_HAL_STM32_GPIO_SET(__pin ) hal_stm32_gpio_set( __pin )
#define CYGHWR_HAL_STM32_GPIO_OUT(__pin, __val ) hal_stm32_gpio_out( __pin, __val )
#define CYGHWR_HAL_STM32_GPIO_IN(__pin,  __val ) hal_stm32_gpio_in( __pin, __val )

//-----------------------------------------------------------------------------

// For the following pin definition macros where __speed is a parameter the
// actual rates available depend on the target family. The generic LOW, MED,
// FAST and HIGH manifests can be used instead of explicit values, or more
// usefully the AT_LEAST(__mhz) and AT_MOST(__mhz) macros can be used to specify
// an acceptable limit instead.

// The CYGHWR_HAL_STM32_PIN_OUT() macro defines a GPIO output pin. The __ppod
// parameter can be one of PUSHPULL or OPENDRAIN. The __pupd parameter can be
// one of NONE, PULLUP or PULLDOWN. For F1 devices the __pupd parameter is
// not-relevant and is ignored.
// e.g.
//   CYGHWR_HAL_STM32_PIN_OUT(B,8,OPENDRAIN,NONE,FAST);
//   CYGHWR_HAL_STM32_PIN_OUT(B,9,OPENDRAIN,NONE.AT_LEAST(50));

// The CYGHWR_HAL_STM32_PIN_ALTFN_OUT() macro defines an alternative function
// output pin. For F1 family devices the __af field is not-relevant and is
// ignored. The __ppod should be PUSHPULL or OPENDRAIN. The __pupd parameter can
// be one of NONE, PULLUP or PULLDOWN. For F1 devices the __pupd parameter is
// not-relevant and is ignored.
// e.g.
//   CYGHWR_HAL_STM32_PIN_ALTFN_OUT(B,6,4,OPENDRAIN,NONE,MED);
//   CYGHWR_HAL_STM32_PIN_ALTFN_OUT(B,10,4,OPENDRAIN,NONE,AT_LEAST(10));

// The CYGHWR_HAL_STM32_PIN_IN() macro is used to define GPIO input pins. The
// __pupd should be one of NONE. FLOATING, PULLUP or PULLDOWN.
// e.g.
//   CYGHWR_HAL_STM32_PIN_IN(B,4,PULLUP);

// The CYGHWR_HAL_STM32_PIN_ALTFN_IN() macro is used to define alternate
// function input pins. The __ppod parameter can be one of PUSHPULL,
// OPENDRAIN or NA. The __pupd should be one of NONE. FLOATING, PULLUP or
// PULLDOWN. For F1 family devices the __af and _ppod fields are not relevant
// and are ignored, and in reality this macro peforms the same function as
// CYGHWR_HAL_STM32_PIN_IN() for F1 family devices since extra AFIO
// configuration is needed.
// e.g.
//   CYGHWR_HAL_STM32_PIN_ALTFN_IN(B,4,6,OPENDRAIN,PULLUP);

// The CYGHWR_HAL_STM32_PIN_ANALOG() macro defines an analog mode pin. For F1
// family devices this is for input only, e.g. ADC.

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#define CYGHWR_HAL_STM32_GPIO_CNF_NONE (CYGHWR_HAL_STM32_GPIO_CNF_FLOATING) // Should not be needed for F1 family but ensure HIPERFORMANCE compatible name NONE exists

#define CYGHWR_HAL_STM32_PIN_OUT(__port,__pin,__ppod,__pupd,__speed) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,OUT_##__speed,OUT_##__ppod)

#define CYGHWR_HAL_STM32_PIN_ALTFN_OUT(__port,__pin,__af,__ppod,__pupd,__speed) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,OUT_##__speed,ALT_##__ppod)

#define CYGHWR_HAL_STM32_PIN_IN(__port,__pin,__ppod,__pupd)     \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,IN,__pupd)

#define CYGHWR_HAL_STM32_PIN_ALTFN_IN(__port,__pin,__af,__ppod,__pupd)  \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,IN,__pupd) // NOTE: Identical to CYGHWR_HAL_STM32_PIN_IN() at the moment

#define CYGHWR_HAL_STM32_PIN_ANALOG(__port,__pin) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,IN,ANALOG)

#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#define CYGHWR_HAL_STM32_GPIO_PUPD_FLOATING (CYGHWR_HAL_STM32_GPIO_PUPD_NONE) // Should not be needed for HIPERFORMANCE family but ensure F1 compatible name FLOATING exists

#define CYGHWR_HAL_STM32_PIN_OUT(__port,__pin,__ppod,__pupd,__speed) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,GPIO_OUT,0,__ppod,__pupd,__speed)

#define CYGHWR_HAL_STM32_PIN_ALTFN_OUT(__port,__pin,__af,__ppod,__pupd,__speed) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,ALTFN,__af,__ppod,__pupd,__speed)

#define CYGHWR_HAL_STM32_PIN_IN(__port,__pin,__pupd) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,GPIO_IN,0,NA,__pupd,NA)

#define CYGHWR_HAL_STM32_PIN_ALTFN_IN(__port,__pin,__af,__ppod,__pupd)  \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,ALTFN,__af,__ppod,__pupd,NA)

#define CYGHWR_HAL_STM32_PIN_ANALOG(__port,__pin) \
  CYGHWR_HAL_STM32_GPIO(__port,__pin,ANALOG,0,NA,NONE,NA)

#else
#error "Unknown STM32 family for GPIO PIN macros"
#endif

//-----------------------------------------------------------------------------
// end of var_io_pins.h
#endif // CYGONCE_HAL_VAR_IO_PINS_H
