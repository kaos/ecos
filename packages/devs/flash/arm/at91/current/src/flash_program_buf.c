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
// Date:         2001-07-17
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
flash_program_buf(volatile unsigned short *addr, unsigned short *data, int len)
{
    volatile unsigned short *PAGE, *ROM;
    int timeout = 50000;
    int cache_on;
    int i, offset;
    unsigned short hold[FLASH_PAGE_SIZE/2];

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    if (len != FLASH_PAGE_SIZE) {
        return FLASH_LENGTH_ERROR;
    }

    ROM = (volatile unsigned short *)((unsigned long)addr & 0xFFFF0000);
    PAGE = (volatile unsigned short *)((unsigned long)addr & FLASH_PAGE_MASK);

    // Copy current contents (allows for partial updates)
    for (i = 0;  i < FLASH_PAGE_SIZE/2;  i++) {
        hold[i] = PAGE[i];
    }

    // Merge data into holding buffer
    offset = (unsigned long)addr & FLASH_PAGE_OFFSET;
    for (i = 0;  i < (len/2);  i++) {
        hold[offset+i] = data[i];
    }

    // Now write the data

    // Send lead-in sequence
    ROM[FLASH_Key_Addr0] = FLASH_Key0;
    ROM[FLASH_Key_Addr1] = FLASH_Key1;

    // Send 'write' command
    ROM[FLASH_Key_Addr0] = FLASH_Program;

    // Send one page/sector of data
    for (i = 0;  i < FLASH_PAGE_SIZE/2;  i++) {
        PAGE[i] = hold[i];
    }

    // Wait for data to be programmed
    while (timeout-- > 0) {
        if (PAGE[(FLASH_PAGE_SIZE/2)-1] == hold[(FLASH_PAGE_SIZE/2)-1]) {
            break;
        }
    }

    if (timeout <= 0) {
        return FLASH_PROGRAM_ERROR;
    }

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return FLASH_PROGRAM_OK;
}
