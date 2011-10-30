//==========================================================================
//
//      ser_freescale_uart.inl
//
//      Freescale UART Serial channel definitions
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
// Date:        2011-02-10
// Purpose:     Freescale UART Serial I/O module (interrupt driven version)
// Description:
//
//
//####DESCRIPTIONEND####
//==========================================================================

#if defined CYGPKG_IO_SERIAL_FREESCALE_UART0
static const uart_pins_t uart0_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART0_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART0_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART0_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART0_PIN_CTS
};
static uart_serial_info uart_serial_info0 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART0_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART0_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART0_INT_PRIORITY,
    pins_p             : &uart0_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART0_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf0[CYGNUM_IO_SERIAL_FREESCALE_UART0_BUFSIZE];
static unsigned char
    uart_serial_in_buf0[CYGNUM_IO_SERIAL_FREESCALE_UART0_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(
                                uart_serial_channel0,
                                uart_serial_funs,
                                uart_serial_info0,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART0_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf0[0],
                                sizeof(uart_serial_out_buf0),
                                &uart_serial_in_buf0[0],
                                sizeof(uart_serial_in_buf0));
#else
static
SERIAL_CHANNEL(uart_serial_channel0,
               uart_serial_funs,
               uart_serial_info0,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART0_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io0,
             CYGDAT_IO_SERIAL_FREESCALE_UART0_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel0);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART0

#if defined CYGPKG_IO_SERIAL_FREESCALE_UART1
static const uart_pins_t uart1_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART1_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART1_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART1_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART1_PIN_CTS
};
static uart_serial_info uart_serial_info1 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART1_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART1_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART1_INT_PRIORITY,
    pins_p             : &uart1_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART1_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf1[CYGNUM_IO_SERIAL_FREESCALE_UART1_BUFSIZE];
static unsigned char
    uart_serial_in_buf1[CYGNUM_IO_SERIAL_FREESCALE_UART1_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(uart_serial_channel1,
                                uart_serial_funs,
                                uart_serial_info1,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART1_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf1[0],
                                sizeof(uart_serial_out_buf1),
                                &uart_serial_in_buf1[0],
                                sizeof(uart_serial_in_buf1));
#else
static
SERIAL_CHANNEL(uart_serial_channel1,
               uart_serial_funs,
               uart_serial_info1,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART1_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io1,
             CYGDAT_IO_SERIAL_FREESCALE_UART1_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel1);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART1

#if defined CYGPKG_IO_SERIAL_FREESCALE_UART2
static const uart_pins_t uart2_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART2_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART2_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART2_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART2_PIN_CTS
};
static uart_serial_info uart_serial_info2 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART2_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART2_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART2_INT_PRIORITY,
    pins_p             : &uart2_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART2_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf2[CYGNUM_IO_SERIAL_FREESCALE_UART2_BUFSIZE];
static unsigned char
    uart_serial_in_buf2[CYGNUM_IO_SERIAL_FREESCALE_UART2_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(uart_serial_channel2,
                                uart_serial_funs,
                                uart_serial_info2,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART2_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf2[0],
                                sizeof(uart_serial_out_buf2),
                                &uart_serial_in_buf2[0],
                                sizeof(uart_serial_in_buf2));
#else
static
SERIAL_CHANNEL(uart_serial_channel2,
               uart_serial_funs,
               uart_serial_info2,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART2_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io2,
             CYGDAT_IO_SERIAL_FREESCALE_UART2_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel2);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART2


#if defined CYGPKG_IO_SERIAL_FREESCALE_UART3
static const uart_pins_t uart3_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART3_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART3_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART3_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART3_PIN_CTS
};
static uart_serial_info uart_serial_info3 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART3_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART3_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART3_INT_PRIORITY,
    pins_p             : &uart3_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART3_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf3[CYGNUM_IO_SERIAL_FREESCALE_UART3_BUFSIZE];
static unsigned char
    uart_serial_in_buf3[CYGNUM_IO_SERIAL_FREESCALE_UART3_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(uart_serial_channel3,
                                uart_serial_funs,
                                uart_serial_info3,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART3_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf3[0],
                                sizeof(uart_serial_out_buf3),
                                &uart_serial_in_buf3[0],
                                sizeof(uart_serial_in_buf3));
#else
static
SERIAL_CHANNEL(uart_serial_channel3,
               uart_serial_funs,
               uart_serial_info3,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART3_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io3,
             CYGDAT_IO_SERIAL_FREESCALE_UART3_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel3);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART3


#if defined CYGPKG_IO_SERIAL_FREESCALE_UART4
static const uart_pins_t uart4_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART4_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART4_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART4_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART4_PIN_CTS
};
static uart_serial_info uart_serial_info4 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART4_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART4_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART4_INT_PRIORITY,
    pins_p             : &uart4_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART4_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf4[CYGNUM_IO_SERIAL_FREESCALE_UART4_BUFSIZE];
static unsigned char
    uart_serial_in_buf4[CYGNUM_IO_SERIAL_FREESCALE_UART4_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(uart_serial_channel4,
                                uart_serial_funs,
                                uart_serial_info4,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART4_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf4[0],
                                sizeof(uart_serial_out_buf4),
                                &uart_serial_in_buf4[0],
                                sizeof(uart_serial_in_buf4));
#else
static
SERIAL_CHANNEL(uart_serial_channel4,
               uart_serial_funs,
               uart_serial_info4,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART4_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io4,
             CYGDAT_IO_SERIAL_FREESCALE_UART4_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel4);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART4


#if defined CYGPKG_IO_SERIAL_FREESCALE_UART5
static const uart_pins_t uart5_pins = {
    rx  : CYGHWR_IO_FREESCALE_UART5_PIN_RX,
    tx  : CYGHWR_IO_FREESCALE_UART5_PIN_TX,
    rts : CYGHWR_IO_FREESCALE_UART5_PIN_RTS,
    cts : CYGHWR_IO_FREESCALE_UART5_PIN_CTS
};
static uart_serial_info uart_serial_info5 = {
    uart_base          : CYGADDR_IO_SERIAL_FREESCALE_UART5_BASE,
    interrupt_num      : CYGNUM_IO_SERIAL_FREESCALE_UART5_INT_VECTOR,
    interrupt_priority : CYGNUM_IO_SERIAL_FREESCALE_UART5_INT_PRIORITY,
    pins_p             : &uart5_pins
};
#if CYGNUM_IO_SERIAL_FREESCALE_UART5_BUFSIZE > 0
static unsigned char
    uart_serial_out_buf5[CYGNUM_IO_SERIAL_FREESCALE_UART5_BUFSIZE];
static unsigned char
    uart_serial_in_buf5[CYGNUM_IO_SERIAL_FREESCALE_UART5_BUFSIZE];

static
SERIAL_CHANNEL_USING_INTERRUPTS(uart_serial_channel5,
                                uart_serial_funs,
                                uart_serial_info5,
                                CYG_SERIAL_BAUD_RATE(
                                     CYGNUM_IO_SERIAL_FREESCALE_UART5_BAUD),
                                CYG_SERIAL_STOP_DEFAULT,
                                CYG_SERIAL_PARITY_DEFAULT,
                                CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                CYG_SERIAL_FLAGS_DEFAULT,
                                &uart_serial_out_buf5[0],
                                sizeof(uart_serial_out_buf5),
                                &uart_serial_in_buf5[0],
                                sizeof(uart_serial_in_buf5));
#else
static
SERIAL_CHANNEL(uart_serial_channel5,
               uart_serial_funs,
               uart_serial_info5,
               CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_FREESCALE_UART5_BAUD),
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT);
#endif
DEVTAB_ENTRY(uart_serial_io5,
             CYGDAT_IO_SERIAL_FREESCALE_UART5_NAME,
             0, // does not depend on a lower level device driver
             &cyg_io_serial_devio,
             uart_serial_init,
             uart_serial_lookup,
             &uart_serial_channel5);
#endif // ifdef CYGPKG_IO_SERIAL_FREESCALE_UART5
