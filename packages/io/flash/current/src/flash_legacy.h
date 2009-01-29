//==========================================================================
//
//      flash_legacy.h
//
//      Flash programming - some internal implementation details.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, Andrew Lunn
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_flash.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>


// Some FLASH devices may require additional support, e.g. to turn on
// appropriate voltage drivers, before any operation.
#ifdef  CYGIMP_FLASH_ENABLE
# define FLASH_Enable(__start, __end) CYGIMP_FLASH_ENABLE((void *)__start, (void *)__end)
extern void CYGIMP_FLASH_ENABLE(void *, void *);
#else
# define FLASH_Enable(_start_, _end_) CYG_EMPTY_STATEMENT
#endif
#ifdef  CYGIMP_FLASH_DISABLE
# define FLASH_Disable(__start, __end) CYGIMP_FLASH_DISABLE((void *)__start, (void *)__end)
extern void CYGIMP_FLASH_DISABLE(void *, void *);
#else
# define FLASH_Disable(_start_, _end_) CYG_EMPTY_STATEMENT
#endif

//
// Some platforms have a DIP switch or jumper that tells the software that
// the flash is write protected.
//
#ifdef CYGSEM_IO_FLASH_SOFT_WRITE_PROTECT
externC cyg_bool plf_flash_query_soft_wp(void *addr, int len);
#endif

//---------------------------------------------------------------------------
// If all of the flash devices handle cache themselves, or do not need any
// special cache treatment, then the flash macros can be no-ops.
#ifndef CYGHWR_IO_FLASH_DEVICE_NEEDS_CACHE_HANDLED
# undef HAL_FLASH_CACHES_OFF
# undef HAL_FLASH_CACHES_ON
# undef HAL_FLASH_CACHES_STATE
# define HAL_FLASH_CACHES_OFF(_d_, _i_)     CYG_EMPTY_STATEMENT
# define HAL_FLASH_CACHES_ON(_d_, _i_)      CYG_EMPTY_STATEMENT
# define HAL_FLASH_CACHES_STATE(_d_, _i_)   CYG_EMPTY_STATEMENT
#endif

// Execution of flash code must be done inside a
// HAL_FLASH_CACHES_OFF/HAL_FLASH_CACHES_ON region - disabling the
// cache on unified cache systems is necessary to prevent burst access
// to the flash area being programmed. With Harvard style caches, only
// the data cache needs to be disabled, but the instruction cache is
// disabled for consistency.

// Targets may provide alternative implementations for these macros in
// the hal_cache.h (or var/plf) files.

// The first part below is a generic, optimal implementation.  The
// second part is the old implementation that has been tested to work
// on some targets - but it is not be suitable for targets that would
// do burst access to the flash (it does not disable the data cache).

// Both implementations must be called with interrupts disabled.

// NOTE: Do _not_ change any of the below macros without checking that
//       the changed code still works on _all_ platforms that rely on these
//       macros. There is no such thing as logical and correct when dealing
//       with different cache and IO models, so _do not_ mess with this code
//       unless you test it properly afterwards.

#ifndef HAL_FLASH_CACHES_OFF

// Some drivers have only been tested with the old macros below.
#ifndef HAL_FLASH_CACHES_OLD_MACROS

#ifdef HAL_CACHE_UNIFIED

// Note: the ucache code has not been tested yet on any target.
#define HAL_FLASH_CACHES_OFF(_d_, _i_)          \
    CYG_MACRO_START                             \
    _i_ = 0; /* avoids warning */               \
    HAL_UCACHE_IS_ENABLED(_d_);                 \
    HAL_UCACHE_SYNC();                          \
    HAL_UCACHE_INVALIDATE_ALL();                \
    HAL_UCACHE_DISABLE();                       \
    CYG_MACRO_END

#define HAL_FLASH_CACHES_ON(_d_, _i_)           \
    CYG_MACRO_START                             \
    if (_d_) HAL_UCACHE_ENABLE();               \
    CYG_MACRO_END

#else  // HAL_CACHE_UNIFIED

#define HAL_FLASH_CACHES_OFF(_d_, _i_)          \
    CYG_MACRO_START                             \
    _i_ = 0; /* avoids warning */               \
    HAL_DCACHE_IS_ENABLED(_d_);                 \
    HAL_DCACHE_SYNC();                          \
    HAL_DCACHE_INVALIDATE_ALL();                \
    HAL_DCACHE_DISABLE();                       \
    HAL_ICACHE_INVALIDATE_ALL();                \
    CYG_MACRO_END

#define HAL_FLASH_CACHES_ON(_d_, _i_)           \
    CYG_MACRO_START                             \
    if (_d_) HAL_DCACHE_ENABLE();               \
    CYG_MACRO_END

#endif // HAL_CACHE_UNIFIED

#else  // HAL_FLASH_CACHES_OLD_MACROS

// Note: This implementation is broken as it will always enable the i-cache
//       even if it was not enabled before. It also doesn't work if the
//       target uses burst access to flash since the d-cache is left enabled.
//       However, this does not mean you can change this code! Leave it as
//       is - if you want a different implementation, provide it in the
//       arch/var/platform cache header file.

#define HAL_FLASH_CACHES_OFF(_d_, _i_)          \
    _d_ = 0; /* avoids warning */               \
    _i_ = 0; /* avoids warning */               \
    HAL_DCACHE_SYNC();                          \
    HAL_ICACHE_DISABLE();

#define HAL_FLASH_CACHES_ON(_d_, _i_)           \
    HAL_ICACHE_ENABLE();

#endif  // HAL_FLASH_CACHES_OLD_MACROS

#endif  // HAL_FLASH_CACHES_OFF

#ifndef HAL_FLASH_CACHES_STATE
# define HAL_FLASH_CACHES_STATE(_d_, _i_) int _d_, _i_
#endif
