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
// Author(s):    Andrew.Lunn@ascom.ch
// Contributors: andrew.lunn
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "synth.h"

#include <pkgconf/devs_flash_synth.h>
#include <cyg/hal/hal_io.h>
#include <cyg/io/flash.h>

int
flash_program_buf(volatile flash_t *addr, flash_t *data, int len,
                  unsigned long block_mask, int buffer_size)
{
  
    int offset = (int)addr;
    offset -= (int)cyg_dev_flash_synth_base;

    cyg_hal_sys_lseek(cyg_dev_flash_synth_flashfd, offset, CYG_HAL_SYS_SEEK_SET);
    cyg_hal_sys_write(cyg_dev_flash_synth_flashfd, data, len);
  
    return FLASH_ERR_OK;
}
