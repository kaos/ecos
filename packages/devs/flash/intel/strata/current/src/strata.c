//==========================================================================
//
//      strata.c
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

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

#include "strata.h"

#define _si(p) ((p[1]<<8)|p[0])

extern void diag_dump_buf(void *buf, CYG_ADDRWORD len);

extern int strncmp(const char *s1, const char *s2, int len);

int
flash_hwr_init(void)
{
    struct FLASH_query data, *qp;
    extern char flash_query, flash_query_end;
    typedef int code_fun(unsigned char *);
    code_fun *_flash_query;
    int code_len, stat, num_regions, region_size, buffer_size;
    int icache_on, dcache_on;

    HAL_DCACHE_IS_ENABLED(dcache_on);
    HAL_ICACHE_IS_ENABLED(icache_on);

    // Copy 'program' code to RAM for execution
    code_len = (unsigned long)&flash_query_end - (unsigned long)&flash_query;
    _flash_query = (code_fun *)flash_info.work_space;
    memcpy(_flash_query, &flash_query, code_len);
    if (dcache_on) {
        HAL_DCACHE_SYNC();  // Should guarantee this code will run
    }
    if (icache_on) {
        HAL_ICACHE_DISABLE(); // is also required to avoid old contents
    }

    stat = (*_flash_query)((unsigned char *)&data);
    if (icache_on) {
        HAL_ICACHE_ENABLE();
    }

    qp = &data;
    if ( (qp->manuf_code == FLASH_Intel_code)
#ifdef CYGOPT_FLASH_IS_BOOTBLOCK
         // device types go as follows: 0x90 for 16-bits, 0xD0 for 8-bits,
         // plus 0 or 1 for -T (Top Boot) or -B (Bottom Boot)
         //     [FIXME: whatever that means :FIXME]
         // [I think it means the boot blocks are top/bottom of addr space]
         // plus the following size codes:
         //    0: 16Mbit    2:  8Mbit    4:  4Mbit
         //    6: 32Mbit    8: 64Mbit
#if 16 == CYGNUM_FLASH_WIDTH         
         && (0x90 == (0xF0 & qp->device_code)) // 16-bit devices
#elif 8 == CYGNUM_FLASH_WIDTH
         && (0xD0 == (0xF0 & qp->device_code)) // 8-bit devices
#else
         && 0
#error Only understand 16 and 8-bit bootblock flash types
#endif
        ) {
        int lookup[] = { 16, 8, 4, 32, 64 };
#define BLOCKSIZE (0x10000)
        region_size = BLOCKSIZE;
        num_regions = qp->device_code & 0x0F;
        num_regions >>= 1;
        if ( num_regions > 4 )
            goto flash_type_unknown;
        num_regions = lookup[num_regions];
        num_regions *= 1024 * 1024;     // to bits
        num_regions /= 8;               // to bytes
        num_regions /= BLOCKSIZE;       // to blocks
        buffer_size = 0;
#else // CYGOPT_FLASH_IS_BOOTBLOCK
         && (strncmp(qp->id, "QRY", 3) == 0)) {
        num_regions = _si(qp->num_regions)+1;
        region_size = _si(qp->region_size)*256;       
        if (_si(qp->buffer_size)) {
            buffer_size = CYGNUM_FLASH_DEVICES << _si(qp->buffer_size);
        } else {
            buffer_size = 0;
        }
#endif // Not CYGOPT_FLASH_IS_BOOTBLOCK

        flash_info.block_size = region_size*CYGNUM_FLASH_DEVICES;
        flash_info.buffer_size = buffer_size;
        flash_info.blocks = num_regions;
        flash_info.start = (void *)CYGNUM_FLASH_BASE;
        flash_info.end = (void *)(CYGNUM_FLASH_BASE +
                        (num_regions*region_size*CYGNUM_FLASH_DEVICES));
#ifdef CYGNUM_FLASH_BASE_MASK
        // Then this gives us a maximum size for the (visible) device.
        // This is to cope with oversize devices fitted, with some high
        // address lines ignored.
        if ( ((unsigned int)flash_info.start & CYGNUM_FLASH_BASE_MASK) !=
             (((unsigned int)flash_info.end - 1) & CYGNUM_FLASH_BASE_MASK ) ) {
            // then the size of the device appears to span >1 device-worth!
            unsigned int x;
            x = (~(CYGNUM_FLASH_BASE_MASK)) + 1; // expected device size
            x += (unsigned int)flash_info.start;
            if ( x < (unsigned int)flash_info.end ) { // 2nd sanity check
                printf("\nFLASH: Oversized device!  End addr %p changed to %p\n",
                       flash_info.end, (void *)x );
                flash_info.end = (void *)x;
            }
        }
#endif // CYGNUM_FLASH_BASE_MASK

        return FLASH_ERR_OK;
    }
 flash_type_unknown:
    printf("Can't identify FLASH, sorry, man %x, dev %x, id [%4s] stat %x\n",
           qp->manuf_code, qp->device_code, qp->id, stat );
    diag_dump_buf(qp, sizeof(data));
    return FLASH_ERR_HWR;
}

// Map a hardware status to a package error
int
flash_hwr_map_error(int err)
{
    if (err & FLASH_ErrorMask) {
        printf("Err = %x\n", err);
        if (err & FLASH_ErrorProgram)
            return FLASH_ERR_PROGRAM;
        else if (err & FLASH_ErrorErase)
            return FLASH_ERR_ERASE;
        else 
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

// EOF strata.c
