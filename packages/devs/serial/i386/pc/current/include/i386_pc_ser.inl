//==========================================================================
//
//      io/serial/i386/pc/i386_pc_ser.inl
//
//      i386 PC Serial I/O Interface Module (interrupt driven)
//      for use with 8250s or 16550s.
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                    
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2001-06-08
// Purpose:      PC Serial I/O module (interrupt driven version)
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_intr.h>

//-----------------------------------------------------------------------------
// Baud rate specification, based on raw 24MHz clock

static unsigned short select_baud[] = {
           0,  // Unused
        2304,  // 50
        1536,  // 75
        1047,  // 110
         857,  // 134.5
         768,  // 150
         576,  // 200
         384,  // 300
         192,  // 600
          96,  // 1200
          64,  // 1800
          48,  // 2400
          32,  // 3600
          24,  // 4800
          16,  // 7200
          12,  // 9600
           8,  // 14400
           6,  // 19200
           3,  // 38400
           2,  // 57600
           1,  // 115200
           0,  // 230400
};

#ifdef CYGPKG_IO_SERIAL_I386_PC_SERIAL0
static pc_serial_info pc_serial_info0 = {CYGNUM_IO_SERIAL_I386_PC_SERIAL0_IOBASE,
                                         CYGNUM_IO_SERIAL_I386_PC_SERIAL0_INT};
#if CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE > 0
static unsigned char pc_serial_out_buf0[CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE];
static unsigned char pc_serial_in_buf0[CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel0,
                                       pc_serial_funs, 
                                       pc_serial_info0,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf0[0], sizeof(pc_serial_out_buf0),
                                       &pc_serial_in_buf0[0], sizeof(pc_serial_in_buf0)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel0,
                      pc_serial_funs, 
                      pc_serial_info0,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io0, 
             CYGDAT_IO_SERIAL_I386_PC_SERIAL0_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel0
    );
#endif //  CYGPKG_IO_SERIAL_I386_PC_SERIAL0

#ifdef CYGPKG_IO_SERIAL_I386_PC_SERIAL1
static pc_serial_info pc_serial_info1 = {CYGNUM_IO_SERIAL_I386_PC_SERIAL1_IOBASE,
                                         CYGNUM_IO_SERIAL_I386_PC_SERIAL1_INT};
#if CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE > 0
static unsigned char pc_serial_out_buf1[CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE];
static unsigned char pc_serial_in_buf1[CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(pc_serial_channel1,
                                       pc_serial_funs, 
                                       pc_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &pc_serial_out_buf1[0], sizeof(pc_serial_out_buf1),
                                       &pc_serial_in_buf1[0], sizeof(pc_serial_in_buf1)
    );
#else
static SERIAL_CHANNEL(pc_serial_channel1,
                      pc_serial_funs, 
                      pc_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_I386_PC_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(pc_serial_io1, 
             CYGDAT_IO_SERIAL_I386_PC_SERIAL1_NAME,
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             pc_serial_init, 
             pc_serial_lookup,     // Serial driver may need initializing
             &pc_serial_channel1
    );
#endif //  CYGPKG_IO_SERIAL_I386_PC_SERIAL1

// EOF i386_pc_ser.inl
