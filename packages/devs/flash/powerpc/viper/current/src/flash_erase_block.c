//==========================================================================
//
//      flash_erase_block.c
//
//      Flash programming
//
//==========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
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
