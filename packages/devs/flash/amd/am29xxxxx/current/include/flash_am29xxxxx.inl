#ifndef CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_INL
#define CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_INL
//==========================================================================
//
//      am29xxxxx.inl
//
//      AMD AM29xxxxx series flash driver
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
// Author(s):    gthomas
// Contributors: gthomas, jskov
// Date:         2001-02-21
// Purpose:      
// Description:  AMD AM29xxxxx series flash device driver
// Notes:        While the parts support sector locking, this is controlled
//               using a 12V source (on F parts anyway) - so it is left
//               unimplemented for now.  When it does get implemented, let
//               the platform CDL control the availability of that feature.
//               FIXME: Does not use FLASH_P2V
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include CYGHWR_MEMORY_LAYOUT_H

#define  _FLASH_PRIVATE_
#include <cyg/io/flash.h>


//----------------------------------------------------------------------------
// Common device details.
#define FLASH_Read_ID                   FLASHWORD( 0x90 )
#define FLASH_Reset                     FLASHWORD( 0xFF )
#define FLASH_Program                   FLASHWORD( 0xA0 )
#define FLASH_Block_Erase               FLASHWORD( 0x30 )

#define FLASH_Data                      FLASHWORD( 0x80 ) // Data complement
#define FLASH_Busy                      FLASHWORD( 0x40 ) // "Toggle" bit
#define FLASH_Err                       FLASHWORD( 0x20 )
#define FLASH_Sector_Erase_Timer        FLASHWORD( 0x08 )

#define FLASH_Setup_Addr1               (0x555)
#define FLASH_Setup_Addr2               (0x2AA)
#define FLASH_Setup_Code1               FLASHWORD( 0xAA )
#define FLASH_Setup_Code2               FLASHWORD( 0x55 )
#define FLASH_Setup_Erase               FLASHWORD( 0x80 )

// Platform code must define the below
// #define CYGNUM_FLASH_INTERLEAVE      : Number of interleaved devices (in parallel)
// #define CYGNUM_FLASH_SERIES          : Number of devices in series
// #define CYGNUM_FLASH_BASE            : Address of first device
// And select one of the below device variants

#ifdef CYGPKG_DEVS_FLASH_AMD_AM29F040B
# define FLASH_BLOCK_SIZE               (0x10000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (8)
# define CYGNUM_FLASH_BASE_MASK         (0xFFF80000u) // 512kB devices
# define CYGNUM_FLASH_WIDTH             (8)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x01)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xA4)
#endif

#define FLASH_DEVICE_SIZE               (FLASH_BLOCK_SIZE*FLASH_NUM_REGIONS)
#define CYGNUM_FLASH_DEVICES            (CYGNUM_FLASH_INTERLEAVE*CYGNUM_FLASH_SERIES)

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
int  flash_program_buf(void* addr, void* data, int len)
    __attribute__ ((section (".2ram.flash_program_buf")));

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
    int i;

    ROM = (volatile flash_data_t*) CYGNUM_FLASH_BASE;

    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Read_ID;

    // Manufacturers' code
    id[0] = ROM[0];
    // Part number
    id[1] = ROM[1];

    ROM[0] = FLASH_Reset;

    // Allow device to settle.
    for (i = 500; i > 0; i--);
}

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
    flash_info.block_size = FLASH_BLOCK_SIZE * CYGNUM_FLASH_INTERLEAVE;
    flash_info.blocks = FLASH_NUM_REGIONS;
    flash_info.start = (void *)CYGNUM_FLASH_BASE;
    flash_info.end = (void *)(CYGNUM_FLASH_BASE+ (FLASH_NUM_REGIONS * FLASH_BLOCK_SIZE * CYGNUM_FLASH_INTERLEAVE * CYGNUM_FLASH_SERIES));
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
// Erase Block

int
flash_erase_block(void* block, unsigned int size)
{
    volatile flash_data_t* ROM;
    volatile flash_data_t* block_ptr = (volatile flash_data_t*) block;
    int timeout = 50000;
    int len;
    flash_data_t res = FLASH_ERR_OK;
    int state;

    ROM = (volatile flash_data_t*)((unsigned long)block & ~(FLASH_DEVICE_SIZE-1));

    // Send erase block command - six step sequence
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Erase;
    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    *block_ptr = FLASH_Block_Erase;

    // Now poll for the completion of the sector erase timer (50us)
    timeout = 5000000;              // how many retries?
    while (true) {
        state = *block_ptr;
        if ((state & FLASH_Sector_Erase_Timer) == 0) break;
        if (state & FLASH_Err) {
            res = FLASH_ERR_ERASE;
            break;
        }
        if (--timeout == 0) {
            res = FLASH_ERR_DRV_TIMEOUT;
            break;
        }
    }

    // Then wait for erase completion.
    if (FLASH_ERR_OK == res) {
        timeout = 5000000;
        while (true) {
            state = *block_ptr;
            if (FLASH_BlankValue == state) {
                break;
            }
            if (state & FLASH_Err) {
                res = FLASH_ERR_ERASE;
                break;
            }
            if (--timeout == 0) {
                res = FLASH_ERR_DRV_TIMEOUT;
                break;
            }
        }
    }

    if (FLASH_ERR_OK != res)
        ROM[0] = FLASH_Reset;

    // Verify erase operation
    len = (FLASH_BLOCK_SIZE*CYGNUM_FLASH_INTERLEAVE);
    while (len > 0) {
        if (*block_ptr != FLASH_BlankValue) {
            // Only update return value if erase operation was OK
            if (FLASH_ERR_OK == res) res = FLASH_ERR_DRV_VERIFY;
            break;
        }
        block_ptr++;
        len -= sizeof(*block_ptr);
    }

    return res;
}

//----------------------------------------------------------------------------
// Program Buffer

int
flash_program_buf(void* addr, void* data, int len)
{
    volatile flash_data_t* ROM;
    volatile flash_data_t* addr_ptr = (volatile flash_data_t*) addr;
    volatile flash_data_t* data_ptr = (volatile flash_data_t*) data;

    int timeout;
    int res = FLASH_ERR_OK;

#if 0
    CYG_ASSERT((unsigned long)data_ptr & (sizeof(flash_data_t)-1) == 0, 
               "Data not properly aligned");
    CYG_ASSERT((unsigned long)addr_ptr & (CYGNUM_FLASH_INTERLEAVE*sizeof(flash_data_t)-1) == 0, 
               "Addr not properly aligned (to first interleaved device)");
#endif

    while (len > 0) {
        int state;

        // Base address of device(s) being programmed. 
        ROM = (volatile flash_data_t*)((unsigned long)addr & ~(FLASH_DEVICE_SIZE-1));

        // Program data [byte] - 4 step sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Program;
        *addr_ptr = *data_ptr;

        timeout = 5000000;
        while (true) {
            state = *addr_ptr;
            if (*data_ptr == state) {
                break;
            }
            if (state & FLASH_Err) {
                res = FLASH_ERR_PROGRAM;
                break;
            }
            if (--timeout == 0) {
                res = FLASH_ERR_DRV_TIMEOUT;
                break;
            }
        }

        if (FLASH_ERR_OK != res)
            ROM[0] = FLASH_Reset;

        if (*addr_ptr++ != *data_ptr++) {
            // Only update return value if erase operation was OK
            if (FLASH_ERR_OK == res) res = FLASH_ERR_DRV_VERIFY;
            break;
        }
        len -= sizeof(*data_ptr);
    }

    // Ideally, we'd want to return not only the failure code, but also
    // the address/device that reported the error.
    return res;
}

#endif // CYGONCE_DEVS_FLASH_AMD_AM29XXXXX_INL
