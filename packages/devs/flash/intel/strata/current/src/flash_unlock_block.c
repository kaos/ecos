//==========================================================================
//
//      flash_unlock_block.c
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
// Author(s):    gthomas, hmt
// Contributors: gthomas
// Date:         2001-02-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "strata.h"

#include <cyg/hal/hal_cache.h>

//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//

//
// The difficulty with this operation is that the hardware does not support
// unlocking single blocks.  However, the logical layer would like this to
// be the case, so this routine emulates it.  The hardware can clear all of
// the locks in the device at once.  This routine will use that approach and
// then reset the regions which are known to be locked.
//

#define MAX_FLASH_BLOCKS 128

int
flash_unlock_block(volatile flash_t *block, int block_size, int blocks)
{
    volatile flash_t *ROM, *bp, *bpv;
    flash_t stat;
    int timeout = 5000000;
    unsigned char is_locked[MAX_FLASH_BLOCKS];
    int i, cache_on;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    // Get base address and map addresses to virtual addresses
    ROM = FLASH_P2V( CYGNUM_FLASH_BASE_MASK & (unsigned int)block );
    block = FLASH_P2V(block);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    // Get current block lock state.  This needs to access each block on
    // the device so currently locked blocks can be re-locked.
    bp = ROM;
    for (i = 0;  i < blocks;  i++) {
        bpv = FLASH_P2V( bp );
        *bpv = FLASH_Read_Query;
        if (bpv == block) {
            is_locked[i] = 0;
        } else {
            is_locked[i] = bpv[2];
        }
        bp += block_size / sizeof(*bp);
    }

    // Clears all lock bits
    ROM[0] = FLASH_Clear_Locks;
    ROM[0] = FLASH_Clear_Locks_Confirm;  // Confirmation
    timeout = 5000000;
    while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
        if (--timeout == 0) break;
    }

    // Restore the lock state
    bp = ROM;
    for (i = 0;  i < blocks;  i++) {
        bpv = FLASH_P2V( bp );
        if (is_locked[i]) {
            *bpv = FLASH_Set_Lock;
            *bpv = FLASH_Set_Lock_Confirm;  // Confirmation
            timeout = 5000000;
            while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
                if (--timeout == 0) break;
            }
        }
        bp += block_size / sizeof(*bp);
    }

    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return stat;
}
