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
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
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
//              #include <cyg/hal/hal_cache.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_cache.h>

// Use this macro to allow the assembler to accept "cache" instructions,
// which are MIPS ISA 3. This is useful if someone is compiling
// with -mips2, but the architecture is really MIPS ISA 3.

#define _HAL_ASM_SET_MIPS_ISA( _isal_ ) asm volatile ( ".set mips" ## #_isal_ )


//=============================================================================
// Default Implementation. This uses the standard MIPS CP0 registers and
// cache instructions. Note that not all variants will have all of the
// functionality defined here. 

//-----------------------------------------------------------------------------
// Cache dimensions.
// These really should be defined in var_cache.h. If they are not, then provide
// a set of numbers that are typical of many variants.

#ifndef HAL_DCACHE_SIZE

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

#endif

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
// There is no default mechanism for enabling or disabling the caches.
#ifndef HAL_DCACHE_ENABLE_DEFINED
#define HAL_DCACHE_ENABLE()
#endif

// Disable the data cache
#ifndef HAL_DCACHE_DISABLE_DEFINED
#define HAL_DCACHE_DISABLE()
#endif

#ifndef HAL_DCACHE_IS_ENABLED_DEFINED
#define HAL_DCACHE_IS_ENABLED(_state_) (_state_) = 1;
#endif

// Invalidate the entire cache
// We simply use HAL_DCACHE_SYNC() to do this. For writeback caches this
// is not quite what we want, but there is no index-invalidate operation
// available.
#ifndef HAL_DCACHE_INVALIDATE_ALL_DEFINED
#define HAL_DCACHE_INVALIDATE_ALL() HAL_DCACHE_SYNC()
#endif

// Synchronize the contents of the cache with memory.
// This uses the index-writeback-invalidate operation.
#ifndef HAL_DCACHE_SYNC_DEFINED
#define HAL_DCACHE_SYNC()                                                       \
{                                                                               \
    register CYG_ADDRESS _baddr_ = 0x80000000;                                  \
    register CYG_ADDRESS _addr_ = 0x80000000;                                   \
    register CYG_WORD _size_ = HAL_DCACHE_SIZE;                                 \
    _HAL_ASM_SET_MIPS_ISA(3);                                                   \
    for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )           \
    { asm volatile ("cache 0x01,0(%0)" : : "r"(_addr_) ); }                     \
    _HAL_ASM_SET_MIPS_ISA(0);                                                   \
}
#endif

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1

// Load the contents of the given address range into the data cache
// and then lock the cache so that it stays there.
// This uses the fetch-and-lock cache operation.
#ifndef HAL_DCACHE_LOCK_DEFINED
#define HAL_DCACHE_LOCK(_base_, _asize_)                                        \
CYG_MACRO_START                                                                 \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    register CYG_WORD _state_;                                                  \
    HAL_DCACHE_IS_ENABLED( _state_ );                                           \
    if( _state_ ) {                                                             \
        _HAL_ASM_SET_MIPS_ISA(3);                                               \
        for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )       \
        { asm volatile ("cache 0x1d,0(%0)" : : "r"(_addr_) ); }                 \
        _HAL_ASM_SET_MIPS_ISA(0);                                               \
    }                                                                           \
CYG_MACRO_END
#endif

// Undo a previous lock operation.
// Do this by flushing the cache, which is defined to clear the lock bit.
#ifndef HAL_DCACHE_UNLOCK_DEFINED
#define HAL_DCACHE_UNLOCK(_base_, _size_) \
        HAL_DCACHE_FLUSH( _base_, _size_ )
#endif

// Unlock entire cache
#ifndef HAL_DCACHE_UNLOCK_ALL_DEFINED
#define HAL_DCACHE_UNLOCK_ALL() \
        HAL_DCACHE_INVALIDATE_ALL()
#endif

//-----------------------------------------------------------------------------
// Data cache line control

// Allocate cache lines for the given address range without reading its
// contents from memory.
//#define HAL_DCACHE_ALLOCATE( _base_ , _size_ )

// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
// This uses the hit-writeback-invalidate cache operation.
#ifndef HAL_DCACHE_FLUSH_DEFINED
#define HAL_DCACHE_FLUSH( _base_ , _asize_ )                                    \
CYG_MACRO_START                                                                 \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    register CYG_WORD _state_;                                                  \
    HAL_DCACHE_IS_ENABLED( _state_ );                                           \
    if( _state_ ) {                                                             \
        _HAL_ASM_SET_MIPS_ISA(3);                                               \
        for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )       \
        { asm volatile ("cache 0x15,0(%0)" : : "r"(_addr_) ); }                 \
        _HAL_ASM_SET_MIPS_ISA(0);                                               \
    }                                                                           \
CYG_MACRO_END
#endif

// Invalidate cache lines in the given range without writing to memory.
// This uses the hit-invalidate cache operation.
#ifndef HAL_DCACHE_INVALIDATE_DEFINED
#define HAL_DCACHE_INVALIDATE( _base_ , _asize_ )                               \
{                                                                               \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    _HAL_ASM_SET_MIPS_ISA(3);                                                   \
    for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )           \
    { asm volatile ("cache 0x11,0(%0)" : : "r"(_addr_) ); }                     \
    _HAL_ASM_SET_MIPS_ISA(0);                                                   \
}
#endif

// Write dirty cache lines to memory for the given address range.
// This uses the hit-writeback cache operation.
#ifndef HAL_DCACHE_STORE_DEFINED
#define HAL_DCACHE_STORE( _base_ , _asize_ )                                    \
CYG_MACRO_START                                                                 \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    register CYG_WORD _state_;                                                  \
    HAL_DCACHE_IS_ENABLED( _state_ );                                           \
    if( _state_ ) {                                                             \
        _HAL_ASM_SET_MIPS_ISA(3);                                               \
        for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )       \
        { asm volatile ("cache 0x19,0(%0)" : : "r"(_addr_) ); }                 \
        _HAL_ASM_SET_MIPS_ISA(0);                                               \
    }                                                                           \
CYG_MACRO_END
#endif

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
// There is no default mechanism for enabling or disabling the caches.
#ifndef HAL_ICACHE_ENABLE_DEFINED
#define HAL_ICACHE_ENABLE()
#endif

// Disable the instruction cache
#ifndef HAL_ICACHE_DISABLE_DEFINED
#define HAL_ICACHE_DISABLE()
#endif

#ifndef HAL_ICACHE_IS_ENABLED_DEFINED
#define HAL_ICACHE_IS_ENABLED(_state_) (_state_) = 1;
#endif

// Invalidate the entire cache
// This uses the index-invalidate cache operation.
#ifndef HAL_ICACHE_INVALIDATE_ALL_DEFINED
#define HAL_ICACHE_INVALIDATE_ALL()                                             \
{                                                                               \
    register CYG_ADDRESS _baddr_ = 0x80000000;                                  \
    register CYG_ADDRESS _addr_ = 0x80000000;                                   \
    _HAL_ASM_SET_MIPS_ISA(3);                                                   \
    for( ; _addr_ < _baddr_+HAL_ICACHE_SIZE; _addr_ += HAL_ICACHE_LINE_SIZE )   \
    { asm volatile ("cache 0x00,0(%0)" : : "r"(_addr_) ); }                     \
    _HAL_ASM_SET_MIPS_ISA(0);                                                   \
}
#endif

// Synchronize the contents of the cache with memory.
// Simply force the cache to reload.
#ifndef HAL_ICACHE_SYNC_DEFINED
#define HAL_ICACHE_SYNC() HAL_ICACHE_INVALIDATE_ALL()
#endif

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)

// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.
// This uses the fetch-and-lock cache operation.
#ifndef HAL_ICACHE_LOCK_DEFINED
#define HAL_ICACHE_LOCK(_base_, _asize_)                                        \
CYG_MACRO_START                                                                 \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    register CYG_WORD _state_;                                                  \
    HAL_ICACHE_IS_ENABLED( _state_ );                                           \
    if( _state_ ) {                                                             \
        _HAL_ASM_SET_MIPS_ISA(3);                                               \
        for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_DCACHE_LINE_SIZE )       \
        { asm volatile ("cache 0x1c,0(%0)" : : "r"(_addr_) ); }                 \
        _HAL_ASM_SET_MIPS_ISA(0);                                               \
    }                                                                           \
CYG_MACRO_END
#endif

// Undo a previous lock operation.
// Do this by invalidating the cache, which is defined to clear the lock bit.
#ifndef HAL_ICACHE_UNLOCK_DEFINED
#define HAL_ICACHE_UNLOCK(_base_, _size_) \
        HAL_ICACHE_INVALIDATE( _base_, _size_ )
#endif

// Unlock entire cache
//#define HAL_ICACHE_UNLOCK_ALL()

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
// This uses the hit-invalidate cache operation.
#ifndef HAL_ICACHE_INVALIDATE_DEFINED
#define HAL_ICACHE_INVALIDATE( _base_ , _asize_ )                               \
{                                                                               \
    register CYG_ADDRESS _baddr_ = (CYG_ADDRESS)(_base_);                       \
    register CYG_ADDRESS _addr_ = (CYG_ADDRESS)(_base_);                        \
    register CYG_WORD _size_ = (_asize_);                                       \
    _HAL_ASM_SET_MIPS_ISA(3);                                                   \
    for( ; _addr_ <= _baddr_+_size_; _addr_ += HAL_ICACHE_LINE_SIZE )           \
    { asm volatile ("cache 0x10,0(%0)" : : "r"(_addr_) ); }                     \
    _HAL_ASM_SET_MIPS_ISA(0);                                                   \
}
#endif

//-----------------------------------------------------------------------------
// Check that a supported configuration has actually defined some macros.

#ifndef HAL_DCACHE_ENABLE

#error Unsupported MIPS configuration

#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
