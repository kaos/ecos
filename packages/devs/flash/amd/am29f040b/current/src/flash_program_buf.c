//==========================================================================
//
//      flash_program_buf.c
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

int
flash_program_buf(volatile flash_data_t *addr, flash_data_t *data, int len)
{
    volatile flash_data_t *ROM;
    int timeout;
    int fail = 0;
    flash_data_t array1[CYGHWR_FLASH_INTERLEAVE];
    flash_data_t array2[CYGHWR_FLASH_INTERLEAVE];
    flash_data_t *state, *new_state, *tmp_state;

#if 0
    CYG_ASSERT(data & (sizeof(flash_data_t)-1) == 0, 
               "Data not properly aligned");
    CYG_ASSERT(addr & (CYGHWR_FLASH_INTERLEAVE*sizeof(flash_data_t)-1) == 0, 
               "Addr not properly aligned (to first interleaved device)");
#endif

    while (len > 0) {
        int i;

        // Base address of device(s) being programmed. 
        ROM = (volatile flash_data_t*)((unsigned long)addr & ~(FLASH_DEVICE_SIZE-1));

#if 1 // So much for my good intentions
        {
        int erase_state2, new_state2;

        // Program data [byte] - 4 step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Program;
        *addr = *data;
        for (timeout = 50;  timeout--;  timeout > 0) ;
        erase_state2 = *addr & FLASH_Busy;
        while (true) {
            new_state2 = *addr;
            if ((new_state2 & FLASH_Busy) == erase_state2) break;  // "toggle" stopped
            erase_state2 = new_state2 & FLASH_Busy;
            if (new_state2 & FLASH_Err) break;
            if (--timeout == 0) break;
        }
        ROM[0] = FLASH_Reset;            
        if (*addr++ != *data++) {
            new_state2 = 0xFF;
            break;
        }
        len -= sizeof(*data);
    }

#else
        // Program data - 4 step sequence
        FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Setup_Code1);
        FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr2, FLASH_Setup_Code2);
        FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Program);
        FLASH_DATA_WRITE(addr, data);

        state = array1;
        new_state = array2;

        FLASH_DATA_READ(addr, state);
        timeout = 5000000;              // how many retries?
        while (true) {
            int i;
            int done = 1;

            FLASH_DATA_READ(addr, new_state);
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
        // We have state in 'state' now, use 'new_state' to verify data.
        for (i = 0; i < CYGHWR_FLASH_INTERLEAVE; i++) {
            if (*addr++ != *data++) {
                state[i] = FLASH_Drv_Verify_Err;
                fail = FLASH_Drv_Verify_Err;
            }
        }
        if (fail) break;

        len -= sizeof(flash_data_t) * CYGHWR_FLASH_INTERLEAVE;
#endif
    }

    // Ideally, we'd want to return not only the failure code, but also
    // the address/device that reported the error.
    return fail;
}
