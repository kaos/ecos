#ifndef CYGONCE_ARM_PID_SERIAL_H
#define CYGONCE_ARM_PID_SERIAL_H

// ====================================================================
//
//      pid_serial.h
//
//      Device I/O - Description of ARM PID7T serial hardware
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
// Date:        1999-02-04
// Purpose:     Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on ARM PID7T

struct serial_port {
    volatile unsigned char _byte[32];
};

// Little-endian version
#if (CYG_BYTEORDER == CYG_LSBFIRST)

#define REG(n) _byte[n*4]

#else // Big-endian version

#define REG(n) _byte[(n*4)^3]

#endif

// Receive control Registers
#define REG_rhr REG(0)    // Receive holding register
#define REG_isr REG(2)    // Interrupt status register
#define REG_lsr REG(5)    // Line status register
#define REG_msr REG(6)    // Modem status register
#define REG_scr REG(7)    // Scratch register

// Transmit control Registers
#define REG_thr REG(0)    // Transmit holding register
#define REG_ier REG(1)    // Interrupt enable register
#define REG_fcr REG(2)    // FIFO control register
#define REG_lcr REG(3)    // Line control register
#define REG_mcr REG(4)    // Modem control register
#define REG_ldl REG(0)    // LSB of baud rate
#define REG_mdl REG(1)    // MSB of baud rate

// Interrupt Enable Register
#define IER_RCV 0x01
#define IER_XMT 0x02
#define IER_LS  0x04
#define IER_MS  0x08

// Line Control Register
#define LCR_WL5 0x00    // Word length
#define LCR_WL6 0x01
#define LCR_WL7 0x02
#define LCR_WL8 0x03
#define LCR_SB1 0x00    // Number of stop bits
#define LCR_SB1_5 0x04  // 1.5 -> only valid with 5 bit words
#define LCR_SB2 0x04
#define LCR_PN  0x00    // Parity mode - none
#define LCR_PE  0x0C    // Parity mode - even
#define LCR_PO  0x08    // Parity mode - odd
#define LCR_PM  0x28    // Forced "mark" parity
#define LCR_PS  0x38    // Forced "space" parity
#define LCR_DL  0x80    // Enable baud rate latch

// Line Status Register
#define LSR_RSR 0x01
#define LSR_OE  0x02
#define LSR_PE  0x04
#define LSR_FE  0x08
#define LSR_BI  0x10
#define LSR_THE 0x20
#define LSR_TEMT 0x40
#define LSR_FIE 0x80

// Modem Control Register
#define MCR_DTR 0x01
#define MCR_RTS 0x02
#define MCR_INT 0x08   // Enable interrupts

// Interrupt status Register
#define ISR_MS  0x00
#define ISR_nIP 0x01
#define ISR_Tx  0x02
#define ISR_LS  0x03
#define ISR_Rx  0x04
#define ISR_RxTO  0x0C

// Modem Status Register
#define MSR_DCTS 0x01
#define MSR_DDSR 0x02
#define MSR_TERI 0x04
#define MSR_DDCD 0x08
#define MSR_CTS  0x10
#define MSR_DSR  0x20
#define MSR_RI   0x40
#define MSR_CD   0x80

static unsigned char select_word_length[] = {
    LCR_WL5,    // 5 bits / word (char)
    LCR_WL6,
    LCR_WL7,
    LCR_WL8
};

static unsigned char select_stop_bits[] = {
    0,
    LCR_SB1,    // 1 stop bit
    LCR_SB1_5,  // 1.5 stop bit
    LCR_SB2     // 2 stop bits
};

static unsigned char select_parity[] = {
    LCR_PN,     // No parity
    LCR_PE,     // Even parity
    LCR_PO,     // Odd parity
    LCR_PM,     // Mark parity
    LCR_PS,     // Space parity
};

static unsigned short select_baud[] = {
    0,    // Unused
    0,    // 50
    0,    // 75
    1047, // 110
    0,    // 134.5
    768,  // 150
    0,    // 200
    384,  // 300
    192,  // 600
    96,   // 1200
    24,   // 1800
    48,   // 2400
    0,    // 3600
    24,   // 4800
    16,   // 7200
    12,   // 9600
    8,    // 14400
    6,    // 19200
    3,    // 38400
    2,    // 57600
    1,    // 115200
    0,    // 230400
};

#endif // CYGONCE_ARM_PID_SERIAL_H
