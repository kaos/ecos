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
// Contributors: gthomas
// Date:         2000-09-10
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "flash.h"

#include <cyg/hal/hal_arch.h>

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
flash_unlock_block(volatile unsigned long *block, int block_size, int blocks)
{
    volatile unsigned long *ROM, *bp;
    unsigned long stat;
    int timeout = 5000000;
    unsigned char is_locked[MAX_FLASH_BLOCKS];
    int i;

    FLASH_WRITE_ENABLE();

    block = (volatile unsigned long *)CYGARC_UNCACHED_ADDRESS((unsigned long)block);
    ROM = (volatile unsigned long *)((unsigned long)block & FLASH_BASE_MASK);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    // Get current block lock state.  This needs to access each block on
    // the device so currently locked blocks can be re-locked.
    bp = ROM;
    for (i = 0;  i < (blocks/2);  i++) {
        if (bp == block) {
            is_locked[i] = 0;
        } else {
	    *bp = FLASH_Read_Query;
            is_locked[i] = bp[2];
        }
        bp += block_size / sizeof(*bp);
    }

    // Clears all lock bits
    block[0] = FLASH_Clear_Locks;
    block[0] = FLASH_Clear_Locks_Confirm;  // Confirmation
    timeout = 5000000;
    while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
        if (--timeout == 0) break;
    }

    // Restore the lock state
    bp = ROM;
    for (i = 0;  i < (blocks/2);  i++) {
        if (is_locked[i]) {
            *bp = FLASH_Set_Lock;
            *bp = FLASH_Set_Lock_Confirm;  // Confirmation
            timeout = 5000000;
            while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
                if (--timeout == 0) break;
            }
        }
        bp += block_size / sizeof(*bp);
    }

    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;

    FLASH_WRITE_DISABLE();

    return stat;
}
