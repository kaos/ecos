#ifndef CYGONCE_HAL_CACHE_H
#define CYGONCE_HAL_CACHE_H

//=============================================================================
//
//      hal_cache.h
//
//      HAL cache control API
//
//=============================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, gthomas
// Contributors:        nickg, gthomas
// Date:        1998-09-28
// Purpose:     Cache control API
// Description: The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:
//              #include <cyg/hal/hal_cache.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// Cache dimensions

// Data cache
#define HAL_DCACHE_SIZE                 0x800    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            4        // Size of a data cache line
#define HAL_DCACHE_WAYS                 4        // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 0x800    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            4        // Size of a cache line
#define HAL_ICACHE_WAYS                 4        // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

// Cache & SRAM control
#define CYG_DEVICE_CCR          ((volatile cyg_uint8  *)0xFFFFA400)
#define CYG_DEVICE_LSCR         ((volatile cyg_uint8  *)0xFFFFA404)

#define CCR_E                   0x01      // Cache enabled
#define CCR_S                   0x02      // SRAM mode enabled
#define CCR_F                   0x04      // Flush mode enabled
#define CCR_I                   0x08      // Invalidate mode

#define LCSR_E                  0x01      // Local SRAM enabled
#define LCSR_L                  0x02      // Local SRAM mapped to high memory

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()                                     \
{                                                               \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, CCR_E);                    \
}

// Disable the data cache
#define HAL_DCACHE_DISABLE()                                    \
{                                                               \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, 0);                        \
}

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL()                             \
{                                                               \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, CCR_I);                    \
}

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()                                               \
{                                                                       \
    cyg_uint32 *cache_SRAM = (cyg_uint32 *)0x60000800;                  \
    int i;                                                              \
    cyg_uint32 old_ccr;                                                 \
    HAL_READ_UINT32(CYG_DEVICE_CCR, old_ccr);                           \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, CCR_F);                            \
    for (i = 0;  i < HAL_DCACHE_SETS;  i++) {                           \
        *cache_SRAM = 0;                                                \
        cache_SRAM += HAL_DCACHE_LINE_SIZE;                             \
    }                                                                   \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, CCR_I);                            \
    HAL_WRITE_UINT32(CYG_DEVICE_CCR, old_ccr);                          \
}

// Purge contents of data cache
#define HAL_DCACHE_PURGE_ALL()  HAL_DCACHE_INVALIDATE_ALL()

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
//#define HAL_DCACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_DCACHE_UNLOCK(_base_, _size_)

// Unlock entire cache
//#define HAL_DCACHE_UNLOCK_ALL()

//-----------------------------------------------------------------------------
// Data cache line control

// Allocate cache lines for the given address range without reading its
// contents from memory.
//#define HAL_DCACHE_ALLOCATE( _base_ , _size_ )

// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
//#define HAL_DCACHE_FLUSH( _base_ , _size_ )

// Invalidate cache lines in the given range without writing to memory.
//#define HAL_DCACHE_INVALIDATE( _base_ , _size_ )

// Write dirty cache lines to memory for the given address range.
//#define HAL_DCACHE_STORE( _base_ , _size_ )

// Preread the given range into the cache with the intention of reading
// from it later.
//#define HAL_DCACHE_READ_HINT( _base_ , _size_ )

// Preread the given range into the cache with the intention of writing
// to it later.
//#define HAL_DCACHE_WRITE_HINT( _base_ , _size_ )

// Allocate and zero the cache lines associated with the given range.
//#define HAL_DCACHE_ZERO( _base_ , _size_ )

//-----------------------------------------------------------------------------
// Global control of Instruction cache - use Data cache controls since they
// are not separatable.

// Enable the instruction cache
#define HAL_ICACHE_ENABLE()      HAL_DCACHE_ENABLE()

// Disable the instruction cache
#define HAL_ICACHE_DISABLE()     HAL_DCACHE_DISABLE()

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()  HAL_DCACHE_SYNC();  HAL_DCACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC()

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
//#define HAL_ICACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_ICACHE_UNLOCK(_base_, _size_)

// Unlock entire cache
//#define HAL_ICACHE_UNLOCK_ALL()

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
//#define HAL_ICACHE_INVALIDATE( _base_ , _size_ )

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
