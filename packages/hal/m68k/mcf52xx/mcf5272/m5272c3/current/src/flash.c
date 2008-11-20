//==========================================================================
//
//      m5272c3_flash.c
//
//      Support for AMD Flash devices on M5272C3 evaluation board
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2006,2008 Free Software Foundation, Inc.
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
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#ifdef CYGPKG_IO_FLASH
# include <cyg/io/flash.h>
# include <cyg/io/flash_dev.h>
# include <cyg/io/am29xxxxx_dev.h>

// There is a single AM29PL160C device.
static const CYG_FLASH_FUNS(hal_m5272c3_flash_amd_funs,
                      &cyg_am29xxxxx_init_check_devid_16,
                      &cyg_flash_devfn_query_nop,
                      &cyg_am29xxxxx_erase_16,
                      &cyg_am29xxxxx_program_16,
                      (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                      &cyg_am29xxxxx_lock_nop,
                      &cyg_am29xxxxx_unlock_nop);

static const cyg_am29xxxxx_dev hal_m5272c3_flash_priv = {
    .devid      = 0x2245,
    .block_info = {
        { 0x00004000, 1 },
        { 0x00002000, 2 },
        { 0x00038000, 1 },
        { 0x00040000, 7 }
    }
};

CYG_FLASH_DRIVER(hal_m5272c3_flash,
                 &hal_m5272c3_flash_amd_funs,
                 0,
                 0xFFE00000,
                 0xFFFFFFFF,
                 4,
                 hal_m5272c3_flash_priv.block_info,
                 &hal_m5272c3_flash_priv
);
#endif
