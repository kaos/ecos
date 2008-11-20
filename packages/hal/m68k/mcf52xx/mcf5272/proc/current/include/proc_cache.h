#ifndef CYGONCE_PROC_CACHE_H
#define CYGONCE_PROC_CACHE_H
//=============================================================================
//
//      proc_cache.h
//
//      Cache details for an mcf5272
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2006,2008 Free Software Foundation, Inc.
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/var_io.h>

// An mcf5272 has a 1k direct-mapped instruction cache, 64 lines * 16 bytes.
// There is no data cache. The cache control register is write-only so it
// is necessary to keep a soft copy.

externC cyg_uint32   hal_mcf5272_cacr;

#define HAL_ICACHE_SIZE         1024
#define HAL_ICACHE_LINE_SIZE      16
#define HAL_ICACHE_WAYS           64
#define HAL_ICACHE_SETS            1

#define HAL_ICACHE_ENABLE()                                                                     \
    CYG_MACRO_START                                                                             \
    hal_mcf5272_cacr = (hal_mcf5272_cacr & ~HAL_MCFxxxx_CACR_CINV) | HAL_MCFxxxx_CACR_CENB;     \
    asm volatile ( "movec.l %0,%%cacr\n" : : "d" (hal_mcf5272_cacr) : "memory") ;               \
    CYG_MACRO_END

#define HAL_ICACHE_DISABLE()                                                                    \
    CYG_MACRO_START                                                                             \
    hal_mcf5272_cacr &= ~(HAL_MCFxxxx_CACR_CENB | HAL_MCFxxxx_CACR_CINV);                       \
    asm volatile ( "movec.l %0,%%cacr\n" : : "d" (hal_mcf5272_cacr) : "memory") ;               \
    CYG_MACRO_END

#define HAL_ICACHE_IS_ENABLED(_state_)                              \
    CYG_MACRO_START                                                 \
    _state_ = (0 != (hal_mcf5272_cacr & HAL_MCFxxxx_CACR_CENB));    \
    CYG_MACRO_END

// A full cache invalidate takes 64 cycles. This is expensive if only one
// or two lines need to be invalidated, but doing the arithmetic and tests
// needed to affect just the necessary lines would also take quite a few
// cycles. Hence it is simpler to just invalidate the lot.
#define HAL_ICACHE_INVALIDATE_ALL()                                                                         \
    CYG_MACRO_START                                                                                         \
    asm volatile ( "movec.l %0,%%cacr\n" : : "d" (hal_mcf5272_cacr | HAL_MCFxxxx_CACR_CINV) : "memory" );   \
    CYG_MACRO_END

#define HAL_ICACHE_INVALIDATE(_base_, _size_)           \
    CYG_MACRO_START                                     \
    HAL_ICACHE_INVALIDATE_ALL();                        \
    CYG_MACRO_END

#define HAL_ICACHE_SYNC()                               \
    CYG_MACRO_START                                     \
    HAL_ICACHE_INVALIDATE_ALL();                        \
    CYG_MACRO_END

#endif // ifndef CYGONCE_PROC_CACHE_H
// End of proc_cache.h

