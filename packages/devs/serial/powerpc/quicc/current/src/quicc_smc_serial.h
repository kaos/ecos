#ifndef CYGONCE_POWERPC_QUICC_SMC_SERIAL_H
#define CYGONCE_POWERPC_QUICC_SMC_SERIAL_H

// ====================================================================
//
//      quicc_smc_serial.h
//
//      Device I/O - Description of PowerPC QUICC/SMC serial hardware
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
// Author(s):           gthomas
// Contributors:        gthomas
// Date:                1999-06-21
// Purpose:     Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports using QUICC/SMC

#include <cyg/hal/quicc/ppc8xx.h>                  // QUICC structure definitions

static unsigned int select_word_length[] = {
    QUICC_SMCMR_CLEN(5),  // 5 bits / word (char)
    QUICC_SMCMR_CLEN(6),
    QUICC_SMCMR_CLEN(7),
    QUICC_SMCMR_CLEN(8)
};

static unsigned int select_stop_bits[] = {
    0, 
    QUICC_SMCMR_SB(1),   // 1 stop bit
    QUICC_SMCMR_SB(1),   // 1.5 stop bit
    QUICC_SMCMR_SB(2)    // 2 stop bits
};

static unsigned int select_parity[] = {
    QUICC_SMCMR_PE(0),                     // No parity
    QUICC_SMCMR_PE(1)|QUICC_SMCMR_PM(1),   // Even parity
    QUICC_SMCMR_PE(1)|QUICC_SMCMR_PM(0),   // Odd parity
    0,                                     // Mark parity
    0,                                     // Space parity
};

// Baud rate values, based on board clock

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
    0,      // 230400
};

#define UART_BITRATE(n) (((CYGHWR_HAL_POWERPC_BOARD_SPEED*1000000)/16)/n)
#define UART_SLOW_BITRATE(n) ((CYGHWR_HAL_POWERPC_BOARD_SPEED*1000000)/n))

#endif // CYGONCE_POWERPC_QUICC_SMC_SERIAL_H
