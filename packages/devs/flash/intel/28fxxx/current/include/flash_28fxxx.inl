#ifndef CYGONCE_DEVS_FLASH_INTEL_28FXXX_INL
#define CYGONCE_DEVS_FLASH_INTEL_28FXXX_INL
//==========================================================================
//
//      flash_28fxxx.inl
//
//      Intel 28Fxxx series flash driver
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
// Author(s):    jskov
// Contributors: jskov
// Date:         2001-03-21
// Purpose:      
// Description:  
//              
// Notes:        Only has FlashFile support at present
//               Needs to get sizing details from part
//               Needs locking.
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#define FLASH_P2V( _a_ ) ((volatile flash_data_t *)((unsigned int)(_a_)))

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>

//----------------------------------------------------------------------------
// Common device details.
#define FLASH_Read_ID                   FLASHWORD( 0x90 )
#define FLASH_Reset                     FLASHWORD( 0xFF )
#define FLASH_Program                   FLASHWORD( 0x40 )
#define FLASH_Write_Buffer              FLASHWORD( 0xe8 )
#define FLASH_Block_Erase  		FLASHWORD( 0x20 )
#define FLASH_Confirm      		FLASHWORD( 0xD0 )

#define FLASH_Read_Status  		FLASHWORD( 0x70 )
#define FLASH_Clear_Status 		FLASHWORD( 0x50 )
#define FLASH_Status_Ready 		FLASHWORD( 0x80 )

// Status that we read back:                         
#define FLASH_ErrorMask			FLASHWORD( 0x7E )
#define FLASH_ErrorProgram		FLASHWORD( 0x10 )
#define FLASH_ErrorErase		FLASHWORD( 0x20 )
#define FLASH_ErrorLowVoltage           FLASHWORD( 0x08 )
#define FLASH_ErrorLocked               FLASHWORD( 0x02 )

// Platform code must define the below
// #define CYGNUM_FLASH_INTERLEAVE      : Number of interleaved devices (in parallel)
// #define CYGNUM_FLASH_SERIES          : Number of devices in series
// #define CYGNUM_FLASH_BASE            : Address of first device
// And select one of the below device variants

#ifdef CYGPKG_DEVS_FLASH_INTEL_28F160
# define FLASH_BLOCK_SIZE               (0x8000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (32)
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define FLASH_BUFFER_SIZE              (32*CYGNUM_FLASH_INTERLEAVE)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0xb0)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xd0)
#endif

#ifdef CYGPKG_DEVS_FLASH_INTEL_28F320
# define FLASH_BLOCK_SIZE               (0x10000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (64)
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define FLASH_BUFFER_SIZE              (32*CYGNUM_FLASH_INTERLEAVE)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0xb0)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xd4)
#endif

#define FLASH_DEVICE_SIZE               (FLASH_BLOCK_SIZE*FLASH_NUM_REGIONS)
#define CYGNUM_FLASH_DEVICES            (CYGNUM_FLASH_INTERLEAVE*CYGNUM_FLASH_SERIES)
#define CYGNUM_FLASH_BASE_MASK          (~(FLASH_DEVICE_SIZE-1))
#define CYGNUM_FLASH_BLOCK_MASK         (~(FLASH_BLOCK_SIZE-1))

//----------------------------------------------------------------------------
// Now that device properties are defined, include magic for defining
// accessor type and constants.
#include <cyg/io/flash_dev.h>

//----------------------------------------------------------------------------
// Functions that put the flash device into non-read mode must reside
// in RAM.
void flash_query(void* data) __attribute__ ((section (".2ram.flash_query")));
int  flash_erase_block(void* block, unsigned int size) 
    __attribute__ ((section (".2ram.flash_erase_block")));
int  flash_program_buf(void* addr, void* data, int len,
                       unsigned long block_mask, int buffer_size)
    __attribute__ ((section (".2ram.flash_program_buf")));


//----------------------------------------------------------------------------
// Initialize driver details
int
flash_hwr_init(void)
{
    flash_data_t id[2];

    flash_dev_query(id);

    // Check that flash_id data is matching the one the driver was
    // configured for.
    if (id[0] != CYGNUM_FLASH_ID_MANUFACTURER
        || id[1] != CYGNUM_FLASH_ID_DEVICE)
        return FLASH_ERR_DRV_WRONG_PART;

    // Hard wired for now
    flash_info.block_size = FLASH_BLOCK_SIZE;
    flash_info.blocks = FLASH_NUM_REGIONS;
    flash_info.start = (void *)CYGNUM_FLASH_BASE;
    flash_info.end = (void *)(CYGNUM_FLASH_BASE+ (FLASH_NUM_REGIONS * FLASH_BLOCK_SIZE * CYGNUM_FLASH_SERIES));
    flash_info.buffer_size = FLASH_BUFFER_SIZE;

    return FLASH_ERR_OK;
}

//----------------------------------------------------------------------------
// Map a hardware status to a package error
int
flash_hwr_map_error(int e)
{
    return e;
}


//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
// Flash Query
//
// Only reads the manufacturer and part number codes for the first
// device(s) in series. It is assumed that any devices in series
// will be of the same type.

void
flash_query(void* data)
{
    volatile flash_data_t *ROM;
    flash_data_t* id = (flash_data_t*) data;
    flash_data_t w;

    ROM = (volatile flash_data_t*) CYGNUM_FLASH_BASE;

    w = ROM[0];

    ROM[0] = FLASH_Read_ID;

    // Manufacturers' code
    id[0] = ROM[0];
    // Part number
    id[1] = ROM[1];

    ROM[0] = FLASH_Reset;

    // Stall, waiting for flash to return to read mode.
    while (w != ROM[0]);

}

//----------------------------------------------------------------------------
// Erase Block
int
flash_erase_block(void* block, unsigned int block_size)
{
    int res = FLASH_ERR_OK;
    int timeout;
    unsigned long len;
    flash_data_t stat;
    volatile flash_data_t *ROM;
    volatile flash_data_t *b_p = (flash_data_t*) block;
    volatile flash_data_t *b_v;
    ROM = FLASH_P2V((unsigned long)block & CYGNUM_FLASH_BASE_MASK);
    b_v = FLASH_P2V(block);

    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    ROM[0] = FLASH_Block_Erase;
    *b_v = FLASH_Confirm;

    timeout = 5000000;
    while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
        if (--timeout == 0) break;
    }
    
    // Restore ROM to "normal" mode
    ROM[0] = FLASH_Reset;

    // Check if block got erased
    len = block_size;
    while (len > 0) {
        b_v = FLASH_P2V(b_p++);
        if (*b_v != FLASH_BlankValue ) break;
        len -= sizeof( flash_data_t );
    }
    if (len != 0) {
        if (stat & FLASH_ErrorMask) {
            if (!(stat & FLASH_ErrorErase))
                res = FLASH_ERR_HWR;    // Unknown error
            else {
                if (stat & FLASH_ErrorLowVoltage)
                    res = FLASH_ERR_LOW_VOLTAGE;
                else if (stat & FLASH_ErrorLocked)
                    res = FLASH_ERR_LOCK;
                else
                    res = FLASH_ERR_ERASE;
            }
        }
        else
            res = FLASH_ERR_DRV_VERIFY;
    }

    return res;
}

//----------------------------------------------------------------------------
// Program Buffer
int
flash_program_buf(void* addr, void* data, int len,
                  unsigned long block_mask, int buffer_size)
{
    flash_data_t stat = 0;
    int timeout;
    int wc, i;

    volatile flash_data_t* ROM;
    volatile flash_data_t* BA;
    volatile flash_data_t* addr_v;
    volatile flash_data_t* addr_p = (flash_data_t*) addr;
    volatile flash_data_t* data_p = (flash_data_t*) data;

    int res = FLASH_ERR_OK;

    // Base address of device(s) being programmed. 
    ROM = FLASH_P2V((unsigned long)addr & CYGNUM_FLASH_BASE_MASK);
    BA = FLASH_P2V((unsigned long)addr & CYGNUM_FLASH_BLOCK_MASK);
        
    // Clear any error conditions
    ROM[0] = FLASH_Clear_Status;

    // Write any big chunks first
    while (len >= buffer_size) {
        wc = buffer_size;
        if (wc > len) wc = len;
        len -= wc;
        wc = wc / ((CYGNUM_FLASH_WIDTH/8)*CYGNUM_FLASH_DEVICES);  // Word count
        timeout = 5000000;

        *BA = FLASH_Write_Buffer;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                res = FLASH_ERR_DRV_TIMEOUT;
                goto bad;
            }
            *BA = FLASH_Write_Buffer;
        }
        *BA = FLASHWORD(wc-1);  // Count is 0..N-1
        for (i = 0; i < wc;  i++) {
            addr_v = FLASH_P2V(addr_p++);
            *addr_v = *data_p++;
        }
        *BA = FLASH_Confirm;

        ROM[0] = FLASH_Read_Status;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                res = FLASH_ERR_DRV_TIMEOUT;
                goto bad;
            }
        }
    }

    while (len > 0) {
        addr_v = FLASH_P2V(addr_p++);
        ROM[0] = FLASH_Program;
        *addr_v = *data_p;
        timeout = 5000000;
        while(((stat = ROM[0]) & FLASH_Status_Ready) != FLASH_Status_Ready) {
            if (--timeout == 0) {
                res = FLASH_ERR_DRV_TIMEOUT;
                goto bad;
            }
        }
        if (stat & FLASH_ErrorMask) {
            if (!(stat & FLASH_ErrorProgram))
                res = FLASH_ERR_HWR;    // Unknown error
            else {
                if (stat & FLASH_ErrorLowVoltage)
                    res = FLASH_ERR_LOW_VOLTAGE;
                else if (stat & FLASH_ErrorLocked)
                    res = FLASH_ERR_LOCK;
                else
                    res = FLASH_ERR_PROGRAM;
            }
            break;
        }
        ROM[0] = FLASH_Clear_Status;
        ROM[0] = FLASH_Reset;            
        if (*addr_v != *data_p++) {
            res = FLASH_ERR_DRV_VERIFY;
            break;
        }
        len -= sizeof( flash_data_t );
    }

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = FLASH_Reset;            
    
    // Ideally, we'd want to return not only the failure code, but also
    // the address/device that reported the error.
    return res;
}

#endif // CYGONCE_DEVS_FLASH_INTEL_28FXXX_INL
