#ifndef CYGONCE_IMP_CACHE_H
#define CYGONCE_IMP_CACHE_H

//=============================================================================
//
//      imp_cache.h
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-02-17
// Purpose:     Cache control API
// Description: The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:
//              #include <cyg/hal/imp_cache.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/plf_cache.h>

//=============================================================================
// Toshiba TX3904

#ifdef CYGPKG_HAL_MIPS_TX3904

//-----------------------------------------------------------------------------
// Cache dimensions

// Data cache
#define HAL_DCACHE_SIZE                 1024    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            4       // Size of a data cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 1       // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
// This uses a bit in the config register, which is TX39 specific.
#define HAL_DCACHE_ENABLE_DEFINED
#define HAL_DCACHE_ENABLE()                     \
{                                               \
    asm volatile ("mfc0 $2,$3;"                 \
                  "ori  $2,$2,0x0010;"          \
                  "mtc0 $2,$3;"                 \
                  :                             \
                  :                             \
                  : "$2"                        \
                 );                             \
                                                \
}

// Disable the data cache
#define HAL_DCACHE_DISABLE_DEFINED
#define HAL_DCACHE_DISABLE()                    \
{                                               \
    asm volatile ("mfc0 $2,$3;"                 \
                  "la   $3,0xFFFFFFEF;"         \
                  "and  $2,$2,$3;"              \
                  "mtc0 $2,$3;"                 \
                  :                             \
                  :                             \
                  : "$2", "$3"                  \
                 );                             \
                                                \
}


// Invalidate the entire cache
// The TX39 only has hit-invalidate on the DCACHE, not
// index-invalidate, so we cannot just empty the cache out without
// knowing what is in it. This is annoying. So, the best we can do is
// fill the cache with data that is unlikely to be there
// otherwise. Hence we read bytes from the ROM space since this is
// most likely to be code, and will not get out of sync even if it is not.
#define HAL_DCACHE_INVALIDATE_ALL_DEFINED
#define HAL_DCACHE_INVALIDATE_ALL()                                             \
{                                                                               \
    CYG_BYTE volatile *addr = (CYG_BYTE *)(0x9fc00000);                         \
    CYG_BYTE volatile tmp = 0;                                                  \
    int i;                                                                      \
    for( i = 0; i < (HAL_DCACHE_SIZE*2); i += HAL_DCACHE_LINE_SIZE )            \
    {                                                                           \
        tmp = addr[i];                                                          \
    }                                                                           \
}

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC_DEFINED
#define HAL_DCACHE_SYNC() HAL_DCACHE_INVALIDATE_ALL()

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
#define HAL_DCACHE_LOCK_DEFINED
#define HAL_DCACHE_LOCK(_base_, _size_)         \
{                                               \
    asm volatile ("mfc0 $2,$7;"                 \
                  "ori  $2,$2,0x0100;"          \
                  "mtc0 $2,$7;"                 \
                  :                             \
                  :                             \
                  : "$2"                        \
                 );                             \
}

// Undo a previous lock operation
#define HAL_DCACHE_UNLOCK_DEFINED
#define HAL_DCACHE_UNLOCK(_base_, _size_)       \
{                                               \
    asm volatile ("mfc0 $2,$7;"                 \
                  "la   $3,0xFFFFFEFF;"         \
                  "and  $2,$2,$3;"              \
                  "mtc0 $2,$7;"                 \
                  :                             \
                  :                             \
                  : "$2", "$3"                  \
                 );                             \
}

// Unlock entire cache
#define HAL_DCACHE_UNLOCK_ALL_DEFINED
#define HAL_DCACHE_UNLOCK_ALL() HAL_DCACHE_UNLOCK(0,HAL_DCACHE_SIZE)


//-----------------------------------------------------------------------------
// Data cache line control

// Allocate cache lines for the given address range without reading its
// contents from memory.
//#define HAL_DCACHE_ALLOCATE( _base_ , _size_ )

// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
//#define HAL_DCACHE_FLUSH( _base_ , _size_ )
#define HAL_DCACHE_FLUSH_DEFINED        // Ensure no default definition

// Write dirty cache lines to memory for the given address range.
//#define HAL_DCACHE_STORE( _base_ , _size_ )
#define HAL_DCACHE_STORE_DEFINED        // Disable default definition

//-----------------------------------------------------------------------------
// Global control of Instruction cache

// Enable the instruction cache
// This uses a bit in the config register, which is TX39 specific.
#define HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE()                     \
{                                               \
    asm volatile ("mfc0 $2,$3;"                 \
                  "ori  $2,$2,0x0020;"          \
                  "mtc0 $2,$3;"                 \
                  :                             \
                  :                             \
                  : "$2"                        \
                 );                             \
                                                \
}

// Disable the instruction cache
#define HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE()                    \
{                                               \
    asm volatile ("mfc0 $2,$3;"                 \
                  "la   $3,0xFFFFFFDF;"         \
                  "and  $2,$2,$3;"              \
                  "mtc0 $2,$3;"                 \
                  :                             \
                  :                             \
                  : "$2", "$3"                  \
                 );                             \
                                                \
}

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
#define HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_LOCK(_base_, _size_)         \
{                                               \
    asm volatile ("mfc0 $2,$7;"                 \
                  "ori  $2,$2,0x0200;"          \
                  "mtc0 $2,$7;"                 \
                  :                             \
                  :                             \
                  : "$2"                        \
                 );                             \
}

// Undo a previous lock operation
#define HAL_ICACHE_UNLOCK_DEFINED
#define HAL_ICACHE_UNLOCK(_base_, _size_)       \
{                                               \
    asm volatile ("mfc0 $2,$7;"                 \
                  "la   $3,0xFFFFFDFF;"         \
                  "and  $2,$2,$3;"              \
                  "mtc0 $2,$7;"                 \
                  :                             \
                  :                             \
                  : "$2", "$3"                  \
                 );                             \
}

// Unlock entire cache
#define HAL_ICACHE_UNLOCK_ALL_DEFINED
#define HAL_ICACHE_UNLOCK_ALL() HAL_ICACHE_UNLOCK(0, HAL_ICACHE_SIZE)

//-----------------------------------------------------------------------------
// Instruction cache line control
// On the TX39, the instruction cache must be disabled to use the index-invalidate
// cache operation.

// Invalidate the entire cache
// This uses the index-invalidate cache operation.
#define HAL_ICACHE_INVALIDATE_ALL_DEFINED
#define HAL_ICACHE_INVALIDATE_ALL()                                             \
{                                                                               \
    register CYG_ADDRESS _baddr_ = 0x80000000;                                  \
    register CYG_ADDRESS _addr_ = 0x80000000;                                   \
    register CYG_WORD _state_;                                                  \
    HAL_ICACHE_IS_ENABLED(_state_);                                             \
    HAL_ICACHE_DISABLE();                                                       \
    for( ; _addr_ < _baddr_+HAL_ICACHE_SIZE; _addr_ += HAL_ICACHE_LINE_SIZE )   \
    {                                                                           \
        asm volatile ("cache 0x00,0(%0)" : : "r"(_addr_) );                     \
    }                                                                           \
    if( _state_ ) HAL_ICACHE_ENABLE();                                          \
}

// Invalidate cache lines in the given range without writing to memory.
// This uses the index-invalidate cache operation since the TX39 does not
// have hit-invalidate on the instruction cache.
#define HAL_ICACHE_INVALIDATE_DEFINED
#define HAL_ICACHE_INVALIDATE( _base_ , _asize_ )                       \
{                                                                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                \
    register CYG_WORD _size_ = (_asize_);                               \
    register CYG_WORD _state_;                                          \
    HAL_ICACHE_IS_ENABLED(_state_);                                     \
    HAL_ICACHE_DISABLE();                                               \
    for( ; _addr_ <= _addr_+_size_; _addr_ += HAL_ICACHE_LINE_SIZE )    \
    {                                                                   \
        asm volatile ("cache 0,0(%0)" : : "r"(_addr_) );                \
    }                                                                   \
    if( _state_ ) HAL_ICACHE_ENABLE();                                  \
}

#endif // CYGPKG_HAL_MIPS_TX3904

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_IMP_CACHE_H
// End of imp_cache.h
