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
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
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
// Author(s):   larsi
// Contributors:
// Date:        2008-07-09
// Purpose:     Cache control API
// Description: The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:       #include <cyg/hal/hal_cache.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_cache.h>

//-----------------------------------------------------------------------------
// Cache dimensions

// FR30 only has an instruction cache, so data cache macros are empty
#ifndef HAL_DCACHE_SIZES_DEFINED
#define HAL_DCACHE_SIZE                 0       // Size of cache in bytes
#define HAL_DCACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#endif

#ifndef HAL_ICACHE_SIZES_DEFINED
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache

#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))
#endif
//-----------------------------------------------------------------------------
// Global control of the data cache
// FR30 only has an instruction cache, so data cache macros are empty

// Enable the cache
#ifndef HAL_DCACHE_ENABLE_DEFINED
#define HAL_DCACHE_ENABLE()
#endif

// Disable the cache
#ifndef HAL_DCACHE_DISABLE_DEFINED
#define HAL_DCACHE_DISABLE()
#endif

// Query the state of the cache
#ifndef HAL_DCACHE_IS_ENABLED_DEFINED
#define HAL_DCACHE_IS_ENABLED(_state_) \
CYG_MACRO_START                        \
_state_ = 1;	                       \
CYG_MACRO_END
#endif

// Invalidate the entire cache
#ifndef HAL_DCACHE_INVALIDATE_ALL_DEFINED
#define HAL_DCACHE_INVALIDATE_ALL()
#endif

// Synchronize the contents of the cache to the memory
#ifndef HAL_DCACHE_SYNC_DEFINED
#define HAL_DCACHE_SYNC()
#endif

// Locks entry in the cache
#ifndef HAL_DCACHE_LOCK_DEFINED
#define HAL_DCACHE_LOCK(_base_, _size_)
#endif

// Unlocks entry in the cache
#ifndef HAL_DCACHE_UNLOCK_DEFINED
#define HAL_DCACHE_UNLOCK(_base_, _size_)
#endif

// Unlocks the whole cache.
#ifndef HAL_DCACHE_UNLOCK_ALL_DEFINED
#define HAL_DCACHE_UNLOCK_ALL()
#endif

//-----------------------------------------------------------------------------
// Global control of the instruction cache

// Enable the cache
#ifndef HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE()
#endif

// Disable the cache
#ifndef HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE()
#endif

// Query the state of the cache
#ifndef HAL_ICACHE_IS_ENABLED_DEFINED
#define HAL_ICACHE_IS_ENABLED(_state_) \
CYG_MACRO_START                        \
_state_ = 1;                           \
CYG_MACRO_END
#endif

// Invalidate the entire cache
#ifndef HAL_ICACHE_INVALIDATE_ALL_DEFINED
#define HAL_ICACHE_INVALIDATE_ALL()
#endif

// Synchronize the contents of the cache to the memory.
#ifndef HAL_ICACHE_SYNC_DEFINED
#define HAL_ICACHE_SYNC()
#endif

// Locks entry in the cache, fr30 architecture does not support locking specifc
// regions, so _base_ and _size are ignored and the whole cache is locked.
// During cache lock only valid entries are locked. Invalid entries are updated.
#ifndef HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_LOCK(_base_, _size_)
#endif

// Unlocks entry in the cache. fr30 architecture does not support locking specifc
// regions, so _base_ and _size are ignored and the whole cache is unlocked.
#ifndef HAL_ICACHE_UNLOCK_DEFINED
#define HAL_ICACHE_UNLOCK(_base_, _size_)
#endif

// Unlocks the whole cache.
#ifndef HAL_ICACHE_UNLOCK_ALL_DEFINED
#define HAL_ICACHE_UNLOCK_ALL()
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
