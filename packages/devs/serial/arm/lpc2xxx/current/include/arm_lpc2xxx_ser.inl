//==========================================================================
//
//      io/serial/arm/arm_lpc2xxx_ser.inl
//
//      ARM LPC2XXX Serial I/O definitions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2004 eCosCentric Limited 
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
// Author(s):    jani 
// Contributors: gthomas, jlarmour
// Date:         1999-02-04
// Purpose:      LPC2XXX Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_intr.h>

//-----------------------------------------------------------------------------
// Baud rate specification

static unsigned int select_baud[] = {
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

// we need dynamically generated divider values because they depend on the 
// value of pclk which in turn is changeable

#define CYG_IO_SERIAL_GENERIC_16X5X_BAUD_GENERATOR \
            CYG_HAL_ARM_LPC2XXX_BAUD_GENERATOR 

#ifdef CYGPKG_IO_SERIAL_ARM_LPC2XXX_SERIAL0
static pc_serial_info lpc2xxx_serial_info0 = 
  { CYGARC_HAL_LPC2XXX_REG_UART0_BASE,
    CYGNUM_HAL_INTERRUPT_UART0
  };

#if CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL0_BUFSIZE > 0
static unsigned char 
lpc2xxx_serial_out_buf0[CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL0_BUFSIZE];
static unsigned char 
lpc2xxx_serial_in_buf0[CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc2xxx_serial_channel0,
                                       pc_serial_funs, 
                                       lpc2xxx_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc2xxx_serial_out_buf0[0], 
                                       sizeof(lpc2xxx_serial_out_buf0),
                                       &lpc2xxx_serial_in_buf0[0], 
                                       sizeof(lpc2xxx_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(lpc2xxx_serial_channel0,
                      pc_serial_funs, 
                      lpc2xxx_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc2xxx_serial_io0, 
             CYGDAT_IO_SERIAL_ARM_LPC2XXX_SERIAL0_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc2xxx_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC2XXX_SERIAL0

#ifdef CYGPKG_IO_SERIAL_ARM_LPC2XXX_SERIAL1
static pc_serial_info lpc2xxx_serial_info1 = 
  { CYGARC_HAL_LPC2XXX_REG_UART1_BASE,
    CYGNUM_HAL_INTERRUPT_UART1
  };
#if CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL1_BUFSIZE > 0
static unsigned char 
lpc2xxx_serial_out_buf1[CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL1_BUFSIZE];
static unsigned char 
lpc2xxx_serial_in_buf1[CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(lpc2xxx_serial_channel1,
                                       pc_serial_funs, 
                                       lpc2xxx_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &lpc2xxx_serial_out_buf1[0], 
                                       sizeof(lpc2xxx_serial_out_buf1),
                                       &lpc2xxx_serial_in_buf1[0], 
                                       sizeof(lpc2xxx_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(lpc2xxx_serial_channel1,
                      pc_serial_funs, 
                      lpc2xxx_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_LPC2XXX_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(lpc2xxx_serial_io1, 
             CYGDAT_IO_SERIAL_ARM_LPC2XXX_SERIAL1_NAME,
             0,                     // Does not depend on a lower
                                    // level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &lpc2xxx_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_ARM_LPC2XXX_SERIAL1

// EOF arm_lpc2xxx_ser.inl
