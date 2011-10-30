#ifndef CYGONCE_SER_FREESCALE_UART_TERMIOS_ADD_INL
#define CYGONCE_SER_FREESCALE_UART_TERMIOS_ADD_INL
//=============================================================================
//
//      ser_freescale_uart_termiostty_add.inl
//
//      Termios definitions beyond #3.
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):      ilijak
// Contributor(s):
// Date:           2011-03-28
// Purpose:        Termios definitions beyond #3.
// Description: 
// Usage:          This file is included by pkgconf/io/serial.h
//
//####DESCRIPTIONEND####
//
//=============================================================================


#ifdef CYGPKG_IO_SERIAL_TERMIOS_TERMIOS4
static struct termios_private_info termios_private_info4;
DEVTAB_ENTRY(termios_io4,
             "/dev/termios4",
             CYGDAT_IO_SERIAL_TERMIOS_TERMIOS4_DEV,
             &termios_devio,
             termios_init,
             termios_lookup,
             &termios_private_info4);
#endif

#ifdef CYGPKG_IO_SERIAL_TERMIOS_TERMIOS5
static struct termios_private_info termios_private_info5;
DEVTAB_ENTRY(termios_io5,
             "/dev/termios5",
             CYGDAT_IO_SERIAL_TERMIOS_TERMIOS5_DEV,
             &termios_devio,
             termios_init,
             termios_lookup,
             &termios_private_info5);
#endif
//-----------------------------------------------------------------------------
// end of ser_freescale_uart_termiostty_add.inl
#endif // CYGONCE_SER_FREESCALE_UART_TERMIOS_ADD_INL
