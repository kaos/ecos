//==========================================================================
//
//      strata_aux.c
//
//      Flash driver for the Intel Strata family - implementation. 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2005, 2006 Free Software Foundation, Inc.                  
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    bartv
// Contributors:
// Date:         2005-06-11
//              
//####DESCRIPTIONEND####
//
//==========================================================================

// This file is #include'd multiple times from the main strata.c file,
// It serves to instantiate the various hardware operations in ways
// appropriate for all the bus configurations.

// The following macros are used to construct suitable function names
// for the current bus configuration. STRATA_SUFFIX is #define'd before
// each #include of strata_aux.c

#ifndef STRATA_STR
# define STRATA_STR1(_a_) # _a_
# define STRATA_STR(_a_) STRATA_STR1(_a_)
# define STRATA_CONCAT3_AUX(_a_, _b_, _c_) _a_##_b_##_c_
# define STRATA_CONCAT3(_a_, _b_, _c_) STRATA_CONCAT3_AUX(_a_, _b_, _c_)
#endif

#define STRATA_FNNAME(_base_) STRATA_CONCAT3(_base_, _,  STRATA_SUFFIX)

// Similarly construct a forward declaration, placing the function in
// the .2ram section. Each function must still be in a separate section
// for linker garbage collection.

# define STRATA_RAMFNDECL(_base_, _args_) \
  STRATA_FNNAME(_base_) _args_ __attribute__((section (".2ram." STRATA_STR(_base_) "_" STRATA_STR(STRATA_SUFFIX))))

// Calculate the CFI and ID offsets based on the device count. The
// main code may override this for specific configurations, e.g. 16as8
#ifndef STRATA_OFFSET_CFI_DATA
# define STRATA_OFFSET_CFI_DATA(_idx_)  _idx_
#endif
#ifndef STRATA_OFFSET_MANUFACTURER_ID
# define STRATA_OFFSET_MANUFACTURER_ID  0x00
#endif
#ifndef STRATA_OFFSET_DEVICE_ID
# define STRATA_OFFSET_DEVICE_ID        0x01
#endif
#ifndef STRATA_OFFSET_LOCK_STATUS
# define STRATA_OFFSET_LOCK_STATUS      0x02
#endif


// For parallel operation commands are issued in parallel and status
// bits are checked in parallel.
#ifndef STRATA_PARALLEL
# define STRATA_PARALLEL(_cmd_)    (_cmd_)
#endif

// ----------------------------------------------------------------------------
// Diagnostic routines.

#if 0
#define sf_diag( __fmt, ... ) diag_printf("SF: %s[%d]: " __fmt, __FUNCTION__, __LINE__, __VA_ARGS__ );
#define sf_dump_buf( __addr, __size ) diag_dump_buf( __addr, __size )
#else
#define sf_diag( __fmt, ... )
#define sf_dump_buf( __addr, __size )
#endif


// ----------------------------------------------------------------------------
// When performing the various low-level operations like erase the flash
// chip can no longer support ordinary data reads. Obviously this is a
// problem if the current code is executing out of flash. The solution is
// to store the key functions in RAM rather than flash, via a special
// linker section .2ram which usually gets placed in the same area as
// .data.
//
// In a ROM startup application anything in .2ram will consume space
// in both the flash and RAM. Hence it is desirable to keep the .2ram
// functions as small as possible, responsible only for the actual
// hardware manipulation.
//
// All these .2ram functions should be invoked with interrupts
// disabled. Depending on the hardware it may also be necessary to
// have the data cache disabled. The .2ram functions must be
// self-contained, even macro invocations like HAL_DELAY_US() are
// banned because on some platforms those could be implemented as
// function calls.

// gcc requires forward declarations with the attributes, then the actual
// definitions.
static void STRATA_RAMFNDECL(strata_hw_query,           (volatile STRATA_TYPE*, cyg_uint32*, cyg_uint32*));
static int  STRATA_RAMFNDECL(strata_hw_cfi,             (struct cyg_flash_dev*, cyg_strata_dev*, volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_erase,           (volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_program,         (volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32 count));
static int  STRATA_RAMFNDECL(strata_hw_bufprogram,      (volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32 count));
static int  STRATA_RAMFNDECL(strata_hw_is_locked,       (volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_lock_j3,         (volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_unlock_all_j3,   (volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_lock_k3,         (volatile STRATA_TYPE*));
static int  STRATA_RAMFNDECL(strata_hw_unlock_k3,       (volatile STRATA_TYPE*));

// Read the device id. This involves a straightforward command
// sequence, followed by a reset to get back into array mode.
// All chips are accessed in parallel, but only the response
// from the least significant is used.
static void
STRATA_FNNAME(strata_hw_query)(volatile STRATA_TYPE* addr, cyg_uint32* manufacturer, cyg_uint32* device)
{
    sf_diag("addr %08x cmd %08x width %d\n", addr, STRATA_COMMAND_READ_ID, sizeof(addr[0]) );
    addr[0]         = STRATA_COMMAND_READ_ID;
    *manufacturer   = STRATA_UNSWAP(addr[STRATA_OFFSET_MANUFACTURER_ID]);
    *device         = STRATA_UNSWAP(addr[STRATA_OFFSET_DEVICE_ID]);
    addr[0]         = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
}

// Perform a CFI query. This involves placing the device(s) into CFI
// mode, checking that this has really happened, and then reading the
// size and block info. The address corresponds to the start of the
// flash.
static int
STRATA_FNNAME(strata_hw_cfi)(struct cyg_flash_dev* dev, cyg_strata_dev* strata_dev, volatile STRATA_TYPE* addr)
{
    int     dev_size;
    int     i;
    int     erase_regions;
    cyg_uint8 writebuffer_lsb, writebuffer_msb;
    cyg_uint32 writebuffer;

    sf_diag("addr %08x %d cmd %08x\n",addr, sizeof(STRATA_TYPE), STRATA_COMMAND_CFI);
    // Just a single write is needed to put the device into CFI mode
    addr[0] = STRATA_COMMAND_CFI;
    sf_dump_buf( addr, 256 );
    // Now check that we really are in CFI mode. There should be a 'Q'
    // at a specific address. This test is not 100% reliable, but should
    // be good enough.
    if ('Q' != (STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_Q]) & 0x00FF)) {
        addr[0]   = STRATA_COMMAND_READ_ARRAY;
        HAL_MEMORY_BARRIER();
        return CYG_FLASH_ERR_PROTOCOL;
    }
    // Device sizes are always a power of 2, and the shift is encoded
    // in a single byte
    dev_size = 0x01 << (STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_SIZE]) & 0x00FF);
    dev->end = dev->start + dev_size - 1;

    // The number of erase regions is also encoded in a single byte.
    // Usually this is no more than 2. A value of 0 indicates that
    // only chip erase is supported, but the driver does not cope
    // with that.
    erase_regions = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_BLOCK_REGIONS]) & 0x00FF;
    if (erase_regions > CYGNUM_DEVS_FLASH_STRATA_V2_ERASE_REGIONS) {
        addr[0] = STRATA_COMMAND_READ_ARRAY;
        HAL_MEMORY_BARRIER();
        return CYG_FLASH_ERR_PROTOCOL;
    }
    dev->num_block_infos    = erase_regions;

    for (i = 0; i < erase_regions; i++) {
        cyg_uint32 count, size;
        cyg_uint32 count_lsb   = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_BLOCK_COUNT_LSB(i)]) & 0x00FF;
        cyg_uint32 count_msb   = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_BLOCK_COUNT_MSB(i)]) & 0x00FF;
        cyg_uint32 size_lsb    = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_BLOCK_SIZE_LSB(i)]) & 0x00FF;
        cyg_uint32 size_msb    = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_BLOCK_SIZE_MSB(i)]) & 0x00FF;

        count = ((count_msb << 8) | count_lsb) + 1;
        size  = (size_msb << 16) | (size_lsb << 8);
        sf_diag("erase_region %d count %d size %d\n", i, count, size);
        strata_dev->block_info[i].block_size  = (size_t) size * STRATA_DEVCOUNT;
        strata_dev->block_info[i].blocks      = count;
    }

    writebuffer_lsb = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_WRITE_BUFFER_LSB]) & 0x00FF;
    writebuffer_msb = STRATA_UNSWAP(addr[STRATA_OFFSET_CFI_WRITE_BUFFER_MSB]) & 0x00FF;
    writebuffer = 1 << writebuffer_lsb;

    strata_dev->bufsize = writebuffer / sizeof(STRATA_TYPE);

    // Get out of CFI mode
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();

    return CYG_FLASH_ERR_OK;
}

// Erase a single sector. There is no API support for chip-erase. The
// generic code operates one sector at a time, invoking the driver for
// each sector, so there is no opportunity inside the driver for
// erasing multiple sectors in a single call. The address argument
// points at the start of the sector.
static int
STRATA_FNNAME(strata_hw_erase)(volatile STRATA_TYPE* addr)
{
    STRATA_TYPE     status;
    int             result  = CYG_FLASH_ERR_OK;
    int             retries;

    sf_diag("addr %08x fc %d\n",addr, first_call);
    // Start the erase operation
    addr[0] = STRATA_COMMAND_CLEAR_STATUS;
    addr[0] = STRATA_COMMAND_ERASE;
    addr[0] = STRATA_COMMAND_ERASE_CONFIRM;        

    // All chips are now erasing in parallel. Loop until all have
    // completed. The SR7 bit will be clear while the erase is
    // proceeding, set when is has completed. For an already erased
    // block in a parallel configuration we'll read the 0xff erased
    // value so the test succeeds there as well.
    for (retries    = CYGNUM_DEVS_FLASH_STRATA_V2_ERASE_TIMEOUT;
         retries > 0;
         retries--) {
        status  = addr[0];
        if ((status & STRATA_STATUS_SR7) == STRATA_STATUS_SR7) {
            break;
        }
    }
    
    if (retries == 0) {
        // The world is messed up. One or more chips are still
        // erasing, status bits may be set, etc. This should not
        // happen, the erase timeout should be big enough. There
        // is no easy way to get back into a sane state.
        return CYG_FLASH_ERR_DRV_TIMEOUT;
    }
    
    // The erase operation has completed. First get the chip(s) back
    // into a sane state.
    addr[0] = STRATA_COMMAND_READ_STATUS;
    status  = addr[0];
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();

    // According to the data sheet the bits should be checked in order SR5,
    // SR3, SR2. This does not appear to work, at least on a J3. If the
    // block is locked then SR5 is set as well as SR1, so the wrong error
    // code would be returned.
    if (status & STRATA_STATUS_SR1) {
        result  = CYG_FLASH_ERR_PROTECT;
    } else if (status & STRATA_STATUS_SR3) {
        result  = CYG_FLASH_ERR_LOW_VOLTAGE;
    } else if (status & STRATA_STATUS_SR5) {
        if (status & STRATA_STATUS_SR4) {
            result = CYG_FLASH_ERR_PROTOCOL;
        } else {
            result  = CYG_FLASH_ERR_ERASE;
        }
    }
    sf_diag("status %08x result %d\n", status, result);
    return result;
}

// Write data to flash, using individual word writes. The destination
// address will be aligned in a way suitable for the bus. The source
// address need not be aligned. The count is in STRATA_TYPE's, i.e. as
// per the bus alignment, not in bytes.
static int
STRATA_FNNAME(strata_hw_program)(volatile STRATA_TYPE* addr, const cyg_uint8* buf, cyg_uint32 count)
{
    int         i;
    int         result  = CYG_FLASH_ERR_OK;
    int         retries = CYGNUM_DEVS_FLASH_STRATA_V2_PROGRAM_TIMEOUT;
    STRATA_TYPE status;
    
//    sf_diag("addr %08x %d\n",addr, count);
    addr[0] = STRATA_COMMAND_CLEAR_STATUS;
    for (i = 0; (i < count) && (result == CYG_FLASH_ERR_OK); i++) {
        addr[i] = STRATA_COMMAND_PROGRAM_WORD;
        addr[i] = STRATA_NEXT_DATUM(buf);

        // The data is now being written. While the write is in progress
        // SR7 will be clear.
        do {
            status  = addr[i];
        } while ((--retries > 0) && ((status & STRATA_STATUS_SR7) != STRATA_STATUS_SR7));

        // The status bits may not all change at the same time.
        // Re-read just to be sure
        status = addr[i];
        
        // Again the order here is not that in the datasheet. If the block is
        // locked then SR4 will be set as well as SR2.
        if (retries == 0) {
            result  = CYG_FLASH_ERR_DRV_TIMEOUT;
        } else if (status & STRATA_STATUS_SR1) {
            result = CYG_FLASH_ERR_PROTECT;
        } else if (status & STRATA_STATUS_SR3) {
            result = CYG_FLASH_ERR_LOW_VOLTAGE;
        } else if (status & STRATA_STATUS_SR4) {
            result = CYG_FLASH_ERR_PROGRAM;
        }
    }
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    return result;
}

static int
STRATA_FNNAME(strata_hw_bufprogram)(volatile STRATA_TYPE* addr, const cyg_uint8* buf, cyg_uint32 count)
{
    int         result  = CYG_FLASH_ERR_OK;
    STRATA_TYPE status;
    int         retries = CYGNUM_DEVS_FLASH_STRATA_V2_PROGRAM_TIMEOUT;
    int         i;
    
//    sf_diag("addr %p buf %p count %d\n",addr, buf, count);
    addr[0] = STRATA_COMMAND_CLEAR_STATUS;
    do {
        // Issue the command and check that the buffer is ready
        *addr   = STRATA_COMMAND_WRITE_BUFFER;
        status  = *addr;
    } while ((--retries > 0) && ((status & STRATA_STATUS_SR7) != STRATA_STATUS_SR7));
    
    // Now issue the count, data, and confirm the operation
    addr[0]   = STRATA_SWAP(STRATA_PARALLEL((count - 1)));
    for( i = 0 ; i < count ; i++ )    
    {
        STRATA_TYPE val = STRATA_NEXT_DATUM(buf);
        addr[i] = val;
    }
    addr[0]   = STRATA_COMMAND_WRITE_CONFIRM;

    // The write is proceeding. Loop for status.
    do {
        status  = addr[0];
    } while ((--retries > 0) && ((status & STRATA_STATUS_SR7) != STRATA_STATUS_SR7));

    // Re-read status, in case bit 7 changed before the others
    status = addr[0];

//    sf_diag("status %08x retries %d SR7 %08x\n", status, retries, STRATA_STATUS_SR7);
    // Again the order here is not that in the datasheet. If the block is
    // locked then SR4 will be set as well as SR1.
    if (retries == 0) {
        result  = CYG_FLASH_ERR_DRV_TIMEOUT;
    } else if (status & STRATA_STATUS_SR1) {
        result = CYG_FLASH_ERR_PROTECT;
    } else if (status & STRATA_STATUS_SR3) {
        result = CYG_FLASH_ERR_LOW_VOLTAGE;
    } else if (status & STRATA_STATUS_SR4) {
        result = CYG_FLASH_ERR_PROGRAM;
    }
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    if( result != CYG_FLASH_ERR_OK )
    {
        sf_diag("status %08x result %d\n", status, result );
    }
    return result;
}

static int
STRATA_FNNAME(strata_hw_is_locked)(volatile STRATA_TYPE* addr)
{
    int result;
    addr[0]         = STRATA_COMMAND_READ_ID;
    sf_dump_buf(addr, 16 );
    result          = addr[STRATA_OFFSET_LOCK_STATUS];
    addr[0]         = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    sf_diag("addr %08x result %08x id_locked %08x\n",addr, result, STRATA_ID_LOCKED);    
    // The bottom bit always holds the block locked status. Other bits
    // may get used, e.g. on the k3 family bit 1 holds the locked-down
    // status. This is ignored for now.
    return (0 != (result & STRATA_ID_LOCKED));
}

// With the J3 family locking involves checking the status register.
// There is no individual block unlock, instead code has to unlock
// all blocks and then selectively relock them.
static int
STRATA_FNNAME(strata_hw_lock_j3)(volatile STRATA_TYPE* addr)
{
    int         result  = CYG_FLASH_ERR_OK;
    STRATA_TYPE status;

    sf_diag("addr %08x\n",addr);
    addr[0] = STRATA_COMMAND_CLEAR_STATUS;
    addr[0] = STRATA_COMMAND_LOCK_BLOCK_0;
    addr[0] = STRATA_COMMAND_LOCK_BLOCK_1;
    do {
        status      = addr[0];
    } while ((status & STRATA_STATUS_SR7) != STRATA_STATUS_SR7);
    status  = addr[0];
    if (status & STRATA_STATUS_SR3) {
        result = CYG_FLASH_ERR_LOW_VOLTAGE;
    } else if ((status & STRATA_STATUS_SR5) && (status & STRATA_STATUS_SR4)) {
        result = CYG_FLASH_ERR_PROTOCOL;
    } else if (status & STRATA_STATUS_SR4) {
        result = CYG_FLASH_ERR_LOCK;
    }
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    sf_diag("addr %08x status %08x result %d\n",addr, status, result );
    return result;
}

static int
STRATA_FNNAME(strata_hw_unlock_all_j3)(volatile STRATA_TYPE* addr)
{
    int         result  = CYG_FLASH_ERR_OK;
    STRATA_TYPE status;
    
    sf_diag("addr %08x\n",addr);
    addr[0] = STRATA_COMMAND_CLEAR_STATUS;
    addr[0] = STRATA_COMMAND_UNLOCK_ALL_0;
    addr[0] = STRATA_COMMAND_UNLOCK_ALL_1;
    do {
        status      = addr[0];
    } while ((status & STRATA_STATUS_SR7) != STRATA_STATUS_SR7);
    status  = addr[0];
    if (status & STRATA_STATUS_SR3) {
        result = CYG_FLASH_ERR_LOW_VOLTAGE;
    } else if ((status & STRATA_STATUS_SR5) && (status & STRATA_STATUS_SR4)) {
        result = CYG_FLASH_ERR_PROTOCOL;
    } else if (status & STRATA_STATUS_SR4) {
        result = CYG_FLASH_ERR_LOCK;
    }
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    sf_diag("addr %08x status %08x result %d\n",addr, status, result );
    return result;
}

// With the K3 family locking involves checking the id rather than the
// status register, and there is a block unlock command. There is no
// exported support for manipulating the block low-down bits so it is
// assumed these bits remain clear.
static int
STRATA_FNNAME(strata_hw_lock_k3)(volatile STRATA_TYPE* addr)
{
    int         result  = CYG_FLASH_ERR_OK;
    STRATA_TYPE status;

    sf_diag("addr %08x\n",addr);
    do {
        addr[0] = STRATA_COMMAND_LOCK_BLOCK_0;
        addr[0] = STRATA_COMMAND_LOCK_BLOCK_1;
        addr[0] = STRATA_COMMAND_READ_ID;
        status  = addr[STRATA_OFFSET_LOCK_STATUS];
    } while ((status & STRATA_ID_LOCKED) != STRATA_ID_LOCKED);
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    return result;
}

static int
STRATA_FNNAME(strata_hw_unlock_k3)(volatile STRATA_TYPE* addr)
{
    int         result  = CYG_FLASH_ERR_OK;
    STRATA_TYPE status;

    do {
        addr[0] = STRATA_COMMAND_UNLOCK_BLOCK_0;
        addr[0] = STRATA_COMMAND_UNLOCK_BLOCK_1;
        addr[0] = STRATA_COMMAND_READ_ID;
        status  = addr[STRATA_OFFSET_LOCK_STATUS];
    } while ((status & STRATA_ID_LOCKED) != 0);
    addr[0] = STRATA_COMMAND_READ_ARRAY;
    HAL_MEMORY_BARRIER();
    return result;
}

// ----------------------------------------------------------------------------
// Exported code, mostly for placing in a cyg_flash_dev_funs structure.

// Just read the device id, either for sanity checking that the system
// has been configured for the right device, or for filling in the
// block info by a platform-specific init routine if the platform may
// be manufactured with one of several different chips.
void
STRATA_FNNAME(cyg_strata_read_devid) (struct cyg_flash_dev* dev, cyg_uint32* manufacturer, cyg_uint32* device)
{
    int                     (*query_fn)(volatile STRATA_TYPE*, cyg_uint32*, cyg_uint32*);
    volatile STRATA_TYPE*   uncached;
    STRATA_INTSCACHE_STATE;

    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    
    uncached = STRATA_UNCACHED_ADDRESS(dev->start);
    query_fn = (int (*)(volatile STRATA_TYPE*, cyg_uint32*, cyg_uint32*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_query) );
    STRATA_INTSCACHE_BEGIN();
    (*query_fn)(uncached, manufacturer, device);
    STRATA_INTSCACHE_END();
}

// Validate that the device statically configured is the one on the
// board.
int
STRATA_FNNAME(cyg_strata_init_check_devid)(struct cyg_flash_dev* dev)
{
    cyg_strata_dev* strata_dev;
    cyg_uint32      manufacturer, device;

    sf_diag("\n", 0);
    strata_dev = (cyg_strata_dev*) dev->priv;
    STRATA_FNNAME(cyg_strata_read_devid)(dev, &manufacturer, &device);
    if ((manufacturer != strata_dev->manufacturer_code) ||
        (device       != strata_dev->device_code)) {
        return CYG_FLASH_ERR_DRV_WRONG_PART;
    }
    // Successfully queried the device, and the id's match. That
    // should be a good enough indication that the flash is working.
    return CYG_FLASH_ERR_OK;
}

// Initialize via a CFI query, instead of statically specifying the
// boot block layout.
int
STRATA_FNNAME(cyg_strata_init_cfi)(struct cyg_flash_dev* dev)
{
    int                     (*cfi_fn)(struct cyg_flash_dev*, cyg_strata_dev*, volatile STRATA_TYPE*);
    volatile STRATA_TYPE*   uncached;
    cyg_strata_dev*         strata_dev;
    int                     result;
    STRATA_INTSCACHE_STATE;
    
    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    strata_dev  = (cyg_strata_dev*) dev->priv;    // Remove const, only place where this is needed.
    uncached    = STRATA_UNCACHED_ADDRESS(dev->start);
    cfi_fn      = (int (*)(struct cyg_flash_dev*, cyg_strata_dev*, volatile STRATA_TYPE*))
        cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_cfi));

    STRATA_INTSCACHE_BEGIN();
    result      = (*cfi_fn)(dev, strata_dev, uncached);
    STRATA_INTSCACHE_END();

    // Now calculate the device size, and hence the end field.
    if (CYG_FLASH_ERR_OK == result) {
        int i;
        int size    = 0;
        for (i = 0; i < dev->num_block_infos; i++) {
            size += (dev->block_info[i].block_size * dev->block_info[i].blocks);
        }
        dev->end = dev->start + size - 1;
    }
    return result;
}

// Erase a single block. The calling code will have supplied a pointer
// aligned to a block boundary.
int
STRATA_FNNAME(cyg_strata_erase)(struct cyg_flash_dev* dev, cyg_flashaddr_t dest)
{
    int                     (*erase_fn)(volatile STRATA_TYPE*);
    volatile STRATA_TYPE*   uncached;
    cyg_flashaddr_t         block_start;
    size_t                  block_size;
    int                     result;
    STRATA_INTSCACHE_STATE;

    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && (dest <= dev->end), "flash address out of device range");

    strata_get_block_info(dev, dest, &block_start, &block_size);
    CYG_ASSERT(dest == block_start, "erase address should be the start of a flash block");
    
    uncached    = STRATA_UNCACHED_ADDRESS(dest);
    erase_fn    = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_erase) );

    STRATA_INTSCACHE_BEGIN();
    result = (*erase_fn)(uncached);
    STRATA_INTSCACHE_END();
    return result;
}

// Write some data to the flash. The destination must be aligned
// appropriately for the bus width (not the device width). Higher
// level code guarantees that the data will not straddle a block
// boundary.
int
STRATA_FNNAME(cyg_strata_program)(struct cyg_flash_dev* dev, cyg_flashaddr_t dest, const void* src, size_t len)
{
    int                     (*program_fn)(volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32);
    volatile STRATA_TYPE*   uncached; 
    const cyg_uint8*        data;
    int                     result  = CYG_FLASH_ERR_OK;
    STRATA_INTSCACHE_STATE;

    sf_diag("dest %p src %p len %p(%d) end %p\n", dest, src, len, len, dest+len-1 );
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    // Only support writes that are aligned to the bus boundary. This
    // may be more restrictive than what the hardware is capable of.
    // However it ensures that the hw_program routine can write as
    // much data as possible each iteration, and hence significantly
    // improves performance. The length had better be a multiple of
    // the bus width as well
    if ((0 != ((CYG_ADDRWORD)dest & (sizeof(STRATA_TYPE) - 1))) ||
        (0 != (len & (sizeof(STRATA_TYPE) - 1)))) {
        return CYG_FLASH_ERR_INVALID;
    }

    uncached    = STRATA_UNCACHED_ADDRESS(dest);
    data        = (const cyg_uint8*) src;
    program_fn  = (int (*)(volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_program) );

    STRATA_INTSCACHE_BEGIN();
    result = (*program_fn)(uncached, data, len / sizeof(STRATA_TYPE));
    STRATA_INTSCACHE_END();
    return result;
}

// Write some data to the flash. The destination must be aligned
// appropriately for the bus width (not the device width).
int
STRATA_FNNAME(cyg_strata_bufprogram)(struct cyg_flash_dev* dev, cyg_flashaddr_t dest, const void* src, size_t len)
{
    int                     (*program_fn)(volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32);
    volatile STRATA_TYPE*   uncached; 
    const cyg_uint8*        data;
    int                     bufsize;
    size_t                  to_write, first_write, this_write;
    int                     result  = CYG_FLASH_ERR_OK;
    STRATA_INTSCACHE_STATE;

    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= dev->start) && ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    bufsize = ((const cyg_strata_dev*)dev->priv)->bufsize;
    
    // Only support writes that are aligned to the bus boundary. This
    // may be more restrictive than what the hardware is capable of.
    // However it ensures that the hw_program routine can write as
    // much data as possible each iteration, and hence significantly
    // improves performance. The length had better be a multiple of
    // the bus width as well
    if ((0 != ((CYG_ADDRWORD)dest & (sizeof(STRATA_TYPE) - 1))) ||
        (0 != (len & (sizeof(STRATA_TYPE) - 1)))) {
        return CYG_FLASH_ERR_INVALID;
    }

    uncached    = STRATA_UNCACHED_ADDRESS(dest);

    data        = (const cyg_uint8*) src;
    program_fn  = (int (*)(volatile STRATA_TYPE*, const cyg_uint8*, cyg_uint32)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_bufprogram) );

    // Buffering works best when the data is aligned to a suitable
    // boundary, so the first write size may be smaller than the
    // buffer size to achieve alignment.
    if ((CYG_ADDRWORD)uncached & ((bufsize * sizeof(STRATA_TYPE)) - 1)) {
        first_write  = (CYG_ADDRWORD)uncached & ((bufsize * sizeof(STRATA_TYPE)) - 1);
        first_write /= sizeof(STRATA_TYPE);
        first_write  = bufsize - first_write;
    } else {
        first_write = 0;
    }
        
    STRATA_INTSCACHE_BEGIN();
    for (to_write = len/sizeof(STRATA_TYPE); to_write > 0; ) {

        if (first_write) {
            this_write  = first_write;
            first_write = 0;
        } else {
            this_write  = bufsize;
        }
        if (this_write > to_write) {
            this_write = to_write;
        }
        result = (*program_fn)(uncached, data, this_write);
        if (result != CYG_FLASH_ERR_OK) {
            break;
        }
        to_write -= this_write;
        if (to_write > 0) {
            uncached    += this_write;
            data        += sizeof(STRATA_TYPE) * this_write;
        }
    }
    STRATA_INTSCACHE_END();
    return result;
}

int
STRATA_FNNAME(cyg_strata_lock_j3)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile STRATA_TYPE*   uncached;
    int                     result;
    int (*lock_fn)(volatile STRATA_TYPE*);
    STRATA_INTSCACHE_STATE;
    
    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) && 
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached    = STRATA_UNCACHED_ADDRESS(dest);
    lock_fn     = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_lock_j3) );
    STRATA_INTSCACHE_BEGIN();
    result  = (*lock_fn)(uncached);
    STRATA_INTSCACHE_END();
    return result;
}

int
STRATA_FNNAME(cyg_strata_unlock_j3)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    int (*locked_fn)(volatile STRATA_TYPE*);
    int (*lock_fn)(volatile STRATA_TYPE*);
    int (*unlock_all_fn)(volatile STRATA_TYPE*);
    cyg_uint8               locked_bits[(CYGNUM_DEVS_FLASH_STRATA_V2_BLOCKS + 7) / 8];
    int                     i, j;
    int                     current_block;
    volatile STRATA_TYPE*   uncached;
    volatile STRATA_TYPE*   uncached_block;
    STRATA_INTSCACHE_STATE;

    sf_diag("\n", 0);
    locked_fn       = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_is_locked));
    lock_fn         = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_lock_j3));
    unlock_all_fn   = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_unlock_all_j3));
    
    memset(locked_bits, 0, sizeof(locked_bits));

    uncached        = STRATA_UNCACHED_ADDRESS(dev->start);
    uncached_block  = STRATA_UNCACHED_ADDRESS(dest);
    
    STRATA_INTSCACHE_BEGIN();

    // The obvious optimization: no point in doing anything if the
    // block is already unlocked.
    if (! (*locked_fn)(uncached_block)) {
        STRATA_INTSCACHE_END();
        return CYG_FLASH_ERR_OK;
    }
    
    for (i = 0, current_block = 0; i < dev->num_block_infos; i++) {
        sf_diag("block_info[%d] n %d sz %d\n",i, dev->block_info[i].blocks, dev->block_info[i].block_size );
        for (j = 0; j < dev->block_info[i].blocks; j++, current_block++) {
            CYG_LOOP_INVARIANT(current_block < CYGNUM_DEVS_FLASH_STRATA_V2_BLOCKS, "Device has too many blocks");
            if ((uncached != uncached_block) && (*locked_fn)(uncached)) {
                locked_bits[current_block >> 3] |= (0x01 << (current_block & 0x07));
            }
            uncached   += dev->block_info[i].block_size/sizeof(STRATA_TYPE);
        }
    }
    sf_diag("locked bits:\n", 0 );
    sf_dump_buf( locked_bits, sizeof(locked_bits) );
    uncached    = STRATA_UNCACHED_ADDRESS(dev->start);
    (*unlock_all_fn)(uncached);
    for (i = 0, current_block = 0; i < dev->num_block_infos; i++) {
        for (j = 0; j < dev->block_info[i].blocks; j++, current_block++) {
            if (locked_bits[current_block >> 3] & (0x01 << (current_block & 0x07))) {
                (*lock_fn)(uncached);
            }
            uncached    += dev->block_info[i].block_size/sizeof(STRATA_TYPE);
        }
    }
    STRATA_INTSCACHE_END();
    return CYG_FLASH_ERR_OK;
}

// An additional exported interface to make life easier for
// application developers. This code assumes the pointer is for a
// strata flash device.
int
STRATA_FNNAME(cyg_strata_unlock_all_j3)(const cyg_flashaddr_t dest)
{
    cyg_flash_info_t        info;
    volatile STRATA_TYPE*   uncached;
    int (*unlock_all_fn)(volatile STRATA_TYPE*);
    int result;
    STRATA_INTSCACHE_STATE;

    sf_diag("\n", 0);
    result = cyg_flash_get_info_addr(dest, &info);
    if (CYG_FLASH_ERR_OK != result) {
        return result;
    }
        
    uncached        = STRATA_UNCACHED_ADDRESS(info.start);
    unlock_all_fn   = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_unlock_all_j3));
    STRATA_INTSCACHE_BEGIN();
    result = (*unlock_all_fn)(uncached);
    STRATA_INTSCACHE_END();
    return result;
}

int
STRATA_FNNAME(cyg_strata_lock_k3)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile STRATA_TYPE*   uncached;
    int                     result;
    int (*lock_fn)(volatile STRATA_TYPE*);
    STRATA_INTSCACHE_STATE;
    
    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) && 
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached    = STRATA_UNCACHED_ADDRESS(dest);
    lock_fn     = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_lock_k3) );
    STRATA_INTSCACHE_BEGIN();
    result      = (*lock_fn)(uncached);
    STRATA_INTSCACHE_END();
    return result;
}

int
STRATA_FNNAME(cyg_strata_unlock_k3)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile STRATA_TYPE*   uncached;
    int                     result;
    int (*unlock_fn)(volatile STRATA_TYPE*);
    STRATA_INTSCACHE_STATE;
    
    sf_diag("\n", 0);
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) && 
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached    = STRATA_UNCACHED_ADDRESS(dest);
    unlock_fn   = (int (*)(volatile STRATA_TYPE*)) cyg_flash_anonymizer( & STRATA_FNNAME(strata_hw_unlock_k3) );
    STRATA_INTSCACHE_BEGIN();
    result  = (*unlock_fn)(uncached);
    STRATA_INTSCACHE_END();
    return result;
}

// ----------------------------------------------------------------------------
// Clean up the various #define's so this file can be #include'd again
#undef STRATA_FNNAME
#undef STRATA_RAMFNDECL
#undef STRATA_OFFSET_MANUFACTURER_ID
#undef STRATA_OFFSET_DEVICE_ID
#undef STRATA_OFFSET_LOCK_STATUS
#undef STRATA_OFFSET_CFI_DATA
#undef STRATA_PARALLEL
