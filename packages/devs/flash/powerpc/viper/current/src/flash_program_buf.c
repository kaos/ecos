//==========================================================================
//
//      flash_program_buf.c
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

int
flash_program_buf(volatile unsigned char *addr, unsigned char *data, int len)
{
    volatile unsigned char *ROM;
    int timeout = 50000;
    int cache_on;
    int erase_state, new_state;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    ROM = (volatile unsigned long *)((unsigned long)addr & 0xFF800000);

    while (len > 0) {
        // Program data [byte] - 4 step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Program;
        *addr = *data;
        for (timeout = 50;  timeout--;  timeout > 0) ;
        erase_state = *addr & FLASH_Busy;
        while (true) {
            new_state = *addr;
            if ((new_state & FLASH_Busy) == erase_state) break;  // "toggle" stopped
            erase_state = new_state & FLASH_Busy;
            if (new_state & FLASH_Err) break;
            if (--timeout == 0) break;
        }
        ROM[0] = FLASH_Reset;            
        if (*addr++ != *data++) {
            new_state = 0xFF;
            break;
        }
        len -= sizeof(*data);
    }
    if (len == 0) new_state = 0;  // Success

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = FLASH_Reset;            

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }
    return new_state;
}
