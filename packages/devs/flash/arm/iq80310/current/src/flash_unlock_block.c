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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, msalter
// Date:         2000-09-10
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "flash.h"

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
flash_unlock_block(volatile unsigned char *block, int block_size, int blocks)
{
    volatile unsigned short *ROM, *bp;
    unsigned short stat;
    int timeout = 5000000;
    unsigned short is_locked[MAX_FLASH_BLOCKS];
    int i, cache_on;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    ROM = FLASH_P2V((unsigned long)block & 0xFF800000);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    // Get current block lock state.  This needs to access each block on
    // the device so currently locked blocks can be re-locked.
    bp = (unsigned short *)((unsigned long)block & 0xFF800000);
    for (i = 0;  i < blocks;  i++) {
        if (bp == block) {
            is_locked[i] = 0;
        } else {
	    *(volatile unsigned short *)FLASH_P2V(bp) = FLASH_Read_Query;
            is_locked[i] = ((volatile unsigned short *)FLASH_P2V(bp))[2];
        }
        bp += block_size / sizeof(*bp);
    }

    // Clears all lock bits
    FLASH_P2V(block)[0] = FLASH_Clear_Locks;
    FLASH_P2V(block)[0] = FLASH_Clear_Locks_Confirm;  // Confirmation
    timeout = 5000000;
    while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
        if (--timeout == 0) goto done;
    }

    // Restore the lock state
    bp = (unsigned char *)((unsigned long)block & 0xFF800000);
    for (i = 0;  i < blocks;  i++) {
        if (is_locked[i]) {
            *FLASH_P2V(bp) = FLASH_Set_Lock;
            *FLASH_P2V(bp) = FLASH_Set_Lock_Confirm;  // Confirmation
            timeout = 5000000;
            while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
                if (--timeout == 0) goto done;
            }
        }
        bp += block_size / sizeof(*bp);
    }

 done:
    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return stat;
}
