#ifndef CYGONCE_V85X_V850_SERIAL_H
#define CYGONCE_V85X_V850_SERIAL_H

// ====================================================================
//
//      v850_ceb_serial.h
//
//      Device I/O - Description of NEC V850 serial hardware
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
// Date:         2000-03-31
// Purpose:      Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on NEC V850/SA1 & SB1

#include <pkgconf/system.h>
#include CYGBLD_HAL_TARGET_H

#include <cyg/hal/v850_common.h>

struct serial_port {
    unsigned char asim;        // Serial interface mode
    unsigned char _filler0;
    unsigned char asis;        // Serial interface status
    unsigned char _filler1;
    unsigned char brgc;        // Baud rate control
    unsigned char _filler2;
    unsigned char txs;         // Transmit shift register
    unsigned char _filler3;
    unsigned char rxs;         // Receive shift register
    unsigned char _filler4[5];
    unsigned char brgm;        // Baud rate mode
    unsigned char _filler5;
#if CYGINT_HAL_V85X_VARIANT_SB1
    unsigned char _filler6[0x10];
    unsigned char brgm1;       // Baud rate overflow
#endif
};

// Relative interrupt numbers
#define INT_ERR 0              // Receive error condition
#define INT_Rx  1              // Receive data
#define INT_Tx  2              // Transmit data

// Serial interface mode
#define ASIM_TxRx_MASK     (3<<6)  // Receive & Transmit enables
#define ASIM_TxRx_Rx       (1<<6)  // Receive enable
#define ASIM_TxRx_Tx       (2<<6)  // Transmit enable
#define ASIM_Parity_MASK   (3<<4)  // Parity mode bits
#define ASIM_Parity_none   (0<<4)  // No parity
#define ASIM_Parity_space  (1<<4)  // Send zero bit, ignore errors
#define ASIM_Parity_odd    (2<<4)  // Odd parity
#define ASIM_Parity_even   (3<<4)  // Even parity
#define ASIM_Length_MASK   (1<<3)  // Character length select
#define ASIM_Length_7      (0<<3)  // 7 bit chars
#define ASIM_Length_8      (1<<3)  // 8 bit chars
#define ASIM_Stop_MASK     (1<<2)  // Stop bit select
#define ASIM_Stop_1        (0<<2)  // 1 stop bit
#define ASIM_Stop_2        (1<<2)  // 2 stop bits
#define ASIM_Error_MASK    (1<<1)  // Receive error select
#define ASIM_Error_enable  (0<<1)  // Issue interrupt on receive error
#define ASIM_Error_disable (1<<1)  // No interrupts on receive error

// Serial interface status (errors only)
#define ASIS_OVE           (1<<0)  // Overrun error
#define ASIS_FE            (1<<1)  // Framing error
#define ASIS_PE            (1<<2)  // Parity error

static unsigned char select_word_length[] = {
    0xFF,                         // 5 bits / word (char)
    0xFF,
    ASIM_Length_7,
    ASIM_Length_8
};

static unsigned char select_stop_bits[] = {
    0,
    ASIM_Stop_1,                  // 1 stop bit
    0xFF,                         // 1.5 stop bit
    ASIM_Stop_2                   // 2 stop bits
};

static unsigned char select_parity[] = {
    ASIM_Parity_none,             // No parity
    ASIM_Parity_even,             // Even parity
    ASIM_Parity_odd,              // Odd parity
    0xFF,                         // Mark parity
    ASIM_Parity_space,            // Space parity
};

static struct v850_baud {
    unsigned char count;
    unsigned char mode;
} select_baud[] = {
#if CYGINT_HAL_V85X_VARIANT_SB1
// Baud rate values, using defined system clock
#define BAUD_COUNT (CYGHWR_HAL_V85X_V850_BOARD_FREQUENCY/2)/19200
      {0, 0},  // Unused
      {0, 0},  // 50
      {0, 0},  // 75
      {0, 0},  // 110
      {0, 0},  // 134.5
      {0, 0},  // 150
      {0, 0},  // 200
      {0, 0},  // 300
      {0, 0},  // 600
    {BAUD_COUNT, 5},  // 1200
      {0, 0},  // 1800
    {BAUD_COUNT, 4},  // 2400
      {0, 0},  // 3600
    {BAUD_COUNT, 3},  // 4800
      {0, 0},  // 7200
    {BAUD_COUNT, 2},  // 9600
      {0, 0},  // 14400
    {BAUD_COUNT, 1},  // 19200
    {BAUD_COUNT, 0},  // 38400
      {0, 0},  // 57600
      {0, 0},  // 115200
      {0, 0},  // 230400
#else
// Baud rate values, based on raw 17MHz clock
      {0, 0},  // Unused
      {0, 0},  // 50
      {0, 0},  // 75
      {0, 0},  // 110
      {0, 0},  // 134.5
      {0, 0},  // 150
      {0, 0},  // 200
      {0, 0},  // 300
      {0, 0},  // 600
    {221, 5},  // 1200
      {0, 0},  // 1800
    {221, 4},  // 2400
      {0, 0},  // 3600
    {221, 3},  // 4800
      {0, 0},  // 7200
    {221, 2},  // 9600
      {0, 0},  // 14400
    {221, 1},  // 19200
    {221, 0},  // 38400
      {0, 0},  // 57600
      {0, 0},  // 115200
      {0, 0},  // 230400
#endif
};

#endif // CYGONCE_V85X_V850_SERIAL_H
