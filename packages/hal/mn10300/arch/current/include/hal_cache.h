#ifndef CYGONCE_HAL_CACHE_H
#define CYGONCE_HAL_CACHE_H

//=============================================================================
//
//	hal_cache.h
//
//	HAL cache control API
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-02-17
// Purpose:	Cache control API
// Description:	The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:
//		#include <cyg/hal/hal_cache.h>
//		...
//		
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//=============================================================================
// MN10300 Simulator

#if defined(CYG_HAL_MN10300_SIM)

//-----------------------------------------------------------------------------
// Cache dimensions

// Data cache
#define HAL_DCACHE_SIZE                 4096    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16      // Size of a data cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()

// Disable the data cache
#define HAL_DCACHE_DISABLE()

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
//#define HAL_DCACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_DCACHE_UNLOCK(_base_, _size_)

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
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_ICACHE_ENABLE()

// Disable the instruction cache
#define HAL_ICACHE_DISABLE()

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC()

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
//#define HAL_ICACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_ICACHE_UNLOCK(_base_, _size_)

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
//#define HAL_ICACHE_INVALIDATE( _base_ , _size_ )

#endif


//=============================================================================
// MN103002 implementation

#if !defined(CYG_HAL_MN10300_SIM) && defined(CYG_HAL_MN10300_MN103002)

//-----------------------------------------------------------------------------
// Cache dimensions


// Data cache
#define HAL_DCACHE_SIZE                 4096    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16      // Size of a data cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Control registers

#define HAL_CHCTR               ((CYG_ADDRWORD *)0x20000070)

#define HAL_CHCTR_ICEN          0x0001
#define HAL_CHCTR_DCEN          0x0002
#define HAL_CHCTR_ICBUSY        0x0004
#define HAL_CHCTR_DCBUSY        0x0008
#define HAL_CHCTR_ICINV         0x0010
#define HAL_CHCTR_DCINV         0x0020
#define HAL_CHCTR_DCWTMD        0x0040
#define HAL_CHCTR_ICWMD         0x0300
#define HAL_CHCTR_DCWMD         0x3000

#define HAL_DCACHE_PURGE_WAY0   ((CYG_ADDRWORD *)0x28400000)
#define HAL_DCACHE_PURGE_WAY1   ((CYG_ADDRWORD *)0x28401000)

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()                     \
{                                               \
    CYG_ADDRWORD chctr = *HAL_CHCTR;            \
    chctr |= HAL_CHCTR_DCEN;                    \
    *HAL_CHCTR = chctr;                         \
}

// Disable the data cache
#define HAL_DCACHE_DISABLE()                    \
{                                               \
    CYG_ADDRWORD chctr = *HAL_CHCTR;            \
    chctr &= ~HAL_CHCTR_DCEN;                   \
    *HAL_CHCTR = chctr;                         \
}

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL()             \
{                                               \
    CYG_ADDRWORD chctr, chctr1;                 \
    chctr1 = *HAL_CHCTR;                        \
    HAL_DCACHE_DISABLE();                       \
    while( *HAL_CHCTR & HAL_CHCTR_DCBUSY );     \
    chctr = *HAL_CHCTR;                         \
    chctr |= HAL_CHCTR_DCINV;                   \
    *HAL_CHCTR = chctr;                         \
    while( *HAL_CHCTR & HAL_CHCTR_DCBUSY );     \
    *HAL_CHCTR = chctr1;                        \
}

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
#define HAL_DCACHE_WRITE_MODE( _mode_ )         \
{                                               \
    CYG_ADDRWORD chctr;                         \
    chctr = *HAL_CHCTR;                         \
    HAL_DCACHE_DISABLE();                       \
    while( *HAL_CHCTR & HAL_CHCTR_DCBUSY );     \
    chctr |= HAL_CHCTR_DCWTMD*(_mode_);         \
    *HAL_CHCTR = chctr;                         \
}

#define HAL_DCACHE_WRITEBACK_MODE       0
#define HAL_DCACHE_WRITETHRU_MODE       1

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
//#define HAL_DCACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_DCACHE_UNLOCK(_base_, _size_)

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
#define HAL_DCACHE_STORE( _base_ , _size_ )                     \
{                                                               \
    volatile CYG_ADDRWORD *way0 = HAL_DCACHE_PURGE_WAY0;        \
    volatile CYG_ADDRWORD *way1 = HAL_DCACHE_PURGE_WAY1;        \
    int i;                                                      \
    CYG_ADDRWORD chctr;                                         \
    chctr = *HAL_CHCTR;                                         \
    HAL_DCACHE_DISABLE();                                       \
    while( *HAL_CHCTR & HAL_CHCTR_DCBUSY );                     \
                                                                \
    way0 += ((CYG_ADDRWORD)_base_) & 0x000007f0;                \
    way1 += ((CYG_ADDRWORD)_base_) & 0x000007f0;                \
    for( i = 0; i < _size_ ; i += HAL_DCACHE_LINE_SIZE )        \
    {                                                           \
        *way0 = 0;                                              \
        *way1 = 0;                                              \
        way0 += HAL_DCACHE_LINE_SIZE;                           \
        way1 += HAL_DCACHE_LINE_SIZE;                           \
    }                                                           \
    *HAL_CHCTR = chctr;                                         \
}

// Preread the given range into the cache with the intention of reading
// from it later.
//#define HAL_DCACHE_READ_HINT( _base_ , _size_ )

// Preread the given range into the cache with the intention of writing
// to it later.
//#define HAL_DCACHE_WRITE_HINT( _base_ , _size_ )

// Allocate and zero the cache lines associated with the given range.
//#define HAL_DCACHE_ZERO( _base_ , _size_ )

//-----------------------------------------------------------------------------
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_ICACHE_ENABLE()                     \
{                                               \
    CYG_ADDRWORD chctr = *HAL_CHCTR;            \
    chctr |= HAL_CHCTR_ICEN;                    \
    *HAL_CHCTR = chctr;                         \
}

// Disable the instruction cache
#define HAL_ICACHE_DISABLE()                    \
{                                               \
    CYG_ADDRWORD chctr = *HAL_CHCTR;            \
    chctr &= ~HAL_CHCTR_ICEN;                   \
    *HAL_CHCTR = chctr;                         \
}

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()             \
{                                               \
    CYG_ADDRWORD chctr, chctr1;                 \
    chctr1 = *HAL_CHCTR;                        \
    HAL_ICACHE_DISABLE();                       \
    while( *HAL_CHCTR & HAL_CHCTR_ICBUSY );     \
    chctr = *HAL_CHCTR;                         \
    chctr |= HAL_CHCTR_ICINV;                   \
    *HAL_CHCTR = chctr;                         \
    while( *HAL_CHCTR & HAL_CHCTR_ICBUSY );     \
    *HAL_CHCTR = chctr1;                        \
}

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC()

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
//#define HAL_ICACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_ICACHE_UNLOCK(_base_, _size_)

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
//#define HAL_ICACHE_INVALIDATE( _base_ , _size_ )

#endif


//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
