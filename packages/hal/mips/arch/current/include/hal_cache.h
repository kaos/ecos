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
// MIPS simulator

#ifdef CYG_HAL_MIPS_SIM

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

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1

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
// Toshiba TX3904

#ifdef CYG_HAL_MIPS_TX3904

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
#define HAL_DCACHE_DISABLE()                    \
{                                               \
    asm volatile ("mfc0 $2,$3;"                  \
                  "la   $3,0xFFFFFFEF;"         \
                  "and  $2,$2,$3;"              \
                  "mtc0 $2,$3;"                  \
                  :                             \
                  :                             \
                  : "$2", "$3"                  \
                 );                             \
                                                \
}

// Invalidate the entire cache
// We do this by reading a word from each line to invalidate the current
// cache contents.
#define HAL_DCACHE_INVALIDATE_ALL()                                             \
{                                                                               \
    extern char __ram_data_start;                                               \
    CYG_WORD *addr = (CYG_WORD *)&__ram_data_start;                             \
    CYG_WORD sum = 0;                                                           \
    int i;                                                                      \
    for( i = 0; i < HAL_DCACHE_SIZE; i += HAL_DCACHE_LINE_SIZE )                \
    {                                                                           \
        sum += addr[i];                                                         \
    }                                                                           \
}

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC() HAL_DCACHE_INVALIDATE_ALL()

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
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

//-----------------------------------------------------------------------------
// Data cache line control

// Allocate cache lines for the given address range without reading its
// contents from memory.
//#define HAL_DCACHE_ALLOCATE( _base_ , _size_ )

// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
//#define HAL_DCACHE_FLUSH( _base_ , _size_ )

// Invalidate cache lines in the given range without writing to memory.
#define HAL_DCACHE_INVALIDATE( _base_ , _asize_ )                       \
{                                                                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                \
    register CYG_WORD _size_ = (_asize_);                               \
    HAL_DCACHE_DISABLE();                                               \
    for( ; _addr_ <= _addr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )    \
    {                                                                   \
        asm volatile ("cache 17,0(%0)" : : "r"(addr) );                 \
    }                                                                   \
    HAL_DCACHE_ENABLE();                                                \
}

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

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()                                             \
{                                                                               \
    register CYG_ADDRESS addr;                                                  \
    HAL_ICACHE_DISABLE();                                                       \
    for( addr = 0; addr < HAL_ICACHE_SIZE; addr += HAL_ICACHE_LINE_SIZE )       \
    {                                                                           \
        asm volatile ("cache 0,0(%0)" : : "r"(addr) );                          \
    }                                                                           \
    HAL_ICACHE_ENABLE();                                                        \
}

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC() HAL_ICACHE_INVALIDATE_ALL()

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
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

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
#define HAL_ICACHE_INVALIDATE( _base_ , _asize_ )                       \
{                                                                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                \
    register CYG_WORD _size_ = (_asize_);                               \
    HAL_ICACHE_DISABLE();                                               \
    for( ; _addr_ <= _addr_+_size_; _addr_ += HAL_ICACHE_LINE_SIZE )    \
    {                                                                   \
        asm volatile ("cache 0,0(%0)" : : "r"(addr) );                  \
    }                                                                   \
    HAL_ICACHE_ENABLE();                                                \
}

#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
