//==========================================================================
//
//      io/serial/common/haldiag.c
//
//      Serial I/O interface module using HAL I/O routines
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
// Author(s):   gthomas
// Contributors:  gthomas
// Date:        1999-02-04
// Purpose:     HAL/diag serial driver
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/io.h>
#include <pkgconf/io_serial.h>
#ifdef CYGPKG_IO_SERIAL_HALDIAG
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_diag.h>

static bool haldiag_init(struct cyg_devtab_entry *tab);
static bool haldiag_putc(serial_channel *chan, unsigned char c);
static unsigned char haldiag_getc(serial_channel *chan);
static bool haldiag_set_config(serial_channel *chan, cyg_serial_info_t *config);

static SERIAL_FUNS(haldiag_funs, 
                   haldiag_putc, 
                   haldiag_getc,
                   haldiag_set_config,
                   0,                      // start xmit - not used
                   0                       // stop xmit - not used
    );

static int _no_data;
static SERIAL_CHANNEL(haldiag_channel0,
               haldiag_funs, 
               _no_data,
               CYG_SERIAL_BAUD_DEFAULT,
               CYG_SERIAL_STOP_DEFAULT,
               CYG_SERIAL_PARITY_DEFAULT,
               CYG_SERIAL_WORD_LENGTH_DEFAULT,
               CYG_SERIAL_FLAGS_DEFAULT
    );
DEVTAB_ENTRY(haldiag_io0, 
             "/dev/haldiag",
             0,                     // Does not depend on a lower level interface
             &cyg_io_serial_devio, 
             haldiag_init, 
             0,                     // No initialization/lookup needed
             &haldiag_channel0);

static void
haldiag_config_port(serial_channel *chan)
{
}

static bool 
haldiag_init(struct cyg_devtab_entry *tab)
{
    serial_channel *chan = (serial_channel *)tab->priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("HAL/diag SERIAL init\n");
#endif
    haldiag_config_port(chan);
    return true;
}

// Return 'true' if character is sent to device
static bool
haldiag_putc(serial_channel *chan, unsigned char c)
{
    HAL_DIAG_WRITE_CHAR(c);
    return true;
}

static unsigned char 
haldiag_getc(serial_channel *chan)
{
    unsigned char c;
    HAL_DIAG_READ_CHAR(c);
    return c;
}

static bool 
haldiag_set_config(serial_channel *chan, cyg_serial_info_t *config)
{
    diag_printf("%s\n", __FUNCTION__);
    return true;
}
#endif // CYGPKG_IO_SERIAL_HALDIAG
