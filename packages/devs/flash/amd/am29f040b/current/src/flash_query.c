//==========================================================================
//
//      flash_query.c
//
//      Flash programming - query device
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
// Date:         2000-10-20
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
#include CYGHWR_MEMORY_LAYOUT_H

//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//

// Only reads the manufacturer and part number codes for the first
// device(s) in series. It is assumed that any devices in series
// will be of the same type.

void
flash_query(void* data)
{
    volatile flash_data_t *ROM;
    flash_data_t* id = (flash_data_t*) data;

    ROM = (volatile flash_data_t*) CYGHWR_FLASH_BASE;

    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Setup_Code1);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr2, FLASH_Setup_Code2);
    FLASH_CMD_WRITE(ROM+FLASH_Setup_Addr1, FLASH_Read_ID);

    // Manufacturers' code
    FLASH_DATA_READ(ROM, id);
    // Part number
    FLASH_DATA_READ(ROM+CYGHWR_FLASH_INTERLEAVE, &id[CYGHWR_FLASH_INTERLEAVE]);

    FLASH_CMD_WRITE(ROM, FLASH_Reset);

    return id;
}
