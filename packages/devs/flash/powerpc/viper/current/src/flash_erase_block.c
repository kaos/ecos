//==========================================================================
//
//      flash_erase_block.c
//
//      Flash programming
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "flash.h"

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//

int flash_erase_block(volatile unsigned char *block)
{
    volatile unsigned char *ROM;
    volatile unsigned char *sector = block;
    int timeout = 50000;
    int cache_on;
    int len, sector_size;
    int erase_state, new_state;
    long offset;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    ROM = (volatile unsigned char *)((unsigned long)block & 0xFF800000);

    // We treat the low 4 sectors like a single block
    // These sectors are 0x4000, 0x2000, 0x2000, 0x8000 long
    offset = ((unsigned long)block - (unsigned long)ROM);
    if (offset < FLASH_BLOCK_SIZE) {
        sector_size = 0x4000;
    } else {
        sector_size = FLASH_BLOCK_SIZE;
    }

    len = 0;
    while (len < FLASH_BLOCK_SIZE) {
        // Erase block - six step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Erase;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        *sector = FLASH_Block_Erase;
        for (timeout = 50;  timeout--;  timeout > 0) ;
        erase_state = *sector & FLASH_Busy;
        timeout = 5000000;
        while (true) {
            new_state = *sector;
            if ((new_state & FLASH_Busy) == erase_state) break;  // "toggle" stopped
            erase_state = new_state & FLASH_Busy;
            if (new_state & FLASH_Err) break;
            if (--timeout == 0) break;
        }
        sector += sector_size;
        len += sector_size;
        offset += sector_size;
        if (offset == 0x4000) {
            sector_size = 0x2000;
        } else if (offset == 0x8000) {
            sector_size = 0x8000;
        }
    }

    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;

    len = FLASH_BLOCK_SIZE;
    while (len > 0) {
        if (*block++ != 0xFF) break;
        len -= sizeof(*block);
    }
    if (len == 0) new_state = 0;

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return new_state;
}
