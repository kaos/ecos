// #========================================================================
// #
// #    mcfxxxx_asm.S
// #
// #    Miscellaneous assembler support functions for an mcfxxxx
// #
// #========================================================================
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
//============================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2008-01-14
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal_m68k_mcfxxxx.h>
#include <cyg/hal/arch.inc>
#include <cyg/hal/var_io.h>

        .file   "mcfxxxx_asm.S"

#ifdef CYGINT_HAL_M68K_MCFxxxx_SOFTWARE_PROFILE_TIMER        
        // See also hal_enable_profile_timer() in mcfxxxx.c
        
        // This VSR should call __profile_hit() with a single argument,
        // the interrupted PC. The VSR has been installed as a 
        // high-priority interrupt source, so there is no need to worry
        // about nested interrupts or an interrupt stack.
        .extern __profile_hit
        
        FUNC_START(hal_mcfxxxx_profile_vsr)
        // On entry sp[0] holds sr and sp[1] holds the pc.
        // Save the caller-save integer registers. There are no floating
        // point registers to worry about. Make space for the argument
        // to __profile_hit() while we are at it.
        sub.l   #20, %sp
        movem.l %d0-%d1/%a0-%a1, 4(%sp)

        // Now pick up the interrupted PC from its new offset and push it
        // on the stack for __profile_hit().
        move.l  24(%sp),(%sp)
        jbsr    __profile_hit

        // The timer runs in restart mode, but the interrupt bit has to be reset.
        move.l # (HAL_MCFxxxx_PITx_PCSR_PRE_64 + HAL_MCFxxxx_PITx_PCSR_OVW +            \
                     HAL_MCFxxxx_PITx_PCSR_PIE + HAL_MCFxxxx_PITx_PCSR_PIF +            \
                     HAL_MCFxxxx_PITx_PCSR_RLD + HAL_MCFxxxx_PITx_PCSR_EN), %d0
        move.w  %d0, HAL_MCFxxxx_PROFILE_TIMER_BASE + HAL_MCFxxxx_PITx_PCSR
        
        movem.l 4(%sp), %d0-%d1/%a0-%a1
        add.l   #20, %sp
        rte
#endif
        
        .end
