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
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andrew.lunn@ascom.ch
// Contributors: andrew.lunn
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "synth.h"

#include <cyg/hal/hal_io.h>
#include <pkgconf/devs_flash_synth.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/io/flash.h>
#include <string.h> // memset

/* This helps speed up the erase. */
static char empty[4096];
static cyg_bool empty_inited = false;

int flash_erase_block(volatile flash_t *block, unsigned int block_size)
{
    int i;
    int offset = (int)block;
    offset -= (int)cyg_dev_flash_synth_base;

    cyg_hal_sys_lseek(cyg_dev_flash_synth_flashfd, offset,
                      CYG_HAL_SYS_SEEK_SET);
  
    if (!empty_inited) {
        memset(empty, 0xff, sizeof(empty));
        empty_inited = true;
    }

    CYG_ASSERT(sizeof(empty) < block_size,
               "Eckk! Can't work with such small blocks");
    CYG_ASSERT((block_size % sizeof(empty)) == 0,
               "Eckk! Can't work with that odd size block");

    for (i=0; (i * sizeof(empty)) < block_size; i++) {
        cyg_hal_sys_write(cyg_dev_flash_synth_flashfd, empty, sizeof(empty));
    }
    return FLASH_ERR_OK;
}


