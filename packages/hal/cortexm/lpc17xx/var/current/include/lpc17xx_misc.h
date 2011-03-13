#ifndef CYGONCE_HAL_CORTEXM_LPC17XX_VAR_LPC17XX_MISC_H
#define CYGONCE_HAL_CORTEXM_LPC17XX_VAR_LPC17XX_MISC_H
//=============================================================================
//
//      lpc17xx_misc.h
//
//      HAL misc variant support code for NCP LPC17xx header file
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andyj 
// Contributors: jani, ilijak
// Date:         2010-12-29
// Purpose:      LPC17XX specific miscellaneous support header file
// Description: 
// Usage:        #include <cyg/hal/lpc17xx_misc.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//-----------------------------------------------------------------------------
// Function to obtain the current processor clock settings
// Use PCLK identifiers below
//
externC cyg_uint32 hal_lpc_get_pclk(cyg_uint32 pclk_id);
#define CYG_HAL_CORTEXM_LPC17XX_PCLK(_pclkid_) hal_lpc_get_pclk(_pclkid_)

//-----------------------------------------------------------------------------
// Identifiers for peripheral clock. Use these identifiers with the function
// hal_get_pclk()
//
#define CYNUM_HAL_LPC17XX_PCLK_WDT    0
#define CYNUM_HAL_LPC17XX_PCLK_TIMER0 1
#define CYNUM_HAL_LPC17XX_PCLK_TIMER1 2
#define CYNUM_HAL_LPC17XX_PCLK_UART0  3
#define CYNUM_HAL_LPC17XX_PCLK_UART1  4
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCLK_PWM0   5
#endif
#define CYNUM_HAL_LPC17XX_PCLK_PWM1   6
#define CYNUM_HAL_LPC17XX_PCLK_I2C0   7
#define CYNUM_HAL_LPC17XX_PCLK_SPI    8
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCLK_RTC    9
#endif
#define CYNUM_HAL_LPC17XX_PCLK_SSP1   10
#define CYNUM_HAL_LPC17XX_PCLK_DAC    11
#define CYNUM_HAL_LPC17XX_PCLK_ADC    12
#define CYNUM_HAL_LPC17XX_PCLK_CAN1   13
#define CYNUM_HAL_LPC17XX_PCLK_CAN2   14
#define CYNUM_HAL_LPC17XX_PCLK_ACF    15
#define CYNUM_HAL_LPC17XX_PCLK_QEI    16
#define CYNUM_HAL_LPC17XX_PCLK_GPIO   17
#define CYNUM_HAL_LPC17XX_PCLK_PCB    18
#define CYNUM_HAL_LPC17XX_PCLK_I2C1   19
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCLK_SSP0   21
#endif
#define CYNUM_HAL_LPC17XX_PCLK_TIMER2 22
#define CYNUM_HAL_LPC17XX_PCLK_TIMER3 23
#define CYNUM_HAL_LPC17XX_PCLK_UART2  24
#define CYNUM_HAL_LPC17XX_PCLK_UART3  25
#define CYNUM_HAL_LPC17XX_PCLK_I2C2   26
#define CYNUM_HAL_LPC17XX_PCLK_I2S    27
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCLK_MCI    28
#endif
#define CYNUM_HAL_LPC17XX_PCLK_RIT    29
#define CYNUM_HAL_LPC17XX_PCLK_SYSCON 30
#define CYNUM_HAL_LPC17XX_PCLK_MC     31


//-----------------------------------------------------------------------------
// Function to enable/disable power for certain peripheral
// Use PCONP identifiers from below
//
externC void hal_lpc_set_power(cyg_uint8 pconp_id, int on);
#define CYG_HAL_CORTEXM_LPC17XX_SET_POWER(_pconp_id_, _on_) \
        hal_lpc_set_power((_pconp_id_), (_on_))


//-----------------------------------------------------------------------------
// Identifiers for power control, hal_get_pclk()
//
#define CYNUM_HAL_LPC17XX_PCONP_TIMER0 1
#define CYNUM_HAL_LPC17XX_PCONP_TIMER1 2
#define CYNUM_HAL_LPC17XX_PCONP_UART0  3
#define CYNUM_HAL_LPC17XX_PCONP_UART1  4
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCONP_PWM0   5
#endif
#define CYNUM_HAL_LPC17XX_PCONP_PWM1   6
#define CYNUM_HAL_LPC17XX_PCONP_I2C0   7
#define CYNUM_HAL_LPC17XX_PCONP_SPI    8
#define CYNUM_HAL_LPC17XX_PCONP_RTC    9
#define CYNUM_HAL_LPC17XX_PCONP_SSP1   10
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCONP_EMC    11
#endif
#define CYNUM_HAL_LPC17XX_PCONP_ADC    12
#define CYNUM_HAL_LPC17XX_PCONP_CAN1   13
#define CYNUM_HAL_LPC17XX_PCONP_CAN2   14
#define CYNUM_HAL_LPC17XX_PCONP_GPIO   15
#define CYNUM_HAL_LPC17XX_PCONP_RIT    16
#define CYNUM_HAL_LPC17XX_PCONP_MCPWM  17
#define CYNUM_HAL_LPC17XX_PCONP_QEI    18
#define CYNUM_HAL_LPC17XX_PCONP_I2C1   19
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCONP_LCD    20
#endif
#define CYNUM_HAL_LPC17XX_PCONP_SSP0   21
#define CYNUM_HAL_LPC17XX_PCONP_TIMER2 22
#define CYNUM_HAL_LPC17XX_PCONP_TIMER3 23
#define CYNUM_HAL_LPC17XX_PCONP_UART2  24
#define CYNUM_HAL_LPC17XX_PCONP_UART3  25
#define CYNUM_HAL_LPC17XX_PCONP_I2C2   26
#define CYNUM_HAL_LPC17XX_PCONP_I2S    27
#if 0 // Not implemented on LPC17xx
# define CYNUM_HAL_LPC17XX_PCONP_SDC    28
#endif
#define CYNUM_HAL_LPC17XX_PCONP_GPDMA  29
#define CYNUM_HAL_LPC17XX_PCONP_ENET   30
#define CYNUM_HAL_LPC17XX_PCONP_USB    31


//-----------------------------------------------------------------------------
// Configure pin function
//
externC void hal_lpc_set_pin_function(cyg_uint8 port, cyg_uint8 pin,
                                  cyg_uint8 function);
#define CYG_HAL_CORTEXM_LPC17XX_PIN_CFG(_port_, _pin_, _func_) \
                hal_lpc_set_pin_function((_port_), (_pin_), (_func_))


//-----------------------------------------------------------------------------
// Macros to derive the baudrate divider values for the internal UARTs
// The LPC17xx family supports different baudrate clocks for each single
// UART. So we need a way to calculate the baudrate for each single UART
// Now we rely on the fact that we use the same baudrate clock for all
// UARTs and we query only UART0
//-----------------------------------------------------------------------------

#define CYG_HAL_CORTEXM_LPC17XX_BAUD_GENERATOR(_pclkid_, baud) \
                (CYG_HAL_CORTEXM_LPC17XX_PCLK(_pclkid_)/((baud)*16))

//-----------------------------------------------------------------------------
// LPC17XX platform reset (watchdog resets the board)
//-----------------------------------------------------------------------------
#if 0
externC void hal_lpc_watchdog_reset(void);

#define HAL_PLATFORM_RESET() hal_lpc_watchdog_reset()
#define HAL_PLATFORM_RESET_ENTRY 0
#endif


//-----------------------------------------------------------------------------
// Compatibility layer for LPC2xxx device drivers
//-----------------------------------------------------------------------------
#define CYNUM_HAL_LPC24XX_PCLK_UART0 CYNUM_HAL_LPC17XX_PCLK_UART0
#define CYNUM_HAL_LPC24XX_PCLK_UART1 CYNUM_HAL_LPC17XX_PCLK_UART1
#define CYNUM_HAL_LPC24XX_PCLK_UART2 CYNUM_HAL_LPC17XX_PCLK_UART2
#define CYNUM_HAL_LPC24XX_PCLK_UART3 CYNUM_HAL_LPC17XX_PCLK_UART3

#define CYG_HAL_ARM_LPC24XX_BAUD_GENERATOR(_pclkid_, baud) \
            CYG_HAL_CORTEXM_LPC17XX_BAUD_GENERATOR(_pclkid_, baud)

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_CORTEXM_LPC17XX_VAR_LPC17XX_MISC_H
// End of lpc17xx_misc.h
