#ifndef CYGONCE_ARM_AT91_SERIAL_H
#define CYGONCE_ARM_AT91_SERIAL_H

// ====================================================================
//
//      at91_serial.h
//
//      Device I/O - Description of Atmel AT91/EB40 serial hardware
//
// ====================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-07-24
// Purpose:      Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on Atmel AT91/EB40

#include <cyg/hal/plf_io.h>  // Register definitions

#define AT91_UART_RX_INTS (AT91_US_IER_RxRDY)

static cyg_uint32 select_word_length[] = {
    AT91_US_MR_LENGTH_5,
    AT91_US_MR_LENGTH_6,
    AT91_US_MR_LENGTH_7,
    AT91_US_MR_LENGTH_8
};

static cyg_uint32 select_stop_bits[] = {
    0,
    AT91_US_MR_STOP_1,      // 1 stop bit
    AT91_US_MR_STOP_1_5,    // 1.5 stop bit
    AT91_US_MR_STOP_2       // 2 stop bits
};

static cyg_uint32 select_parity[] = {
    AT91_US_MR_PARITY_NONE,  // No parity
    AT91_US_MR_PARITY_EVEN,  // Even parity
    AT91_US_MR_PARITY_ODD,   // Odd parity
    AT91_US_MR_PARITY_MARK,  // Mark (1) parity
    AT91_US_MR_PARITY_SPACE  // Space (0) parity
};

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

#endif // CYGONCE_ARM_AT91_SERIAL_H
