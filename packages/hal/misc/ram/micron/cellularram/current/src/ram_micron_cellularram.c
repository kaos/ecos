//==========================================================================
//
//      ram_micron_cellularram.c
//
//      Micron CellularRam 1.5 support functions.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
// Author(s):      ilijak
// Contributor(s):
// Date:           2011-02-05
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>           // HAL header

#include <cyg/devs/ram_micron_cellularram.h>

cyg_uint16 ram_micron_reg_get(cyg_uint16 mem_reg_i,
                              volatile cyg_uint16 *mem_base_p,
                              cyg_uint32 ram_size)
{
    mem_base_p +=  (ram_size/2) - 1;
    cyg_uint16 reg_val __attribute__((unused));

    reg_val = *mem_base_p;
    reg_val = *mem_base_p;
    *mem_base_p = mem_reg_i;
    mem_reg_i = *mem_base_p;
    reg_val = mem_base_p[-16];
    return mem_reg_i;
}

void ram_micron_reg_set(cyg_uint16 mem_reg_i, cyg_uint16 setting,
                       volatile cyg_uint16 *mem_base_p, cyg_uint32 ram_size)
{
    mem_base_p += (ram_size/2) - 1;
    cyg_uint16 reg_val __attribute__((unused));

    reg_val = *mem_base_p;
    reg_val = *mem_base_p;
    *mem_base_p = mem_reg_i;
    *mem_base_p = setting;
    reg_val = mem_base_p[-16];
}

//==========================================================================
// EOF ram_micron_cellularram.c
