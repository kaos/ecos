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
// Contributors: gthomas, msalter
// Date:         2000-07-14
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
flash_program_buf(volatile unsigned char *addr, unsigned char *data, int len)
{
    volatile unsigned char *ROM;
    volatile unsigned char *BA;
    unsigned short stat;
    int timeout = 5000000;
    int i, wc, cache_on;

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    ROM = FLASH_P2V((unsigned long)addr & 0xFF800000);
    BA  = FLASH_P2V((unsigned long)addr & 0xFFFE0000);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    wc = 32;
    while (len >= wc) {
        len -= wc;

	// The IQ803010 has a hole in flash which must be avoided.
	if (((unsigned char *)0x1000) <= addr && addr < ((unsigned char *)0x2000)) {
	    addr += wc;
	    data += wc;
	    continue;
	}

        *BA = FLASH_Write_Buffer;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                stat |= 0x0100;
                goto bad;
            }
            *BA = FLASH_Write_Buffer;
        }
        *BA = wc-1;  // Count is 0..N-1
	if (FLASH_P2V(addr) != addr) {
	    volatile unsigned char *tmp;

	    tmp = FLASH_P2V(addr);
	    for (i = 0;  i < wc;  i++)
		*tmp++ = *data++;
	    addr += wc;
	} else {
	    for (i = 0;  i < wc;  i++)
		*addr++ = *data++;
	}
        *BA = FLASH_Confirm;
	stat = *BA;
    }
    
    ROM[0] = FLASH_Read_Status;
    timeout = 5000000;
    while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
        if (--timeout == 0) {
            stat |= 0x0200;
            goto bad;
        }
    }

    while (len > 0) {
        ROM[0] = FLASH_Program;
	
        *FLASH_P2V(addr) = *data++;
	addr++;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                stat |= 0x0300;
                goto bad;
            }
        }
        --len;
    }

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = FLASH_Reset;            

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return stat;
}


