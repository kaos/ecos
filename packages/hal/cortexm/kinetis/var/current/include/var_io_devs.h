#ifndef CYGONCE_HAL_VAR_IO_DEVS_H
#define CYGONCE_HAL_VAR_IO_DEVS_H
//===========================================================================
//
//      var_io_devs.h
//
//      Variant specific registers
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-02-05
// Purpose:       Kinetis variant IO provided to various device drivers
// Description:
// Usage:         #include <cyg/hal/var_io.h> //var_io.h includes this file
//
//####DESCRIPTIONEND####
//
//===========================================================================


//=============================================================================
// DEVS:
// Following macros may be, and usually are borrwed by some device drivers.
//-----------------------------------------------------------------------------
// Freescale UART
// Borrow some HAL resources to Freescale UART driver
// UART  macros are used by both:
//      src/hal_diag.c
//      devs/serial/<version>/src/ser_freescale_uart.c

#define CYGADDR_IO_SERIAL_FREESCALE_UART0_BASE  0x4006A000
#define CYGADDR_IO_SERIAL_FREESCALE_UART1_BASE  0x4006B000
#define CYGADDR_IO_SERIAL_FREESCALE_UART2_BASE  0x4006C000
#define CYGADDR_IO_SERIAL_FREESCALE_UART3_BASE  0x4006D000
#define CYGADDR_IO_SERIAL_FREESCALE_UART4_BASE  0x400EA000
#define CYGADDR_IO_SERIAL_FREESCALE_UART5_BASE  0x400EB000

// UART PIN configuration
// Note: May be overriden by plf_io.h

#define CYGHWR_HAL_KINETIS_PORT_PIN_NONE CYGHWR_HAL_KINETIS_PIN_NONE

#ifndef CYGHWR_IO_FREESCALE_UART0_PIN_RX
# define CYGHWR_IO_FREESCALE_UART0_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART0_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART1_PIN_RX
# define CYGHWR_IO_FREESCALE_UART1_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART1_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART2_PIN_RX
# define CYGHWR_IO_FREESCALE_UART2_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART2_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART3_PIN_RX
# define CYGHWR_IO_FREESCALE_UART3_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART3_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART4_PIN_RX
# define CYGHWR_IO_FREESCALE_UART4_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART4_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

#ifndef CYGHWR_IO_FREESCALE_UART5_PIN_RX
# define CYGHWR_IO_FREESCALE_UART5_PIN_RX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_TX CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_RTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
# define CYGHWR_IO_FREESCALE_UART5_PIN_CTS CYGHWR_HAL_KINETIS_PORT_PIN_NONE
#endif

// Lend some HAL dependent functions to the UART serial device driver

#ifndef __ASSEMBLER__

# define CYGHWR_IO_FREESCALE_UART_BAUD_SET(__uart_p, _baud_) \
        hal_freescale_uart_setbaud(__uart_p, _baud_)

# define CYGHWR_IO_FREESCALE_UART_PIN(__pin) \
        hal_set_pin_function(__pin)

// Set baud rate
__externC void hal_freescale_uart_setbaud( CYG_ADDRESS uart, cyg_uint32 baud );

#endif

//---------------------------------------------------------------------------
// ENET
// Lend some HAL dependent functions to the Ethernet device driver
#define CYGADDR_IO_ETH_FREESCALE_ENET0_BASE  (0x400C0000)

#ifndef __ASSEMBLER__

# define CYGHWR_IO_FREESCALE_ENET_PIN(__pin) \
        hal_set_pin_function(__pin)

#endif

//-----------------------------------------------------------------------------
// end of var_io_devs.h
#endif // CYGONCE_HAL_VAR_IO_DEVS_H
