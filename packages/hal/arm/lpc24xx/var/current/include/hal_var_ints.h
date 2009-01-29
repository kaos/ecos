#ifndef CYGONCE_HAL_VAR_INTS_H
#define CYGONCE_HAL_VAR_INTS_H
//==========================================================================
//
//      hal_var_ints.h
//
//      HAL Interrupt and clock support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler 
// Contributors: 
// Date:         2008-07-05
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the LPC24XX are defined here.
// Usage:
//		 #include <pkgconf/system.h>
//		 #include CYGBLD_HAL_VARIANT_H
//               #include CYGBLD_HAL_VAR_INTS_H
//
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#define CYGNUM_HAL_INTERRUPT_WD      0
#define CYGNUM_HAL_INTERRUPT_SOFT    1
#define CYGNUM_HAL_INTERRUPT_DCC_RX  2
#define CYGNUM_HAL_INTERRUPT_DCC_TX  3
#define CYGNUM_HAL_INTERRUPT_TIMER0  4
#define CYGNUM_HAL_INTERRUPT_TIMER1  5
#define CYGNUM_HAL_INTERRUPT_UART0   6
#define CYGNUM_HAL_INTERRUPT_UART1   7
#define CYGNUM_HAL_INTERRUPT_PWM0    8
#define CYGNUM_HAL_INTERRUPT_I2C     9
#define CYGNUM_HAL_INTERRUPT_SPI0    10
#define CYGNUM_HAL_INTERRUPT_SPI1    11
#define CYGNUM_HAL_INTERRUPT_PLL     12
#define CYGNUM_HAL_INTERRUPT_RTCDEV  13	// actual RTC device not the
                                        // eCos 'real time clock'
                                        // interrupt. The latter is on
                                        // TIMER0.
#define CYGNUM_HAL_INTERRUPT_EINT0   14
#define CYGNUM_HAL_INTERRUPT_EINT1   15
#define CYGNUM_HAL_INTERRUPT_EINT2   16
#define CYGNUM_HAL_INTERRUPT_EINT3   17
#define CYGNUM_HAL_INTERRUPT_AD      18 
#define CYGNUM_HAL_INTERRUPT_I2C1    19
#define CYGNUM_HAL_INTERRUPT_BOD     20
#define CYGNUM_HAL_INTERRUPT_ETH     21
#define CYGNUM_HAL_INTERRUPT_USB     22
#define CYGNUM_HAL_INTERRUPT_CAN     23
#define CYGNUM_HAL_INTERRUPT_SD_MMC  24
#define CYGNUM_HAL_INTERRUPT_DMA     25
#define CYGNUM_HAL_INTERRUPT_TIMER2  26
#define CYGNUM_HAL_INTERRUPT_TIMER3  27
#define CYGNUM_HAL_INTERRUPT_UART2   28
#define CYGNUM_HAL_INTERRUPT_UART3   29
#define CYGNUM_HAL_INTERRUPT_I2C2    30
#define CYGNUM_HAL_INTERRUPT_I2S     31


#define CYGNUM_HAL_ISR_MIN           0
#define CYGNUM_HAL_ISR_MAX           (31)

#define CYGNUM_HAL_ISR_COUNT         (CYGNUM_HAL_ISR_MAX+1)

/* use non-vectored interrupts in kernel tests intr0/kintr0 */
#define HAL_INTR_TEST_PRIO_A 16
#define HAL_INTR_TEST_PRIO_B 16
#define HAL_INTR_TEST_PRIO_C 16

//The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC     CYGNUM_HAL_INTERRUPT_TIMER0

// Other entries here moved to variant specific include file
// This is included here to avoid breaking anything
#include <cyg/hal/lpc24xx_misc.h>

//---------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_INTS_H
