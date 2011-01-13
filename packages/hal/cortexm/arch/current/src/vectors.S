/*==========================================================================
//
//      vectors.S
//
//      Cortex-M exception vectors
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.                        
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
// Description:  This file defines the code placed into the exception
//               vectors. It also contains the first level default VSRs
//               that save and restore state for both exceptions and
//               interrupts.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/hal/variant.inc>

//==========================================================================

        .syntax unified
        .thumb

//==========================================================================
// Initial exception vector table
//
// This only contains the stack and entry point for reset. The table
// to be used at runtime is constructed by code in hal_reset_vsr().

        .section        ".vectors","ax"

        .global         hal_vsr_table
hal_vsr_table_init:

        .long           hal_startup_stack               //  0 Reset stack
        .long           hal_reset_vsr                   //  1 Reset entry

//==========================================================================
        
        .text
        .thumb
        
//==========================================================================
// Fake entry point.
//
// The ELF file entry point points here. When loading an executable
// via RedBoot/Stubs or via JTAG the PC will be set to this address.
// The code here sets up the SP and branches to the reset VSR in
// emulation of the hardware reset behaviour.
        
        .align  2
        .global reset_vector
        .thumb
        .thumb_func
        .type   reset_vector, %function
reset_vector:

        ldr     sp,=hal_startup_stack
        b       hal_reset_vsr

        .pool

#if !defined(CYG_HAL_STARTUP_RAM)        
//==========================================================================
// State switch VSR
//
// This is called from the init code to switch execution from the main
// stack to the process stack. We also take the opportunity to do some
// other things that are best done in asm code such as disabling interrupts
// and setting the control register.
//
// The adjustment to MSP by 1/2 interrupt stack size allows code to
// throw exceptions without corrupting the execution stack. This is
// only necessary for non-kernel configurations (e.g. RedBoot, Stubs)
// since kernel configurations will switch to a thread stack before
// they should throw an exception.        
        
        .global hal_switch_state_vsr
        .thumb
        .thumb_func
        .type   hal_switch_state_vsr, %function
hal_switch_state_vsr:

        mov     r0,#CYGNUM_HAL_CORTEXM_PRIORITY_MAX
        msr     basepri,r0
        
        mov     r0,#2                   // Set CONTROL register to 2
        msr     control,r0
        isb                             // Insert a barrier
        
        mov     r0,sp
        msr     psp,r0                  // Copy SP to PSP

#if !defined(CYGPKG_KERNEL)
        sub     sp,#(CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE/2)
#endif        
        
        orr     lr,#0xD                 // Adjust return link
        bx      lr                      // Return to init code on PSP now

#endif
        
//==========================================================================
// Default exception VSR
//
// This is attached to all exception vectors. It saves the entire
// machine state and calls into the eCos exception handling code.
//
// NOTE: At present this implementation does not permit an exception
// handler to suspend the faulting thread and enter the scheduler to
// switch elsewhere. However, I know of no code that does anything
// like this. If there is then this may need treating in the same way
// as the interrupt end code.
        
        .global hal_default_exception_vsr
        .thumb
        .thumb_func
        .type   hal_default_exception_vsr, %function
hal_default_exception_vsr:

        mrs     r0,psp                  // Get process stack
        sub     r1,r0,#(4*12)           // Make space for saved state
        msr     psp,r1                  // Ensure PSP is up to date
        
        mov     r1,#1                   // R1 = exception state type
        mrs     r2,ipsr                 // R2 = vector number
        mrs     r3,basepri              // R3 = basepri
        stmfd   r0!,{r1-r11,lr}         // Push type, vector, basepri, r4-11 

        mov     r4,r0                   // R4 = saved state pointer

        bl      hal_deliver_exception

        mov     r0,r4                   // R0 = state saved across call
        ldmfd   r0!,{r1-r11,lr}         // Pop type, vec, basepri, registers and LR
        msr     psp,r0                  // Restore PSP
        msr     basepri,r3              // Restore basepri
        
        bx      lr                      // Return
        
        .pool

//==========================================================================
// Default interrupt VSR
//
// This is a trampoline that translates from the hardware defined entry point
// to the ISR defined by eCos. The CPU will switch automatically to the main
// (interrupt) stack with the process state saved on the process stack. Apart
// from saving a pointer to the interrupt state for Ctrl-C support, and fetching        
// the vector number, most of the work is actually done in hal_deliver_interrupt().


        .global hal_default_interrupt_vsr
        .thumb
        .thumb_func
        .type   hal_default_interrupt_vsr, %function
hal_default_interrupt_vsr:

        push    {lr}                    // Save return link
        sub     sp,#4                   // Realign SP to 8 bytes
        
#if CYGINT_HAL_COMMON_SAVED_INTERRUPT_STATE_REQUIRED > 0
        // If we are supporting Ctrl-C interrupts from GDB, we must squirrel
        // away a pointer to the saved interrupt state here so that we can
        // plant a breakpoint at some later time.

       .extern  hal_saved_interrupt_state
        mrs     r1,psp                  // Get PSP
        mov     r0,#3                   // Interrupt state type        
        stmfd   r1!,{r0}                // Push interrupt type
        ldr     r12,=hal_saved_interrupt_state
        str     r1,[r12]
#endif
        
        mrs     r0,ipsr                 // R0 = arg0 = vector number
        sub     r0,#15                  // Adjust to interrupt range

        bl      hal_deliver_interrupt

        add     sp,#4                   // pop alignment padding
        pop     {pc}                    // Pop LR and return
        
        .pool

//==========================================================================
// Pendable SVC VSR
//
// This is invoked if an interrupt posts a DSR. It calls the DSR
// and finalizes interrupt processing by calling interrupt_end(). We want
// to run interrupt_end() on the PSP of the current thread. So we push
// a fake exception frame onto the PSP which will take us to hal_interrupt_end(),
// which will make the call. The return link loaded by that frame takes us
// back to hal_interrupt_end_done which will unwind the real exception
// frame that is still on the PSP.        

        
        .global hal_pendable_svc_vsr
        .thumb
        .thumb_func
        .type   hal_pendable_svc_vsr, %function
hal_pendable_svc_vsr:

        mrs     r12,psp                 // R12 = thread's PSP
        sub     r0,r12,#0x20            // Make space for frame
        msr     psp,r0                  // Put it back
        
        ldr     r3,=0x01000000          // R3 = PSR = thumb bit set
        ldr     r2,=hal_interrupt_end   // R2 = PC = interrupt end entry point
        ldr     r1,=hal_interrupt_end_done // R1 = LR = restore code
        stmfd   r12!,{r0-r3}            // Save fake R12, LR, PC, PSR
        stmfd   r12!,{r0-r3}            // Save fake R0-R3
        
        bx      lr                      // Return to hal_interrupt_end

        .pool
        
//==========================================================================                        
// Interrupt end done
//
// After calling interrupt end a thread returns here to unstack the
// exception frame used to enter hal_pendable_svc_vsr. We can only
// successfully unstack a frame by doing a proper exception return
// from handler mode, so we use a SWI which will discard its own
// frame and restore the saved one.

        
        .global hal_interrupt_end_done
        .thumb
        .thumb_func
        .type   hal_interrupt_end_done, %function
hal_interrupt_end_done:

        ldr     r3,=hal_interrupt_end_vsr
        swi 0
        
//==========================================================================        
// Interrupt end VSR
//
// This is the SVC VSR invoked by hal_interrupt_end_done to restore the
// original exception frame from a pendable SVC entry. It does this
// by discarding its own frame and using the one below it on the
// stack to return.
        
        .global hal_interrupt_end_vsr
        .thumb
        .thumb_func
        .type   hal_interrupt_end_vsr, %function
hal_interrupt_end_vsr: 
        
        mrs     r12,psp                 // R12 = thread's PSP
        add     r12,#32                 // Skip our saved state
        msr     psp,r12                 // Restore thread's PSP

        bx      lr                      // And return

//==========================================================================
// Run DSRs VSR
//
// This is invoked from the kernel via a SWI to run DSRs on the
// interrupt/main stack. It merely branches to
// cyg_interrupt_call_pending_DSRs() which will then directly return
// from the SVC exception.

        .global hal_call_dsrs_vsr
        .thumb
        .thumb_func
        .type   hal_call_dsrs_vsr, %function
        
hal_call_dsrs_vsr:

        .extern cyg_interrupt_call_pending_DSRs
        b       cyg_interrupt_call_pending_DSRs

//==========================================================================
// SVC VSR
//
// The SVC VSR is used as a general-purpose mechanism for running code
// in handler mode. R3 contains the address of a piece of code to run,
// R0-R2 contain any arguments. Once entered the code is responsible for
// handling the system state and returning to thread mode.
//        
// Note that R0-R3 must be explicitly restored from their stacked
// copies since a late arriving interrupt can preempt the SVC entry
// and corrupt these registers before we get here.

        .global hal_default_svc_vsr
        .thumb
        .thumb_func
        .type   hal_default_svc_vsr, %function
hal_default_svc_vsr:

        mrs     r12,psp
        ldmfd   r12,{r0-r3}
        bx      r3                      // Jump to routine in R3

        .pool
        
//==========================================================================        
// end of vectors.S
