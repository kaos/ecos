#ifndef CYGONCE_DEVS_FLASH_ATMEL_AT49XXXX_INL
#define CYGONCE_DEVS_FLASH_ATMEL_AT49XXXX_INL
//==========================================================================
//
//      at49xxxx.inl
//
//      Atmel AT49xxxx series flash driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####

//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Jani Monoses <jani@iv.ro>
// Contributors: Cristian Vlasin <cris@iv.ro>, tdrury
// Date:         2002-06-24
// Purpose:
// Description:
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
#define FLASH_Read_ID_Exit              FLASHWORD( 0xF0 )
#define FLASH_Program                   FLASHWORD( 0xA0 )
#define FLASH_Sector_Erase              FLASHWORD( 0x30 )

#define FLASH_Busy                      FLASHWORD( 0x40 ) // "Toggle" bit

#define FLASH_Setup_Addr1               (0x5555)
#define FLASH_Setup_Addr2               (0x2AAA)
#define FLASH_Setup_Code1               FLASHWORD( 0xAA )
#define FLASH_Setup_Code2               FLASHWORD( 0x55 )
#define FLASH_Setup_Erase               FLASHWORD( 0x80 )

// Platform code must define the below
// #define CYGNUM_FLASH_INTERLEAVE      : Number of interleaved devices (in parallel)
// #define CYGNUM_FLASH_SERIES          : Number of devices in series
// #define CYGNUM_FLASH_BASE            : Address of first device
// And select one of the below device variants

#ifdef CYGPKG_DEVS_FLASH_ATMEL_AT49LV8011
# define FLASH_BLOCK_SIZE               (0x10000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (14)
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x1F)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xCB)
#endif
#ifdef CYGPKG_DEVS_FLASH_ATMEL_AT49LV8011T
# define FLASH_BLOCK_SIZE               (0x10000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (14)
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x1F)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0x4A)
#endif
#ifdef CYGPKG_DEVS_FLASH_ATMEL_AT49LV1614
// NOTE: the smaller plane A of the flash is ignored for now (tdrury)
# define FLASH_BLOCK_SIZE               (0x10000*CYGNUM_FLASH_INTERLEAVE)
# define FLASH_NUM_REGIONS              (31)
# define CYGNUM_FLASH_WIDTH             (16)
# define CYGNUM_FLASH_BLANK             (1)
# define CYGNUM_FLASH_ID_MANUFACTURER   FLASHWORD(0x1F)
# define CYGNUM_FLASH_ID_DEVICE         FLASHWORD(0xC0)
#endif

#define FLASH_DEVICE_SIZE               (FLASH_BLOCK_SIZE*FLASH_NUM_REGIONS)

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
    int i;

    ROM = (volatile flash_data_t*) CYGNUM_FLASH_BASE;

    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Read_ID;

    // FIXME: 10ms delay?

    // Manufacturers' code
    id[0] = ROM[0];
    // Part number
    id[1] = ROM[1];

    ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
    ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
    ROM[FLASH_Setup_Addr1] = FLASH_Read_ID_Exit;

    // FIXME: 10ms delay?
}

// Wait for completion (bit 6 stops toggling)
static int wait_while_busy(int timeout, volatile flash_data_t* addr_ptr)
{
        flash_data_t state, prev_state;
        prev_state = *addr_ptr & FLASH_Busy;
        while (true) {
            state = *addr_ptr & FLASH_Busy;
            if (prev_state == state) {
            	return FLASH_ERR_OK;
	    }
            if (--timeout == 0) {
                return FLASH_ERR_DRV_TIMEOUT;
            }
            prev_state = state;
        }
}

//----------------------------------------------------------------------------
// Erase Block
int
flash_erase_block(void* block, unsigned int len)
{
	volatile flash_data_t* ROM;
	volatile flash_data_t* addr_ptr = (volatile flash_data_t*) block;

	int res = FLASH_ERR_OK;
        int  i;

        flash_data_t state, prev_state;

//	diag_printf("\nERASE: Block %p, len: %u\n",addr_ptr,len);

	// Base address of device(s) being programmed.
        ROM = (volatile flash_data_t*) CYGNUM_FLASH_BASE;


	//Erase sector 6-byte sequence
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Erase;
        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
        addr_ptr[0] = FLASH_Sector_Erase;

	res = wait_while_busy(5000000,addr_ptr);
#if 0
	// Verify loaded data bytes
        for (i = 0; i < len;) {
            if (*addr_ptr != FLASH_BlankValue) {
                // Only update return value if erase operation was OK
                if (FLASH_ERR_OK == res) res = FLASH_ERR_DRV_VERIFY;
                break;
            }
            addr_ptr++;
            i += sizeof(*addr_ptr);
        }
#endif
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

	int res = FLASH_ERR_OK;
	
	// Base address of device(s) being programmed. 
        ROM = (volatile flash_data_t*) CYGNUM_FLASH_BASE;

	while ((FLASH_ERR_OK == res) && (len > 0)) {
        	// Program data [byte] - 4 step sequence
	        ROM[FLASH_Setup_Addr1] = FLASH_Setup_Code1;
	        ROM[FLASH_Setup_Addr2] = FLASH_Setup_Code2;
	        ROM[FLASH_Setup_Addr1] = FLASH_Program;
		addr_ptr[0] = data_ptr[0];

		addr_ptr++;
		data_ptr++;
		
		//if (*data_ptr++ == 0)
		//	*addr_ptr = 0;
		
		len -= sizeof (flash_data_t);

		res = wait_while_busy(5000000,addr_ptr);
//		if (res==FLASH_ERR_DRV_TIMEOUT) break;
		
	}
#if 0
        // Verify loaded data bytes
        for (i = 0; (i<len);) {
            if (*addr_ptr != *data_ptr) {
                // Only update return value if erase operation was OK
                if (FLASH_ERR_OK == res) res = FLASH_ERR_DRV_VERIFY;
                break;
            }
            addr_ptr++;
            data_ptr++;
            i += sizeof(*data_ptr);
        }
#endif	

    return res;
}

#endif // CYGONCE_DEVS_FLASH_ATMEL_AT49XXXX_INL
