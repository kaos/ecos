#ifndef CYGONCE_HAL_PROC_ARCH_H
#define CYGONCE_HAL_PROC_ARCH_H
//=============================================================================
//
//      proc_arch.h
//
//      Processor variant specific abstractions
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
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal_m68k_mcf5272.h>
#include <cyg/hal/plf_arch.h>
#include <cyg/infra/cyg_type.h>

// A processor-specific initialization routine is needed
#if !defined(__ASSEMBLER__) && !defined(HAL_M68K_PROC_INIT)
externC void hal_m68k_mcf5272_init(void);
#define HAL_M68K_PROC_INIT()    hal_m68k_mcf5272_init()
#endif

// The MCF5272 does not support the bitrev or ff1 instructions so the
// architectural default versions of HAL_LSBIT_INDEX() and
// HAL_MSBIT_INDEX() have to be used instead of the mcfxxxx ones.
#define _HAL_M68K_MCFxxxx_NO_FF1_    1

// Idle action support.
#if !defined(HAL_IDLE_THREAD_ACTION)
# if defined(CYGIMP_HAL_M68K_MCF5272_IDLE_run)
    // Do nothing, just use the architectural empty default
# else

# define HAL_IDLE_THREAD_ACTION(_count_)                    \
    CYG_MACRO_START                                         \
    asm volatile (                                          \
        "mov.w  %%sr, %%d0 ; \n"                            \
        "mov.w  %%d0, 0(%[alpr]); \n"                       \
        "stop   #0x2000 ; \n"                               \
        "mov.w  %%d0, %%sr ; \n"                            \
        :                                                   \
        : [alpr] "a" (HAL_MCFxxxx_MBAR + HAL_MCF5272_ALPR)  \
        : "d0" );                                           \
    CYG_MACRO_END

# endif
#endif

// ----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PROC_ARCH_H

