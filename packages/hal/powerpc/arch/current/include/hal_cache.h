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

#include <cyg/hal/ppc_regs.h>

#include <cyg/hal/plf_cache.h>


//=============================================================================
// Memory mapping
typedef struct {
    CYG_ADDRESS  virtual_addr;
    CYG_ADDRESS  physical_addr;
    cyg_int32    size;
    cyg_uint8    flags;
} cyg_memdesc_t;

// each platform HAL must provide one of these to describe how memory
// should be mapped/cached, ideally weak aliased so that apps can override:
externC cyg_memdesc_t cyg_hal_mem_map[];

#define CYGARC_MEMDESC_CI       1       // cache inhibit
#define CYGARC_MEMDESC_GUARDED  2       // guarded

// these macros should ease that task, and ease any future extension of the
// structure (physical == virtual addresses):
#define CYGARC_MEMDESC_CACHE( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), 0 }

#define CYGARC_MEMDESC_NOCACHE( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_CI }

#define CYGARC_MEMDESC_CACHEGUARD( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_GUARDED }

#define CYGARC_MEMDESC_NOCACHEGUARD( _va_, _sz_ ) \
        { (_va_), (_va_), (_sz_), CYGARC_MEMDESC_GUARDED|CYGARC_MEMDESC_CI }

#define CYGARC_MEMDESC_TABLE_END      {0, 0, 0, 0}
#define CYGARC_MEMDESC_TABLE          cyg_memdesc_t cyg_hal_mem_map[]
#define CYGARC_MEMDESC_EMPTY_TABLE    { CYGARC_MEMDESC_TABLE_END }

//=============================================================================
// PowerPC simulator

#ifdef CYG_HAL_POWERPC_SIM

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

// Query the state of the data cache
#define HAL_DCACHE_IS_ENABLED(_state_)          \
    CYG_MACRO_START                             \
    (_state_) = 0;                              \
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
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_ICACHE_ENABLE()

// Disable the instruction cache
#define HAL_ICACHE_DISABLE()

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC()

// Query the state of the instruction cache
#define HAL_ICACHE_IS_ENABLED(_state_)          \
    CYG_MACRO_START                             \
    (_state_) = 0;                              \
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


//=============================================================================
// Motorola MPC8xx

#elif defined(CYG_HAL_POWERPC_MPC8xx)

//-----------------------------------------------------------------------------
// Cache dimensions

#if defined(CYG_HAL_POWERPC_MPC860)
// Data cache
#define HAL_DCACHE_SIZE                 4096    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16      // Size of a data cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 4096    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache
#endif // defined(CYG_HAL_POWERPC_MPC860)

#if defined(CYG_HAL_POWERPC_MPC823) || defined(CYG_HAL_POWERPC_MPC850)
// Data cache
#define HAL_DCACHE_SIZE                 1024    // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            16      // Size of a data cache line
#define HAL_DCACHE_WAYS                 2       // Associativity of the cache

// Instruction cache
#define HAL_ICACHE_SIZE                 2048    // Size of cache in bytes
#define HAL_ICACHE_LINE_SIZE            16      // Size of a cache line
#define HAL_ICACHE_WAYS                 2       // Associativity of the cache
#endif // defined(CYG_HAL_POWERPC_MPC823) || defined(CYG_HAL_POWERPC_MPC850)

#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()                     \
    asm volatile ("sync;"                       \
                  "mtspr %0, %1;"               \
                  : : "I" (CYGARC_REG_DC_CST), "r" (CYGARC_REG_DC_CMD_CE))

// Disable the data cache
#define HAL_DCACHE_DISABLE()                    \
    asm volatile ("sync;"                       \
                  "mtspr %0, %1;"               \
                  : : "I" (CYGARC_REG_DC_CST), "r" (CYGARC_REG_DC_CMD_CD))

// Invalidate the entire cache
// Note: Any locked lines will not be invalidated.
#define HAL_DCACHE_INVALIDATE_ALL()                     \
    asm volatile ("sync;"                               \
                  "mtspr %0, %1;"                       \
                  : : "I" (CYGARC_REG_DC_CST),          \
                      "r" (CYGARC_REG_DC_CMD_IA))

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()                                                     \
    CYG_MACRO_START                                                           \
    cyg_int32 i;                                                              \
    for (i = 0; i < HAL_DCACHE_SETS; i++){                                    \
        asm volatile ("sync;"                                                 \
                      "mtspr %0, %2;"                                         \
                      "mtspr %1, %4;"                                         \
                      "mtspr %0, %3;"                                         \
                      "mtspr %1, %4;"                                         \
                      : /* no output */                                       \
                      : /* %0 */ "I" (CYGARC_REG_DC_ADR),                     \
                        /* %1 */ "I" (CYGARC_REG_DC_CST),                     \
                        /* %2 */ "r" (CYGARC_REG_DC_ADR_WAY0                  \
                                      |(i << CYGARC_REG_DC_ADR_SETID_SHIFT)), \
                        /* %3 */ "r" (CYGARC_REG_DC_ADR_WAY1                  \
                                      |(i << CYGARC_REG_DC_ADR_SETID_SHIFT)), \
                        /* %4 */ "r" (CYGARC_REG_DC_CMD_FL));                 \
    }                                                                         \
    CYG_MACRO_END

// Query the state of the data cache
#define HAL_DCACHE_IS_ENABLED(_state_)                          \
    asm volatile ("mfspr  %0, %1;"                              \
                  "rlwinm %0,%0,1,31,31;"                       \
                  : "=r" (_state_) : "I" (CYGARC_REG_DC_CST))

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

//#define HAL_DCACHE_WRITETHRU_MODE       0
//#define HAL_DCACHE_WRITEBACK_MODE       1


// Load the contents of the given address range into the data cache 
// and then lock the cache so that it stays there.  

// Restrictions: This implementation only allows a single area to be
// locked at any one time. This area must be 2kB or less in size.

// Implementation: Flush entire cache, then invalidate it. This
// ensures that the fetched data go into way0.

#define HAL_DCACHE_LOCK(_base_, _size_)                                       \
    CYG_MACRO_START                                                           \
    cyg_int32 __scratch;                                                      \
    cyg_uint32 __base = (cyg_uint32)(_base_);                                 \
    cyg_int32 __l = ((__base / HAL_DCACHE_LINE_SIZE) % HAL_DCACHE_SETS);      \
    cyg_int32 __count = ((_size_) / HAL_DCACHE_LINE_SIZE);                    \
    HAL_DCACHE_DISABLE();                                                     \
    HAL_DCACHE_SYNC ();                                                       \
    HAL_DCACHE_INVALIDATE_ALL ();                                             \
    HAL_DCACHE_ENABLE();                                                      \
    do {                                                                      \
        asm volatile ("lbz   %0,0(%1);"                                       \
                      "sync;"                                                 \
                      "mtspr %2, %4;"                                         \
                      "mtspr %3, %5;"                                         \
                      : /* %0 */ "=&r" (__scratch)                            \
                      : /* %1 */ "b" (__base),                                \
                        /* %2 */ "I" (CYGARC_REG_DC_ADR),                     \
                        /* %3 */ "I" (CYGARC_REG_DC_CST),                     \
                        /* %4 */ "r" (CYGARC_REG_DC_ADR_WAY0                  \
                                      |(__l<<CYGARC_REG_DC_ADR_SETID_SHIFT)), \
                        /* %5 */ "r" (CYGARC_REG_DC_CMD_LL));                 \
        __l++;                                                                \
        __base += HAL_DCACHE_LINE_SIZE;                                       \
    } while (__count--);                                                      \
    CYG_MACRO_END

        
// Undo a previous lock operation

// Implementation: Unlocks entire cache.

#define HAL_DCACHE_UNLOCK(_base_, _size_)               \
    HAL_DCACHE_UNLOCK_ALL()    


// Unlock entire cache
#define HAL_DCACHE_UNLOCK_ALL()                         \
    CYG_MACRO_START                                     \
    asm volatile ("sync;"                               \
                  "mtspr %0, %1;"                       \
                  : : "I" (CYGARC_REG_DC_CST),          \
                      "r" (CYGARC_REG_DC_CMD_UA));      \
    CYG_MACRO_END
   


//-----------------------------------------------------------------------------
// Data cache line control

// Allocate cache lines for the given address range without reading its
// contents from memory.
//#define HAL_DCACHE_ALLOCATE( _base_ , _size_ )

// Write dirty cache lines to memory and invalidate the cache entries
// for the given address range.
#define HAL_DCACHE_FLUSH( _base_ , _size_ )                     \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbf 0,%0;sync;" : : "r" (__base));      \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END
   
// Invalidate cache lines in the given range without writing to memory.
#define HAL_DCACHE_INVALIDATE( _base_ , _size_ )                \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbi 0,%0;sync;" : : "r" (__base));      \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END

// Write dirty cache lines to memory for the given address range.
#define HAL_DCACHE_STORE( _base_ , _size_ )                     \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbst 0,%0;sync;" : : "r" (__base));     \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END

// Preread the given range into the cache with the intention of reading
// from it later.
#define HAL_DCACHE_READ_HINT( _base_ , _size_ )                 \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbt 0,%0;" : : "r" (__base));           \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END

// Preread the given range into the cache with the intention of writing
// to it later.
#define HAL_DCACHE_WRITE_HINT( _base_ , _size_ )                \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbtst 0,%0;" : : "r" (__base));         \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END

// Allocate and zero the cache lines associated with the given range.
#define HAL_DCACHE_ZERO( _base_ , _size_ )                      \
    CYG_MACRO_START                                             \
    cyg_uint32 __base = (cyg_uint32) (_base_);                  \
    cyg_int32 __size = (cyg_int32) (_size_);                    \
    while (__size > 0) {                                        \
        asm volatile ("dcbz 0,%0;" : : "r" (__base));           \
        __base += HAL_DCACHE_LINE_SIZE;                         \
        __size -= HAL_DCACHE_LINE_SIZE;                         \
    }                                                           \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// Global control of Instruction cache

// Enable the instruction cache
#define HAL_ICACHE_ENABLE()                     \
    asm volatile ("isync;"                      \
                  "mtspr %0, %1;"               \
                  "isync"                       \
                  : : "I" (CYGARC_REG_IC_CST), "r" (CYGARC_REG_IC_CMD_CE))

// Disable the instruction cache
#define HAL_ICACHE_DISABLE()                    \
    asm volatile ("isync;"                      \
                  "mtspr %0, %1;"               \
                  "isync"                       \
                  : : "I" (CYGARC_REG_IC_CST), "r" (CYGARC_REG_IC_CMD_CD))

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()                     \
    asm volatile ("isync;"                              \
                  "mtspr %0, %1;"                       \
                  "isync"                               \
                  : : "I" (CYGARC_REG_IC_CST),          \
                      "r" (CYGARC_REG_IC_CMD_IA))

// Synchronize the contents of the cache with memory.
#define HAL_ICACHE_SYNC()                               \
    HAL_ICACHE_INVALIDATE_ALL()

// Query the state of the instruction cache
#define HAL_ICACHE_IS_ENABLED(_state_)                          \
    asm volatile ("mfspr  %0, %1;"                              \
                  "rlwinm %0,%0,1,31,31;"                       \
                  : "=r" (_state_) : "I" (CYGARC_REG_IC_CST))

// Set the instruction cache refill burst size
//#define HAL_ICACHE_BURST_SIZE(_size_)


// Load the contents of the given address range into the instruction cache
// and then lock the cache so that it stays there.

// Restrictions: This implementation only allows a single area to be
// locked at any one time. This area must be 2kB or less in size.

// Implementation: Flush entire cache, then invalidate it. This
// ensures that the fetched data go into way0.

#define HAL_ICACHE_LOCK(_base_, _size_)                                       \
    CYG_MACRO_START                                                           \
    unsigned long __base =                                                    \
        ((unsigned long) (_base_)) & ~(HAL_ICACHE_LINE_SIZE-1);               \
    int __count = ((_size_) / HAL_ICACHE_LINE_SIZE);                          \
    do {                                                                      \
        asm volatile ("mtspr %0, %2;"                                         \
                      "mtspr %1, %3;"                                         \
                      "isync;"                                                \
                      : /* no output */                                       \
                      : /* %0 */ "I" (CYGARC_REG_IC_ADR),                     \
                        /* %1 */ "I" (CYGARC_REG_IC_CST),                     \
                        /* %2 */ "r" (__base),                                \
                        /* %3 */ "r" (CYGARC_REG_IC_CMD_LL));                 \
        __base += HAL_ICACHE_LINE_SIZE;                                       \
    } while (__count--);                                                      \
    CYG_MACRO_END

// Undo a previous lock operation

// Implementation: Unlocks entire cache.
#define HAL_ICACHE_UNLOCK(_base_, _size_)       \
    HAL_ICACHE_UNLOCK_ALL()

// Unlock entire cache
#define HAL_ICACHE_UNLOCK_ALL()                         \
    CYG_MACRO_START                                     \
    asm volatile ("sync;"                               \
                  "mtspr %0, %1;"                       \
                  : : "I" (CYGARC_REG_IC_CST),          \
                      "r" (CYGARC_REG_IC_CMD_UA));      \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// Instruction cache line control

// Invalidate cache lines in the given range without writing to memory.
//#define HAL_ICACHE_INVALIDATE( _base_ , _size_ )

#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
