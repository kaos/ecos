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
// Author(s):   hmt
// Contributors:hmt
// Date:        1999-07-05
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
//#include <cyg/hal/hal_mmu.h>

//-----------------------------------------------------------------------------
// Cache dimensions

#define HAL_DCACHE_SIZE                 0x8000 // Size of data cache in bytes
#define HAL_DCACHE_LINE_SIZE            32     // Size of a data cache line
#define HAL_DCACHE_WAYS                 32     // Associativity of the cache
#define HAL_DCACHE_SETS (HAL_DCACHE_SIZE/(HAL_DCACHE_LINE_SIZE*HAL_DCACHE_WAYS))

#define HAL_ICACHE_SIZE                 0x8000 // Size of icache in bytes
#define HAL_ICACHE_LINE_SIZE            32     // Size of ins cache line
#define HAL_ICACHE_WAYS                 32     // Associativity of the cache
#define HAL_ICACHE_SETS (HAL_ICACHE_SIZE/(HAL_ICACHE_LINE_SIZE*HAL_ICACHE_WAYS))

//-----------------------------------------------------------------------------
// Global control of data cache

// Enable the data cache
#define HAL_DCACHE_ENABLE()                                             \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "mrc  p15,0,r1,c7,c10,4;"   /* drain write buffer */            \
        "mrc  p15,0,r1,c1,c0,0;"                                        \
        "orr  r1,r1,#0x0007;"  /* enable DCache (also ensures the */    \
                               /* MMU and alignment faults are    */    \
                               /* enabled)                        */    \
        "mcr  p15,0,r1,c1,c0,0;"                                        \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END

// Disable the data cache (and invalidate it, required semanitcs)
#define HAL_DCACHE_DISABLE()                                            \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "mrc  p15,0,r1,c1,c0,0;"                                        \
        "bic  r1,r1,#4;"                                                \
        "mcr  p15,0,r1,c1,c0,0;"                                        \
        /* cpuwait */                                                   \
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              \
        "mov    r1,r1;"                                                 \
        "sub    pc,pc,#4;"                                              \
        "mcr    p15,0,r1,c7,c6,0;" /* invalidate data cache */          \
        /* cpuwait */                                                   \
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              \
        "mov    r1,r1;"                                                 \
        "sub    pc,pc,#4;"                                              \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END

// Invalidate the entire cache (and both TLBs, just in case)
#define HAL_DCACHE_INVALIDATE_ALL()                                     \
CYG_MACRO_START                                                         \
    /* this macro can discard dirty cache lines. */                     \
    asm volatile (                                                      \
        "mcr    p15,0,r1,c7,c6,0;"  /* invalidate data cache */         \
        "mcr    p15,0,r1,c8,c7,0;"  /* flush I+D TLBs */                \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END
     

// Synchronize the contents of the cache with memory.
#define HAL_DCACHE_SYNC()                                               \
CYG_MACRO_START                                                         \
    /* The best way to evict a dirty line is by using the          */   \
    /* line allocate operation on non-existent memory.             */   \
    asm volatile (                                                      \
        "mov    r0, #0xC0000000;"   /* cache flush region */            \
        "add    r1, r0, #0x8000;"   /* 32KB cache         */            \
 "667: "                                                                \
        "mcr    p15,0,r0,c7,c2,5;"  /* allocate a line    */            \
        "add    r0, r0, #32;"       /* 32 bytes/line      */            \
        "teq    r1, r0;"                                                \
        "bne    667b;"                                                  \
        "mcr    p15,0,r0,c7,c6,0;"  /* invalidate data cache */         \
        /* cpuwait */                                                   \
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              \
        "mov    r1,r1;"                                                 \
        "sub    pc,pc,#4;"                                              \
        "mcr    p15,0,r0,c7,c10,4;" /* and drain the write buffer */    \
        /* cpuwait */                                                   \
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              \
        "mov    r1,r1;"                                                 \
        "sub    pc,pc,#4;"                                              \
        "nop"                                                           \
        :                                                               \
        :                                                               \
        : "r0","r1"      /* Clobber list */                             \
        );                                                              \
CYG_MACRO_END

// Query the state of the data cache
#define HAL_DCACHE_IS_ENABLED(_state_)                                  \
CYG_MACRO_START                                                         \
    register int reg;                                                   \
    asm volatile ("mrc  p15,0,%0,c1,c0,0"                               \
                  : "=r"(reg)                                           \
                  :                                                     \
                /*:*/                                                   \
        );                                                              \
    (_state_) = (0 != (4 & reg)); /* Bit 2 is DCache enable */          \
CYG_MACRO_END

// Set the data cache refill burst size
//#define HAL_DCACHE_BURST_SIZE(_size_)

// Set the data cache write mode
//#define HAL_DCACHE_WRITE_MODE( _mode_ )

#define HAL_DCACHE_WRITETHRU_MODE       0
#define HAL_DCACHE_WRITEBACK_MODE       1

// Get the current writeback mode - or only writeback mode if fixed
#define HAL_DCACHE_QUERY_WRITE_MODE( _mode_ ) CYG_MACRO_START           \
    _mode_ = HAL_DCACHE_WRITEBACK_MODE;                                 \
CYG_MACRO_END

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
// ---- this seems not to work despite the documentation ---
//#define HAL_DCACHE_FLUSH( _base_ , _size_ )
//CYG_MACRO_START
//    HAL_DCACHE_STORE( _base_ , _size_ );
//    HAL_DCACHE_INVALIDATE( _base_ , _size_ );
//CYG_MACRO_END

// Invalidate cache lines in the given range without writing to memory.
// ---- this seems not to work despite the documentation ---
//#define HAL_DCACHE_INVALIDATE( _base_ , _size_ )
//CYG_MACRO_START
//    register int addr, enda;
//    for ( addr = (~(HAL_DCACHE_LINE_SIZE - 1)) & (int)(_base_),
//              enda = (int)(_base_) + (_size_);
//          addr < enda ;
//          addr += HAL_DCACHE_LINE_SIZE )
//    {
//        asm volatile (
//                      "mcr  p15,0,%0,c7,c6,1;" /* flush entry away */
//                      :
//                      : "r"(addr)
//                      : "memory"
//            );
//    }
//CYG_MACRO_END
                          
// Write dirty cache lines to memory for the given address range.
// ---- this seems not to work despite the documentation ---
//#define HAL_DCACHE_STORE( _base_ , _size_ )
//CYG_MACRO_START
//    register int addr, enda;
//    for ( addr = (~(HAL_DCACHE_LINE_SIZE - 1)) & (int)(_base_),
//              enda = (int)(_base_) + (_size_);
//          addr < enda ;
//          addr += HAL_DCACHE_LINE_SIZE )
//    {
//        asm volatile ("mcr  p15,0,%0,c7,c10,1" /* push entry to RAM */
//                      :
//                      : "r"(addr)
//                      : "memory"
//            );
//    }
//CYG_MACRO_END


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
#define HAL_ICACHE_ENABLE()                                             \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "mrc  p15,0,r1,c1,c0,0;"                                        \
        "orr  r1,r1,#0x1000;" /* enable ICache */                       \
        "mcr  p15,0,r1,c1,c0,0;"                                        \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END

// Disable the instruction cache (and invalidate it, required semanitcs)
#define HAL_ICACHE_DISABLE()                                            \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "mrc    p15,0,r1,c1,c0,0;"                                      \
        "bic    r1,r1,#0x1000;" /* disable Icache */                    \
        "mcr    p15,0,r1,c1,c0,0;"                                      \
        "mcr    p15,0,r1,c7,c5,0;"  /* invalidate instruction cache */  \
        "nop;" /* next few instructions may be via cache */             \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop"                                                           \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END

// Invalidate the entire cache
#define HAL_ICACHE_INVALIDATE_ALL()                                     \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "mcr    p15,0,r1,c7,c5,0;"  /* clear instruction cache */       \
        "mcr    p15,0,r1,c8,c5,0;"  /* flush I TLB only */              \
        /* cpuwait */                                                   \
        "mrc    p15,0,r1,c2,c0,0;"  /* arbitrary read   */              \
        "mov    r1,r1;"                                                 \
        "sub    pc,pc,#4;"                                              \
        "nop;" /* next few instructions may be via cache */             \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop;"                                                          \
        "nop"                                                           \
        :                                                               \
        :                                                               \
        : "r1" /* Clobber list */                                       \
        );                                                              \
CYG_MACRO_END
     

// Synchronize the contents of the cache with memory.
// (which includes flushing out pending writes)
#define HAL_ICACHE_SYNC()                                       \
CYG_MACRO_START                                                 \
    HAL_DCACHE_SYNC(); /* ensure data gets to RAM */            \
    HAL_ICACHE_INVALIDATE_ALL(); /* forget all we know */       \
CYG_MACRO_END

// Query the state of the instruction cache
#define HAL_ICACHE_IS_ENABLED(_state_)                                  \
CYG_MACRO_START                                                         \
    /* SA-110 manual states clearly that the control reg is readable */ \
    register cyg_uint32 reg;                                            \
    asm volatile ("mrc  p15,0,%0,c1,c0,0"                               \
                  : "=r"(reg)                                           \
                  :                                                     \
                /*:*/                                                   \
        );                                                              \
    (_state_) = (0 != (0x1000 & reg)); /* Bit 12 is ICache enable */    \
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
