#ifndef CYGONCE_HAL_CACHE_H
#define CYGONCE_HAL_CACHE_H

//=============================================================================
//
//      hal_cache.h
//
//      HAL Cache control support (such as it is in the simulator)
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
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
// Author(s):   nickg, gthomas, hmt
// Contributors:        nickg, gthomas, hmt
// Date:        1999-01-28
// Purpose:     Define Interrupt support
// Description: The macros defined here provide the HAL APIs for handling
//              the caches.
//              
// Usage:
//              #include <cyg/hal/hal_cache.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_sparclite.h>

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// SPARClite cache macros

// The MB6863x Control Registers are in Address Space 1:
#define HAL_SPARC_ASI_1_READ( addr, res )                                   \
    asm volatile(                                                           \
        "lda [ %1 ] 1, %0"                                                  \
        : "=r"(res)                                                         \
        : "r"(addr)                                                         \
    );

#define HAL_SPARC_ASI_1_WRITE( addr, val )                                  \
    asm volatile(                                                           \
        "sta %0, [ %1 ] 1"                                                  \
        :                                                                   \
        : "r"(val),"r"(addr)                                                \
    );

#define HAL_SPARC_MMCR_CBIR    0x00     // Cache/BusInterfaceUnit Control
#define HAL_SPARC_MMCR_LCR     0x04     // Lock Control Register
#define HAL_SPARC_MMCR_LCSR    0x08     // Lock Control Save Reg
#define HAL_SPARC_MMCR_CSR     0x0c     // Cache Status Reg
#define HAL_SPARC_MMCR_RLCR    0x10     // Restore Lock Control Register

#define HAL_SPARC_MMCR_CBIR_ICE  0x01   // Instruction Cache Enable
#define HAL_SPARC_MMCR_CBIR_GICL 0x02   // Global IC Lock
#define HAL_SPARC_MMCR_CBIR_DCE  0x04   // Data CE
#define HAL_SPARC_MMCR_CBIR_GDCL 0x08   // G Data CE
#define HAL_SPARC_MMCR_CBIR_PBE  0x10   // Prefetch Buffer Enable
#define HAL_SPARC_MMCR_CBIR_WBE  0x20   // Write BE


//-----------------------------------------------------------------------------
// Cache dimensions

// These definitions are suitable for any MB8683x processor:
//    The largest possible cachesize and "ways",
//    the smallest possible line size.
// This gives values that can correctly manipulate the cache by
// jumping on memory.

// Data cache
#define HAL_DCACHE_SIZE                 0x2000   // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16       // Size of a data cache line
#define HAL_DCACHE_WAYS                 2        // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 0x2000   // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16       // Size of a cache line
#define HAL_ICACHE_WAYS                 2        // Associativity of the cache

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE() CYG_MACRO_START                                 \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    _v_ |= HAL_SPARC_MMCR_CBIR_DCE;                                         \
    HAL_SPARC_ASI_1_WRITE( HAL_SPARC_MMCR_CBIR, _v_ );                      \
    asm volatile ( "nop; nop; nop; nop;" );                                 \
CYG_MACRO_END

// Disable the data cache
#define HAL_DCACHE_DISABLE() CYG_MACRO_START                                \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    _v_ &=~ HAL_SPARC_MMCR_CBIR_DCE;                                        \
    HAL_SPARC_ASI_1_WRITE( HAL_SPARC_MMCR_CBIR, _v_ );                      \
    asm volatile ( "nop; nop; nop; nop;" );                                 \
CYG_MACRO_END

// Invalidate the entire cache
#define HAL_DCACHE_INVALIDATE_ALL() CYG_MACRO_START                         \
    asm volatile (                                                          \
        "set    3, %%l0;"                                                   \
        "set    0x00001000, %%l1;"                                          \
        "set    0x80001000, %%l2;"                                          \
        "sta    %%l0, [ %%l1 ] 0x0e;"                                       \
        "sta    %%l0, [ %%l2 ] 0x0e;"                                       \
        "nop;"                                                              \
        "nop;"                                                              \
        "nop;"                                                              \
        "nop" : : : "l0","l1","l2" );                                       \
CYG_MACRO_END

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC() CYG_MACRO_START                                   \
    /* read 8k from the ROM; that should do it... */                        \
    volatile cyg_uint32 *_p_ = (cyg_uint32 *)0;                             \
    volatile cyg_uint32 *_q_ = (cyg_uint32 *)HAL_DCACHE_SIZE;               \
    volatile cyg_uint32 _tmp_;                                              \
    for ( ; _p_ < _q_; _p_ ++ ) _tmp_ = *_q_;                               \
CYG_MACRO_END

// Query the state of the data cache
#define HAL_DCACHE_IS_ENABLED(_state_) CYG_MACRO_START                      \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    (_state_) = (0 != (_v_ & HAL_SPARC_MMCR_CBIR_DCE));                     \
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

// Enable the data cache
#define HAL_ICACHE_ENABLE() CYG_MACRO_START                                 \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    _v_ |= HAL_SPARC_MMCR_CBIR_ICE;                                         \
    HAL_SPARC_ASI_1_WRITE( HAL_SPARC_MMCR_CBIR, _v_ );                      \
    asm volatile ( "nop; nop; nop; nop;" );                                 \
CYG_MACRO_END

// Disable the data cache
#define HAL_ICACHE_DISABLE() CYG_MACRO_START                                \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    _v_ &=~ HAL_SPARC_MMCR_CBIR_ICE;                                        \
    HAL_SPARC_ASI_1_WRITE( HAL_SPARC_MMCR_CBIR, _v_ );                      \
    asm volatile ( "nop; nop; nop; nop;" );                                 \
CYG_MACRO_END

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL() CYG_MACRO_START                         \
    asm volatile (                                                          \
        "set    3, %%l0;"                                                   \
        "set    0x00001000, %%l1;"                                          \
        "set    0x80001000, %%l2;"                                          \
        "sta    %%l0, [ %%l1 ] 0x0c;"                                       \
        "sta    %%l0, [ %%l2 ] 0x0c;"                                       \
        "nop;"                                                              \
        "nop;"                                                              \
        "nop;"                                                              \
        "nop" : : : "l0","l1","l2" );                                       \
CYG_MACRO_END

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC() CYG_MACRO_START                                   \
    HAL_DCACHE_SYNC();           /* Ensure data is in memory */             \
    HAL_ICACHE_INVALIDATE_ALL(); /* Pick up new memory contents */          \
CYG_MACRO_END

// Query the state of the instruction cache
#define HAL_ICACHE_IS_ENABLED(_state_) CYG_MACRO_START                      \
    int _v_;                                                                \
    HAL_SPARC_ASI_1_READ( HAL_SPARC_MMCR_CBIR, _v_ );                       \
    (_state_) = (0 != (_v_ & HAL_SPARC_MMCR_CBIR_ICE));                     \
CYG_MACRO_END

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
