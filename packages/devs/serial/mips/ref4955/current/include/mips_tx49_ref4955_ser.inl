//==========================================================================
//
//      devs/serial/mips/ref4955/src/mips_tx49_ref4955_ser.inl
//
//      REF4955 Serial I/O definitions.
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:gthomas, jskov
// Date:        2000-05-24
// Purpose:     REF4955 Serial definitions
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>

//-----------------------------------------------------------------------------
// There are two serial ports.
#define CYG_DEVICE_SERIAL_SCC1    0xb40003f8 // port 1
#define CYG_DEVICE_SERIAL_SCC2    0xb40002f8 // port 2

//-----------------------------------------------------------------------------
// The REF4955 board has a 14.318 MHz crystal, but the PC87338 part
// uses a 24MHz internal clock for baud rate calculation.
#define BAUD_DIVISOR(_x_) 24000000/13/16/(_x_)

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
    BAUD_DIVISOR(1200),
    0,    // 1800
    BAUD_DIVISOR(2400),
    0,    // 3600
    BAUD_DIVISOR(4800),
    0 ,   // 7200
    BAUD_DIVISOR(9600),
    BAUD_DIVISOR(14400),
    BAUD_DIVISOR(19200),
    BAUD_DIVISOR(38400),
    BAUD_DIVISOR(57600),
    BAUD_DIVISOR(115200),
    0,    // 230400
};

//-----------------------------------------------------------------------------
// Port 0 descriptors

#ifdef CYGPKG_IO_SERIAL_MIPS_REF4955_SERIAL0
static pc_serial_info pc_serial_info0 = {CYG_DEVICE_SERIAL_SCC1, 
                                         CYGNUM_HAL_INTERRUPT_DEBUG_UART};
#if CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL0_BUFSIZE > 0
static unsigned char pc_serial_out_buf0[CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL0_BUFSIZE];
static unsigned char pc_serial_in_buf0[CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel0,
                                       pc_serial_funs, 
                                       pc_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf0[0], 
                                       sizeof(pc_serial_out_buf0),
                                       &pc_serial_in_buf0[0], 
                                       sizeof(pc_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel0,
                      pc_serial_funs, 
                      pc_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io0, 
             CYGDAT_IO_SERIAL_MIPS_REF4955_SERIAL0_NAME,
             0,                 // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel0
    );
#endif

//-----------------------------------------------------------------------------
// Port 1 descriptors

#ifdef CYGPKG_IO_SERIAL_MIPS_REF4955_SERIAL1
static pc_serial_info pc_serial_info1 = {CYG_DEVICE_SERIAL_SCC2, 
                                         CYGNUM_HAL_INTERRUPT_USER_UART};
#if CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL1_BUFSIZE > 0
static unsigned char pc_serial_out_buf1[CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL1_BUFSIZE];
static unsigned char pc_serial_in_buf1[CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel1,
                                       pc_serial_funs, 
                                       pc_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf1[0], 
                                       sizeof(pc_serial_out_buf1),
                                       &pc_serial_in_buf1[0], 
                                       sizeof(pc_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel1,
                      pc_serial_funs, 
                      pc_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_MIPS_REF4955_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io1, 
             CYGDAT_IO_SERIAL_MIPS_REF4955_SERIAL1_NAME,
             0,                   // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel1
    );
#endif

// EOF mips_tx49_ref4955_ser.inl
