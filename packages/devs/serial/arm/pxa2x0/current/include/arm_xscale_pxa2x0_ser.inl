//==========================================================================
//
//      io/serial/arm/arm_xscale_pxa2x0_ser.inl
//
//      Generic PXA 2X0 Serial I/O definitions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):    msalter
// Contributors: msalter, Alexander Neundorf
// Date:         21st November 2006
// Purpose:      PXA2X0 Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_intr.h>

//-----------------------------------------------------------------------------
// Baud rate specification

static unsigned short select_baud[] = {
    0,    // Unused
    0,    // 50
    0,    // 75
    0,    // 110
    0,    // 134.5
    0,    // 150
    0,    // 200
    0,    // 300
    0,    // 600
    0,    // 1200
    0,    // 1800
    0,    // 2400
    0,    // 3600
    0,    // 4800
    0,    // 7200
    0,    // 9600
   64,    // 14400
   48,    // 19200
   24,    // 38400
   16,    // 57600
    8,    // 115200
};

#ifdef CYGPKG_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0
static pc_serial_info pxa2x0_serial_info0 = {PXA2X0_FFUART_BASE,
                                          CYGNUM_HAL_INTERRUPT_FFUART};
#if CYGNUM_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_BUFSIZE > 0
static unsigned char pxa2x0_serial_out_buf0[CYGNUM_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_BUFSIZE];
static unsigned char pxa2x0_serial_in_buf0[CYGNUM_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pxa2x0_serial_channel0,
                                       pc_serial_funs, 
                                       pxa2x0_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pxa2x0_serial_out_buf0[0], sizeof(pxa2x0_serial_out_buf0),
                                       &pxa2x0_serial_in_buf0[0], sizeof(pxa2x0_serial_in_buf0)
                                      );
#else
static SERIAL_CHANNEL(pxa2x0_serial_channel0,
                      pc_serial_funs, 
                      pxa2x0_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
                     );
#endif

DEVTAB_ENTRY(pxa2x0_serial_io0,
             CYGDAT_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pxa2x0_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_ARM_XSCALE_PXA2X0_SERIAL0


// EOF arm_xscale_pxa2x0_ser.inl
