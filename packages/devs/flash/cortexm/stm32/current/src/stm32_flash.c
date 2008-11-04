//==========================================================================
//
//      stm32_flash.c
//
//      STM32 internal flash driver
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 eCosCentric Limited.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-09-22
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/devs_flash_stm32.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <string.h>

#include <cyg/io/stm32_flash.h>

#include CYGHWR_MEMORY_LAYOUT_H

// ----------------------------------------------------------------------------

typedef cyg_uint16 STM32_TYPE;

# define STM32_INTSCACHE_STATE     int _saved_ints_
# define STM32_INTSCACHE_BEGIN()   HAL_DISABLE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_SUSPEND() HAL_RESTORE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_RESUME()  HAL_DISABLE_INTERRUPTS(_saved_ints_)
# define STM32_INTSCACHE_END()     HAL_RESTORE_INTERRUPTS(_saved_ints_)

#define STM32_UNCACHED_ADDRESS(__x) ((STM32_TYPE *)(__x))

// ----------------------------------------------------------------------------
// Forward declarations for functions that need to be placed in RAM:

static int stm32_flash_hw_erase(cyg_flashaddr_t addr) __attribute__((section (".2ram.stm32_flash_hw_erase")));
static int stm32_flash_hw_program( volatile STM32_TYPE* addr, const cyg_uint16* buf, cyg_uint32 count) __attribute__((section (".2ram.stm32_flash_hw_program")));
    
// ----------------------------------------------------------------------------
// Diagnostic routines.

#if 0
#define stf_diag( __fmt, ... ) diag_printf("STF: %20s[%3d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define stf_dump_buf( __addr, __size ) diag_dump_buf( __addr, __size )
#else
#define stf_diag( __fmt, ... )
#define stf_dump_buf( __addr, __size )
#endif

// ----------------------------------------------------------------------------
// Initialize the flash.


static int
stm32_flash_init(struct cyg_flash_dev* dev)
{
    cyg_stm32_flash_dev *stm32_dev = (cyg_stm32_flash_dev *)dev->priv;
    CYG_ADDRESS base = CYGHWR_HAL_STM32_FLASH;
    cyg_uint32 sig, id;
    cyg_uint32 flash_size, block_size = 0;
    
    // Set up the block info entries.

    dev->block_info                             = &stm32_dev->block_info[0];
    dev->num_block_infos                        = 1;

    // Get flash size from device signature and MCU ID

    HAL_READ_UINT32( CYGHWR_HAL_STM32_DEV_SIG, sig );
    HAL_READ_UINT32( CYGHWR_HAL_STM32_MCU_ID, id );

    stf_diag("sig %08x id %08x\n", sig, id );
    
    flash_size = CYGHWR_HAL_STM32_DEV_SIG_FSIZE(sig);

    if( CYGHWR_HAL_STM32_MCU_ID_DEV(id) == CYGHWR_HAL_STM32_MCU_ID_DEV_MEDIUM )
    {
        block_size = 1024;
        if( flash_size == 0xFFFF ) flash_size = 128;
    }
    else if( CYGHWR_HAL_STM32_MCU_ID_DEV(id) == CYGHWR_HAL_STM32_MCU_ID_DEV_HIGH )
    {
        block_size = 2048;
        if( flash_size == 0xFFFF ) flash_size = 512;
    }

    flash_size *= 1024;
    
    stm32_dev->block_info[0].blocks             = flash_size/block_size;
    stm32_dev->block_info[0].block_size         = block_size;
    
    // Set end address
    dev->end                                    = dev->start+flash_size-1;

    stf_diag("block_size %d size %08x end %08x\n", block_size, flash_size, dev->end );    

    // Unlock the flash control registers

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY1 );
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_KEYR, CYGHWR_HAL_STM32_FLASH_KEYR_KEY2 );
    
    return CYG_FLASH_ERR_OK;
}

// ----------------------------------------------------------------------------

static size_t
stm32_flash_query(struct cyg_flash_dev* dev, void* data, size_t len)
{
    static char query[] = "STM32 Internal Flash";
    memcpy( data, query, sizeof(query));
    return sizeof(query);
}

// ----------------------------------------------------------------------------
// Get info about the current block, i.e. base and size.

static void
stm32_flash_get_block_info(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr, cyg_flashaddr_t* block_start, size_t* block_size)
{
    size_t          offset  = addr - dev->start;
    
    *block_start    = dev->start + (offset & (dev->block_info[0].block_size-1));
    *block_size     = dev->block_info[0].block_size;
}

// ----------------------------------------------------------------------------

static int stm32_flash_decode_error( int sr )
{
    int result = CYG_FLASH_ERR_OK;

    if( sr & CYGHWR_HAL_STM32_FLASH_SR_PGERR )
        result = CYG_FLASH_ERR_PROGRAM;

    if( sr & CYGHWR_HAL_STM32_FLASH_SR_WRPRTERR )
        result = CYG_FLASH_ERR_PROTECT;
    
    return result;
}

// ----------------------------------------------------------------------------
// Erase a single sector. There is no API support for chip-erase. The
// generic code operates one sector at a time, invoking the driver for
// each sector, so there is no opportunity inside the driver for
// erasing multiple sectors in a single call. The address argument
// points at the start of the sector.

static int
stm32_flash_hw_erase(cyg_flashaddr_t addr)
{
    cyg_uint32 base = CYGHWR_HAL_STM32_FLASH;
    cyg_uint32 sr, cr = 0;
    cyg_uint32 timeout = 100000;

    cr |= CYGHWR_HAL_STM32_FLASH_CR_PER;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_AR, addr );

    cr |= CYGHWR_HAL_STM32_FLASH_CR_STRT;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );
    
    do
    {
        HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_SR, sr );
    } while( (sr & CYGHWR_HAL_STM32_FLASH_SR_BSY) && timeout-- > 0);

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, 0 );
    
    return sr;
}

// ----------------------------------------------------------------------------
// Write data to flash, using individual word writes. The destination
// address will be aligned in a way suitable for the bus. The source
// address need not be aligned. The count is in STM32_TYPE's, not in
// bytes.

static int
stm32_flash_hw_program( volatile STM32_TYPE* addr, const cyg_uint16* buf, cyg_uint32 count)
{
    cyg_uint32 base = CYGHWR_HAL_STM32_FLASH;
    cyg_uint32 sr = 0, cr = 0;
    
    cr |= CYGHWR_HAL_STM32_FLASH_CR_PG;
    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, cr );

    stf_diag("cr %08x\n", cr );
    
    while( count-- )
    {
        cyg_uint32 timeout = 100000;
        
        HAL_WRITE_UINT16( addr, *buf );

        addr++;
        buf++;

        do
        {
            HAL_READ_UINT32( base+CYGHWR_HAL_STM32_FLASH_SR, sr );
//            stf_diag("sr %08x\n", sr );
        } while( (sr & CYGHWR_HAL_STM32_FLASH_SR_BSY) && timeout-- > 0 );
    }

    HAL_WRITE_UINT32( base+CYGHWR_HAL_STM32_FLASH_CR, 0 );

    stf_diag("cr %08x sr %08x\n", cr, sr );
    
    return sr;    
}

// ----------------------------------------------------------------------------
// Erase a single block. The calling code will have supplied a pointer
// aligned to a block boundary.

static int
stm32_flash_erase(struct cyg_flash_dev* dev, cyg_flashaddr_t dest)
{
    int                     (*erase_fn)(cyg_uint32);
    volatile STM32_TYPE*    uncached;
    cyg_flashaddr_t         block_start;
    size_t                  block_size;
    int                     result;
    STM32_INTSCACHE_STATE;

    stf_diag("dest %p\n", dest);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && (dest <= dev->end), "flash address out of device range");

    stm32_flash_get_block_info(dev, dest, &block_start, &block_size);
    CYG_ASSERT(dest == block_start, "erase address should be the start of a flash block");

    uncached    = STM32_UNCACHED_ADDRESS(dest);
    erase_fn    = (int (*)(cyg_uint32)) cyg_flash_anonymizer( & stm32_flash_hw_erase );

    STM32_INTSCACHE_BEGIN();    

    result = (*erase_fn)(block_start);
    result = stm32_flash_decode_error( result );
    
    STM32_INTSCACHE_END();
    
    return result;
}

// ----------------------------------------------------------------------------
// Write some data to the flash. The destination must be aligned to a
// 16 bit boundary. Higher level code guarantees that the data will
// not straddle a block boundary.

int
stm32_flash_program(struct cyg_flash_dev* dev, cyg_flashaddr_t dest, const void* src, size_t len)
{
    int                     (*program_fn)(volatile STM32_TYPE*, const cyg_uint16*, cyg_uint32);
    volatile STM32_TYPE*    uncached; 
    const cyg_uint16*       data;
    size_t                  to_write;
    int                     result  = CYG_FLASH_ERR_OK;

    STM32_INTSCACHE_STATE;

    stf_diag("dest %p src %p len %p(%d)\n", dest, src, len, len);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    // Source and destination must be 16-bit aligned.
    if( (0 != ((CYG_ADDRESS)dest & 1)) ||
        (0 != ((CYG_ADDRESS)src  & 1)) )
        return CYG_FLASH_ERR_INVALID;
    
    uncached    = STM32_UNCACHED_ADDRESS(dest);
    data        = (const cyg_uint16*) src;
    to_write    = len / sizeof(STM32_TYPE);      // Number of words, not bytes
    program_fn  = (int (*)(volatile STM32_TYPE*, const cyg_uint16*, cyg_uint32)) cyg_flash_anonymizer( & stm32_flash_hw_program );

    STM32_INTSCACHE_BEGIN();
    while (to_write > 0)
    {
        size_t this_write = (to_write < CYGNUM_DEVS_FLASH_STM32_V2_PROGRAM_BURST_SIZE) ?
                             to_write : CYGNUM_DEVS_FLASH_STM32_V2_PROGRAM_BURST_SIZE;

        
        result = (*program_fn)(uncached, data, this_write);
        result = stm32_flash_decode_error( result );
        if (result != CYG_FLASH_ERR_OK)
        {
            break;
        }
        to_write -= this_write;
        if (to_write > 0)
        {
            // There is still more to be written. The last write must have been a burst size
            uncached    += this_write;
            data        += this_write;
            STM32_INTSCACHE_SUSPEND();
            STM32_INTSCACHE_RESUME();
        }
    }
    STM32_INTSCACHE_END();
    return result;
}

// ----------------------------------------------------------------------------
// Function table

const CYG_FLASH_FUNS(cyg_stm32_flash_funs,
                     &stm32_flash_init,
                     &stm32_flash_query,
                     &stm32_flash_erase,
                     &stm32_flash_program,
                     (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                     cyg_flash_devfn_lock_nop,
                     cyg_flash_devfn_unlock_nop);

// ----------------------------------------------------------------------------
// End of stm32_flash.c
