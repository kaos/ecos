#ifndef CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
#define CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
/*=============================================================================
//
//      plf_mmap.h
//
//      Platform specific memory map support
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         2001-01-04
// Purpose:      Intel SA11x0 series platform-specific memory mapping macros
// Description:  Macros to convert a cached, virtual address to
//               1) an uncached adddress for the same memory (if available)
//               2) the equivalent physical address for giving to external
//               hardware eg. DMA engines.
//               
//               NB: this mapping is expressed here statically, independent
//               of the actual mapping installed in the MMAP table.  So if
//               someone changes that, or its initialisation is changed,
//               then this module must change.  This is intended to be
//               efficient at a cost of generality.  It is also intended to
//               be called with constants (such as &my_struct) as input
//               args, so that all the work can be done at compile time,
//               with optimization on.
//
// Usage:        #include <cyg/hal/hal_cache.h>
//		 (which includes this file itself)
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/hal_misc.h>

// Get the pagesize for a particular virtual address:

// This does not depend on the vaddr.
#define HAL_MM_PAGESIZE( vaddr, pagesize ) CYG_MACRO_START      \
    (pagesize) = SZ_1M;                                         \
CYG_MACRO_END

// Get the physical address from a virtual address:

// Only RAM and ROM are mapped; we just pass through all other values,
// rather than detecting nonexistent areas here.

#define HAL_VIRT_TO_PHYS_ADDRESS( vaddr, paddr ) CYG_MACRO_START           \
    cyg_uint32 _v_ = (cyg_uint32)(vaddr);                                  \
    if (  4 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 0 from  0-4Mb */ \
        _v_ += 0xc00u * SZ_1M;                                             \
    if (  8 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 1 from  4-8Mb */ \
        _v_ += 0xc80u * SZ_1M;                                             \
    if ( 12 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 2 from 8-12Mb */ \
        _v_ += 0xd00u * SZ_1M;                                             \
    if ( 16 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 3 from 12-16M */ \
        _v_ += 0xd80u * SZ_1M;                                             \
    else if ( 0x400u * SZ_1M > _v_ ) /* Space between RAM and mapped ROM */\
        /* no change */ ;                                                  \
    else if ( 0x401u * SZ_1M > _v_ ) /* Mapped boot ROM size  1Mb */       \
        _v_ -= 0x400u * SZ_1M;                                             \
    else                            /* Rest of it */                       \
        /* no change */ ;                                                  \
    (paddr) = _v_;                                                         \
CYG_MACRO_END

// Get the virtual address for a physical address:

// Only RAM and ROM are mapped; we just pass through all other values,
// rather than detecting nonexistent areas here.

#define HAL_PHYS_TO_VIRT_ADDRESS( paddr, vaddr ) CYG_MACRO_START           \
    cyg_uint32 _p_ = (cyg_uint32)(paddr);                                  \
    if (  1 * SZ_1M > _p_ )         /*  1Mb Boot ROM mapped to 0x400Mb */  \
        _p_ += 0x400u * SZ_1M;                                             \
    else if ( 0xc00u * SZ_1M > _p_ ) /* Space between ROM and SDRAM */     \
        /* no change */ ;                                                  \
    else if ( 0xc04u * SZ_1M > _p_ ) /* Raw RAM bank 0, 4Mb at 0xc00 */    \
        _p_ -= 0xc00u * SZ_1M;                                             \
    else if ( 0xc80u * SZ_1M > _p_ ) /* Space between SDRAM banks */       \
        /* no change */ ;                                                  \
    else if ( 0xc84u * SZ_1M > _p_ ) /* Raw RAM bank 1, 4Mb at 0xc80 */    \
        _p_ -= 0xc80u * SZ_1M;                                             \
    else if ( 0xd00u * SZ_1M > _p_ ) /* Space between SDRAM banks */       \
        /* no change */ ;                                                  \
    else if ( 0xd04u * SZ_1M > _p_ ) /* Raw RAM bank 2, 4Mb at 0xd00 */    \
        _p_ -= 0xd00u * SZ_1M;                                             \
    else if ( 0xd80u * SZ_1M > _p_ ) /* Space between SDRAM banks */       \
        /* no change */ ;                                                  \
    else if ( 0xd84u * SZ_1M > _p_ ) /* Raw RAM bank 3, 4Mb at 0xd80 */    \
        _p_ -= 0xd80u * SZ_1M;                                             \
    else                            /* Rest of it */                       \
        /* no change */ ;                                                  \
    (vaddr) = _p_ ;                                                        \
CYG_MACRO_END

// Get a non-cacheable address for accessing the same store as a virtual
// (assumed cachable) address:

// Only RAM is mapped: ROM is only available cachable, everything else is
// not cachable anyway.

#define HAL_VIRT_TO_UNCACHED_ADDRESS( vaddr, uaddr ) CYG_MACRO_START       \
    cyg_uint32 _v_ = (cyg_uint32)(vaddr);                                  \
    if (  4 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 0 from  0-4Mb */ \
        _v_ += 0xc00u * SZ_1M;                                             \
    if (  8 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 1 from  4-8Mb */ \
        _v_ += 0xc80u * SZ_1M;                                             \
    if ( 12 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 2 from 8-12Mb */ \
        _v_ += 0xd00u * SZ_1M;                                             \
    if ( 16 * SZ_1M > _v_ )         /*  4Mb of SDRAM Bank 3 from 12-16M */ \
        _v_ += 0xd80u * SZ_1M;                                             \
    else            /* Everything else is already uncacheable or is ROM */ \
        /* no change */ ;                                                  \
    (uaddr) = _v_ ;                                                        \
CYG_MACRO_END

//---------------------------------------------------------------------------
#endif // CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
// EOF plf_mmap.h
