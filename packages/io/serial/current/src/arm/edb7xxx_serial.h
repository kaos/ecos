#ifndef CYGONCE_ARM_EDB7XXX_SERIAL_H
#define CYGONCE_ARM_EDB7XXX_SERIAL_H

// ====================================================================
//
//      edb7xxx_serial.h
//
//      Device I/O - Description of Cirrus Logic EDB7XXX serial hardware
//
// ====================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on Cirrus Logic EDB7XXX

#include <cyg/hal/hal_edb7xxx.h>  // Hardware definitions

static unsigned int select_word_length[] = {
    UBLCR_WRDLEN5,    // 5 bits / word (char)
    UBLCR_WRDLEN6,
    UBLCR_WRDLEN7,
    UBLCR_WRDLEN8
};

static unsigned int select_stop_bits[] = {
    0,
    0,              // 1 stop bit
    0,              // 1.5 stop bit
    UBLCR_XSTOP     // 2 stop bits
};

static unsigned int select_parity[] = {
    0,                             // No parity
    UBLCR_PRTEN|UBLCR_EVENPRT,     // Even parity
    UBLCR_PRTEN,                   // Odd parity
    0,                             // Mark parity
    0,                             // Space parity
};

// Baud rate values, based on PLL clock

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

#endif // CYGONCE_ARM_EDB7XXX_SERIAL_H
