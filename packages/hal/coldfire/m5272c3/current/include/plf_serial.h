#ifndef CYGONCE_PLF_SERIAL_H
#define CYGONCE_PLF_SERIAL_H

//=============================================================================
//
//      plf_serial.h
//
//      Platform specific definitions for diagnstic ouput via serial port
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Definitions for diagnostic output via serial port
// Usage:         #include <cyg/hal/plf_serial.h>
//
//####DESCRIPTIONEND####
//========================================================================

#define MCF5272_UART_UMR_8BNP       (0x13)
#define MCF5272_UART_UMR_1S         (0x07)

#define MCF5272_UART_USR_RRDY       (1<<0)
#define MCF5272_UART_USR_FFUL       (1<<1)
#define MCF5272_UART_USR_TXRDY      (1<<2)
#define MCF5272_UART_USR_TXEMP      (1<<3)
#define MCF5272_UART_USR_OE         (1<<4)
#define MCF5272_UART_USR_PE         (1<<5)
#define MCF5272_UART_USR_FE         (1<<6)
#define MCF5272_UART_USR_RB         (1<<7)

#define MCF5272_UART_UCSR_CLKIN     (0xDD)

#define MCF5272_UART_UCR_RMR        (0x01<<4)
#define MCF5272_UART_UCR_RRX        (0x02<<4)
#define MCF5272_UART_UCR_RTX        (0x03<<4)
#define MCF5272_UART_UCR_RES        (0x04<<4)
#define MCF5272_UART_UCR_RBC        (0x05<<4)
#define MCF5272_UART_UCR_TXEN       (1<<2)
#define MCF5272_UART_UCR_TXDE       (1<<3)
#define MCF5272_UART_UCR_RXEN       (1<<0)
#define MCF5272_UART_UCR_RXDE       (1<<1)

#define MCF5272_UART_UCR_TXRXEN     \
        (MCF5272_UART_UCR_TXEN |    \
         MCF5272_UART_UCR_RXEN)
        
#define MCF5272_UART_UIMR_FFULL     (0x02)
        
#define MCF5272_UART_UTF_TXB        (0x1F)

#define MCF5272_UART_UOP0_RTS       (0x01)
#define MCF5272_UART_UOP1_RTS       (0x01)

#define MCF5272_GPIO_PBCNT_URT0_EN      (0x00000155)
#define MCF5272_GPIO_PBCNT_URT0_DE      (0x00000000)
#define MCF5272_GPIO_PBCNT_URT0_MSK     (0x000003FF)

#define MCF5272_GPIO_PDCNT_URT1_EN      (0x000002AA)
#define MCF5272_GPIO_PDCNT_URT1_DE      (0x00000000)
#define MCF5272_GPIO_PDCNT_URT1_MSK     (0x000003FF)

// ---------------------------------------------------------------------------
// End of plf_serial.h
#endif // CYGONCE_PLF_SERIAL_H
