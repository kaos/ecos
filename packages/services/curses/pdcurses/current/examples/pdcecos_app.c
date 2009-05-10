// ==========================================================================
// 
//      pdcecos_app.c
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
// Purpose:      Early startup for PDCurses stuff
// Description:
// 
// ####DESCRIPTIONEND####
// 
// ========================================================================*/

// ---------------------------------------------------------------------------
// CONFIGURATION CHECKS 
// 

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
#ifdef CYGPKG_FS_JFFS2
# include <pkgconf/io_flash.h>
#define USE_JFFS2
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

#include <pkgconf/pdcurses.h>

// ---------------------------------------------------------------------------
// INCLUDES 
// 

#include <stdio.h>                     // printf
#include <stdlib.h>                    // printf

#include <cyg/kernel/kapi.h>           // All the kernel specific stuff
#include <cyg/hal/hal_arch.h>          // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include "pdcecos_app.h"

// Define table boundaries
CYG_HAL_TABLE_BEGIN(__PDC_APP_TAB__, _pdc_apps);
CYG_HAL_TABLE_END(__PDC_APP_TAB_END__, _pdc_apps);
extern struct _pdc_app_entry __PDC_APP_TAB__[],
                __PDC_APP_TAB_END__;

// ---------------------------------------------------------------------------
// PDCurses startup thread. 
// 
static char     startup_stack[STACKSIZE];
cyg_handle_t    startup_thread;
cyg_thread      startup_thread_obj;

static void
startup(CYG_ADDRESS data)
{
    cyg_ucount32    pdc_data_index;
    struct _pdc_app_entry *pdc;

    printf("SYSTEM INITIALIZATION in progress\n");

#ifdef USE_JFFS2
    {
        int             res;
        printf("... Mounting JFFS2 on \"/\"\n");
        res = mount(CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1, "/", "jffs2");
        if (res < 0) {
            printf("Mount \"/\" failed - res: %d\n", res);
        }
        chdir("/");
    }
#endif

    pdc_data_index = cyg_thread_new_data_index();
    printf("data index = %d\n", pdc_data_index);

    printf("Creating system threads\n");
    for (pdc = __PDC_APP_TAB__; pdc != &__PDC_APP_TAB_END__; pdc++) {
        printf("Creating %s thread\n", pdc->name);
        cyg_thread_create(pdc->prio,
                          pdc->entry,
                          (cyg_addrword_t) pdc_data_index,
                          pdc->name,
                          (void *) pdc->stack, STACKSIZE,
                          &pdc->t, &pdc->t_obj);
    }
    printf("Starting threads\n");
    for (pdc = __PDC_APP_TAB__; pdc != &__PDC_APP_TAB_END__; pdc++) {
        printf("Starting %s\n", pdc->name);
        cyg_thread_resume(pdc->t);
        if (pdc->init) {
            (pdc->init) (pdc_data_index);
        }
    }

    printf("SYSTEM THREADS STARTED!\n");
}

void
cyg_user_start(void)
{
    // Create the initial thread and start it up
    cyg_thread_create(PDC_ECOS_STARTUP_PRIORITY,
                      startup,
                      (cyg_addrword_t) 0,
                      "System startup",
                      (void *) startup_stack, STACKSIZE,
                      &startup_thread, &startup_thread_obj);
    cyg_thread_resume(startup_thread);
}

// ---------------------------------------------------------------------------
// EOF pdcecos_app.c
