//==========================================================================
//
//      assabet_flash.c
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
// Date:         2000-07-26
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

#define _si(p) ((p[1]<<8)|p[0])

int
flash_hwr_init(void)
{
    struct FLASH_query data, *qp;
    extern char flash_query, flash_query_end;
    typedef int code_fun(unsigned char *);
    code_fun *_flash_query;
    int code_len, stat, num_regions, region_size;

    // Copy 'program' code to RAM for execution
    code_len = (unsigned long)&flash_query_end - (unsigned long)&flash_query;
    _flash_query = (code_fun *)flash_info.work_space;
    memcpy(_flash_query, &flash_query, code_len);
    HAL_DCACHE_SYNC();  // Should guarantee this code will run
    HAL_ICACHE_DISABLE(); // is also required to avoid old contents

    stat = (*_flash_query)(&data);
    HAL_ICACHE_ENABLE();

    qp = &data;
    if (/*(qp->manuf_code == FLASH_Intel_code) && */
        (strncmp(qp->id, "QRY", 3) == 0)) {
        num_regions = _si(qp->num_regions)+1;
        region_size = _si(qp->region_size)*256;

        flash_info.block_size = region_size*2;   // Pairs of chips
        flash_info.blocks = num_regions;
        flash_info.start = (void *)0x50000000;
        flash_info.end = (void *)(0x50000000+(num_regions*region_size*2));
        return FLASH_ERR_OK;
    } else {
        printf("Can't identify FLASH, sorry\n");
        diag_dump_buf(data, sizeof(data));
        return FLASH_ERR_HWR;
    }
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    if (err & 0x007E007E) {
        printf("Err = %x\n", err);
        if (err & 0x00100010) {
            return FLASH_ERR_PROGRAM;
        } else 
        if (err & 0x00200020) {
            return FLASH_ERR_ERASE;
        } else 
        return FLASH_ERR_HWR;  // FIXME
    } else {
        return FLASH_ERR_OK;
    }
}

// See if a range of FLASH addresses overlaps currently running code
bool
flash_code_overlaps(void *start, void *end)
{
    extern char _stext[], _etext[];

    return ((((unsigned long)&_stext >= (unsigned long)start) &&
             ((unsigned long)&_stext < (unsigned long)end)) ||
            (((unsigned long)&_etext >= (unsigned long)start) &&
             ((unsigned long)&_etext < (unsigned long)end)));
}
