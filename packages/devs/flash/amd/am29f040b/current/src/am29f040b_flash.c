//==========================================================================
//
//      am29f040b_flash.c
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

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

#include "flash.h"

static flash_data_t id[2*CYGHWR_FLASH_INTERLEAVE];

int
flash_hwr_init(void)
{
    flash_dev_query(&id);
#if 0
    dump_buf(id, sizeof(id));
#endif

    // Check that flash_id data is valid - use to set up sizes etc.

    // Hard wired for now
    flash_info.block_size = FLASH_BLOCK_SIZE * CYGHWR_FLASH_INTERLEAVE;
    flash_info.blocks = FLASH_NUM_REGIONS;
    flash_info.start = (void *)CYGHWR_FLASH_BASE;
    flash_info.end = (void *)(CYGHWR_FLASH_BASE+ (FLASH_NUM_REGIONS * FLASH_BLOCK_SIZE * CYGHWR_FLASH_INTERLEAVE * CYGHWR_FLASH_SERIES));
    return FLASH_ERR_OK;
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int e)
{
    flash_data_t err = (flash_data_t) e;

    if (err & (FLASH_Err | FLASH_Drv_Verify_Err)) {
        if ((err & FLASH_Drv_Verify_Err) == FLASH_Drv_Verify_Err) {
            return FLASH_ERR_DRV_VERIFY;
        } else 
        if (err & FLASH_Err) {
            // FIXME: Can't tell error type from status word. Should
            // we keep an internal driver state with that that
            // information?
            return FLASH_ERR_HWR;
        }
        return FLASH_ERR_HWR;
    } else {
        return FLASH_ERR_OK;
    }
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern unsigned char _stext[], _etext[];

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}
