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
// Contributors: Philippe Robin
// Date:         November 7, 2000
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
#include <cyg/hal/hal_integrator.h>

//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//

int
flash_erase_block(volatile cyg_uint32 *block)
{
    volatile cyg_uint32 stat = 0;
    int timeout = 5000000;    
    int cache_on;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    HAL_FLASH_WRITE_ENABLE();
    // Clear any error conditions
    *block = FLASH_Clear_Status;

    // Set to erase
    *block = FLASH_Block_Erase;
    *block = FLASH_Confirm;
    while((stat & FLASH_Status_Ready) != FLASH_Status_Ready) {
        *block = FLASH_Read_Status;
	stat = *block;
        if (--timeout == 0) break;
    }

    // Restore ROM to "normal" mode
    *block = FLASH_Reset;

    HAL_FLASH_WRITE_DISABLE();

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return stat;
}
