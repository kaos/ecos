#ifndef CYGONCE_HAL_CACHE_H
#define CYGONCE_HAL_CACHE_H
//=============================================================================
//
//      hal_cache.h
//
//      HAL cache control API
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2012 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour, ilijak
// Contributors:
// Date:         2012-05-02
// Purpose:      Cache control API
// Description:  The macros defined here provide the HAL APIs for handling
//               cache control operations.
//
// Usage:
//               #include <cyg/hal/hal_cache.h>
//               ...
//
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Cache dimensions
#ifdef CYGINT_HAL_CACHE

// Data cache
#define HAL_DCACHE_SIZE                 (8192*2) // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16       // Size of a data cache line
#define HAL_DCACHE_WAYS                 (2*2)      // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 (8192*2)   // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16       // Size of a cache line
#define HAL_ICACHE_WAYS                 (2*2)      // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

#include <cyg/hal/var_io_lmem.h>

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE() \
    HAL_CORTEXM_KINETIS_CACHE_PS_ENABLE(); \
    HAL_CORTEXM_KINETIS_CACHE_PC_ENABLE()

// Disable the data cache
#define HAL_DCACHE_DISABLE() \
    HAL_CORTEXM_KINETIS_CACHE_PS_DISABLE(); \
    HAL_CORTEXM_KINETIS_CACHE_PC_DISABLE()

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL() \
    HAL_CORTEXM_KINETIS_CACHE_PS_INVALL(); \
    HAL_CORTEXM_KINETIS_CACHE_PC_INVALL()

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC() \
    HAL_CORTEXM_KINETIS_CACHE_PS_SYNC(); \
    HAL_CORTEXM_KINETIS_CACHE_PC_SYNC()

// Purge contents of data cache
#define HAL_DCACHE_PURGE_ALL() \
    HAL_CORTEXM_KINETIS_CACHE_PS_CLEAR(); \
    HAL_CORTEXM_KINETIS_CACHE_PC_CLEAR()

// Query the state of the data cache (does not affect the caching)
#define HAL_DCACHE_IS_ENABLED(_state_)          \
    CYG_MACRO_START                             \
    (_state_) = (HAL_CORTEXM_KINETIS_CACHE_PS_IS_ENABLED() && \
                HAL_CORTEXM_KINETIS_CACHE_PC_IS_ENABLED()); \
    CYG_MACRO_END

// Invalidate cache lines in the given range without writing to memory.
#define HAL_DCACHE_INVALIDATE( _base_ , _size_ ) \
            HAL_CORTEXM_KINETIS_CACHE_PS_INVALIDATE(_base_, _size_); \
            HAL_CORTEXM_KINETIS_CACHE_PC_INVALIDATE(_base_, _size_)

#else // CYGINT_HAL_CACHE

// Data cache
//#define HAL_DCACHE_SIZE                 0     // Size of data cache in bytes
//#define HAL_DCACHE_LINE_SIZE            0     // Size of a data cache line
//#define HAL_DCACHE_WAYS                 0      // Associativity of the cache

// Instruction cache
//#define HAL_ICACHE_SIZE                 0     // Size of cache in bytes
//#define HAL_ICACHE_LINE_SIZE            0      // Size of a cache line
//#define HAL_ICACHE_WAYS                 0      // Associativity of the cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()

// Disable the data cache
#define HAL_DCACHE_DISABLE()

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()

// Purge contents of data cache
#define HAL_DCACHE_PURGE_ALL()

// Query the state of the data cache (does not affect the caching)
#define HAL_DCACHE_IS_ENABLED(_state_)          \
    CYG_MACRO_START                             \
    (_state_) = 0; \
    CYG_MACRO_END

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
#define HAL_DCACHE_INVALIDATE( _base_ , _size_ )

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

#endif // CYGINT_HAL_CACHE

//-----------------------------------------------------------------------------
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_ICACHE_ENABLE() HAL_DCACHE_ENABLE()

// Disable the instruction cache
#define HAL_ICACHE_DISABLE() HAL_DCACHE_DISABLE()

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL() HAL_DCACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC() HAL_DCACHE_SYNC()

// Query the state of the instruction cache (does not affect the caching)
#define HAL_ICACHE_IS_ENABLED(_state_) HAL_DCACHE_IS_ENABLED(_state_)

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
#define HAL_ICACHE_INVALIDATE( _base_ , _size_ ) HAL_DCACHE_INVALIDATE( _base_ , _size_ )

//-----------------------------------------------------------------------------
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_UCACHE_ENABLE() HAL_DCACHE_ENABLE()

// Disable the instruction cache
#define HAL_UCACHE_DISABLE() HAL_DCACHE_DISABLE()

// Invalidate the entire cache
#define HAL_UCACHE_INVALIDATE_ALL() HAL_DCACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_UCACHE_SYNC() HAL_DCACHE_SYNC()

// Query the state of the instruction cache (does not affect the caching)
#define HAL_UCACHE_IS_ENABLED(_state_) HAL_DCACHE_IS_ENABLED(_state_)

// Set the instruction cache refill burst size
//#define HAL_UCACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
//#define HAL_UCACHE_LOCK(_base_, _size_)

// Undo a previous lock operation
//#define HAL_UCACHE_UNLOCK(_base_, _size_)

// Unlock entire cache
//#define HAL_UCACHE_UNLOCK_ALL()

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
#define HAL_UCACHE_INVALIDATE( _base_ , _size_ ) HAL_DCACHE_INVALIDATE( _base_ , _size_ )

#endif // CYGONCE_HAL_CACHE_H

//-----------------------------------------------------------------------------
// End of hal_cache.h
