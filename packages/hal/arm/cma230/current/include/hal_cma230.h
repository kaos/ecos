#ifndef CYGONCE_HAL_CL7211_H
#define CYGONCE_HAL_CL7211_H

/*=============================================================================
//
//      hal_cl7211.h
//
//      HAL Support for Kernel Diagnostic Routines
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-04-19
// Purpose:      Cogent CMA230 hardware description
// Description:
// Usage:        #include <cyg/hal/hal_cma230.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

// Note: these defintions match the documentation, thus no attempt is made
// to sanitize (mangle) the names.  Also, care should be taken to keep this
// clean for use in assembly code (no "C" constructs).

#define CMA230_ISR  0x0F600000  // Interrupt source register    - Read only
#define CMA230_CLR  0x0F600008  // Clear interrupt source       - Write only
#define CMA230_IMRr 0x0F600010  // Interrupt mask register      - Read only
#define CMA230_IMRw 0x0F600018  // Interrupt mask register      - Write only
#define CMA230_ACK1 0x0F600020  // Interrupt acknowledge slot 1 - Read only
#define CMA230_ACK2 0x0F600028  // Interrupt acknowledge slot 2 - Read only
#define CMA230_ACK3 0x0F600030  // Interrupt acknowledge slot 3 - Read only

#define CMA230_TC_COUNT     0x0F700020  // Timer value   - 16 bits - Read only
#define CMA230_TC_PRELOAD   0x0F700028  // Timer preload - 16 bits - Write only
#define CMA230_TC_CLEAR     0x0F700030  // Timer clear             - Write only
#define CMA230_TC_ENABLE    0x0F700038  // Timer enable/start      - Write only

// Motherboard definitions

#define CMA101_DUARTB      0x0E900000  // Base address
#define CMA101_DUARTB_RHR  0x0E900000  // Receive holding register
#define CMA101_DUARTB_THR  0x0E900000  // Transmit holding register
#define CMA101_DUARTB_LBR  0x0E900000  // Low byte of baud rate
#define CMA101_DUARTB_IER  0x0E900008  // Interrupt enable
#define CMA101_DUARTB_HBR  0x0E900008  // High byte of baud rate
#define CMA101_DUARTB_ISR  0x0E900010  // Interrupt status
#define CMA101_DUARTB_FCTL 0x0E900010  // FIFO control
#define CMA101_DUARTB_LCTL 0x0E900018  // Line control
#define CMA101_DUARTB_MCTL 0x0E900020  // Modem control
#define CMA101_DUARTB_LSR  0x0E900028  // Line status
#define CMA101_DUARTB_MSR  0x0E900030  // Modem status
#define CMA101_DUARTB_SCR  0x0E900038  // Scratch

#define CMA101_DUARTA      0x0E900040  // Base address
#define CMA101_DUARTA_RHR  0x0E900040  // Receive holding register
#define CMA101_DUARTA_THR  0x0E900040  // Transmit holding register
#define CMA101_DUARTA_LBR  0x0E900040  // Low byte of baud rate
#define CMA101_DUARTA_IER  0x0E900048  // Interrupt enable
#define CMA101_DUARTA_HBR  0x0E900048  // High byte of baud rate
#define CMA101_DUARTA_ISR  0x0E900050  // Interrupt status
#define CMA101_DUARTA_FCTL 0x0E900050  // FIFO control
#define CMA101_DUARTA_LCTL 0x0E900058  // Line control
#define CMA101_DUARTA_MCTL 0x0E900060  // Modem control
#define CMA101_DUARTA_LSR  0x0E900068  // Line status
#define CMA101_DUARTA_MSR  0x0E900070  // Modem status
#define CMA101_DUARTA_SCR  0x0E900078  // Scratch

/*---------------------------------------------------------------------------*/
/* end of hal_cma230.h                                                         */
#endif /* CYGONCE_HAL_CMA230_H */
