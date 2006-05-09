#ifndef CYGONCE_HAL_MEMMAP_H
#define CYGONCE_HAL_MEMMAP_H

//=============================================================================
//
//      hal_memmap.h
//
//      Platform specific memory section definitions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:  Wade Jensen
// Date:          2005-25-06
// Purpose:       Memory section definitions specific to the M5272C3 board
// Usage:         Included via CYGHWR_MEMORY_LAYOUT_H
//
//####DESCRIPTIONEND####
//========================================================================

#include <cyg/infra/cyg_type.h>

// WARNING: DO NOT CHANGE THE TYPE OF THESE LABELS. THE LINKER DEFINES
// THESE AND WE WANT TO USE THE VARIABLE ADDRESSES NOT THE VARIABLES
// THEMSELVES.

#define SECTION_DEC(_name_) \
    externC unsigned char __ ## _name_ ## _start[]; \
    externC unsigned char __ ## _name_ ## _end[]; \
    externC unsigned char __ ## _name_ ## _size[];

SECTION_DEC(bss)
SECTION_DEC(ram_data)
SECTION_DEC(rom_data)
SECTION_DEC(uninvar)
SECTION_DEC(romvec)
SECTION_DEC(ramvec)

// ---------------------------------------------------------------------------
// End of hal_memmap.h
#endif // CYGONCE_HAL_MEMMAP_H
