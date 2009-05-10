#ifndef CYGONCE_PDCECOS_APP_H
#define CYGONCE_PDCECOS_APP_H
// ==========================================================================
// 
//      pdcecos_app.h
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
// Purpose:      Definitions for PDCECOS thread
// Description:
// Usage:        #include "pdcecos_app.h"
// 
// ####DESCRIPTIONEND####
// 
// ========================================================================*/

#include <cyg/kernel/kapi.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/hal/hal_arch.h>          // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <pkgconf/pdcurses.h>

#define STACKSIZE ( CYGNUM_HAL_STACK_SIZE_TYPICAL * 2 )

typedef void    fun(CYG_ADDRWORD);
struct _pdc_app_entry {
    char           *name;
    fun            *entry;
    int             prio;
    fun            *init;
    cyg_handle_t    t;
    cyg_thread      t_obj;
    char            stack[STACKSIZE];
} CYG_HAL_TABLE_TYPE;

#define _pdc_app(_name_,_id_,_pri_,_init_)              \
externC void _id_##_thread(CYG_ADDRWORD data);          \
struct _pdc_app_entry _pdc_app_##_pri_##_##_id_         \
   CYG_HAL_TABLE_QUALIFIED_ENTRY(_pdc_apps,_pri_) =     \
     { _name_, _id_##_thread, _pri_, _init_};


#define INIT_PER_THREAD_DATA()

#define PDC_ECOS_STARTUP_PRIORITY 11
#define PDC_ECOS_CORE_PRIORITY    (PDC_ECOS_STARTUP_PRIORITY+3)

int
pdcecos_main(int argc, char **argv)
    CYGBLD_ATTRIB_WEAK;
#endif

// ---------------------------------------------------------------------
// EOF pdcecos_app.h
