#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H
//==========================================================================
//
//      var_intr.h
//
//      HAL Interrupt and clock assignments for LPC17XX variants
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010 Free Software Foundation, Inc.                        
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
// Author(s):     ilijak
// Date:          2010-12-29
// Purpose:       Define Interrupt support
// Description:   The interrupt specifics for NXP LPC17XX variants are
//                defined here.
//              
// Usage:         #include <cyg/hal/var_intr.h>
//                However applications should include using <cyg/hal/hal_intr.h>
//                instead to allow for platform overrides.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/plf_intr.h>

//==========================================================================

#define CYGNUM_HAL_INTERRUPT_WD      (0+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIMER0  (1+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIMER1  (2+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIMER2  (3+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_TIMER3  (4+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART0   (5+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART1   (6+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART2   (7+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_UART3   (8+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PWM1    (9+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C0    (9+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C1    (10+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2C2    (11+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_SPI     (13+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SSP0    (14+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_SSP1    (15+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_PLL0    (16+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_RTCDEV  (17+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_EINT0   (18+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EINT1   (19+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EINT2   (20+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_EINT3   (21+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_AD      (22+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_BOD     (23+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_USB     (24+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CAN     (25+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_DMA     (26+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_I2S     (27+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_ETH     (28+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_RITINT  (29+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_MCPWM   (30+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_QENC    (31+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_PLL1    (32+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_USBAI   (33+CYGNUM_HAL_INTERRUPT_EXTERNAL)
#define CYGNUM_HAL_INTERRUPT_CANWAKE (34+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_INTERRUPT_NVIC_MAX (CYGNUM_HAL_INTERRUPT_CANWAKE+CYGNUM_HAL_INTERRUPT_EXTERNAL)

#define CYGNUM_HAL_ISR_MIN            0
#define CYGNUM_HAL_ISR_MAX            CYGNUM_HAL_INTERRUPT_CANWAKE
#define CYGNUM_HAL_ISR_COUNT          (CYGNUM_HAL_ISR_MAX+1)

#define CYGNUM_HAL_VSR_MIN            0

#ifndef CYGNUM_HAL_VSR_MAX
# define CYGNUM_HAL_VSR_MAX (CYGNUM_HAL_VECTOR_SYS_TICK+CYGNUM_HAL_INTERRUPT_NVIC_MAX)
#endif

#define CYGNUM_HAL_VSR_COUNT          (CYGNUM_HAL_VSR_MAX+1)

//==========================================================================
// Interrupt mask and config for variant-specific devices

//----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTR_H
// EOF var_intr.h
