#ifndef CYGONCE_HAL_ARM_LPC24XX_VAR_LPC24XX_MISC_H
#define CYGONCE_HAL_ARM_LPC24XX_VAR_LPC24XX_MISC_H
//=============================================================================
//
//      lpc24xx_misc.h
//
//      HAL misc variant support code for NCP LPC24xx header file
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric Limited 
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andyj 
// Contributors: jani
// Date:         2006-02-04
// Purpose:      LPC2XXX specific miscellaneous support header file
// Description: 
// Usage:        #include <cyg/hal/lpc24xx_misc.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//-----------------------------------------------------------------------------
// Functions to obtain the current processor clock settings
//-----------------------------------------------------------------------------
externC cyg_uint32 hal_lpc_get_pclk(cyg_uint32 peripheral_id);

//
// Identifiers for peripheral clock. Use these identifiers with the function
// hal_get_pclk()
//
#define CYNUM_HAL_LPC24XX_PCLK_WDT    0
#define CYNUM_HAL_LPC24XX_PCLK_TIMER0 1
#define CYNUM_HAL_LPC24XX_PCLK_TIMER1 2
#define CYNUM_HAL_LPC24XX_PCLK_UART0  3
#define CYNUM_HAL_LPC24XX_PCLK_UART1  4
#define CYNUM_HAL_LPC24XX_PCLK_PWM0   5
#define CYNUM_HAL_LPC24XX_PCLK_PWM1   6
#define CYNUM_HAL_LPC24XX_PCLK_I2C0   7
#define CYNUM_HAL_LPC24XX_PCLK_SPI    8
#define CYNUM_HAL_LPC24XX_PCLK_RTC    9
#define CYNUM_HAL_LPC24XX_PCLK_SSP1   10
#define CYNUM_HAL_LPC24XX_PCLK_DAC    11
#define CYNUM_HAL_LPC24XX_PCLK_ADC    12
#define CYNUM_HAL_LPC24XX_PCLK_CAN1   13
#define CYNUM_HAL_LPC24XX_PCLK_CAN2   14
#define CYNUM_HAL_LPC24XX_PCLK_ACF    15
#define CYNUM_HAL_LPC24XX_PCLK_BATRAM 16
#define CYNUM_HAL_LPC24XX_PCLK_GPIO   17
#define CYNUM_HAL_LPC24XX_PCLK_PCB    18
#define CYNUM_HAL_LPC24XX_PCLK_I2C1   19
#define CYNUM_HAL_LPC24XX_PCLK_SSP0   21
#define CYNUM_HAL_LPC24XX_PCLK_TIMER2 22
#define CYNUM_HAL_LPC24XX_PCLK_TIMER3 23
#define CYNUM_HAL_LPC24XX_PCLK_UART2  24
#define CYNUM_HAL_LPC24XX_PCLK_UART3  25
#define CYNUM_HAL_LPC24XX_PCLK_I2C2   26
#define CYNUM_HAL_LPC24XX_PCLK_I2S    27
#define CYNUM_HAL_LPC24XX_PCLK_MCI    28
#define CYNUM_HAL_LPC24XX_PCLK_SYSCON 30


//-----------------------------------------------------------------------------
// Macros to derive the baudrate divider values for the internal UARTs
// The LPC24xx family supports differents baudrate clocks for each single
// UART. So we need a way to calculate the baudrate for each single UART
// Now we rely on the fact that we use the same baurate clock for all
// UARTs and we query only UART0
//-----------------------------------------------------------------------------
#define CYG_HAL_ARM_LPC24XX_PCLK(_pclkid_) hal_lpc_get_pclk(_pclkid_) 
#define CYG_HAL_ARM_LPC2XXX_BAUD_GENERATOR(baud) \
            (CYG_HAL_ARM_LPC24XX_PCLK(CYNUM_HAL_LPC24XX_PCLK_UART0)/((baud)*16))
#define CYG_HAL_ARM_LPC24XX_BAUD_GENERATOR(_pclkid_, baud) \
            (CYG_HAL_ARM_LPC24XX_PCLK(_pclkid_)/((baud)*16))
            

//-----------------------------------------------------------------------------
// LPX24xx platform reset (watchdog resets the board)
//-----------------------------------------------------------------------------
externC void hal_lpc_watchdog_reset(void);

#define HAL_PLATFORM_RESET() hal_lpc_watchdog_reset()
#define HAL_PLATFORM_RESET_ENTRY 0

//-----------------------------------------------------------------------------
// end of lpc24xx_misc.h
#endif // CYGONCE_HAL_ARM_LPC24XX_VAR_LPC24XX_MISC_H
