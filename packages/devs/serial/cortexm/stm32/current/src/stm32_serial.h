#ifndef CYGONCE_CORTEXM_STM32_SERIAL_H
#define CYGONCE_CORTEXM_STM32_SERIAL_H

// ====================================================================
//
//      stm32_serial.h
//
//      Device I/O - Description of ST STM32 serial hardware
//
// ====================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-09-10
// Purpose:      Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <cyg/hal/hal_io.h>  // Register definitions

// ====================================================================
// Translate system word length selector into local values.

static cyg_uint32 select_word_length[] = {
    0,
    0,
    0,
    CYGHWR_HAL_STM32_UART_CR1_M_8
};

// ====================================================================
// Translate system stop bit selector into control register bits.

static cyg_uint32 select_stop_bits[] = {
    CYGHWR_HAL_STM32_UART_CR2_STOP_0_5, // 0.5 stop bits
    CYGHWR_HAL_STM32_UART_CR2_STOP_1,   // 1 stop bit
    CYGHWR_HAL_STM32_UART_CR2_STOP_1_5, // 1.5 stop bit
    CYGHWR_HAL_STM32_UART_CR2_STOP_2    // 2 stop bit
};

// Translate system parity selector into local values.
static cyg_uint32 select_parity[] = {
    0,                                                                  // No parity
    CYGHWR_HAL_STM32_UART_CR1_PCE|CYGHWR_HAL_STM32_UART_CR1_PS_EVEN,    // Even parity
    CYGHWR_HAL_STM32_UART_CR1_PCE|CYGHWR_HAL_STM32_UART_CR1_PS_ODD,     // Odd parity
    0,  // Mark (1) parity -- not supported
    0  // Space (0) parity -- not supported
};

// ====================================================================
// Translate system baud selector into direct baud rate value. This is
// then used to calculate the clock divisor from the PCLK clock.

static cyg_int32 select_baud[] = {
    0,      // Unused
    50,     // 50
    75,     // 75
    110,    // 110
    0,      // 134.5
    150,    // 150
    200,    // 200
    300,    // 300
    600,    // 600
    1200,   // 1200
    1800,   // 1800
    2400,   // 2400
    3600,   // 3600
    4800,   // 4800
    7200,   // 7200
    9600,   // 9600
    14400,  // 14400
    19200,  // 19200
    38400,  // 38400
    57600,  // 57600
    115200, // 115200
    230400, // 230400
};

// ====================================================================
#endif // CYGONCE_CORTEXM_STM32_SERIAL_H
