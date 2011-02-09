#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H
//==========================================================================
//
//      var_intr.h
//
//      HAL Interrupt and clock assignments for Stellaris Cortex-M3 variants
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    ccoutand
// Date:         2011-01-18
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for Stellaris Cortex-M3
//               are defined here.
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

#define CYGNUM_HAL_INTERRUPT_GPIOA          (0+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOB          (1+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOC          (2+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOD          (3+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOE          (4+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART0          (5+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART1          (6+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SSI0           (7+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C            (8+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PWM0           (10+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PWM1           (11+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PWM2           (12+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_QIE            (13+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ADC0_S0        (14+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ADC0_S1        (15+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ADC0_S2        (16+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ADC0_S3        (17+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_WDT            (18+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM0_A        (19+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM0_B        (20+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM1_A        (21+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM1_B        (22+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM2_A        (23+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM2_B        (24+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_AC0            (25+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_AC1            (26+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_AC2            (27+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SCTL           (28+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_FMS            (29+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOF          (30+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOG          (31+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GPIOH          (32+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM3_A        (35+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_GTIM3_B        (36+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN0           (39+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ETH0           (42+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_HIM            (43+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#ifndef CYGNUM_HAL_INTERRUPT_NVIC_MAX
# define CYGNUM_HAL_INTERRUPT_NVIC_MAX      (CYGNUM_HAL_INTERRUPT_HIM)
#endif

#define CYGNUM_HAL_ISR_MIN                  0
#ifndef CYGNUM_HAL_ISR_MAX
# define CYGNUM_HAL_ISR_MAX                 (CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#endif
#define CYGNUM_HAL_ISR_COUNT                (CYGNUM_HAL_ISR_MAX + 1)

#define CYGNUM_HAL_VSR_MIN                  0
#ifndef CYGNUM_HAL_VSR_MAX
# define CYGNUM_HAL_VSR_MAX                 (CYGNUM_HAL_VECTOR_SYS_TICK+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#endif
#define CYGNUM_HAL_VSR_COUNT                (CYGNUM_HAL_VSR_MAX+1)


//----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTR_H
// EOF var_intr.h
