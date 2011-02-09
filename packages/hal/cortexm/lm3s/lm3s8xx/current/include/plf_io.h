#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Stellaris Cortex-M3 800 Series specific registers
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
// Purpose:
// Description:
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================
#include <pkgconf/hal_cortexm_lm3s8xx.h>

// Number of IOs on port A / B and C
#define CYGHWR_HAL_LM3S_GPIOA_IOs                 6
#define CYGHWR_HAL_LM3S_GPIOB_IOs                 8
#define CYGHWR_HAL_LM3S_GPIOC_IOs                 8

// Number of IOs on port D and E varies
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S800) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S801)
#define CYGHWR_HAL_LM3S_GPIOD_IOs                 8
#define CYGHWR_HAL_LM3S_GPIOE_IOs                 6
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S808) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S828)
#define CYGHWR_HAL_LM3S_GPIOD_IOs                 4
#define CYGHWR_HAL_LM3S_GPIOE_IOs                 2
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S811)
#define CYGHWR_HAL_LM3S_GPIOD_IOs                 8
#define CYGHWR_HAL_LM3S_GPIOE_IOs                 2
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S812) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S815)
#define CYGHWR_HAL_LM3S_GPIOD_IOs                 8
#define CYGHWR_HAL_LM3S_GPIOE_IOs                 4
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S817)|| \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S818)
#define CYGHWR_HAL_LM3S_GPIOD_IOs                 6
#define CYGHWR_HAL_LM3S_GPIOE_IOs                 2
#endif

// UART1
#define CYGHWR_HAL_LM3S_UART1_TX                  CYGHWR_HAL_LM3S_GPIO( D, 2, PERIPH, NONE, NONE, DISABLE )
#define CYGHWR_HAL_LM3S_UART1_RX                  CYGHWR_HAL_LM3S_GPIO( D, 3, PERIPH, NONE, NONE, DISABLE )
#define CYGHWR_HAL_LM3S_P_UART1_GPIO              CYGHWR_HAL_LM3S_P_GPIOD

// Number of ADC channel
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S812) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S815)
#define CYGHWR_HAL_LM3S_ADC0_CHAN                 2
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S808) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S811)
#define CYGHWR_HAL_LM3S_ADC0_CHAN                 4
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S817) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S818)
#define CYGHWR_HAL_LM3S_ADC0_CHAN                 6
#elif defined(CYGHWR_HAL_CORTEXM_CORTEXM_LM3S828) || \
        defined(CYGHWR_HAL_CORTEXM_CORTEXM_LM3S808)
#define CYGHWR_HAL_LM3S_ADC0_CHAN                 8
#endif

// Number of PWM channel
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S812)
#define CYGHWR_HAL_LM3S_MAX_PWM_CHAN              2
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S801) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S811) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S815) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S817) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S818)
#define CYGHWR_HAL_LM3S_PWM_CHAN                  6
#endif

// Number of I2C channels
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S800) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S801) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S808) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S811) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S815) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S828)
#define CYGHWR_HAL_LM3S_I2C_CHAN                  1
// I2C
#define CYGHWR_HAL_LM3S_I2C_SCL                   CYGHWR_HAL_LM3S_GPIO( B, 3, PERIPH, 2_MA, OP_PULLUP, DISABLE )
#define CYGHWR_HAL_LM3S_I2C_SDA                   CYGHWR_HAL_LM3S_GPIO( B, 2, PERIPH, 2_MA, OP_PULLUP, DISABLE )
#endif

// Number of analog Comparator
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S800) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S801) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S815)
#define CYGHWR_HAL_LM3S_AC_CHAN                   3
#elif defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S808) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S811) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S812) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S817) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S818)
#define CYGHWR_HAL_LM3S_AC_CHAN                   1
#endif

// Number of Quadrature Encoder Channel
#if defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S801) || \
        defined(CYGHWR_HAL_CORTEXM_LM3S8XX_LM3S818)
#define CYGHWR_HAL_LM3S_QEI_CHAN                  1
#endif

// SSI channel
#define CYGHWR_HAL_LM3S_SSI_CHAN                  1

// Global timer channel
#define CYGHWR_HAL_LM3S_GPTIM_CHAN                3

//-----------------------------------------------------------------------------
#endif //CYGONCE_HAL_PLF_IO_H
// EOF plf_io.h
