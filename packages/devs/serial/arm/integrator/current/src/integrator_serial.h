#ifndef CYGONCE_ARM_INTEGRATOR_SERIAL_H
#define CYGONCE_ARM_INTEGRATOR_SERIAL_H

// ====================================================================
//
//      integrator_serial.h
//
//      Device I/O - Description of ARM INTEGRATOR serial hardware
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
// Author(s):           David A Rusling
// Contributors:        Philippe Robin
// Date:        	November 7, 2000
// Purpose:     	Internal interfaces for serial I/O drivers
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

// Description of serial ports on ARM INTEGRATOR7T

struct serial_port {
    unsigned char _byte[32];
};

// Little-endian version
#if (CYG_BYTEORDER == CYG_LSBFIRST)

#define reg(n) _byte[n*4]

#else // Big-endian version

#define reg(n) _byte[(n*4)^3]

#endif

/* -------------------------------------------------------------------------------
 *  From AMBA UART (PL010) Block Specification (ARM-0001-CUST-DSPC-A03)
 * -------------------------------------------------------------------------------
 *  UART Register Offsets.
 *  
 */
#define AMBA_UARTDR                     0x00	 /*  Data read or written from the interface. */
#define AMBA_UARTRSR                    0x04	 /*  Receive status register (Read). */
#define AMBA_UARTECR                    0x04	 /*  Error clear register (Write). */
#define AMBA_UARTLCR_H                  0x08	 /*  Line control register, high byte. */
#define AMBA_UARTLCR_M                  0x0C	 /*  Line control register, middle byte. */
#define AMBA_UARTLCR_L                  0x10	 /*  Line control register, low byte. */
#define AMBA_UARTCR                     0x14	 /*  Control register. */
#define AMBA_UARTFR                     0x18	 /*  Flag register (Read only). */
#define AMBA_UARTIIR                    0x1C	 /*  Interrupt indentification register (Read). */
#define AMBA_UARTICR                    0x1C	 /*  Interrupt clear register (Write). */
#define AMBA_UARTILPR                   0x20	 /*  IrDA low power counter register. */

#define AMBA_UARTRSR_OE                 0x08
#define AMBA_UARTRSR_BE                 0x04
#define AMBA_UARTRSR_PE                 0x02
#define AMBA_UARTRSR_FE                 0x01

#define AMBA_UARTFR_TXFF                0x20
#define AMBA_UARTFR_RXFE                0x10
#define AMBA_UARTFR_BUSY                0x08
#define AMBA_UARTFR_TMSK                (AMBA_UARTFR_TXFF + AMBA_UARTFR_BUSY)
 
#define AMBA_UARTCR_RTIE                0x40
#define AMBA_UARTCR_TIE                 0x20
#define AMBA_UARTCR_RIE                 0x10
#define AMBA_UARTCR_MSIE                0x08
#define AMBA_UARTCR_IIRLP               0x04
#define AMBA_UARTCR_SIREN               0x02
#define AMBA_UARTCR_UARTEN              0x01
 
#define AMBA_UARTLCR_H_WLEN_8           0x60
#define AMBA_UARTLCR_H_WLEN_7           0x40
#define AMBA_UARTLCR_H_WLEN_6           0x20
#define AMBA_UARTLCR_H_WLEN_5           0x00
#define AMBA_UARTLCR_H_FEN              0x10
#define AMBA_UARTLCR_H_STP2             0x08
#define AMBA_UARTLCR_H_EPS              0x04
#define AMBA_UARTLCR_H_PEN              0x02
#define AMBA_UARTLCR_H_BRK              0x01

#define AMBA_UARTIIR_RTIS               0x08
#define AMBA_UARTIIR_TIS                0x04
#define AMBA_UARTIIR_RIS                0x02
#define AMBA_UARTIIR_MIS                0x01

#define ARM_BAUD_460800                 1
#define ARM_BAUD_230400                 3
#define ARM_BAUD_115200                 7
#define ARM_BAUD_57600                  15
#define ARM_BAUD_38400                  23
#define ARM_BAUD_19200                  47
#define ARM_BAUD_14400                  63
#define ARM_BAUD_9600                   95
#define ARM_BAUD_4800                   191
#define ARM_BAUD_2400                   383
#define ARM_BAUD_1200                   767

// Interrupt Enable Register
#define IER_RCV 0x01
#define IER_XMT 0x02
#define IER_LS  0x04
#define IER_MS  0x08

// Line Control Register
#define LCR_WL5 0x00    // Word length
#define LCR_WL6 0x20
#define LCR_WL7 0x40
#define LCR_WL8 0x60

#define LCR_SB1 0x00    // Number of stop bits
#define LCR_SB1_5 0x00  // 1.5 -> only valid with 5 bit words
#define LCR_SB2 0x08

#define LCR_PN  0x00    // Parity mode - none
#define LCR_PE  0x06    // Parity mode - even
#define LCR_PO  0x02    // Parity mode - odd
#define LCR_PM  0x00    // Forced "mark" parity
#define LCR_PS  0x00    // Forced "space" parity

// Line Status Register
#define LSR_RSR 0x01
#define LSR_THE 0x20

// Modem Control Register
#define MCR_DTR 0x01
#define MCR_RTS 0x02
#define MCR_INT 0x08   // Enable interrupts

static unsigned short select_baud[] = {
    0,               // Unused
    0,               // 50
    0,               // 75
    0,               // 110
    0,               // 134.5
    0,               // 150
    0,               // 200
    0,               // 300
    0,               // 600
    ARM_BAUD_1200,   // 1200
    0,               // 1800
    ARM_BAUD_2400,   // 2400
    0,               // 3600
    ARM_BAUD_4800,   // 4800
    0,               // 7200
    ARM_BAUD_9600,   // 9600
    ARM_BAUD_14400,  // 14400
    ARM_BAUD_19200,  // 19200
    ARM_BAUD_38400,  // 38400
    ARM_BAUD_57600,  // 57600
    ARM_BAUD_115200, // 115200
    ARM_BAUD_230400, // 230400
};

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

#endif // CYGONCE_ARM_INTEGRATOR_SERIAL_H
