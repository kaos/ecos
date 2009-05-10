// ==========================================================================
// 
//      pdcecos_thread.c
// 
//      Public Domain Curses for eCos
// 
// ===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.
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
// ===========================================================================
// ===========================================================================
// #####DESCRIPTIONBEGIN####
// 
// Author(s):    Sergei Gavrikov
// Contributors: Sergei Gavrikov
// Date:         2009-03-26
// Purpose:      Used to run PDCurses program as eCos thread.
// Description:
// 
// ####DESCRIPTIONEND####
// 
// ========================================================================*/

#include <pkgconf/system.h>            // which packages are enabled/disabled
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif
#ifdef CYGPKG_LIBC
# include <pkgconf/libc.h>
#endif
#ifdef CYGPKG_IO_SERIAL
# include <pkgconf/io_serial.h>
#endif

#ifndef CYGFUN_KERNEL_API_C
# error Kernel API must be enabled to build this application
#endif

#ifndef CYGPKG_LIBC_STDIO
# error C library standard I/O must be enabled to build this application
#endif

#ifndef CYGPKG_IO_SERIAL_HALDIAG
# error I/O HALDIAG pseudo-device driver must be enabled to build this application
#endif

// --------------------------------------------------------------------------
// INCLUDES
// 

#include <cyg/kernel/kapi.h>           // All the kernel specific stuff
#include <cyg/infra/diag.h>

#include <pkgconf/pdcurses.h>
#include <curses.h>

#include "pdcecos_app.h"

extern void     pdcecos_init(CYG_ADDRWORD data);
_pdc_app("PDCECOS", pdcecos, PDC_ECOS_CORE_PRIORITY, pdcecos_init);

// --------------------------------------------------------------------------
// Component interfaces
// 

// --------------------------------------------------------------------------
// pdcecos_main --
// 
// Note: the pdcecos_main() is declared as a weaked function, Look at the
//       "pdcecos_app.h". So, you can have own one in a user space.
int
pdcecos_main(int argc, char **argv)
{
    initscr();
    printw("Hello PDCurses+eCos World!");
    refresh();
    getch();
    endwin();

    return 0;
}

// --------------------------------------------------------------------------
// pdcecos_thread --
// 
void
pdcecos_thread(CYG_ADDRWORD data)
{
    int             argc = 0;
    char          **argv = NULL;

    CYG_UNUSED_PARAM(CYG_ADDRWORD, data);

    INIT_PER_THREAD_DATA();

    pdcecos_main(argc, argv);
}

// ---------------------------------------------------------------------------
// EOF pdcecos_thread.c
