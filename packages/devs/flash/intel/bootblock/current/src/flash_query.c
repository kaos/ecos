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
// Date:         2000-07-26
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

#define CNT 200*1000*10  // Approx 20ms

/*
88F4 0089 88F4 0089 FFC7 0089 88F4 0089 
88F4 0089 88F4 0089 FFC7 0089 88F4 0089 
88F4 0089 88F4 0089 FFC7 0089 88F4 0089 
88F4 0089 88F4 0089 FFC7 0089 88F4
*/

int
flash_query(unsigned short *data)
{
    volatile unsigned short *ROM;
    int cnt;
    int cache_on;

    ROM = (volatile unsigned short *)0x03000000;

    ROM[0] = FLASH_Read_ID;
    for (cnt = CNT;  cnt > 0;  cnt--) ;
    *data++ = *ROM++;  // Manufacturer code
    *data++ = *ROM++;  // Device identifier

    ROM[0] = FLASH_Reset;

    return 0;
}
