#ifndef CYGONCE_DEVS_SH_CQ7708_SCI_H
#define CYGONCE_DEVS_SH_CQ7708_SCI_H

//==========================================================================
//
//      io/serial/sh/sh_sh3_cq7708_sci.inl
//
//      Serial I/O Interface Module definitions for SH3/CQ7708
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
// Contributors:jskov
// Date:        1999-06-16
// Purpose:     Defines SCI serial resources for SH3/CQ7708.
// Description: 
//
//####DESCRIPTIONEND####
//==========================================================================


#include <pkgconf/io_serial_sh_cq7708.h>

static sh_sci_info sh_serial_info = {CYGARC_REG_SCSPTR,
                                     CYGNUM_HAL_INTERRUPT_SCI_ERI,
                                     CYGNUM_HAL_INTERRUPT_SCI_RXI,
                                     CYGNUM_HAL_INTERRUPT_SCI_TXI,
                                     SH_SERIAL_SCI_BASE};

#if CYGNUM_IO_SERIAL_SH_CQ7708_SERIAL1_BUFSIZE > 0
static unsigned char sh_serial_out_buf[CYGNUM_IO_SERIAL_SH_CQ7708_SERIAL1_BUFSIZE];
static unsigned char sh_serial_in_buf[CYGNUM_IO_SERIAL_SH_CQ7708_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(sh_serial_channel,
                                       sh_serial_funs, 
                                       sh_serial_info,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_SH_CQ7708_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &sh_serial_out_buf[0], 
                                       sizeof(sh_serial_out_buf),
                                       &sh_serial_in_buf[0],  
                                       sizeof(sh_serial_in_buf)
    );
#else
static SERIAL_CHANNEL(sh_serial_channel,
                      sh_serial_funs, 
                      sh_serial_info,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_SH_CQ7708_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(sh_serial_io,
             CYGDAT_IO_SERIAL_SH_CQ7708_SERIAL1_NAME,
             0,                 // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             sh_serial_init, 
             sh_serial_lookup,          // Serial driver may need initializing
             &sh_serial_channel
    );

#endif // CYGONCE_DEVS_SH_CQ7708_SCI_H
