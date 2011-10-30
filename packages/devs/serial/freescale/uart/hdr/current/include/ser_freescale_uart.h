#ifndef CYGONCE_DEVS_SERIAL_FREESCALE_UART_H
#define CYGONCE_DEVS_SERIAL_FREESCALE_UART_H
//==========================================================================
//
//      ser_freescale_uart.h
//
//      Freescale UART I/O definitions.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.                        
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Kocho <ilijak@siva.com.mk>
// Contributors:
// Date:        2011-02-05
// Purpose:     Freescale UART I/O definitions.
// Description:
//
//
//####DESCRIPTIONEND####
//==========================================================================

enum {
    CYGHWR_DEV_FREESCALE_UART_BDH,         // UART Baud Rate Register High
    CYGHWR_DEV_FREESCALE_UART_BDL,         // UART Baud Rate Register Low
    CYGHWR_DEV_FREESCALE_UART_C1,          // UART Control Register 1
    CYGHWR_DEV_FREESCALE_UART_C2,          // UART Control Register 2
    CYGHWR_DEV_FREESCALE_UART_S1,          // UART Status Register 1
    CYGHWR_DEV_FREESCALE_UART_S2,          // UART Status Register 2
    CYGHWR_DEV_FREESCALE_UART_C3,          // UART Control Register 3
    CYGHWR_DEV_FREESCALE_UART_D,           // UART Data Register
    CYGHWR_DEV_FREESCALE_UART_MA1,         // UART Match Address Registers 1
    CYGHWR_DEV_FREESCALE_UART_MA2,         // UART Match Address Registers 2
    CYGHWR_DEV_FREESCALE_UART_C4,          // UART Control Register 4
    CYGHWR_DEV_FREESCALE_UART_C5,          // UART Control Register 5
    CYGHWR_DEV_FREESCALE_UART_ED,          // UART Extended Data Register
    CYGHWR_DEV_FREESCALE_UART_MODEM,       // UART Modem Register
    CYGHWR_DEV_FREESCALE_UART_IR,          // UART Infrared Register
    CYGHWR_DEV_FREESCALE_UART_Res_0,
    CYGHWR_DEV_FREESCALE_UART_PFIFO,       // UART FIFO Parameters
    CYGHWR_DEV_FREESCALE_UART_CFIFO,       // UART FIFO Control Register
    CYGHWR_DEV_FREESCALE_UART_SFIFO,       // UART FIFO Status Register
    CYGHWR_DEV_FREESCALE_UART_TWFIFO,      // UART FIFO Transmit Watermark
    CYGHWR_DEV_FREESCALE_UART_TCFIFO,      // UART FIFO Transmit Count
    CYGHWR_DEV_FREESCALE_UART_RWFIFO,      // UART FIFO Receive Watermark
    CYGHWR_DEV_FREESCALE_UART_RCFIFO,      // UART FIFO Receive Count
    CYGHWR_DEV_FREESCALE_UART_Res_1,
    CYGHWR_DEV_FREESCALE_UART_C7816,       // UART 7816 Control Register
    CYGHWR_DEV_FREESCALE_UART_IE7816,      // UART 7816 Interrupt Enable Register
    CYGHWR_DEV_FREESCALE_UART_IS7816,      // UART 7816 Interrupt Status Register
    CYGHWR_DEV_FREESCALE_UART_WP7816_T0T1, // UART 7816 Wait Parameter Register
    CYGHWR_DEV_FREESCALE_UART_WN7816,      // UART 7816 Wait N Register
    CYGHWR_DEV_FREESCALE_UART_WF7816,      // UART 7816 Wait FD Register
    CYGHWR_DEV_FREESCALE_UART_ET7816,      // UART 7816 Error Threshold Register
    CYGHWR_DEV_FREESCALE_UART_TL7816       // UART 7816 Transmit Length Register
};

// CYGHWR_IO_FREESCALE_UART_BAUD_SET(__uart_p, _baud_) should be provided by HAL.
// CYGHWR_IO_FREESCALE_UART_PIN(__pin) should be provided by HAL.
// CYGHWR_IO_FREESCALE_UARTn_PIN_RX should be provided by HAL.
// CYGHWR_IO_FREESCALE_UARTn_PIN_TX should be provided by HAL.
// CYGHWR_IO_FREESCALE_UARTn_PIN_RTS should be provided by HAL.
// CYGHWR_IO_FREESCALE_UARTn_PIN_CTS should be provided by HAL.


#define CYGHWR_DEV_FREESCALE_UART_C1_LOOPS     (0x80)
#define CYGHWR_DEV_FREESCALE_UART_C1_UARTSWAI  (0x40)
#define CYGHWR_DEV_FREESCALE_UART_C1_RSRC      (0x20)
#define CYGHWR_DEV_FREESCALE_UART_C1_M         (0x10)
#define CYGHWR_DEV_FREESCALE_UART_C1_WAKE      (0x08)
#define CYGHWR_DEV_FREESCALE_UART_C1_ILT       (0x04)
#define CYGHWR_DEV_FREESCALE_UART_C1_PE        (0x02)
#define CYGHWR_DEV_FREESCALE_UART_C1_PT        (0x01)

#define CYGHWR_DEV_FREESCALE_UART_C2_TIE       (0x80)
#define CYGHWR_DEV_FREESCALE_UART_C2_TCIE      (0x40)
#define CYGHWR_DEV_FREESCALE_UART_C2_RIE       (0x20)
#define CYGHWR_DEV_FREESCALE_UART_C2_ILIE      (0x10)
#define CYGHWR_DEV_FREESCALE_UART_C2_TE        (0x08)
#define CYGHWR_DEV_FREESCALE_UART_C2_RE        (0x04)
#define CYGHWR_DEV_FREESCALE_UART_C2_RWU       (0x02)
#define CYGHWR_DEV_FREESCALE_UART_C2_SBK       (0x01)

#define CYGHWR_DEV_FREESCALE_UART_S1_TDRE      (0x80)
#define CYGHWR_DEV_FREESCALE_UART_S1_TC        (0x40)
#define CYGHWR_DEV_FREESCALE_UART_S1_RDRF      (0x20)
#define CYGHWR_DEV_FREESCALE_UART_S1_IDLE      (0x10)
#define CYGHWR_DEV_FREESCALE_UART_S1_OR        (0x08)
#define CYGHWR_DEV_FREESCALE_UART_S1_NF        (0x04)
#define CYGHWR_DEV_FREESCALE_UART_S1_FE        (0x02)
#define CYGHWR_DEV_FREESCALE_UART_S1_PF        (0x01)

#define CYGHWR_DEV_FREESCALE_UART_S2_LBKDIF    (0x80)
#define CYGHWR_DEV_FREESCALE_UART_S2_RXEDGIF   (0x40)
#define CYGHWR_DEV_FREESCALE_UART_S2_MSBF      (0x20)
#define CYGHWR_DEV_FREESCALE_UART_S2_RXINV     (0x10)
#define CYGHWR_DEV_FREESCALE_UART_S2_RWUID     (0x08)
#define CYGHWR_DEV_FREESCALE_UART_S2_BRK13     (0x04)
#define CYGHWR_DEV_FREESCALE_UART_S2_LBKDE     (0x02)
#define CYGHWR_DEV_FREESCALE_UART_S2_RAF       (0x01)

#define CYGHWR_DEV_FREESCALE_UART_C3_R8        (0x80)
#define CYGHWR_DEV_FREESCALE_UART_C3_T8        (0x40)
#define CYGHWR_DEV_FREESCALE_UART_C3_TXDIR     (0x20)
#define CYGHWR_DEV_FREESCALE_UART_C3_TXINV     (0x10)
#define CYGHWR_DEV_FREESCALE_UART_C3_ORIE      (0x08)
#define CYGHWR_DEV_FREESCALE_UART_C3_NEIE      (0x04)
#define CYGHWR_DEV_FREESCALE_UART_C3_FEIE      (0x02)
#define CYGHWR_DEV_FREESCALE_UART_C3_PEIE      (0x01)


#endif // CYGONCE_DEVS_SERIAL_FREESCALE_UART_H
