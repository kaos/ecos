//=============================================================================
//
//	io_diag.c
//
//	Redirect diag output to the configured console device
//
//============================================================================
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
//============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	jskov
// Contributors:jskov
// Date:	1999-04-09
// Purpose:     Kernel diagnostic output
// Description:	Implementations of kernel diagnostic routines.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/infra.h>              // CYGDBG_INFRA_DIAG_USE_DEVICE

#if defined(CYGDBG_INFRA_DIAG_USE_DEVICE)

#include <pkgconf/io_serial.h>          // TTY definitions

#include <cyg/infra/cyg_ass.h>

#include <cyg/io/io.h>
#include <cyg/io/io_diag.h>

//#include <cyg/io/devtab.h>
//#include <cyg/io/ttyio.h>

//-----------------------------------------------------------------------------
// Write single char to output

static cyg_ucount8 sync_mode = 0;
static cyg_io_handle_t console_handle = 0;
static cyg_io_handle_t hal_handle = 0;

externC void
diag_device_init(void)
{
    Cyg_ErrNo res;
#ifdef CYGPKG_IO_SERIAL_HALDIAG
    res = cyg_io_lookup("/dev/haldiag", &hal_handle);
    // FIXME: This ASSERT will try to call diag_device_write_char
    // to print out the error string... Recursive crash...
    CYG_ASSERT(ENOERR == res, "Can't lookup /dev/haldiag");
#endif
#ifdef CYGPKG_IO_SERIAL_TTY
    res = cyg_io_lookup(CYGDAT_IO_SERIAL_TTY_CONSOLE, &console_handle);
    // FIXME: This ASSERT will try to call diag_device_write_char
    // to print out the error string... Recursive crash...
    CYG_ASSERT(ENOERR == res, "Can't lookup " CYGDAT_IO_SERIAL_TTY_CONSOLE);
#endif
}

externC void 
diag_device_write_char(char c)
{
    int len = 1;

    if (sync_mode)
        cyg_io_write(hal_handle, &c, &len);
    else
        cyg_io_write(console_handle, &c, &len);
}

externC void 
diag_device_start_sync()
{
    sync_mode = 1;
}

externC void 
diag_device_end_sync()
{
    sync_mode = 0;
}

#endif // CYGDBG_INFRA_DIAG_USE_DEVICE

//-----------------------------------------------------------------------------
// End of io_diag.c
