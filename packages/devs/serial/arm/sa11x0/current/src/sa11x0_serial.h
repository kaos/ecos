#ifndef CYGONCE_ARM_SA11X0_SERIAL_H
#define CYGONCE_ARM_SA11X0_SERIAL_H

// ====================================================================
//
//      sa11x0_serial.h
//
//      Device I/O - Description of StrongARM SA11x0 serial hardware
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-05-08
// Purpose:      Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on StrongARM SA11x0

#include <cyg/hal/hal_sa11x0.h>  // Register definitions

struct serial_port {
    unsigned long ctl0;   // 0x00
    unsigned long ctl1;   // 0x04
    unsigned long ctl2;   // 0x08
    unsigned long ctl3;   // 0x0C
    unsigned long _unused0;
    unsigned long data;   // 0x14
    unsigned long _unused1;
    unsigned long stat0;  // 0x1C
    unsigned long stat1;  // 0x20
};

#define SA11X0_UART_RX_INTS (SA11X0_UART_RX_SERVICE_REQUEST|SA11X0_UART_RX_IDLE)

static unsigned char select_word_length[] = {
    0xFF,                         // 5 bits / word (char)
    0xFF,
    SA11X0_UART_DATA_BITS_7,
    SA11X0_UART_DATA_BITS_8
};

static unsigned char select_stop_bits[] = {
    0,
    SA11X0_UART_STOP_BITS_1,      // 1 stop bit
    0xFF,                         // 1.5 stop bit
    SA11X0_UART_STOP_BITS_2       // 2 stop bits
};

static unsigned char select_parity[] = {
    SA11X0_UART_PARITY_DISABLED,  // No parity
    SA11X0_UART_PARITY_ENABLED|   // Even parity
    SA11X0_UART_PARITY_EVEN,
    SA11X0_UART_PARITY_ENABLED|   // Odd parity
    SA11X0_UART_PARITY_ODD,
    0xFF,                         // Mark parity
    0xFF,                         // Space parity
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

#endif // CYGONCE_ARM_SA11X0_SERIAL_H
