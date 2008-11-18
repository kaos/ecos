//==========================================================================
//
//      am29xxxxx_aux.c
//
//      Flash driver for the AMD family - implementation. 
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2005, 2006, 2007, 2008 eCosCentric Ltd
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
// Author(s):    bartv
// Contributors:
// Date:         2004-11-05
//              
//####DESCRIPTIONEND####
//
//==========================================================================

// This file is #include'd multiple times from the main am29xxxxx.c file,
// It serves to instantiate the various hardware operations in ways
// appropriate for all the bus configurations.

// The following macros are used to construct suitable function names
// for the current bus configuration. AM29_SUFFIX is #define'd before
// each #include of am29xxxxx_aux.c

#ifndef AM29_STR
# define AM29_STR1(_a_) # _a_
# define AM29_STR(_a_) AM29_STR1(_a_)
# define AM29_CONCAT3_AUX(_a_, _b_, _c_) _a_##_b_##_c_
# define AM29_CONCAT3(_a_, _b_, _c_) AM29_CONCAT3_AUX(_a_, _b_, _c_)
#endif

#define AM29_FNNAME(_base_) AM29_CONCAT3(_base_, _,  AM29_SUFFIX)

// Similarly construct a forward declaration, placing the function in
// the .2ram section. Each function must still be in a separate section
// for linker garbage collection.

# define AM29_RAMFNDECL(_base_, _args_) \
  AM29_FNNAME(_base_) _args_ __attribute__((section (".2ram." AM29_STR(_base_) "_" AM29_STR(AM29_SUFFIX))))

// Calculate the various offsets, based on the device count.
// The main code may override these settings for specific
// configurations, e.g. 16as8
#ifndef AM29_OFFSET_COMMAND
# define AM29_OFFSET_COMMAND            0x0555
#endif
#ifndef AM29_OFFSET_COMMAND2
# define AM29_OFFSET_COMMAND2           0x02AA
#endif
#ifndef AM29_OFFSET_MANUFACTURER_ID
# define AM29_OFFSET_MANUFACTURER_ID    0x0000
#endif
#ifndef AM29_OFFSET_DEVID
# define AM29_OFFSET_DEVID              0x0001
#endif
#ifndef AM29_OFFSET_DEVID2
# define AM29_OFFSET_DEVID2             0x000E
#endif
#ifndef AM29_OFFSET_DEVID3
# define AM29_OFFSET_DEVID3             0x000F
#endif
#ifndef AM29_OFFSET_CFI
# define AM29_OFFSET_CFI                0x0055
#endif
#ifndef AM29_OFFSET_CFI_DATA
# define AM29_OFFSET_CFI_DATA(_idx_)    _idx_
#endif
#ifndef AM29_OFFSET_AT49_LOCK_STATUS
# define AM29_OFFSET_AT49_LOCK_STATUS   0x02
#endif

// For parallel operation commands are issued in parallel and status
// bits are checked in parallel.
#ifndef AM29_PARALLEL
# define AM29_PARALLEL(_cmd_)    (_cmd_)
#endif

// ----------------------------------------------------------------------------
// Diagnostic routines.

#if 0
#define amd_diag( __fmt, ... ) diag_printf("AMD: %s[%d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define amd_dump_buf( __addr, __size ) diag_dump_buf( __addr, __size )
#else
#define amd_diag( __fmt, ... )
#define amd_dump_buf( __addr, __size )
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
static int  AM29_RAMFNDECL(am29_hw_query, (volatile AM29_TYPE*));
static int  AM29_RAMFNDECL(am29_hw_cfi, (struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*));
static int  AM29_RAMFNDECL(am29_hw_erase, (volatile AM29_TYPE*));
static int  AM29_RAMFNDECL(am29_hw_program, (volatile AM29_TYPE*, volatile AM29_TYPE*, const cyg_uint8*, cyg_uint32 count, int retries));
static int  AM29_RAMFNDECL(at49_hw_softlock,        (volatile AM29_TYPE*));
static int  AM29_RAMFNDECL(at49_hw_hardlock,        (volatile AM29_TYPE*));
static int  AM29_RAMFNDECL(at49_hw_unlock,          (volatile AM29_TYPE*));


// ----------------------------------------------------------------------------

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_RESET_NEEDS_RESUME
// With this flash component (e.g. AT49xxxxx), the reset does not
// cause a suspended erase/program to be aborted. Instead all we
// can do is resume any suspended operations. We do this on each
// block as some parts have different granularity.

static void
AM29_FNNAME(am29_hw_force_all_suspended_resume)(struct cyg_flash_dev* dev, cyg_am29xxxxx_dev* am29_dev, volatile AM29_TYPE* addr)
{
    cyg_ucount16 i,j;
    AM29_TYPE datum1, datum2;

    AM29_2RAM_ENTRY_HOOK();
    
    for (i=0; i<dev->num_block_infos; i++)
    {
        for (j=0; j<am29_dev->block_info[i].blocks; j++)
        {
            addr[AM29_OFFSET_COMMAND] = AM29_COMMAND_ERASE_RESUME;
            HAL_MEMORY_BARRIER();
            // We don't know if the suspended operation was an erase or
            // program, so just compare the whole word to spot _any_ toggling.
            do {
                datum1  = addr[AM29_OFFSET_COMMAND];
                datum2  = addr[AM29_OFFSET_COMMAND];
            } while (datum1 != datum2);

            addr += am29_dev->block_info[i].block_size/sizeof(AM29_TYPE);
        }
    }
    
    AM29_2RAM_EXIT_HOOK();
}
#endif // ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_RESET_NEEDS_RESUME

// Read the device id. This involves a straightforward command
// sequence, followed by a reset to get back into array mode.
// All chips are accessed in parallel, but only the response
// from the least significant is used.
static int
AM29_FNNAME(am29_hw_query)(volatile AM29_TYPE* addr)
{
    int devid;
    cyg_uint32 onedevmask;

    AM29_2RAM_ENTRY_HOOK();

    // Fortunately the compiler should optimise the below
    // tests such that onedevmask is a constant.
    if ( 1 == (sizeof(AM29_TYPE) / AM29_DEVCOUNT) )
        onedevmask = 0xFF;
    else if ( 2 == (sizeof(AM29_TYPE) / AM29_DEVCOUNT) )
        onedevmask = 0xFFFF;
    else {
        CYG_ASSERT( 4 == (sizeof(AM29_TYPE) / AM29_DEVCOUNT), 
                    "Unexpected flash width per device" );
        onedevmask = 0xFFFFFFFF;
    }
    
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]  = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_AUTOSELECT;
    HAL_MEMORY_BARRIER();

    devid                       = AM29_UNSWAP(addr[AM29_OFFSET_DEVID]) & onedevmask;

//    amd_diag("devid %x\n", devid );
//    amd_dump_buf(addr, 64 );
    
    // The original AMD chips only used a single-byte device id, but
    // all codes have now been used up. Newer devices use a 3-byte
    // devid. The above devid read will have returned 0x007E. The
    // test allows for boards with a mixture of old and new chips.
    // The amount of code involved is too small to warrant a config
    // option.
    // FIXME by jifl: What happens when a single device is connected 16-bits
    // (or 32-bits) wide per device? Is the code still 0x7E, and all the
    // other devids are 8-bits only?
    if (0x007E == devid) {
        devid <<= 16;
        devid  |= ((AM29_UNSWAP(addr[AM29_OFFSET_DEVID2]) & 0x00FF) << 8);
        devid  |=  (AM29_UNSWAP(addr[AM29_OFFSET_DEVID3]) & 0x00FF);
    }
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
    
//    amd_diag("devid %x\n", devid );
    
    AM29_2RAM_EXIT_HOOK();
    return devid;
}

// Perform a CFI query. This involves placing the device(s) into CFI
// mode, checking that this has really happened, and then reading the
// size and block info. The address corresponds to the start of the
// flash.
static int
AM29_FNNAME(am29_hw_cfi)(struct cyg_flash_dev* dev, cyg_am29xxxxx_dev* am29_dev, volatile AM29_TYPE* addr)
{
    int     dev_size;
    int     i;
    int     erase_regions;

    AM29_2RAM_ENTRY_HOOK();
    
#ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CFI_BOGOSITY
    int     manufacturer_id;
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]  = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_AUTOSELECT;
    HAL_MEMORY_BARRIER();

    manufacturer_id             = AM29_UNSWAP(addr[AM29_OFFSET_MANUFACTURER_ID]) & 0x00FF;
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
#endif
    
    // Just a single write is needed to put the device into CFI mode
    addr[AM29_OFFSET_CFI]   = AM29_COMMAND_CFI;
    HAL_MEMORY_BARRIER();
//    amd_diag("CFI data:\n");
//    amd_dump_buf( addr, 256 );
    // Now check that we really are in CFI mode. There should be a 'Q'
    // at a specific address. This test is not 100% reliable, but should
    // be good enough.
    if ('Q' != (AM29_UNSWAP(addr[AM29_OFFSET_CFI_Q]) & 0x00FF)) {
        addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_RESET;
        HAL_MEMORY_BARRIER();
        AM29_2RAM_EXIT_HOOK();
        return CYG_FLASH_ERR_PROTOCOL;
    }
    // Device sizes are always a power of 2, and the shift is encoded
    // in a single byte
    dev_size = 0x01 << (AM29_UNSWAP(addr[AM29_OFFSET_CFI_SIZE]) & 0x00FF);
    dev->end = dev->start + dev_size - 1;

    // The number of erase regions is also encoded in a single byte.
    // Usually this is no more than 4. A value of 0 indicates that
    // only chip erase is supported, but the driver does not cope
    // with that.
    erase_regions   = AM29_UNSWAP(addr[AM29_OFFSET_CFI_BLOCK_REGIONS]) & 0x00FF;
    if (erase_regions > CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_ERASE_REGIONS) {
        addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_RESET;
        HAL_MEMORY_BARRIER();
        AM29_2RAM_EXIT_HOOK();
        return CYG_FLASH_ERR_PROTOCOL;
    }
    dev->num_block_infos    = erase_regions;

    for (i = 0; i < erase_regions; i++) {
        cyg_uint32 count, size;
        cyg_uint32 count_lsb   = AM29_UNSWAP(addr[AM29_OFFSET_CFI_BLOCK_COUNT_LSB(i)]) & 0x00FF;
        cyg_uint32 count_msb   = AM29_UNSWAP(addr[AM29_OFFSET_CFI_BLOCK_COUNT_MSB(i)]) & 0x00FF;
        cyg_uint32 size_lsb    = AM29_UNSWAP(addr[AM29_OFFSET_CFI_BLOCK_SIZE_LSB(i)]) & 0x00FF;
        cyg_uint32 size_msb    = AM29_UNSWAP(addr[AM29_OFFSET_CFI_BLOCK_SIZE_MSB(i)]) & 0x00FF;

        count = ((count_msb << 8) | count_lsb) + 1;
        size  = (size_msb << 16) | (size_lsb << 8);
        am29_dev->block_info[i].block_size  = (size_t) size * AM29_DEVCOUNT;
        am29_dev->block_info[i].blocks      = count;
    }

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CFI_BOGOSITY

    // Some flash parts have a peculiar implementation of CFI. The
    // device erase regions may not be in the order specified in the
    // main CFI area. Instead the erase regions are given in a
    // manufacturer dependent fixed order, regardless of whether this
    // is a top or bottom boot block device. A vendor-specific
    // extended query block has an entry saying whether the boot
    // blocks are at the top or bottom. This code works out whether
    // the erase regions appear to be specified in the wrong order,
    // and then swaps them over.

    {
        enum { bottom, symmetric, top } boot_type = symmetric;
        cyg_uint32 vspec = AM29_SWAP(addr[AM29_OFFSET_CFI_DATA(0x15)]) & 0x00FF;

        // Take a look at the vendor specific area for the boot block
        // order.
        
        switch( manufacturer_id )
        {
            // Atmel appear to have their own layout for the vendor
            // specific area. Offset 0x06 of the vendor specific area
            // contains a single bit: 0x00 = top boot, 0x01 = bottom
            // boot. There appears to be no way of specifying
            // symmetric formats.
        case 0x1F:
            if( (addr[AM29_OFFSET_CFI_DATA(vspec+0x06)] & AM29_SWAP(0x1)) == AM29_SWAP(0x1) )
                boot_type = bottom;
            else boot_type = top;
            break;

            // Most other manufacturers seem to follow the same layout
            // and encoding. Offset 0xF of the vendor specific area
            // contains the boot sector layout: 0x00 = uniform, 0x01 =
            // 8x8k top and bottom, 0x02 = bottom boot, 0x03 = top
            // boot, 0x04 = both top and bottom.
            //
            // The following manufacturers support this layout:
            // AMD, Spansion, ST, Macronix.
        default:
            if( (addr[AM29_OFFSET_CFI_DATA(vspec+0xF)] == AM29_SWAP(0x2)) )
                boot_type = bottom;                
            else if( (addr[AM29_OFFSET_CFI_DATA(vspec+0xF)] == AM29_SWAP(0x3)) )
                boot_type = top;
            // All other options are symmetric
            break;
        }

        // If the device is marked as top boot, but the erase region
        // list appears to be in bottom boot order, then reverse the
        // list. Also swap, if it is marked as bottom boot but the
        // erase regions appear to be in top boot order. This code
        // assumes that the first boot block is always smaller than
        // regular blocks; it is possible to imagine flash layouts for
        // which that is not true.
        
        if( ((boot_type == top) &&
             (am29_dev->block_info[0].block_size < am29_dev->block_info[erase_regions-1].block_size)) ||
            ((boot_type == bottom) &&
             (am29_dev->block_info[0].block_size > am29_dev->block_info[erase_regions-1].block_size)))
        {
            int lo, hi;

            for( lo = 0, hi = erase_regions-1 ; lo < hi ; lo++, hi-- )
            {
                size_t size                          = am29_dev->block_info[lo].block_size;
                cyg_uint32 count                     = am29_dev->block_info[lo].blocks;
                am29_dev->block_info[lo].block_size  = am29_dev->block_info[hi].block_size;
                am29_dev->block_info[lo].blocks      = am29_dev->block_info[hi].blocks;
                am29_dev->block_info[hi].block_size  = size;
                am29_dev->block_info[hi].blocks      = count;
            }
        }
    }
#endif
        
    // Get out of CFI mode
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();

    AM29_2RAM_EXIT_HOOK();
    return CYG_FLASH_ERR_OK;
}

// Erase a single sector. There is no API support for chip-erase. The
// generic code operates one sector at a time, invoking the driver for
// each sector, so there is no opportunity inside the driver for
// erasing multiple sectors in a single call. The address argument
// points at the start of the sector.
static int
AM29_FNNAME(am29_hw_erase)(volatile AM29_TYPE* addr)
{
    int         retries;
    AM29_TYPE   datum;

    AM29_2RAM_ENTRY_HOOK();
    
    // Start the erase operation
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]  = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_ERASE;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]  = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]   = AM29_COMMAND_ERASE_SECTOR;
    HAL_MEMORY_BARRIER();
    // There is now a 50us window in which we could send additional
    // ERASE_SECTOR commands, but the driver API does not allow this

    // All chips are now erasing in parallel. Loop until all have
    // completed. This can be detected in a number of ways. The DQ7
    // bit will be 0 until the erase is complete, but there is a
    // problem if something went wrong (e.g. the sector is locked),
    // the erase has not actually started, and the relevant bit was 0
    // already. More useful is DQ6. This will toggle during the 50us
    // window and while the erase is in progress, then stop toggling.
    // If the erase does not actually start then the bit won't toggle
    // at all so the operation completes rather quickly.
    //
    // If at any time DQ5 is set (indicating a timeout inside the
    // chip) then a reset command must be issued and the erase is
    // aborted. It is not clear this can actually happen during an
    // erase, but just in case.
    for (retries = CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_ERASE_TIMEOUT;
         retries > 0;
         retries--) {
        
        datum  = addr[AM29_OFFSET_COMMAND];
        // The operation completes when all DQ7 bits are set.
        if ((datum & AM29_STATUS_DQ7) == AM29_STATUS_DQ7) {
            break;
        }
        // Otherwise, for any flash chips where DQ7 is still clear, it is
        // necessary to test DQ5.
        if (((datum ^ AM29_STATUS_DQ7) >> 2) & datum & AM29_STATUS_DQ5) {
            // DQ5 is set, indicating a hardware error. The calling code
            // will always verify that the erase really was successful
            // so we do not need to distinguish between error conditions.
            addr[AM29_OFFSET_COMMAND] = AM29_COMMAND_RESET;
            HAL_MEMORY_BARRIER();
            break;
        }
    }

    // A result of 0 indicates a timeout.
    // A non-zero result indicates
    // that the erase completed or there has been a fatal error.
    AM29_2RAM_EXIT_HOOK();
    return retries;
}

// Write data to flash. At most one block will be processed at a time,
// but the write may be for a subset of the write. The destination
// address will be aligned in a way suitable for the bus. The source
// address need not be aligned. The count is in AM29_TYPE's, i.e.
// as per the bus alignment, not in bytes.
static int
AM29_FNNAME(am29_hw_program)(volatile AM29_TYPE* block_start, volatile AM29_TYPE* addr, const cyg_uint8* buf, cyg_uint32 count, int retries)
{
    int     i;

    AM29_2RAM_ENTRY_HOOK();
    
    for (i = 0; (i < count) && (retries > 0); i++) {
        AM29_TYPE   datum, current, active_dq7s;
        
        // We can only clear bits, not set them, so any bits that were
        // already clear need to be preserved.
        current = addr[i];
        datum   = AM29_NEXT_DATUM(buf) & current;
        if (datum == current) {
            // No change, so just move on.
            continue;
        }
        
        block_start[AM29_OFFSET_COMMAND]    = AM29_COMMAND_SETUP1;
        HAL_MEMORY_BARRIER();
        block_start[AM29_OFFSET_COMMAND2]   = AM29_COMMAND_SETUP2;
        HAL_MEMORY_BARRIER();
        block_start[AM29_OFFSET_COMMAND]    = AM29_COMMAND_PROGRAM;
        HAL_MEMORY_BARRIER();
        addr[i] = datum;
        HAL_MEMORY_BARRIER();

        // The data is now being written. The official algorithm is
        // to poll either DQ7 or DQ6, checking DQ5 along the way for
        // error conditions. This gets complicated with parallel
        // flash chips because they may finish at different times.
        // The alternative approach is to ignore the status bits
        // completely and just look for current==datum until the
        // retry count is exceeded. However that does not cope
        // cleanly with cases where the flash chip reports an error
        // early on, e.g. because a flash block is locked.

        while (--retries > 0) {
#if CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_PROGRAM_DELAY > 0
            // Some chips want a delay between polling
            { int j; for( j = 0; j < CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_PROGRAM_DELAY; j++ ); }
#endif
            // While the program operation is in progress DQ7 will read
            // back inverted from datum.
            current = addr[i];
            if ((current & AM29_STATUS_DQ7) == (datum & AM29_STATUS_DQ7)) {
                // All DQ7 bits now match datum, so the operation has completed.
                // But not necessarily successfully. On some chips DQ7 may
                // toggle before DQ0-6 are valid, so we need to read the
                // data one more time.
                current = addr[i];
                if (current != datum) {
                    retries = 0;    // Abort this burst.
                }
                break;
            }

            // Now we want to check the DQ5 status bits, but only for those
            // chips which are still programming. ((current^datum) & DQ7) gives
            // ones for chips which are still programming, zeroes for chips when
            // the programming is complete.
            active_dq7s = (current ^ datum) & AM29_STATUS_DQ7;
            
            if (0 != (current & (active_dq7s >> 2))) {
                // Unfortunately this is not sufficient to prove an error. On
                // some chips DQ0-6 switch to the data while DQ7 is still a
                // status flag, so the set DQ5 bit detected above may be data
                // instead of an error. Check again, this time DQ7 may
                // indicate completion.
                //
                // Next problem. Suppose chip A gave a bogus DQ5 result earlier
                // because it was just finishing. For this read chip A gives
                // back datum, but now chip B is finishing and has reported a
                // bogus DQ5.
                //
                // Solution: if any of the DQ7 lines have changed since the last
                // time, go around the loop again. When an error occurs DQ5
                // remains set and DQ7 remains toggled, so there is no harm
                // in one more polling loop.
                
                current = addr[i];
                if (((current ^ datum) & AM29_STATUS_DQ7) != active_dq7s) {
                    continue;
                }

                // DQ5 has been set in a chip where DQ7 indicates an ongoing
                // program operation for two successive reads. That is an error.
                // The hardware is in a strange state so must be reset.
                block_start[AM29_OFFSET_COMMAND]    = AM29_COMMAND_RESET;
                HAL_MEMORY_BARRIER();
                retries = 0;
                break;
            }
            // No DQ5 bits set in chips which are still programming. Poll again.
        }   // Retry for current word
    }       // Next word

    // At this point retries holds the total number of retries left.
    //  0 indicates a timeout or fatal error.
    // >0 indicates success.
    AM29_2RAM_EXIT_HOOK();
    return retries;
}

// FIXME: implement a separate program routine for buffered writes. 

#if 0
// Unused for now, but might be useful later
static int
AM29_FNNAME(at49_hw_is_locked)(volatile AM29_TYPE* addr)
{
    int result;
    AM29_TYPE plane;

    AM29_2RAM_ENTRY_HOOK();
    
    // Plane is bits A21-A20 for AT49BV6416
    // A more generic formula is needed.
    plane = AM29_PARALLEL( ((((CYG_ADDRESS)addr)>>21) & 0x3) );
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]        = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND + plane] = AM29_COMMAND_AUTOSELECT;
    HAL_MEMORY_BARRIER();
    result          = addr[AM29_OFFSET_AT49_LOCK_STATUS];
    addr[0]         = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
    // The bottom two bits hold the lock status, LSB indicates
    // soft lock, next bit indicates hard lock. We don't distinguish
    // in this function.
    AM29_2RAM_EXIT_HOOK();
    return (0 != (result & AM29_ID_LOCKED));
}
#endif

static int
AM29_FNNAME(at49_hw_softlock)(volatile AM29_TYPE* addr)
{
    int result  = CYG_FLASH_ERR_OK;

    AM29_2RAM_ENTRY_HOOK();
    
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]        = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_AT49_SOFTLOCK_BLOCK_0;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]        = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[0]                           = AM29_COMMAND_AT49_SOFTLOCK_BLOCK_1;
    HAL_MEMORY_BARRIER();
    // not sure if this is required:
    addr[0]                           = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
    AM29_2RAM_EXIT_HOOK();
    return result;
}

static int
AM29_FNNAME(at49_hw_hardlock)(volatile AM29_TYPE* addr)
{
    int result  = CYG_FLASH_ERR_OK;

    AM29_2RAM_ENTRY_HOOK();
    
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]        = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_AT49_HARDLOCK_BLOCK_0;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[AM29_OFFSET_COMMAND2]        = AM29_COMMAND_SETUP2;
    HAL_MEMORY_BARRIER();
    addr[0]                           = AM29_COMMAND_AT49_HARDLOCK_BLOCK_1;
    HAL_MEMORY_BARRIER();
    // not sure if this is required:
    addr[0]                           = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
    AM29_2RAM_EXIT_HOOK();
    return result;
}

static int
AM29_FNNAME(at49_hw_unlock)(volatile AM29_TYPE* addr)
{
    int result  = CYG_FLASH_ERR_OK;

    AM29_2RAM_ENTRY_HOOK();
    
    addr[AM29_OFFSET_COMMAND]         = AM29_COMMAND_SETUP1;
    HAL_MEMORY_BARRIER();
    addr[0]                           = AM29_COMMAND_AT49_UNLOCK_BLOCK;
    HAL_MEMORY_BARRIER();
    // not sure if this is required:
    addr[0]                           = AM29_COMMAND_RESET;
    HAL_MEMORY_BARRIER();
    AM29_2RAM_EXIT_HOOK();
    return result;
}


// ----------------------------------------------------------------------------
// Exported code, mostly for placing in a cyg_flash_dev_funs structure.

// Just read the device id, either for sanity checking that the system
// has been configured for the right device, or for filling in the
// block info by a platform-specific init routine if the platform may
// be manufactured with one of several different chips.
int
AM29_FNNAME(cyg_am29xxxxx_read_devid) (struct cyg_flash_dev* dev)
{
    int                 (*query_fn)(volatile AM29_TYPE*);
    int                 devid;
    volatile AM29_TYPE* addr;
    AM29_INTSCACHE_STATE;

    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");

    amd_diag("\n");
    
    addr     = AM29_UNCACHED_ADDRESS(dev->start);
    query_fn = (int (*)(volatile AM29_TYPE*)) cyg_flash_anonymizer( & AM29_FNNAME(am29_hw_query) );
    AM29_INTSCACHE_BEGIN();
    devid    = (*query_fn)(addr);
    AM29_INTSCACHE_END();
    return devid;
}

// Validate that the device statically configured is the one on the
// board.
int
AM29_FNNAME(cyg_am29xxxxx_init_check_devid)(struct cyg_flash_dev* dev)
{
    cyg_am29xxxxx_dev*  am29_dev;
    int                 devid;

    amd_diag("\n");
    
    am29_dev = (cyg_am29xxxxx_dev*) dev->priv;
    devid    = AM29_FNNAME(cyg_am29xxxxx_read_devid)(dev);
    if (devid != am29_dev->devid) {
        return CYG_FLASH_ERR_DRV_WRONG_PART;
    }

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_RESET_NEEDS_RESUME
    {
        volatile AM29_TYPE *addr = AM29_UNCACHED_ADDRESS(dev->start);
        void (*resume_fn)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*);
        resume_fn = (void (*)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*))
            cyg_flash_anonymizer( &AM29_FNNAME(am29_hw_force_all_suspended_resume) );
        AM29_INTSCACHE_STATE;

        AM29_INTSCACHE_BEGIN();
        (*resume_fn)(dev, am29_dev, addr);
        AM29_INTSCACHE_END();
    }
#endif

    // Successfully queried the device, and the id's match. That
    // should be a good enough indication that the flash is working.
    return CYG_FLASH_ERR_OK;
}

// Initialize via a CFI query, instead of statically specifying the
// boot block layout.
int
AM29_FNNAME(cyg_am29xxxxx_init_cfi)(struct cyg_flash_dev* dev)
{
    int                 (*cfi_fn)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*);
    volatile AM29_TYPE* addr;
    cyg_am29xxxxx_dev*  am29_dev;
    int                 result;
    AM29_INTSCACHE_STATE;
    
    amd_diag("\n");
    
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    am29_dev    = (cyg_am29xxxxx_dev*) dev->priv;    // Remove const, only place where this is needed.
    addr        = AM29_UNCACHED_ADDRESS(dev->start);
    cfi_fn      = (int (*)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*))
        cyg_flash_anonymizer( & AM29_FNNAME(am29_hw_cfi));

    AM29_INTSCACHE_BEGIN();
    result      = (*cfi_fn)(dev, am29_dev, addr);
    AM29_INTSCACHE_END();

    // Now calculate the device size, and hence the end field.
    if (CYG_FLASH_ERR_OK == result) {
        int i;
        int size    = 0;
        for (i = 0; i < dev->num_block_infos; i++) {
            amd_diag("region %d: 0x%08x * %d\n", i, (int)dev->block_info[i].block_size, dev->block_info[i].blocks );
            size += (dev->block_info[i].block_size * dev->block_info[i].blocks);
        }
        dev->end = dev->start + size - 1;

#ifdef CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_RESET_NEEDS_RESUME
    {
        void (*resume_fn)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*);
        resume_fn = (void (*)(struct cyg_flash_dev*, cyg_am29xxxxx_dev*, volatile AM29_TYPE*))
            cyg_flash_anonymizer( &AM29_FNNAME(am29_hw_force_all_suspended_resume) );

        AM29_INTSCACHE_BEGIN();
        (*resume_fn)(dev, am29_dev, addr);
        AM29_INTSCACHE_END();
    }
#endif
    }
    return result;
}

// Erase a single block. The calling code will have supplied a pointer
// aligned to a block boundary.
int
AM29_FNNAME(cyg_am29xxxxx_erase)(struct cyg_flash_dev* dev, cyg_flashaddr_t addr)
{
    int                 (*erase_fn)(volatile AM29_TYPE*);
    volatile AM29_TYPE* block;
    cyg_flashaddr_t     block_start;
    size_t              block_size;
    int                 i;
    int                 result;
    AM29_INTSCACHE_STATE;

    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((addr >= dev->start) && (addr <= dev->end), "flash address out of device range");

    am29_get_block_info(dev, addr, &block_start, &block_size);
    CYG_ASSERT(addr == block_start, "erase address should be the start of a flash block");
    
    amd_diag("addr %p block %p[%d]\n", (void*)addr, (void*)block_start, (int)block_size );
    
    block       = AM29_UNCACHED_ADDRESS(addr);
    erase_fn    = (int (*)(volatile AM29_TYPE*)) cyg_flash_anonymizer( & AM29_FNNAME(am29_hw_erase) );

    AM29_INTSCACHE_BEGIN();
    result = (*erase_fn)(block);
    AM29_INTSCACHE_END();

    // The erase may have failed for a number of reasons, e.g. because
    // of a locked sector. The best thing to do here is to check that the
    // erase has succeeded.
    block = (AM29_TYPE*) addr;
    for (i = 0; i < (block_size / sizeof(AM29_TYPE)); i++) {
        if (block[i] != (AM29_TYPE)~0) {
            // There is no easy way of detecting the specific error,
            // e.g. locked flash block, timeout, ... So return a
            // useless catch-all error.
            return CYG_FLASH_ERR_ERASE;
        }
    }
    return CYG_FLASH_ERR_OK;
}

// Write some data to the flash. The destination must be aligned
// appropriately for the bus width (not the device width).
int
AM29_FNNAME(cyg_am29xxxxx_program)(struct cyg_flash_dev* dev, cyg_flashaddr_t dest, const void* src, size_t len)
{
    int                 (*program_fn)(volatile AM29_TYPE*, volatile AM29_TYPE*, const cyg_uint8*, cyg_uint32, int);
    volatile AM29_TYPE* block;
    volatile AM29_TYPE* addr; 
    cyg_flashaddr_t     block_start;
    size_t              block_size;
    const cyg_uint8*    data;
    int                 retries;
    int                 i;

    AM29_INTSCACHE_STATE;

    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");

    amd_diag("dest %p src %p len %d\n", (void*)dest, (void*)src, (int)len );
    
    // Only support writes that are aligned to the bus boundary. This
    // may be more restrictive than what the hardware is capable of.
    // However it ensures that the hw_program routine can write as
    // much data as possible each iteration, and hence significantly
    // improves performance. The length had better be a multiple of
    // the bus width as well
    if ((0 != ((CYG_ADDRWORD)dest & (sizeof(AM29_TYPE) - 1))) ||
        (0 != (len & (sizeof(AM29_TYPE) - 1)))) {
        return CYG_FLASH_ERR_INVALID;
    }

    addr        = AM29_UNCACHED_ADDRESS(dest);
    CYG_ASSERT((dest >= dev->start) && (dest <= dev->end), "flash address out of device range");

    am29_get_block_info(dev, dest, &block_start, &block_size);
    CYG_ASSERT(((dest - block_start) + len) <= block_size, "write cannot cross block boundary");
    
    block       = AM29_UNCACHED_ADDRESS(block_start);
    data        = (const cyg_uint8*) src;
    retries     = CYGNUM_DEVS_FLASH_AMD_AM29XXXXX_V2_PROGRAM_TIMEOUT;

    program_fn  = (int (*)(volatile AM29_TYPE*, volatile AM29_TYPE*, const cyg_uint8*, cyg_uint32, int))
        cyg_flash_anonymizer( & AM29_FNNAME(am29_hw_program) );

    AM29_INTSCACHE_BEGIN();
    (*program_fn)(block, addr, data, len / sizeof(AM29_TYPE), retries);
    AM29_INTSCACHE_END();

    // Too many things can go wrong when manipulating the h/w, so
    // verify the operation by actually checking the data.
    addr = (volatile AM29_TYPE*) dest;
    data = (const cyg_uint8*) src;
    for (i = 0; i < (len / sizeof(AM29_TYPE)); i++) {
        AM29_TYPE   datum   = AM29_NEXT_DATUM(data);
        AM29_TYPE   current = addr[i];
        if ((datum & current) != current) {
            amd_diag("data %p addr[i] %p datum %08x current %08x\n", data-sizeof(AM29_TYPE), &addr[i], datum, current );
            return CYG_FLASH_ERR_PROGRAM;
        }
    }
    return CYG_FLASH_ERR_OK;
}

int
AM29_FNNAME(cyg_at49xxxx_softlock)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile AM29_TYPE*   uncached;
    int                     result;
    int (*lock_fn)(volatile AM29_TYPE*);
    AM29_INTSCACHE_STATE;

    amd_diag("\n");
    
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) &&
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached    = AM29_UNCACHED_ADDRESS(dest);
    lock_fn     = (int (*)(volatile AM29_TYPE*)) cyg_flash_anonymizer( & AM29_FNNAME(at49_hw_softlock) );
    AM29_INTSCACHE_BEGIN();
    result  = (*lock_fn)(uncached);
    AM29_INTSCACHE_END();
    return result;
}

int
AM29_FNNAME(cyg_at49xxxx_hardlock)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile AM29_TYPE*   uncached;
    int                     result;
    int (*lock_fn)(volatile AM29_TYPE*);
    AM29_INTSCACHE_STATE;

    amd_diag("\n");
    
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) &&
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached    = AM29_UNCACHED_ADDRESS(dest);
    lock_fn     = (int (*)(volatile AM29_TYPE*)) cyg_flash_anonymizer( & AM29_FNNAME(at49_hw_hardlock) );
    AM29_INTSCACHE_BEGIN();
    result  = (*lock_fn)(uncached);
    AM29_INTSCACHE_END();
    return result;
}

int
AM29_FNNAME(cyg_at49xxxx_unlock)(struct cyg_flash_dev* dev, const cyg_flashaddr_t dest)
{
    volatile AM29_TYPE* uncached;
    int                 result;
    int (*unlock_fn)(volatile AM29_TYPE*);
    AM29_INTSCACHE_STATE;

    amd_diag("\n");
    
    CYG_CHECK_DATA_PTR(dev, "valid flash device pointer required");
    CYG_ASSERT((dest >= (cyg_flashaddr_t)dev->start) &&
               ((CYG_ADDRESS)dest <= dev->end), "flash address out of device range");

    uncached = AM29_UNCACHED_ADDRESS(dest);
    unlock_fn = (int (*)(volatile AM29_TYPE*)) cyg_flash_anonymizer( & AM29_FNNAME(at49_hw_unlock) );

    AM29_INTSCACHE_BEGIN();
    result  = (*unlock_fn)(uncached);
    AM29_INTSCACHE_END();
    return result;
}

// ----------------------------------------------------------------------------
// Clean up the various #define's so this file can be #include'd again
#undef AM29_FNNAME
#undef AM29_RAMFNDECL
#undef AM29_OFFSET_COMMAND
#undef AM29_OFFSET_COMMAND2
#undef AM29_OFFSET_DEVID
#undef AM29_OFFSET_DEVID2
#undef AM29_OFFSET_DEVID3
#undef AM29_OFFSET_CFI
#undef AM29_OFFSET_CFI_DATA
#undef AM29_OFFSET_AT49_LOCK_STATUS
#undef AM29_PARALLEL
