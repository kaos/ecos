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
// Copyright (C) 2003, 2006, 2008 Free Software Foundation, Inc.            
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
//####DESCRIPTIONBEGIN####
//
// Author(s): 	bartv
// Date:	2003-06-04
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>
#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#endif

#ifdef CYGINT_HAL_M68K_VARIANT_CACHE
# include <cyg/hal/var_cache.h>
#endif

// Default anything not defined by the variant HAL. Some of these
// macros get used unconditionally by other code so must be
// defined, to no-op.
#ifndef HAL_ICACHE_ENABLE
# define HAL_ICACHE_ENABLE()
#endif
#ifndef HAL_ICACHE_DISABLE
# define HAL_ICACHE_DISABLE()
#endif
#ifndef HAL_ICACHE_SYNC
# define HAL_ICACHE_SYNC()
#endif
#ifndef HAL_ICACHE_INVALIDATE_ALL
# define HAL_ICACHE_INVALIDATE_ALL()
#endif
#ifndef HAL_ICACHE_IS_ENABLED
# define HAL_ICACHE_IS_ENABLED(_state_)     \
    CYG_MACRO_START                         \
    (_state_) = 0;                          \
    CYG_MACRO_END
#endif

#ifndef HAL_DCACHE_ENABLE
# define HAL_DCACHE_ENABLE()
#endif
#ifndef HAL_DCACHE_DISABLE
# define HAL_DCACHE_DISABLE()
#endif
#ifndef HAL_DCACHE_SYNC
# define HAL_DCACHE_SYNC()
#endif
#ifndef HAL_DCACHE_INVALIDATE_ALL
# define HAL_DCACHE_INVALIDATE_ALL()
#endif
#ifndef HAL_DCACHE_IS_ENABLED
# define HAL_DCACHE_IS_ENABLED(_state_)     \
    CYG_MACRO_START                         \
    (_state_) = 0;                          \
    CYG_MACRO_END
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_CACHE_H
// End of hal_cache.h
