#ifndef CYGONCE_DEVS_RAM_MICRON_CELLULAR_H
#define CYGONCE_DEVS_RAM_MICRON_CELLULAR_H
//==========================================================================
//
//      ram_micron_cellularram.h
//
//      Micron CellularRam 1.5
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):    ilijak
// Contributors:
// Date:         2011-10-05
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_ram_micron_cellularram.h>

#define CYGHWR_DEVS_RAM_MICRON_RCR  0
#define CYGHWR_DEVS_RAM_MICRON_BCR  1
#define CYGHWR_DEVS_RAM_MICRON_DIDR 2

#ifndef __ASSEMBLER__

__externC cyg_uint16 ram_micron_reg_get(cyg_uint16 mem_reg_i,
                                        volatile cyg_uint16 *ram_base_p,
                                        cyg_uint32 ram_size);

__externC void ram_micron_reg_set(cyg_uint16 mem_reg_i, cyg_uint16 setting,
                                  volatile cyg_uint16 *ram_base_p,
                                  cyg_uint32 ram_size);
#endif

#endif  // CYGONCE_DEVS_RAM_MICRON_CELLULAR_H
// ------------------------------------------------------------------------
// EOF ram_micron_cellularram.h
