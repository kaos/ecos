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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov
// Date:         2000-12-05
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

int flash_erase_block(volatile flash_data_t *block)
{
    volatile flash_data_t *ROM;
    int timeout = 50000;
    int len;
    int fail = 0;
    flash_data_t array1[CYGHWR_FLASH_INTERLEAVE];
    flash_data_t array2[CYGHWR_FLASH_INTERLEAVE];
    flash_data_t *state, *new_state, *tmp_state;

    ROM = (volatile flash_data_t*)((unsigned long)block & ~(FLASH_DEVICE_SIZE-1));

#if 1 // So much for my good intentions
    {
    int erase_state2, new_state2;

    // Erase block - six step sequence
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Erase;
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    *block = FLASH_Block_Erase;
    for (timeout = 50;  timeout--;  timeout > 0) ;
    erase_state2 = *block & FLASH_Busy;
    timeout = 5000000;
    while (true) {
        new_state2 = *block;
        if ((new_state2 & FLASH_Busy) == erase_state2) break;  // "toggle" stopped
        erase_state2 = new_state2 & FLASH_Busy;
        if (new_state2 & FLASH_Err) break;
        if (--timeout == 0) break;
    }

    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;
    }
#else
    // Erase block - six step sequence
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Setup_Code1);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr2, FLASH_Setup_Code2);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Setup_Erase);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Setup_Code1);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr2, FLASH_Setup_Code2);
    FLASH_CMD_WRITE(block, FLASH_Block_Erase);

    state = array1;
    new_state = array2;

    // Now poll for the completion of the sector erase timer (50us)
    timeout = 5000000;              // how many retries?
    while (true) {
        int i;
        int done = 1;

        FLASH_DATA_READ(block, state);
        for (i = 0; i < CYGHWR_FLASH_INTERLEAVE; i++) {
            if (0 == (state[i] & FLASH_Sector_Erase_Timer))
                done = 0;
        }

        if (done) break;            // sector erase timers completed
        if (--timeout == 0) break;  // too many tries without success
    }

    FLASH_DATA_READ(block, state);
    timeout = 5000000;              // how many retries?
    while (true) {
        int i;
        int done = 1;

        FLASH_DATA_READ(block, new_state);
        for (i = 0; i < CYGHWR_FLASH_INTERLEAVE; i++) {
            if ((new_state[i] & FLASH_Busy) != (state[i] & FLASH_Busy)) {
                done = 0;
                if (new_state[i] & FLASH_Err)
                    fail = FLASH_Err;
            }
        }
        // Switch state arrays
        tmp_state = state;
        state = new_state;
        new_state = state;

        if (done) break;            // "toggle" stopped
        if (fail) break;            // device failure
        if (--timeout == 0) break;  // too many tries without success
    }
    FLASH_CMD_WRITE(ROM, FLASH_Reset);
#endif    
    if (!fail) {
        // Verify erase operation
        len = (FLASH_BLOCK_SIZE*CYGHWR_FLASH_INTERLEAVE);
        while (len > 0) {
            if (*block++ != 0xFF) {
                fail = FLASH_Drv_Verify_Err;
                break;
            }
            len -= sizeof(*block);
        }
    }

    return fail;
}
