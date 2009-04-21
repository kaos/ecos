//==========================================================================
//
//      am29xxxxx.c
//
//      Flash driver for the AMD family
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004, 2005, 2006, 2007 Free Software Foundation, Inc.      
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
// Date:         2004-11-05
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_amd_am29xxxxx_v2.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/io/am29xxxxx_dev.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>

// This driver supports multiple banks of AMD am29xxxxx flash devices
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
// Erase, program, and the locking functions need real
// implementations, although locking is not always available.

// ----------------------------------------------------------------------------
// The protocol understood by AMD flash chips and compatibles.
// The AM29_PARALLEL() macro is used in bus configurations with multiple
// devices in parallel, to issue commands to all the devices in a single
// write. In theory some of the operations, e.g. READ_DEVID, only need
// to access a single chip but then you get into complications for the
// SETUP commands. The AM29_SWAP() macro deals with endianness issues
// on some targets and can also help with h/w where things are just not
// wired right.
#define AM29_COMMAND_SETUP1                  AM29_SWAP(AM29_PARALLEL(0x00AA))
#define AM29_COMMAND_SETUP2                  AM29_SWAP(AM29_PARALLEL(0x0055))
#define AM29_COMMAND_RESET                   AM29_SWAP(AM29_PARALLEL(0x00F0))
#define AM29_COMMAND_AUTOSELECT              AM29_SWAP(AM29_PARALLEL(0x0090))
#define AM29_COMMAND_ERASE                   AM29_SWAP(AM29_PARALLEL(0x0080))
#define AM29_COMMAND_ERASE_SECTOR            AM29_SWAP(AM29_PARALLEL(0x0030))
#define AM29_COMMAND_ERASE_RESUME            AM29_SWAP(AM29_PARALLEL(0x0030))
#define AM29_COMMAND_CFI                     AM29_SWAP(AM29_PARALLEL(0x0098))
#define AM29_COMMAND_PROGRAM                 AM29_SWAP(AM29_PARALLEL(0x00A0))
// Following are specific to AT49 derivatives
#define AM29_COMMAND_AT49_SOFTLOCK_BLOCK_0   AM29_SWAP(AM29_PARALLEL(0x0080))
#define AM29_COMMAND_AT49_SOFTLOCK_BLOCK_1   AM29_SWAP(AM29_PARALLEL(0x0040))
#define AM29_COMMAND_AT49_HARDLOCK_BLOCK_0   AM29_SWAP(AM29_PARALLEL(0x0080))
#define AM29_COMMAND_AT49_HARDLOCK_BLOCK_1   AM29_SWAP(AM29_PARALLEL(0x0060))
#define AM29_COMMAND_AT49_UNLOCK_BLOCK       AM29_SWAP(AM29_PARALLEL(0x0070))

// CFI offsets of interest. This assumes that the standard query table
// has not been replaced by the extended query table, although the
// CFI standard allows that behaviour.
#define AM29_OFFSET_CFI_Q                       AM29_OFFSET_CFI_DATA(0x0010)
#define AM29_OFFSET_CFI_SIZE                    AM29_OFFSET_CFI_DATA(0x0027)
#define AM29_OFFSET_CFI_BLOCK_REGIONS           AM29_OFFSET_CFI_DATA(0x002C)
#define AM29_OFFSET_CFI_BLOCK_COUNT_LSB(_i_)    AM29_OFFSET_CFI_DATA(0x002D + (4 * (_i_)))
#define AM29_OFFSET_CFI_BLOCK_COUNT_MSB(_i_)    AM29_OFFSET_CFI_DATA(0x002E + (4 * (_i_)))
#define AM29_OFFSET_CFI_BLOCK_SIZE_LSB(_i_)     AM29_OFFSET_CFI_DATA(0x002F + (4 * (_i_)))
#define AM29_OFFSET_CFI_BLOCK_SIZE_MSB(_i_)     AM29_OFFSET_CFI_DATA(0x0030 + (4 * (_i_)))

#define AM29_STATUS_DQ7             AM29_SWAP(AM29_PARALLEL(0x0080))
#define AM29_STATUS_DQ6             AM29_SWAP(AM29_PARALLEL(0x0040))
#define AM29_STATUS_DQ5             AM29_SWAP(AM29_PARALLEL(0x0020))
#define AM29_STATUS_DQ4             AM29_SWAP(AM29_PARALLEL(0x0010))
#define AM29_STATUS_DQ3             AM29_SWAP(AM29_PARALLEL(0x0008))
#define AM29_STATUS_DQ2             AM29_SWAP(AM29_PARALLEL(0x0004))
#define AM29_STATUS_DQ1             AM29_SWAP(AM29_PARALLEL(0x0002))
#define AM29_STATUS_DQ0             AM29_SWAP(AM29_PARALLEL(0x0001))
#define AM29_ID_LOCKED              AM29_SWAP(AM29_PARALLEL(0x03))

// When programming the flash the source data may not be aligned
// correctly (although usually it will be). Hence it is necessary to
// construct the 16-bit or 32-bit numbers to be written to the flash
// from individual bytes, allowing for endianness.
#define AM29_NEXT_DATUM_8(_ptr_) (*_ptr_++)
#if CYG_BYTEORDER == CYG_LSBFIRST
# define AM29_NEXT_DATUM_16(_ptr_)                  \
    ({                                              \
        cyg_uint16 _result_;                        \
        _result_  = (_ptr_[1] << 8) | _ptr_[0];     \
        _ptr_    += 2;                              \
        _result_; })

# define AM29_NEXT_DATUM_32(_ptr_)                                                      \
    ({                                                                                  \
        cyg_uint32 _result_;                                                            \
        _result_  = (_ptr_[3] << 24) | (_ptr_[2] << 16) | (_ptr_[1] << 8) | _ptr_[0];   \
        _ptr_    += 4;                                                                  \
        _result_; })
#else
# define AM29_NEXT_DATUM_16(_ptr_)                  \
    ({                                              \
        cyg_uint16 _result_;                        \
        _result_  = (_ptr_[0] << 8) | _ptr_[1];     \
        _ptr_    += 2;                              \
        _result_; })

# define AM29_NEXT_DATUM_32(_ptr_)                                                      \
    ({                                                                                  \
        cyg_uint32 _result_;                                                            \
        _result_  = (_ptr_[0] << 24) | (_ptr_[1] << 16) | (_ptr_[2] << 8) | _ptr_[3];   \
        _ptr_    += 4;                                                                  \
        _result_; })

#endif

// The addresses used for programming the flash may be different from
// the ones used to read the flash. The macro
// HAL_AM29XXXXX_UNCACHED_ADDRESS() can be supplied by one of the HAL
// packages. Otherwise if CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CACHED_ONLY
// is not implemented then the macro CYGARC_UNCACHED_ADDRESS()
// will be used. If there is no way of bypassing the cache then
// the addresses will remain unchanged and instead the INTSCACHE
// macros will disable the cache.

#if defined(HAL_AM29XXXXX_UNCACHED_ADDRESS)
# define AM29_UNCACHED_ADDRESS(_addr_)  ((volatile AM29_TYPE*)HAL_AM29XXXXX_UNCACHED_ADDRESS(_addr_))
#elif !defined(CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CACHED_ONLY)
# ifndef CYGARC_UNCACHED_ADDRESS
#  error Cache should be bypassed but CYGARC_UNCACHED_ADDRESS is not defined.
# endif
# define AM29_UNCACHED_ADDRESS(_addr_)  ((volatile AM29_TYPE*)CYGARC_UNCACHED_ADDRESS(_addr_))
#elif defined(HAL_AM29XXXXX_P2V)
// HAL_AM29XXXXX_P2V is a deprecated macro that is only retained for
// backward compatibility.
# define AM29_UNCACHED_ADDRESS(_addr_)  ((volatile AM29_TYPE*)HAL_AM29XXXXX_P2V(_addr_))
#else
# define AM29_UNCACHED_ADDRESS(_addr_)  ((volatile AM29_TYPE*)(_addr_))
#endif

// The bits on the data bus may need swapping, either because of
// endianness issues or because some lines are just wired wrong.
// SWAP is for commands going to the flash chip. UNSWAP is for
// data coming back from the flash chip. The swapping takes
// effect after allowing for AM29_PARALLEL(). Data is never
// swapped, it does not matter if bit 5 of a datum is actually
// stored in bit 3 of the flash as long as the data reads back
// right.
#if defined(HAL_AM29XXXXX_SWAP)
# define AM29_SWAP(_data_)      HAL_AM29XXXXX_SWAP(_data_)
#else
# define AM29_SWAP(_data_)      (_data_)
#endif
#if defined(HAL_AM29XXXXX_UNSWAP)
# define AM29_UNSWAP(_data_)    HAL_AM29XXXXX_UNSWAP(_data_)
#else
# define AM29_UNSWAP(_data_)    (_data_)
#endif

// On some platforms there may be almost inexplicable failures, caused
// by very subtle effects such as instruction cache lines still being
// filled from flash memory which the _hw routines in .2ram sections are
// already running and have taken the flash out of read-array mode.
// These are very rare effects and not amenable to a generic solution,
// so instead the platform HAL (usually) can define additional hook
// macros that get invoked by the .2ram functions. These can e.g.
// add a short delay or invalidate a couple of instruction cache lines,
// but only if the code is executing from flash. Any such hooks will
// affect interrupt latency so should only be used when absolutely
// necessary. They must also be simple code, e.g. no calls to other
// functions that may be in flash.

#ifdef HAL_AM29XXXXX_2RAM_ENTRY_HOOK
# define AM29_2RAM_ENTRY_HOOK() HAL_AM29XXXXX_2RAM_ENTRY_HOOK()
#else
# define AM29_2RAM_ENTRY_HOOK() CYG_EMPTY_STATEMENT
#endif
#ifdef HAL_AM29XXXXX_2RAM_EXIT_HOOK
# define AM29_2RAM_EXIT_HOOK()  HAL_AM29XXXXX_2RAM_EXIT_HOOK()
#else
# define AM29_2RAM_EXIT_HOOK()  CYG_EMPTY_STATEMENT
#endif

// Cache and interrupt manipulation. This driver supports fine-grained
// control over interrupts and the cache, using three macros. These may
// be provided by the platform HAL, or by defaults here. There are
// three variants:
//
// 1) control both interrupts and cache. This is necessary if
//    CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CACHED_ONLY is implemented,
//    i.e. if the cache cannot be bypassed. The cache must be temporarily
//    disabled for flash operations, and interrupts have to be disabled
//    while the cache is disabled to prevent interrupts and context switches.
// 2) control interrupts only, the default if the cache can be bypassed
//    when accessing the flash. The flash is still in an unusable
//    state during flash operations so interrupts and context switches
//    should be avoided.
// 3) only invalidate at the end, if the cache can be bypassed and the
//    application guarantees that the flash will not be accessed by any interrupt
//    handlers or other threads.

#if defined(CYGHWR_DEVS_FLASH_AMD_AM29XXXXX_V2_CACHED_ONLY)

// First, the amount of state that should be preserved. By default
// this means the interrupt state and the data cache state.
# define AM29_INTSCACHE_DEFAULT_STATE   int _saved_ints_, _saved_dcache_

// Start an operation on the flash. Make sure that interrupts are
// disabled and then save the current state of the data cache. The
// actual flash manipulation should happen with the cache disabled.
// There may still be data in the cache that has not yet been flushed
// to memory, so take care of that first. The invalidate the cache
// lines so that when the cache is re-enabled later on the processor
// gets everything from memory, rather than reusing old data in the
// cache.
# define AM29_INTSCACHE_DEFAULT_BEGIN()         \
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
#define AM29_INTSCACHE_DEFAULT_END()            \
    CYG_MACRO_START                             \
    if (_saved_dcache_) {                       \
        HAL_DCACHE_ENABLE();                    \
    }                                           \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END

#elif !defined(CYGIMP_DEVS_FLASH_AMD_AM29XXXXX_V2_LEAVE_INTERRUPTS_ENABLED)

# define AM29_INTSCACHE_DEFAULT_STATE     int _saved_ints_
# define AM29_INTSCACHE_DEFAULT_BEGIN()   HAL_DISABLE_INTERRUPTS(_saved_ints_)

# if defined(HAL_DCACHE_SYNC) && defined(HAL_DCACHE_INVALIDATE_ALL)
// The following blips the interrupt enable to allow pending interrupts
// to run, which will reduce interrupt latency given the dcache sync/invalidate
// may be relatively lengthy.
#  define AM29_INTSCACHE_DEFAULT_END()          \
    CYG_MACRO_START                             \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    HAL_DISABLE_INTERRUPTS(_saved_ints_);       \
    HAL_DCACHE_SYNC();                          \
    HAL_DCACHE_INVALIDATE_ALL();                \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END
# else
#  define AM29_INTSCACHE_DEFAULT_END()    HAL_RESTORE_INTERRUPTS(_saved_ints_)
# endif
#else

# define AM29_INTSCACHE_DEFAULT_STATE     CYG_EMPTY_STATEMENT
# define AM29_INTSCACHE_DEFAULT_BEGIN()   CYG_EMPTY_STATEMENT
# if defined(HAL_DCACHE_SYNC) && defined(HAL_DCACHE_INVALIDATE_ALL)
#  define AM29_INTSCACHE_DEFAULT_END()          \
    CYG_MACRO_START                             \
    int _saved_ints_;                           \
    HAL_DISABLE_INTERRUPTS(_saved_ints_);       \
    HAL_DCACHE_SYNC();                          \
    HAL_DCACHE_INVALIDATE_ALL();                \
    HAL_RESTORE_INTERRUPTS(_saved_ints_);       \
    CYG_MACRO_END
# else
#  define AM29_INTSCACHE_DEFAULT_END()    CYG_EMPTY_STATEMENT
# endif
#endif

#ifdef HAL_AM29XXXXX_INTSCACHE_STATE
# define AM29_INTSCACHE_STATE       HAL_AM29XXXXX_INTSCACHE_STATE
#else
# define AM29_INTSCACHE_STATE       AM29_INTSCACHE_DEFAULT_STATE
#endif
#ifdef HAL_AM29XXXXX_INTSCACHE_BEGIN
# define AM29_INTSCACHE_BEGIN       HAL_AM29XXXXX_INTSCACHE_BEGIN
#else
# define AM29_INTSCACHE_BEGIN       AM29_INTSCACHE_DEFAULT_BEGIN
#endif
#ifdef HAL_AM29XXXXX_INTSCACHE_END
# define AM29_INTSCACHE_END         HAL_AM29XXXXX_INTSCACHE_END
#else
# define AM29_INTSCACHE_END         AM29_INTSCACHE_DEFAULT_END
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
am29_get_block_info(struct cyg_flash_dev* dev, const cyg_flashaddr_t addr, cyg_flashaddr_t* block_start, size_t* block_size)
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
#define AM29_SUFFIX             8
#define AM29_TYPE               cyg_uint8
#define AM29_DEVCOUNT           1
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_8(_ptr_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// A single 16-bit device.
#define AM29_SUFFIX             16
#define AM29_TYPE               cyg_uint16
#define AM29_DEVCOUNT           1
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_16(_ptr_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// A single 32-bit device.
#define AM29_SUFFIX             32
#define AM29_TYPE               cyg_uint32
#define AM29_DEVCOUNT           1
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_32(_ptr_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// Two 8-bit devices, giving a 16-bit bus. 
#define AM29_SUFFIX             88
#define AM29_TYPE               cyg_uint16
#define AM29_DEVCOUNT           2
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_16(_ptr_)
#define AM29_PARALLEL(_cmd_)     ((_cmd_ << 8) | _cmd_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// Four 8-bit devices, giving a 32-bit bus. 
#define AM29_SUFFIX             8888
#define AM29_TYPE               cyg_uint32
#define AM29_DEVCOUNT           4
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_32(_ptr_)
#define AM29_PARALLEL(_cmd_)    ((_cmd_ << 24) | (_cmd_ << 16) | (_cmd_ << 8) | _cmd_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// Two 16-bit devices, giving a 32-bit bus.
#define AM29_SUFFIX             1616
#define AM29_TYPE               cyg_uint32
#define AM29_DEVCOUNT           2
#define AM29_NEXT_DATUM(_ptr_)  AM29_NEXT_DATUM_32(_ptr_)
#define AM29_PARALLEL(_cmd_)    ((_cmd_ << 16) | _cmd_)

#include "am29xxxxx_aux.c"

#undef AM29_SUFFIX
#undef AM29_TYPE
#undef AM29_DEVCOUNT
#undef AM29_NEXT_DATUM

// 16AS8. A 16-bit device hooked up so that only byte accesses are
// allowed. This requires unusual offsets
#define AM29_SUFFIX                 16as8
#define AM29_TYPE                   cyg_uint8
#define AM29_DEVCOUNT               1
#define AM29_NEXT_DATUM(_ptr_)      AM29_NEXT_DATUM_8(_ptr_)
#define AM29_OFFSET_COMMAND         0x0AAA
#define AM29_OFFSET_COMMAND2        0x0555
#define AM29_OFFSET_MANUFACTURER_ID 0x0000
#define AM29_OFFSET_DEVID           0x0002
#define AM29_OFFSET_DEVID2          0x001C
#define AM29_OFFSET_DEVID3          0x001E
#define AM29_OFFSET_AT49_LOCK_STATUS 04
#define AM29_OFFSET_CFI             0x00AA
#define AM29_OFFSET_CFI_DATA(_idx_) (2 * (_idx_))

#include "am29xxxxx_aux.c"
