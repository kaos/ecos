/*==========================================================================
//
//      context.S
//
//      Cortex-M context switch code
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-07-30
// Description:  This file contains thread context switch code.
//
//####DESCRIPTIONEND####
//
//========================================================================
*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

//==========================================================================

        .syntax unified
        .thumb
        .text

//==========================================================================
// Context switch
//
// R0 contains a pointer to the SP of the thread to load, R1 contains
// a pointer to the SP of the current thread.

        .globl  hal_thread_switch_context
        .thumb
        .thumb_func
        .type   hal_thread_switch_context, %function
hal_thread_switch_context:

        push    {r0-r12,lr}             // Push all savable register
        mov     r2,#2                   // Set state type == thread
        mrs     r3,basepri              // Get priority base register
        mov     r4,sp                   // Get SP (for info only)
        push    {r2-r4}                 // Push them

        str     sp,[r1]                 // Save SP

        // Fall through

//--------------------------------------------------------------------------
// Load context
//
// This is used to load a thread context, abandoning the current one. This
// function is also the second half of hal_thread_switch_context.

        .globl  hal_thread_load_context
        .thumb
        .thumb_func
        .type   hal_thread_load_context, %function
hal_thread_load_context:

        ldr     sp,[r0]                 // Load SP
        pop     {r2-r4}                 // Pop type, basepri and SP (discarded)
        msr     basepri,r3              // Set BASEPRI
        pop     {r0-r12,pc}             // Pop all register and return

//==========================================================================
//  HAL longjmp, setjmp implementations
//
//  hal_setjmp saves only to callee save registers R4-14
//  and LR into buffer supplied in r0[arg0].

        .globl  hal_setjmp
        .thumb
        .thumb_func
        .type   hal_setjmp, %function
hal_setjmp:
        mov     r3,sp
        stmea   r0,{r3-r12,r14}
        mov     r0,#0
        bx      lr

//  hal_longjmp loads state from r0[arg0] and returns

        .globl  hal_longjmp
        .thumb
        .thumb_func
        .type   hal_longjmp, %function
hal_longjmp:
        ldmfd   r0,{r3-r12,r14}
        mov     sp,r3
        mov     r0,r1           // return [arg1]
        bx      lr

//==========================================================================
// EOF context.S
