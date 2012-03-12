//==========================================================================
//
//      ser_cortexm_a2fxxx.inl
//
//      eCos serial Cortex-M3/Actel Smartfusion I/O definitions
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
// Author(s):    ccoutand
// Contributors:
// Date:         2011-03-20
// Purpose:      Cortex-M3/Actel Smartfusion Serial I/O module (interrupt driven version)
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_intr.h>

//-----------------------------------------------------------------------------
// Baud rate specification

// Baud rate specification
static const unsigned int select_baud[] =
{
    9999,    // Unused
    50,
    75,
    110,
    134.5,
    150,
    200,
    300,
    600,
    1200,
    1800,
    2400,
    3600,
    4800,
    7200,
    9600,
    14400,
    19200,
    38400,
    57600,
    115200,
    230400
};


//-----------------------------------------------------------------------------
// Return baudrate devisor for certain baudrate

unsigned short a2fxxx_baud_generator(pc_serial_info *ser_chan,
                                      cyg_serial_baud_rate_t  baud)
{
    cyg_uint8 pclk_id = 0;
    switch (ser_chan->base)
    {
        case CYGHWR_HAL_A2FXXX_UART0:
             pclk_id = 0;
             break;

        case CYGHWR_HAL_A2FXXX_UART1:
             pclk_id = 1;
             break;
        default:
            CYG_FAIL("Invalid UART base address");
    } // (ser_chan->base)

    return CYG_HAL_CORTEXM_A2FXXX_BAUD_GENERATOR(pclk_id, select_baud[baud]);
}


#define CYG_IO_SERIAL_GENERIC_16X5X_CHAN_BAUD_GENERATOR(_ser_chan_, _baud_) \
        a2fxxx_baud_generator((_ser_chan_), (_baud_))


#ifdef CYGPKG_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0
static pc_serial_info a2fxxx_serial_info0 =
  { CYGHWR_HAL_A2FXXX_UART0,
    CYGNUM_HAL_INTERRUPT_UART0,
    CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_INTPRIO
  };

#if CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_BUFSIZE > 0
static unsigned char
a2fxxx_serial_out_buf0[CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_BUFSIZE];
static unsigned char
a2fxxx_serial_in_buf0[CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(a2fxxx_serial_channel0,
                                       pc_serial_funs,
                                       a2fxxx_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &a2fxxx_serial_out_buf0[0],
                                       sizeof(a2fxxx_serial_out_buf0),
                                       &a2fxxx_serial_in_buf0[0],
                                       sizeof(a2fxxx_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(a2fxxx_serial_channel0,
                      pc_serial_funs,
                      a2fxxx_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(a2fxxx_serial_io0,
             CYGDAT_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio,
             pc_serial_init,
             pc_serial_lookup,     // Serial driver may need initializing
             &a2fxxx_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_A2FXXX_SERIAL0

#ifdef CYGPKG_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1
static pc_serial_info a2fxxx_serial_info1 =
  { CYGHWR_HAL_A2FXXX_UART1,
    CYGNUM_HAL_INTERRUPT_UART1,
    CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_INTPRIO
  };
#if CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_BUFSIZE > 0
static unsigned char
a2fxxx_serial_out_buf1[CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_BUFSIZE];
static unsigned char
a2fxxx_serial_in_buf1[CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(a2fxxx_serial_channel1,
                                       pc_serial_funs,
                                       a2fxxx_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &a2fxxx_serial_out_buf1[0],
                                       sizeof(a2fxxx_serial_out_buf1),
                                       &a2fxxx_serial_in_buf1[0],
                                       sizeof(a2fxxx_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(a2fxxx_serial_channel1,
                      pc_serial_funs,
                      a2fxxx_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(a2fxxx_serial_io1,
             CYGDAT_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio,
             pc_serial_init,
             pc_serial_lookup,     // Serial driver may need initializing
             &a2fxxx_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_CORTEXM_A2FXXX_SERIAL1

// EOF ser_cortexm_a2fxxx.inl
