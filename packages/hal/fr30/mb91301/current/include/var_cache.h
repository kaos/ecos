#ifndef CYGONCE_VAR_CACHE_H
#define CYGONCE_VAR_CACHE_H
//=============================================================================
//
//      var_cache.h
//
//      VAR cache control API
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):   Lars Poeschel
// Contributors:
// Date:        2008-07-09
// Purpose:     Cache control API for mb91301 variant
// Description: The macros defined here provide the HAL APIs for handling
//              cache control operations.
// Usage:       Included via "hal_cache.h". Do not use directly.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/plf_cache.h>

//-----------------------------------------------------------------------------
// Cache configuration registers
#define CYG_HAL_FR30_CACHE_ISIZE    0x307
#define CYG_HAL_FR30_CACHE_ICHCR    0x3e7

#define CYG_HAL_FR30_CACHE_ICHCR_RAM  0x80
#define CYG_HAL_FR30_CACHE_ICHCR_GBLK 0x20
#define CYG_HAL_FR30_CACHE_ICHCR_ALFL 0x10
#define CYG_HAL_FR30_CACHE_ICHCR_EOLK 0x08
#define CYG_HAL_FR30_CACHE_ICHCR_ELKR 0x04
#define CYG_HAL_FR30_CACHE_ICHCR_FLSH 0x02
#define CYG_HAL_FR30_CACHE_ICHCR_ENAB 0x01

//-----------------------------------------------------------------------------
// Cache dimensions

// Unified cache (in reality it is only an instruction cache)
#define HAL_ICACHE_SIZES_DEFINED
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache

#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of the cache

// Enable the cache
// This is setting the RAM, ELKR, FLSH and ENAB bits in ICHCR
#ifndef HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE()                                         \
CYG_MACRO_START                                                     \
  HAL_WRITE_UINT8(CYG_HAL_FR30_CACHE_ICHCR,                         \
                  CYG_HAL_FR30_CACHE_ICHCR_RAM |                    \
                  CYG_HAL_FR30_CACHE_ICHCR_ELKR |                   \
                  CYG_HAL_FR30_CACHE_ICHCR_FLSH |                   \
                  CYG_HAL_FR30_CACHE_ICHCR_ENAB)                    \
CYG_MACRO_END
#endif

// Disable the cache
// This is clearing the ENAB bit in ICHCR
#ifndef HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE()                                        \
CYG_MACRO_START                                                     \
    HAL_WRITE_UINT8(CYG_HAL_FR30_CACHE_ICHCR,                       \
                    CYG_HAL_FR30_CACHE_ICHCR_RAM |                  \
                    CYG_HAL_FR30_CACHE_ICHCR_FLSH)                  \
CYG_MACRO_END
#endif

// Query the state of the cache
// read ICHCR, when enabled bit0 should be 1, else 0
#ifndef HAL_ICACHE_IS_ENABLED_DEFINED
#define HAL_ICACHE_IS_ENABLED_DEFINED
#define HAL_ICACHE_IS_ENABLED(_state_)                      \
CYG_MACRO_START                                             \
    HAL_READ_UINT8(CYG_HAL_FR30_CACHE_ICHCR, _state_)       \
    _state_ &= 0x01;                                        \
CYG_MACRO_END
#endif

// Invalidate the entire cache
// The processor manual is not clear about this. I clear the valid bit
// and the subblock valid bits of each subblock in all 128 tags of all
// 2 ways.
#ifndef HAL_ICACHE_INVALIDATE_ALL_DEFINED
#define HAL_ICACHE_INVALIDATE_ALL_DEFINED
#define HAL_ICACHE_INVALIDATE_ALL()                         \
CYG_MACRO_START                                             \
int _i_;                                                    \
for(_i_ = 0x10000; _i_ < 0x10800; _i_ += 0x10) {            \
    HAL_WRITE_UINT32(_i_, 0x00000000);                      \
    HAL_WRITE_UINT32(_i_ + 0x4000, 0x00000000);             \
}                                                           \
CYG_MACRO_END
#endif

// Synchronize the contents of the cache to the memory.(flush)
#ifndef HAL_ICACHE_SYNC_DEFINED
#define HAL_ICACHE_SYNC_DEFINED
#define HAL_ICACHE_SYNC()                                   \
CYG_MACRO_START                                             \
cyg_uint8 _value_;                                          \
HAL_READ_UINT8(CYG_HAL_FR30_CACHE_ICHCR, _value_);          \
HAL_WRITE_UINT8(CYG_HAL_FR30_CACHE_ICHCR,                   \
                _value_ | CYG_HAL_FR30_CACHE_ICHCR_FLSH);   \
CYG_MACRO_END
#endif

// Locks entry in the cache, fr30 architecture does not support
// locking specific regions. It is only possible to lock specific
// entries in the cache. This seems not to match eCos understanding of
// cache locking, so _base_ and _size_ are ignored and the whole cache
// is locked. During cache lock only valid entries are locked. Invalid
// entries are updated.
#ifndef HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_LOCK(_base_, _size_)                     \
CYG_MACRO_START                                             \
CYG_UINT8 _value_;                                          \
HAL_READ_UINT8(CYG_HAL_FR30_CACHE_ICHCR, _value_);          \
HAL_WRITE_UINT8(CYG_HAL_FR30_CACHE_ICHCR,                   \
                _value_ | CYG_HAL_FR30_CACHE_ICHCR_GBLK);   \
CYG_MACRO_END
#endif

// Unlocks entry in the cache. fr30 architecture does not support
// locking specific regions, so _base_ and _size are ignored and the
// whole cache is unlocked.
#ifndef HAL_ICACHE_UNLOCK_DEFINED
#define HAL_ICACHE_UNLOCK_DEFINED
#define HAL_ICACHE_UNLOCK(_base_, _size_)                   \
CYG_MACRO_START                                             \
cyg_uint8 _value_;                                          \
HAL_READ_UINT8(CYG_HAL_FR30_CACHE_ICHCR, _value_);          \
HAL_WRITE_UINT8(CYG_HAL_FR30_CACHE_ICHCR,                   \
                _value_ & ~CYG_HAL_FR30_CACHE_ICHCR_GBLK);  \
CYG_MACRO_END
#endif

// Unlocks the whole cache. This is the same as HAL_ICACHE_UNLOCK
#ifndef HAL_ICACHE_UNLOCK_ALL_DEFINED
#define HAL_ICACHE_UNLOCK_ALL_DEFINED
#define HAL_ICACHE_UNLOCK_ALL()     HAL_ICACHE_UNLOCK(0, 0)
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_CACHE_H
// End of var_cache.h
