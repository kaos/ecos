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
// Author(s):    gthomas, hmt
// Contributors: gthomas
// Date:         2001-02-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "strata.h"

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//

int
flash_program_buf(volatile flash_t *addr, flash_t *data, int len,
                  unsigned long block_mask, int buffer_size)
{
    volatile flash_t *ROM;
    volatile flash_t *BA;
    flash_t stat = 0;
    int timeout = 50000;
    int cache_on;
#ifdef FLASH_Write_Buffer
    int i, wc;
#endif

    HAL_DCACHE_IS_ENABLED(cache_on);
    if (cache_on) {
        HAL_DCACHE_SYNC();
        HAL_DCACHE_DISABLE();
    }

    // Get base address and map addresses to virtual addresses
    ROM = FLASH_P2V( CYGNUM_FLASH_BASE_MASK & (unsigned int)addr );
    BA = FLASH_P2V( block_mask & (unsigned int)addr );
    addr = FLASH_P2V(addr);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

#ifdef FLASH_Write_Buffer
    // Write any big chunks first
    while (len >= buffer_size) {
        wc = buffer_size;
        if (wc > len) wc = len;
        len -= wc;
	// convert 'wc' in bytes to 'wc' in 'flash_t' 
        wc = wc / sizeof(flash_t);  // Word count
        *BA = FLASH_Write_Buffer;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                goto bad;
            }
            *BA = FLASH_Write_Buffer;
        }
        *BA = FLASHWORD(wc-1);  // Count is 0..N-1
        for (i = 0;  i < wc;  i++) {
#ifdef CYGHWR_FLASH_WRITE_ELEM
            CYGHWR_FLASH_WRITE_ELEM(addr+i, data+i);
#else
            *(addr+i) = *(data+i);
#endif
        }
        *BA = FLASH_Confirm;
    
        ROM[0] = FLASH_Read_Status;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                goto bad;
            }
        }
        // Jump out if there was an error
        if (stat & FLASH_ErrorMask) {
            goto bad;
        }
        // And verify the data - also increments the pointers.
        *BA = FLASH_Reset;            
        for (i = 0;  i < wc;  i++) {
            if ( *addr++ != *data++ ) {
                stat = FLASH_ErrorNotVerified;
                goto bad;
            }
        }
    }
#endif

    while (len > 0) {
        ROM[0] = FLASH_Program;
#ifdef CYGHWR_FLASH_WRITE_ELEM
        CYGHWR_FLASH_WRITE_ELEM(addr, data);
#else
        *addr = *data;
#endif
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                goto bad;
            }
        }
        if (stat & FLASH_ErrorMask) {
            break;
        }
        ROM[0] = FLASH_Reset;            
        if (*addr++ != *data++) {
            stat = FLASH_ErrorNotVerified;
            break;
        }
        len -= sizeof( flash_t );
    }

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = FLASH_Reset;            

    if (cache_on) {
        HAL_DCACHE_ENABLE();
    }

    return stat;
}
