//==========================================================================
//
//      strata.c
//
//      Flash driver for the Intel Strata family
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

#include <pkgconf/devs_flash_strata_v2.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/io/strata_dev.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>
#include <string.h>

// This driver supports multiple banks of Intel Strata flash devices
// or compatibles. These are NOR-flash devices, requiring explicit
// erase operations with an erase value of 0xff.
//
// The devices may be 8-bit, 16-bit, or 32-bit (64-bit devices are not
// yet supported). Most but not all 16-bit devices can also be
// accessed as 8-bit, in which case the chip may be hooked up to an
// 8-bit bus. A bank of flash may involve just a single chip, or there
// may be several chips in parallel. Typical combinations are 88 to
// get 16-bit, 8888 for 32-bit, and 1616 for 32-bit. It is assumed
// that all chips within a bank are the same device. There may also be
// several banks of flash, and different banks may use different
// devices.
//
// This driver instantiates support for the various bus
// configurations: 8, 16, 16AS8, 32, 88, 8888, and 1616. On any given
// platform only one or two of these combinations will be of interest,
// but the remainder will be eliminated via linker garbage collection.
// To avoid excessive duplication an auxiliary file contains the
// actual implementations. Compiler optimization should eliminate any
// unnecessary code.

// A flash driver is supposed to provide the following functions:
//  int     (*init)(...)
//  size_t  (*query)(...)
//  int     (*erase)(...)
//  int     (*program)(...)
//  int     (*hwr_map_error)(...)
//  int     (*block_lock)(...)
//  int     (*block_unlock)(...)
//
// The devices do not need any special initialization. However a given
// board may be manufactured with any one of several devices, which
// complicates things. The main complication is that there may be
// different bootsector layouts. The primary job of the init function
// is to check the device id, possibly fill in the bootsector info,
// or even to use the CFI support to get the bootsector info from the
// device itself. There may be other complications, e.g. minor variations
// of a given board design. These can be handled by h/w specific init
// functions in the platform HAL.
//
// The query function need not do anything useful, it is
// driver-defined.
//
// No read function need be supplied because the flash memory is
// always directly accessible to the cpu.
//
// The hwr_map_error is a no-op.
//
// Erase, program, and the locking functions need real
// implementations.

// ----------------------------------------------------------------------------
// The protocol understood by Strata flash chips and compatibles. The
// STRATA_PARALLEL() macro is used in bus configurations with multiple
// devices in parallel, to issue commands to all the devices in a
// single write. For CFI only one of the chips is queried. For READ_ID
// when getting the manufacturer and device id only a single device
// has to be queried, but when checking lock status all devices have
// to be checked.
#define STRATA_COMMAND_READ_ARRAY       STRATA_SWAP(STRATA_PARALLEL(0x00FF))
#define STRATA_COMMAND_READ_ID          STRATA_SWAP(STRATA_PARALLEL(0x0090))
#define STRATA_COMMAND_READ_STATUS      STRATA_SWAP(STRATA_PARALLEL(0x0070))
#define STRATA_COMMAND_CLEAR_STATUS     STRATA_SWAP(STRATA_PARALLEL(0x0050))
#define STRATA_COMMAND_PROGRAM_WORD     STRATA_SWAP(STRATA_PARALLEL(0x0040))
#define STRATA_COMMAND_WRITE_BUFFER     STRATA_SWAP(STRATA_PARALLEL(0x00E8))
#define STRATA_COMMAND_WRITE_CONFIRM    STRATA_SWAP(STRATA_PARALLEL(0x00D0))
#define STRATA_COMMAND_ERASE            STRATA_SWAP(STRATA_PARALLEL(0x0020))
#define STRATA_COMMAND_ERASE_CONFIRM    STRATA_SWAP(STRATA_PARALLEL(0x00D0))
#define STRATA_COMMAND_CFI              STRATA_SWAP((0x0098))
#define STRATA_COMMAND_CONFIGURATION    STRATA_SWAP(STRATA_PARALLEL(0x00B8))
#define STRATA_COMMAND_LOCK_BLOCK_0     STRATA_SWAP(STRATA_PARALLEL(0x0060))
#define STRATA_COMMAND_LOCK_BLOCK_1     STRATA_SWAP(STRATA_PARALLEL(0x0001))
#define STRATA_COMMAND_UNLOCK_BLOCK_0   STRATA_SWAP(STRATA_PARALLEL(0x0060))
#define STRATA_COMMAND_UNLOCK_BLOCK_1   STRATA_SWAP(STRATA_PARALLEL(0x00D0))
#define STRATA_COMMAND_UNLOCK_ALL_0     STRATA_SWAP(STRATA_PARALLEL(0x0060))
#define STRATA_COMMAND_UNLOCK_ALL_1     STRATA_SWAP(STRATA_PARALLEL(0x00D0))

// CFI offsets of interest. This assumes that the standard query table
// has not been replaced by the extended query table, although the
// CFI standard allows that behaviour.
#define STRATA_OFFSET_CFI_Q                     STRATA_OFFSET_CFI_DATA(0x0010)
#define STRATA_OFFSET_CFI_SIZE                  STRATA_OFFSET_CFI_DATA(0x0027)
#define STRATA_OFFSET_CFI_WRITE_BUFFER_LSB      STRATA_OFFSET_CFI_DATA(0x002A)
#define STRATA_OFFSET_CFI_WRITE_BUFFER_MSB      STRATA_OFFSET_CFI_DATA(0x002B)
#define STRATA_OFFSET_CFI_BLOCK_REGIONS         STRATA_OFFSET_CFI_DATA(0x002C)
#define STRATA_OFFSET_CFI_BLOCK_COUNT_LSB(_i_)  STRATA_OFFSET_CFI_DATA(0x002D + (4 * _i_))
#define STRATA_OFFSET_CFI_BLOCK_COUNT_MSB(_i_)  STRATA_OFFSET_CFI_DATA(0x002E + (4 * _i_))
#define STRATA_OFFSET_CFI_BLOCK_SIZE_LSB(_i_)   STRATA_OFFSET_CFI_DATA(0x002F + (4 * _i_))
#define STRATA_OFFSET_CFI_BLOCK_SIZE_MSB(_i_)   STRATA_OFFSET_CFI_DATA(0x0030 + (4 * _i_))

#define STRATA_STATUS_SR7   STRATA_SWAP(STRATA_PARALLEL(0x0080))
#define STRATA_STATUS_SR6   STRATA_SWAP(STRATA_PARALLEL(0x0040))
#define STRATA_STATUS_SR5   STRATA_SWAP(STRATA_PARALLEL(0x0020))
#define STRATA_STATUS_SR4   STRATA_SWAP(STRATA_PARALLEL(0x0010))
#define STRATA_STATUS_SR3   STRATA_SWAP(STRATA_PARALLEL(0x0008))
#define STRATA_STATUS_SR2   STRATA_SWAP(STRATA_PARALLEL(0x0004))
#define STRATA_STATUS_SR1   STRATA_SWAP(STRATA_PARALLEL(0x0002))
#define STRATA_STATUS_SR0   STRATA_SWAP(STRATA_PARALLEL(0x0001))
#define STRATA_ID_LOCKED    STRATA_SWAP(STRATA_PARALLEL(0x01))

// When programming the flash the source data may not be aligned
// correctly (although usually it will be). Hence it is necessary to
// construct the 16-bit or 32-bit numbers to be written to the flash
// from individual bytes, allowing for endianness.
#define STRATA_NEXT_DATUM_8(_ptr_) (*_ptr_++)
#if CYG_BYTEORDER == CYG_LSBFIRST
# define STRATA_NEXT_DATUM_16(_ptr_)                  \
    ({                                              \
        cyg_uint16 _result_;                        \
        _result_  = (_ptr_[1] << 8) | _ptr_[0];     \
        _ptr_    += 2;                              \
        _result_; })

# define STRATA_NEXT_DATUM_32(_ptr_)                                                      \
    ({                                                                                  \
        cyg_uint32 _result_;                                                            \
        _result_  = (_ptr_[3] << 24) | (_ptr_[2] << 16) | (_ptr_[1] << 8) | _ptr_[0];   \
        _ptr_    += 4;                                                                  \
        _result_; })
#else
# define STRATA_NEXT_DATUM_16(_ptr_)                  \
    ({                                              \
        cyg_uint16 _result_;                        \
        _result_  = (_ptr_[0] << 8) | _ptr_[1];     \
        _ptr_    += 2;                              \
        _result_; })

# define STRATA_NEXT_DATUM_32(_ptr_)                                                      \
    ({                                                                                  \
        cyg_uint32 _result_;                                                            \
        _result_  = (_ptr_[0] << 24) | (_ptr_[1] << 16) | (_ptr_[2] << 8) | _ptr_[3];   \
        _ptr_    += 4;                                                                  \
        _result_; })

#endif

// The addresses used for programming the flash may be different from
// the ones used to read the flash. The macro
// HAL_STRATA_UNCACHED_ADDRESS() can be supplied by one of the HAL
// packages. Otherwise if CYGHWR_DEVS_FLASH_STRATA_V2_CACHED_ONLY
// is not implemented then the macro CYGARC_UNCACHED_ADDRESS()
// will be used. If there is no way of bypassing the cache then
// the addresses will remain unchanged and instead the INTSCACHE
// macros will disable the cache.
#if defined(HAL_STRATA_UNCACHED_ADDRESS)
# define STRATA_UNCACHED_ADDRESS(_addr_)  (volatile STRATA_TYPE*)HAL_STRATA_UNCACHED_ADDRESS(_addr_)
#elif !defined(CYGHWR_DEVS_FLASH_STRATA_V2_CACHED_ONLY)
# ifndef CYGARC_UNCACHED_ADDRESS
#  error Cache should be bypassed but CYGARC_UNCACHED_ADDRESS is not defined.
# endif
# define STRATA_UNCACHED_ADDRESS(_addr_)  (volatile STRATA_TYPE*)CYGARC_UNCACHED_ADDRESS(_addr_)
#else
# define STRATA_UNCACHED_ADDRESS(_addr_)  (volatile STRATA_TYPE*)(_addr_)
#endif

// The bits on the data bus may need swapping, either because of
// endianness issues or because some lines are just wired wrong.
// SWAP is for commands going to the flash chip. UNSWAP is for
// data coming back from the flash chip. The swapping takes
// effect after allowing for STRATA_PARALLEL(). Data is never
// swapped, it does not matter if bit 5 of a datum is actually
// stored in bit 3 of the flash as long as the data reads back
// right.
#if defined(HAL_STRATA_SWAP)
# define STRATA_SWAP(_data_)      HAL_STRATA_SWAP(_data_)
#else
# define STRATA_SWAP(_data_)      (_data_)
#endif
#if defined(HAL_STRATA_UNSWAP)
# define STRATA_UNSWAP(_data_)    HAL_STRATA_UNSWAP(_data_)
#else
# define STRATA_UNSWAP(_data_)    (_data_)
#endif

// Cache and interrupt manipulation. This driver supports fine-grained
// control over interrupts and the cache, using three macros. These may
// be provided by the platform HAL, or by defaults here. There are
// three variants:
//
// 1) control both interrupts and cache, needed if
//    CYGHWR_DEVS_FLASH_STRATA_V2_CACHED_ONLY is implemented i.e. if it
//    is necessary to disable the cache to get direct access to the flash.
// 2) control interrupts only, the default if the cache can be bypassed
//    when accessing the flash.
// 3) do nothing, if the cache can be bypassed and the application
//    guarantees that the flash will not be accessed by any interrupt
//    handlers or other threads.

#if defined(CYGHWR_DEVS_FLASH_STRATA_V2_CACHED_ONLY)

// First, the amount of state that should be preserved. By default
// this means the interrupt state and the data cache state.
# define STRATA_INTSCACHE_DEFAULT_STATE   int _saved_ints_, _saved_dcache_

// Start an operation on the flash. Make sure that interrupts are
// disabled and then save the current state of the data cache. The
// actual flash manipulation should happen with the cache disabled.
// There may still be data in the cache that has not yet been flushed
// to memory, so take care of that first. Then invalidate the cache
// lines so that when the cache is re-enabled later on the processor
// gets everything from memory, rather than reusing old data in the
// cache.
# define STRATA_INTSCACHE_DEFAULT_BEGIN()       \
    CYG_MACRO_START                             \
    HAL_DISABLE_INTERRUPTS(_saved_ints_);       \
    HAL_DCACHE_IS_ENABLED(_saved_dcache_);      \
    HAL_DCACHE_SYNC();                          \
    if (_saved_dcache_) {                       \
        HAL_DCACHE_DISABLE();                   \
    }                                           \
    HAL_DCACHE_INVALIDATE_ALL();                \
    CYG_MACRO_END

// A flash operation has completed. Restore the situation to what it
// was before. Because of suspend/resume support interrupt handlers
// and other threads may have run, filling various cache lines with
// useful data. However it is assumed that none of those cache
// lines contain any of the data that has been manipulated by this
// flash operation (the stack and the flash block), so there is
// no need for another sync or invalidate. It is also assumed that
// we have not been executing any code out of the block of flash
// that has just been erased or programmed, so no need to worry
// about the icache.
#define STRATA_INTSCACHE_DEFAULT_END()          \
    CYG_MACRO_START                             \
    if (_saved_dcache_) {                       \
        HAL_DCACHE_ENABLE();                    \
    }                                           \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END

#elif !defined(CYGIMP_DEVS_FLASH_STRATA_V2_LEAVE_INTERRUPTS_ENABLED)

# define STRATA_INTSCACHE_DEFAULT_STATE     int _saved_ints_
# define STRATA_INTSCACHE_DEFAULT_BEGIN()   HAL_DISABLE_INTERRUPTS(_saved_ints_)
// The following blips the interrupt enable to allow pending interrupts
// to run, which will reduce interrupt latency given the dcache sync/invalidate
// may be relatively lengthy.
# define STRATA_INTSCACHE_DEFAULT_END()         \
    CYG_MACRO_START                             \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    HAL_DISABLE_INTERRUPTS(_saved_ints_);       \
    HAL_DCACHE_SYNC();                          \
    HAL_DCACHE_INVALIDATE_ALL();                \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END

#else

# define STRATA_INTSCACHE_DEFAULT_STATE     CYG_EMPTY_STATEMENT
# define STRATA_INTSCACHE_DEFAULT_BEGIN()   CYG_EMPTY_STATEMENT
# define STRATA_INTSCACHE_DEFAULT_END()         \
    CYG_MACRO_START                             \
    int _saved_ints_;                           \
    HAL_DISABLE_INTERRUPTS(_saved_ints_);       \
    HAL_DCACHE_SYNC();                          \
    HAL_DCACHE_INVALIDATE_ALL();                \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END

#endif

#ifdef HAL_STRATA_INTSCACHE_STATE
# define STRATA_INTSCACHE_STATE       HAL_STRATA_INTSCACHE_STATE
#else
# define STRATA_INTSCACHE_STATE       STRATA_INTSCACHE_DEFAULT_STATE
#endif
#ifdef HAL_STRATA_INTSCACHE_BEGIN
# define STRATA_INTSCACHE_BEGIN       HAL_STRATA_INTSCACHE_BEGIN
#else
# define STRATA_INTSCACHE_BEGIN       STRATA_INTSCACHE_DEFAULT_BEGIN
#endif
#ifdef HAL_STRATA_INTSCACHE_END
# define STRATA_INTSCACHE_END         HAL_STRATA_INTSCACHE_END
#else
# define STRATA_INTSCACHE_END         STRATA_INTSCACHE_DEFAULT_END
#endif

// Some HALs require a special instruction to flush write buffers.
// Not all HALs do though, so we define it empty if it isn't already present.
#ifndef HAL_MEMORY_BARRIER
# define HAL_MEMORY_BARRIER() CYG_EMPTY_STATEMENT
#endif

// ----------------------------------------------------------------------------
// Generic code.

// Get info about the current block, i.e. base and size.
static void
strata_get_block_info(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr, cyg_flashaddr_t* block_start, size_t* block_size)
{
    cyg_uint32      i;
    size_t          offset  = addr - dev->start;
    cyg_flashaddr_t result;

    result  = dev->start;
    
    for (i = 0; i < dev->num_block_infos; i++) {
        if (offset < (dev->block_info[i].blocks * dev->block_info[i].block_size)) {
            offset         -= (offset % dev->block_info[i].block_size);
            *block_start    = result + offset;
            *block_size     = dev->block_info[i].block_size;
            return;
        }
        result  += (dev->block_info[i].blocks * dev->block_info[i].block_size);
        offset  -= (dev->block_info[i].blocks * dev->block_info[i].block_size);
    }
    CYG_FAIL("Address out of range of selected flash device");
}

// ----------------------------------------------------------------------------
// Instantiate all of the h/w functions appropriate for the various
// configurations.
//   The suffix is used to construct the function names.
//   Types for the width of the bus, controlling the granularity of access.
//   devcount specifies the number of devices in parallel, and is used for looping
//   The NEXT_DATUM() macro allows for misaligned source data.
//   The PARALLEL macro, if defined, is used for sending commands and reading
//   status bits from all devices in the bank in one operation.

// A single 8-bit device on an 8-bit bus.
#define STRATA_SUFFIX               8
#define STRATA_TYPE                 cyg_uint8
#define STRATA_DEVCOUNT             1
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_8(_ptr_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// A single 16-bit device.
#define STRATA_SUFFIX               16
#define STRATA_TYPE                 cyg_uint16
#define STRATA_DEVCOUNT             1
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_16(_ptr_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// A single 32-bit device.
#define STRATA_SUFFIX               32
#define STRATA_TYPE                 cyg_uint32
#define STRATA_DEVCOUNT             1
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_32(_ptr_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// Two 8-bit devices, giving a 16-bit bus. 
#define STRATA_SUFFIX               88
#define STRATA_TYPE                 cyg_uint16
#define STRATA_DEVCOUNT             2
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_16(_ptr_)
#define STRATA_PARALLEL(_cmd_)      ((_cmd_ << 8) | _cmd_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// Four 8-bit devices, giving a 32-bit bus. 
#define STRATA_SUFFIX               8888
#define STRATA_TYPE                 cyg_uint32
#define STRATA_DEVCOUNT             4
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_32(_ptr_)
#define STRATA_PARALLEL(_cmd_)      ((_cmd_ << 24) | (_cmd_ << 16) | (_cmd_ << 8) | _cmd_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// Two 16-bit devices, giving a 32-bit bus.
#define STRATA_SUFFIX               1616
#define STRATA_TYPE                 cyg_uint32
#define STRATA_DEVCOUNT             2
#define STRATA_NEXT_DATUM(_ptr_)    STRATA_NEXT_DATUM_32(_ptr_)
#define STRATA_PARALLEL(_cmd_)      ((_cmd_ << 16) | _cmd_)

#include "strata_aux.c"

#undef STRATA_SUFFIX
#undef STRATA_TYPE
#undef STRATA_DEVCOUNT
#undef STRATA_NEXT_DATUM

// 16AS8. A 16-bit device hooked up so that only byte accesses are
// allowed. This requires unusual offsets for the CFI and query data.
#define STRATA_SUFFIX                   16as8
#define STRATA_TYPE                     cyg_uint8
#define STRATA_DEVCOUNT                 1
#define STRATA_NEXT_DATUM(_ptr_)        STRATA_NEXT_DATUM_8(_ptr_)
#define STRATA_OFFSET_MANUFACTURER_ID   00
#define STRATA_OFFSET_DEVICE_ID         02
#define STRATA_OFFSET_LOCK_STATUS       04
#define STRATA_OFFSET_CFI_DATA(_idx_)   (2 * _idx_)

#include "strata_aux.c"
