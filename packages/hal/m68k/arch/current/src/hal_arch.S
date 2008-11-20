// #========================================================================
// #
// #    hal_arch.S
// #
// #    M68K support for contexts, exceptions and interrupts
// #
// #========================================================================
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2005,2006,2008 Free Software Foundation, Inc.
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
//============================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
//
//###DESCRIPTIONEND####
//========================================================================

        .file   "hal_arch.S"

#include <pkgconf/system.h>                
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>
#ifdef CYGPKG_KERNEL
        // For instrumentation options
# include <pkgconf/kernel.h>
#endif                
#include <cyg/hal/arch.inc>

// ----------------------------------------------------------------------------
// The system startup and/or interrupt stack, unless the platform HAL
// provides its own.
//
// The default behaviour is a separate interrupt stack via common HAL
// configuration options, with the interrupt stack reused as the
// startup stack. If the use of an interrupt stack is disabled then
// a separate startup stack is needed. Either the startup stack, the
// interrupt stack, or both can be provided by the platform HAL, for
// example to place the stack in an area of memory that is not otherwise
// readily usable.                
                
#if defined(_HAL_M68K_INSTANTIATE_STARTUP_STACK_) || defined(_HAL_M68K_INSTANTIATE_INTERRUPT_STACK_)
        .section        .bss
        .balign         4
# ifdef _HAL_M68K_INSTANTIATE_STARTUP_STACK_
        .global         _hal_m68k_startup_stack_base_
_hal_m68k_startup_stack_base_:        
# endif
# ifdef _HAL_M68K_INSTANTIATE_INTERRUPT_STACK_
        .global         _hal_m68k_interrupt_stack_base_
_hal_m68k_interrupt_stack_base_:        
# endif
# ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        .rept   CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
        .byte   0
        .endr
        .balign 4
# else
        .rept   CYGNUM_HAL_M68K_STARTUP_STACK_SIZE
        .byte   0
        .endr
        .balign 4
# endif
# ifdef _HAL_M68K_INSTANTIATE_STARTUP_STACK_
        .global _hal_m68k_startup_stack_
_hal_m68k_startup_stack_:        
# endif
# ifdef _HAL_M68K_INSTANTIATE_INTERRUPT_STACK_
        .global _hal_m68k_interrupt_stack_
_hal_m68k_interrupt_stack_:        
# endif                
        
#endif
        
// ----------------------------------------------------------------------------
// M68K calling conventions.
//
// The stack pointer always points at the first word of the stack that
// is currently in use. Pushing additional data requires a predecrement.
//        
// Frame pointers are not required but the compiler usually defaults to
// using them, to make debugging easier. This assembler code typically
// does not use a frame pointer.        
//
// All arguments are pushed onto the stack, never in registers. D0 is
// used for return values.        
//
// According to CALL_USED_REGISTERS in gcc/config/m68k/m68k.h,
// d0/d1/a0/a1/fp0/fp1 are caller-save, i.e. must be saved in an interrupt
// handler but can be ignored in a function called from C. The fp status
// registers are also caller-save.
//
// d2-d7/a2-a6/fp2-fp7 are callee-save registers, i.e. must be saved by
// a C function. fpsr is not callee-save, a C function can perform floating
// point operations and hence clobber the status. Similarly fpiar is not
// callee-save. fpcr is 
//
// Floating point support is only available on some variants. There can also
// be other hardware units such as multiply-accumulators which may have state
// that should be saved.

// ----------------------------------------------------------------------------
// Context switch support.
//
// The context consists of generic integer registers, possibly floating
// point registers, possibly another hardware unit, and space for pc/sr
// at the end in a variant-specific format.        
                
        .equ    hal_context_integer_offset,     0
        .equ    hal_context_integer_d0_offset,  0
        .equ    hal_context_integer_d2_offset,  (2 * 4)
        .equ    hal_context_integer_a0_offset,  (8 * 4)
        .equ    hal_context_integer_a2_offset,  ((8+2) * 4)
        .equ    hal_context_integer_size,       (15 * 4)
        
        .equ    hal_context_fpu_offset,         (hal_context_integer_offset + hal_context_integer_size)
#ifdef CYGIMP_HAL_M68K_FPU_SAVE
        .equ    hal_context_fpu_fpsr_offset,    (hal_context_fpu_offset + 0)
        .equ    hal_context_fpu_fpiar_offset,   (hal_context_fpu_offset + 4)
        .equ    hal_context_fpu_fp0_offset,     (hal_context_fpu_offset + 8)
        .equ    hal_context_fpu_fp2_offset,     (hal_context_fpu_fp0_offset + (2 * 12))
        .equ    hal_context_fpu_size,           ((2 * 4) + (8 * 12))
#else
        .equ    hal_context_fpu_size,           0        
#endif

        .equ    hal_context_other_offset,       (hal_context_fpu_offset + hal_context_fpu_size)
#ifdef HAL_CONTEXT_OTHER_SIZE
        .equ    hal_context_other_size,         HAL_CONTEXT_OTHER_SIZE
#else
        .equ    hal_context_other_size,         0                
#endif
        
        .equ    hal_context_pcsr_offset,        (hal_context_other_offset + hal_context_other_size)
#ifdef HAL_CONTEXT_PCSR_SIZE
        .equ    hal_context_pcsr_size,          HAL_CONTEXT_PCSR_SIZE
#else
        .equ    hal_context_pcsr_size,          8
#endif
        .equ    hal_context_size,               (hal_context_pcsr_offset + hal_context_pcsr_size)

// The offset to be used when switching to a new thread. For some variants
// this is 0 because the rte instruction will pop the entire pcsr part of
// the structure. For other variants some additional data may have to be popped.
#ifdef HAL_CONTEXT_PCSR_RTE_ADJUST
        .equ    hal_context_rte_adjust,         (hal_context_pcsr_offset + HAL_CONTEXT_PCSR_RTE_ADJUST)
#else
        .equ    hal_context_rte_adjust,         hal_context_pcsr_offset
#endif                

                
// Definitions for saving/restoring FPU context, if the FPU is part of the
// saved context. It can be assumed that %sp points at the thread context.
// FIXME: this code should probably use fsave/frestore to ensure that
// floating point operations have completed.        
#ifdef CYGIMP_HAL_M68K_FPU_SAVE

        .macro  hal_context_fpu_save_caller areg=%sp
        fmove.l         %fpsr, hal_context_fpu_fpsr_offset(\areg)
        fmove.l         %fpiar, hal_context_fpu_fpiar_offset(\areg)
        fmovem.x        %fp0-%fp1, hal_context_fpu_fp0_offset(\areg)
        .endm

        .macro  hal_context_fpu_load_caller areg=%sp
        fmove.l         hal_context_fpu_fpsr_offset(\areg), %fpisr
        fmove.l         hal_context_fpu_fpiar_offset(\areg), %fpiar
        fmovem.x        hal_context_fpu_fp0_offset(\areg),%fp0-%fp1
        .endm

        .macro  hal_context_fpu_save_callee areg=%sp
        fmovem.x        %fp2-%fp7, hal_context_fpu_fp2_offset(\areg)
        .endm

        .macro  hal_context_fpu_load_callee areg=%sp
        fmovem.x        hal_context_fpu_fp2_offset(\areg), %fp2-%fp7
        .endm

        .macro  hal_context_fpu_save_all areg=%sp
        fmove.l         %fpsr, hal_context_fpu_fpsr_offset(\areg)
        fmove.l         %fpiar, hal_context_fpu_fpiar_offset(\areg)
        fmovem.x        %fp0-%fp7, hal_context_fpu_fp0_offset(\areg)
        .endm

        .macro  hal_context_fpu_load_all areg=%sp
        fmove.l         hal_context_fpu_fpsr_offset(\areg), %fpsr
        fmove.l         hal_context_fpu_fpiar_offset(\areg), %fpiar
        fmovem.x        hal_context_fpu_fp0_offset(\areg), %fp0-%fp7
        .endm                         
#else
        .macro hal_context_fpu_save_caller areg=%sp
        .endm
        .macro hal_context_fpu_load_caller areg=%sp
        .endm
        .macro hal_context_fpu_save_callee areg=%sp
        .endm
        .macro hal_context_fpu_load_callee areg=%sp
        .endm
        .macro hal_context_fpu_save_all areg=%sp
        .endm
        .macro hal_context_fpu_load_all areg=%sp
        .endm
#endif

// No-op defaults for saving/restoring the OTHER context. If there is in fact
// something to be done then the variant HAL should have defined
// HAL_CONTEXT_OTHER_SIZE
#ifndef HAL_CONTEXT_OTHER_SIZE
        .macro hal_context_other_save_caller areg=%sp
        .endm
        .macro hal_context_other_load_caller areg=%sp
        .endm
        .macro hal_context_other_save_callee areg=%sp
        .endm
        .macro hal_context_other_load_callee areg=%sp
        .endm
        .macro hal_context_other_save_all areg=%sp
        .endm
        .macro hal_context_other_load_all areg=%sp
        .endm
#endif                

// ----------------------------------------------------------------------------
// LOAD_CONTEXT is called to start a new thread, usually only during system
// startup.
//
// void hal_thread_load_context(void* sp)

        FUNC_START(hal_thread_load_context)
        move.l  4(%sp),%sp
        hal_context_other_load_all
        hal_context_fpu_load_all
        movem.l hal_context_integer_offset(%sp),%d0-%d7/%a0-%a6
        add.l   #hal_context_rte_adjust, %sp
        rte

// ----------------------------------------------------------------------------
// void hal_thread_switch_context(void** from, void* to)

        FUNC_START(hal_thread_switch_context)
        
        // Space for the saved thread context. The PC is already on the
        // stack.  "from" is on the stack immediately after the context,
        // then "to".
        mov.l   4(%sp),%a0
        sub.l   # (hal_context_size - 4), %sp
        mov.l   %sp, 0(%a0)

#ifdef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM        
        // Save all the callee-save integer registers, so that they become
        // available to the other macros.
        movem.l %d2-%d7, hal_context_integer_d2_offset(%sp)
        movem.l %a2-%a6, hal_context_integer_a2_offset(%sp)

        // The status register must be saved here, since loading a thread
        // context always involves an rte instruction. The details are
        // variant-specific.
        hal_context_pcsr_save_sr %sp,0,%d0

        // Now the fpu and other contexts can be saved. All registers
        // are available.
        hal_context_fpu_save_callee
        hal_context_other_save_callee
#else
        movem.l %d0-%d7/%a0-%a6, hal_context_integer_offset(%sp)
        hal_context_pcsr_save_sr %sp,0,%d0
        hal_context_fpu_save_all
        hal_context_other_save_all
#endif                

        // All thread state has now been saved, so switch to the new one.
        mov.l   (hal_context_size+4)(%sp),%sp

#ifdef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
        hal_context_other_load_callee
        hal_context_fpu_load_callee
        movem.l hal_context_integer_d2_offset(%sp),%d2-%d7
        movem.l hal_context_integer_a2_offset(%sp),%a2-%a6
#else
        hal_context_other_load_all
        hal_context_fpu_load_all
        movem.l hal_context_integer_offset(%sp),%d0-%d7/%a0-%a6
#endif        

        add.l   #hal_context_rte_adjust, %sp
        rte                        
        
// ----------------------------------------------------------------------------
// setjmp()/longjmp(). See hal_arch.h for details.
//
// int  hal_m68k_setjmp(hal_jmp_buf);
// void hal_m68k_longjmp(hal_jmp_buf, val)       
        
        FUNC_START_WEAK(hal_m68k_setjmp)
        lea.l       4(%sp),%a1                  // return stack pointer. longjmp() does an indirect jmp, not an rts
        move.l      0(%a1),%a0                  // the jmp_buf structure
        move.l      0(%sp),0(%a0)               // return pc
        move.l      %a1,4(%a0)
        movem.l     %d2-%d7/%a2-%a6,8(%a0)      // 11 longs, occupying offsets 8 to 51
#ifdef CYGINT_HAL_M68K_VARIANT_FPU
        fmovem.x    %fp2-%fp7,52(%a0)
#endif                
        clr.l   %d0                             // setjmp() always returns 0
        rts

        FUNC_START_WEAK(hal_m68k_longjmp)
        move.l      8(%sp),%d0                  // val argument, and hence return value
        move.l      4(%sp),%a0                  // The jmp_buf structure
        movem.l     8(%a0),%d2-%d7/%a2-%a6
#ifdef CYGINT_HAL_M68K_VARIANT_FPU
        fmovem.x    52(%a0),%fp2-%fp7
#endif
        move.l      0(%a0),%a1                  // The return pc
        move.l      4(%a0),%sp
        jmp         (%a1)        

// ----------------------------------------------------------------------------
// Synchronous exceptions. Interrupts are disabled because that could confuse
// some code like gdb stubs (breakpoints also involve synchronous exceptions).
// Full state is saved on the current stack, then usually we switch to the
// interrupt stack and handle the rest of the exception there. That avoids 
// having to worry about gdb stubs stack requirements in every thread context.                
//
// It is assumed that on entry there is pc/sr context information already
// on the stack. Some or all of this will have been provided by the hardware.
// Other bits, e.g. the specific exception number, may have been pushed by
// software that then jumped here. The hardware may also have pushed some
// additional state which has already been removed or stashes elsewhere, 
// to keep things simple.
        
        .extern hal_m68k_exception_handler        

        FUNC_START(hal_m68k_exception_vsr)
        mov.w   #0x2700, %sr
        sub.l   #hal_context_pcsr_offset, %sp

        // If CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT is enabled
        // then only caller-save registers need to be saved here, but
        // exceptions should be rare and the additional info may prove
        // useful to higher-level code.
        movem.l %d0-%d7/%a0-%a6, hal_context_integer_offset(%sp)
        hal_context_fpu_save_all
        hal_context_other_save_all

        // Remember the current stack pointer in a callee-save register,
        // which have all been saved anyway. That makes it easier to restore
        // the stack later.
        move.l  %sp,%a2

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK        
        // Do we need to switch to the interrupt stack?
        cmpa.l  _HAL_M68K_INTERRUPT_STACK_, %sp
        jbgt    1f
        cmpa.l  _HAL_M68K_INTERRUPT_STACK_BASE_, %sp
        jbge    2f
1:      
        mova.l  _HAL_M68K_INTERRUPT_STACK_, %sp
2:
#endif
        // Zero out the frame pointer to encourage GDB to backtrace correctly
        suba.l  %a6, %a6
        
        // Now call into C with a single argument, the HAL_SavedRegisters
        mov.l   %a2,-(%sp)
        jbsr    hal_m68k_exception_handler

        // We can switch back to the right stack by re-using a2, irrespective
        // of whether or not we switched to the interrupt stack.
        move.l  %a2,%sp
        
        // Restore the entire state. In theory only the callee-save registers
        // should have changed, but some others may have been manipulated by
        // gdb.
        hal_context_other_load_all
        hal_context_fpu_load_all
        movem.l hal_context_integer_offset(%sp), %d0-%d7/%a0-%a6
        add.l   #hal_context_rte_adjust, %sp
        rte
                
// ----------------------------------------------------------------------------
// Interrupt handling.
//
// The current stack may be a thread stack or the interrupt stack.
// On some variants this code will be called directly by the hardware.
// On others there will have been a trampoline doing some stack
// manipulation and then jumping here. We assume that the stack is
// properly aligned and that the pc/sr part has been correctly saved.

        FUNC_START(hal_m68k_interrupt_vsr)

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
        // If nesting is not of interest then just disable all interrupts.
        // Otherwise interrupt nesting is controlled via the M68K IPL
        move.w  #0x2700, %sr
#endif        

        sub.l   # hal_context_pcsr_offset, %sp
#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        // Even for a minimum context the callee-save a2 register
        // is saved. This makes it easier to restore the stack
        // to the right place after the ISR call, allowing for
        // the optional interrupt stack.
        movem.l %d0-%d1,hal_context_integer_d0_offset(%sp)
        movem.l %a0-%a2,hal_context_integer_a0_offset(%sp)
        hal_context_fpu_save_caller %sp
        hal_context_other_save_caller %sp
#else
        movem.l %d0-%d7/%a0-%a6, hal_context_integer_d0_offset(%sp)
        hal_context_fpu_save_all %sp
        hal_context_other_save_all %sp
#endif                

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        // Let gdb stubs know which thread was interrupted, in case this
        // interrupt was a ctrl-C.
        .extern hal_saved_interrupt_state
        move.l  %sp, hal_saved_interrupt_state
#endif
        
        // d0/d1/a0/a1/a2 are now available. 
        // We want to extract the ISR vector while it is readily accessible. 
        // The details depend the particular M68K variant. The result should
        // be the vector << 2 to facilitate indexing. On some processors that
        // actually requires less code than getting the vector itself.
        hal_context_extract_isr_vector_shl2 %sp,0,%d0
        
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        // We need to call cyg_instrument(INTR.RAISE, vector, intr_number)
        // The actual interrupt number is not readily available, it depends
        // on the variant and possibly the platform, so just instrument 0
        // instead. The vector is preserved in callee-save a2 since it may
        // take several instructions to refetch it.
        movea.l %d0, %a2
        lsr.l   #2, %d0
        move.l  #0, -(%sp)
        move.l  %d0, -(%sp)
        move.l  #0x0301, -(%sp)
        jbsr    cyg_instrument
        add.l   #12, %sp
        move.l  %a2, %d0
#endif  

        // %d0 holds the ISR vector << 2. d1/a0/a1/a2 are available.

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT                 
        // Increment the scheduler lock. In theory the ISR should not look
        // at this so it could be deferred till just before the interrupt_end()
        // call, but this provides protection against badly written ISR's.
        //
        // This is not sufficient on SMP systems. The test of the scheduler lock 
        // below also needs attention.
        .extern cyg_scheduler_sched_lock
        addq.l  #1, cyg_scheduler_sched_lock
#endif        
        
        // At the end of the VSR we need to call interrupt_end().
        // The last two arguments are cyg_hal_interrupt_objects[vec]
        // and the saved registers. These are pushed now, while
        // vec is readily available, at the cost of a slight increase
        // in interrupt latency but saving some instructions later.
        move.l  %sp,-(%sp)
        lea     cyg_hal_interrupt_objects,%a0
        move.l  0(%a0,%d0), -(%sp)

        // We want to be able to restore the stack pointer to this location,
        // irrespective of whether or not an interrupt stack is being used.
        // Store it in a callee-save register.
        movea.l %sp, %a2
        
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        // Switch to the interrupt stack if we are not already running there.
        cmpa.l  _HAL_M68K_INTERRUPT_STACK_, %sp
        jbgt    1f
        cmpa.l  _HAL_M68K_INTERRUPT_STACK_BASE_, %sp
        jbge    2f
1:
        movea.l _HAL_M68K_INTERRUPT_STACK_, %sp
2:
#endif
        // FIXME: Some variation of the following should be considered to ensure that
        // GDB can't get permanently lost when doing a backtrace from within an ISR.
#if 0 && defined(CYGPKG_INFRA_DEBUG) && !defined(CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT)
        // Zero out the frame pointer to encourage GDB to backtrace correctly
        // But only if explicitly debugging so that we don't adversely affect
        // critical ISR latency
        move.w  #0,%a6
#endif

        // d0 is the isr vector << 2. a2 holds a saved stack pointer. d1/a0/a1 are available.
        // We need to call (*cyg_hal_interrupt_handlers[vec])(vec, cyg_hal_interrupt_data[vec])
        lea     cyg_hal_interrupt_data,%a0
        move.l  0(%a0,%d0),-(%sp)
        lea     cyg_hal_interrupt_handlers,%a0
        move.l  0(%a0,%d0),%a1
        lsr.l   #2,%d0
        move.l  %d0,-(%sp)
        jbsr    (%a1)

        // We now want to return to the right position on the current
        // stack, or to the thread stack if we switched to the interrupt
        // stack.
        movea.l %a2, %sp

        // Next we need to call interrupt_end(). If the scheduler was unlocked
        // at the time of the interrupt then that will run the DSRs, possibly
        // cause a context switch, and eventually return to this VSR. If the
        // scheduler was already locked then interrupt_end() will return to
        // the VSR almost immediately. The IPL level will be restored when
        // the VSR executes the final rte.
        //
        // DSRs should run with interrupts re-enabled. Ideally we want to
        // run them with the IPL level of the interrupted thread, and at
        // the end reset the IPL level to what it is now. That way this
        // VSR gets to return, without risk of unconstrained stack usage.
        // However this is very hard: currently the desired IPL levels
        // cannot be passed on the stack; they cannot be passed via globals
        // either because of the possibility of a context switch.
        //
        // Instead there is a defined IPL level for running all DSRs.
        // HAL_INTERRUPT_STACK_CALL_PENDING_DSRs() sets the IPL level to
        // that value, and restores the IPL level when done. For most
        // applications that will be fine, but it does make it more
        // difficult to manipulate IPL levels on a per-thread level
        // or anything similarly fancy.

        // We need to call interrupt_end(isr_return, cyg_hal_interrupt_objects[vec], saved_state_pointer)
        // The last two have already been pushed onto the stack. d0 holds the return value.
        move.l  %d0,-(%sp)
        jbsr    interrupt_end
        add.l   #12,%sp

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        hal_context_other_load_caller %sp
        hal_context_fpu_load_caller %sp
        movem.l hal_context_integer_a0_offset(%sp),%a0-%a2
        movem.l hal_context_integer_d0_offset(%sp),%d0-%d1
#else
        hal_context_other_load_all %sp
        hal_context_fpu_load_all   %sp
        movem.l hal_context_integer_d0_offset(%sp),%d0-%d7/%a0-%a6
#endif
        add.l   #hal_context_rte_adjust,%sp
        rte                

// ----------------------------------------------------------------------------
// On configurations with an interrupt stack HAL_INTERRUPT_STACK_CALL_PENDING_DSRS()
// is used to run the DSRs on the interrupt stack rather than the thread stack,
// reducing stack size requirements for the latter. It is called only on a thread
// stack. In addition to the stack switch the IPL level is manipulated here,
// ensuring that DSRs run with interrupts enabled.
//
// On other architectures HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() is only called
// when a separate interrupt stack is used. If the interrupt stack is disabled
// then DSRs run with interrupts disabled, which is a bad idea. On the M68K
// HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() is always defined, thus sorting out
// the interrupt level, but only does the stack switch if an interrupt stack        
// is actually being used.                

        .extern hal_m68k_dsr_ipl_level
        .extern cyg_interrupt_call_pending_DSRs

        FUNC_START(hal_interrupt_stack_call_pending_DSRs)
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK        
        // Use callee-save registers to store the current stack and status register
        // Also store the frame pointer so we can zero it to improve GDB debugging backtraces
        move.l  %a2,-(%sp)
        move.l  %d2,-(%sp)
        move.l  %a6,-(%sp)
        movea.l %sp,%a2
        move.w  %sr,%d2
        suba.l  %a6,%a6

        // There is a problem if the configuration contains the kernel but interrupts
        // are enabled while still running the startup code. The kernel's interrupt_end()
        // rather than the driver API's will be used, so this code gets to run. We now
        // get to switch to the interrupt stack while still on the startup stack, which
        // is a shame since the two are the same. This could be easily fixed by checking
        // the current stack as happens in the interrupt_vsr, but at the cost of an
        // extra four instructions in the DSR path. The scenario should probably not
        // arise so those four instructions are left out for now.
        movea.l _HAL_M68K_INTERRUPT_STACK_, %sp
        move.l  hal_m68k_dsr_ipl_level, %d0
        move.w  %d0, %sr
        
        jbsr    cyg_interrupt_call_pending_DSRs

        move.w  %d2, %sr
        movea.l %a2, %sp
        movea.l (%sp)+,%a6
        move.l  (%sp)+,%d2
        movea.l (%sp)+,%a2
        rts
#else
        move.l  %d2, -(%sp)
        move.w  %sr, %d2
        move.l  hal_m68k_dsr_ipl_level, %d0
        move.w  %d0, %sr
        jbsr    cyg_interrupt_call_pending_DSRs
        move.w  %d2, %sr
        move.l  (%sp)+, %d2
        rts
#endif // CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        
                
// ----------------------------------------------------------------------------
// A dummy entry point for unused entries in the system exception vector.

        FUNC_START(hal_m68k_rte)
        rte

// ----------------------------------------------------------------------------
// Fast implementations of lsbit_index() and msbit_index()

        FUNC_START(hal_lsbit_index)
        clr.l   %d0                     // result
        mov.l   4(%sp),%d1              // mask
        bne     1f                      // special case for 0
        subq.l  #1,%d0                  // lsbit_index(0) -> -1
        rts
1:              
        tst.w   %d1                     // anything set in the bottom 16 bits?
        bne     2f
        swap    %d1                     // the top 16 bits only are of interest
        moveq.l #16, %d0
2:      
        tst.b   %d1                     // anything set in the bottom 8 bits?
        bne     3f
        addq.l  #8, %d0                 // bottom byte is 0, switch to next byte
        lsr.l    #8, %d1
3:      
        move.l  %d1, %a0                // backup current data, so that we can manipulate it
        and.l   #0x0F, %d1              // anything in the bottom nibble?
        bne     4f
        move.l  %a0, %d1                // nope, restore and use the next nibble
        lsr.l    #4, %d1
        and.l   #0x0F, %d1
        addq.l  #4, %d0
4:      
                                        // The bottom nibble contains a number between 1 and 15
                                        // (not 0, that would have been caught by the test at the top).
        lea     5f, %a0
        mov.b   0(%a0,%d1),%d1          // only zap bottom byte, the other bytes are already 0
        add.l   %d1,%d0
        rts
5:
        // Every odd number has an index of 0. The first entry is irrelevent.
        .byte 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0

        FUNC_START(hal_msbit_index)
        clr.l   %d0                     // result
        mov.l   4(%sp), %d1             // mask
        bne     1f                      // special case for 0
        subq.l  #1, %d0                 // msbit_index(0) -> -1
        rts
1:
        cmpi.l  #0x0000FFFF,%d1
        bls     2f
        moveq.l #16,%d0
        clr.w   %d1
        swap    %d1
2:      
        cmpi.l  #0x00FF, %d1
        bls     3f
        addq.l  #8, %d0
        lsr.l   #8, %d1
3:      
        cmpi.l  #0x000F, %d1
        bls     4f
        addq.l  #4, %d0
        lsr.l   #4, %d1

4:      
        lea     5f, %a0
        mov.b   0(%a0,%d1), %d1
        add.l   %d1,%d0
        rts

5:
        .byte 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3

// ----------------------------------------------------------------------------
// Profiling support. When code is compiled with -pg the compiler inserts
// calls to mcount() at the start of each function. This can be used to
// build a callgraph of the application. mcount() is tied to the compiler
// internals and does not always use standard calling conventions.
//
// mcount() should call __profile_mcount() with two arguments, caller_pc
// and callee_pc, where callee_pc refers to the function that calls
// mcount() and caller_pc refers to the place where that function is
// called from. callee_pc is readily accessed relative to sp, caller_pc
// can be accessed using the frame pointer. This assumes the code has
// not been built with -fomit-frame-pointer, a safe assumption since
// the compiler disallows combining -pg and -fomit-frame-pointer.        
//
// The m68k compiler appears to implement mcount() a bit differently
// from other targets. It reserves a word for every function entry
// point which presumably is intended to act as the start of a linked
// list chain. On other targets the PC is used to index a hash table
// which contains the start of that linked list. That extra word is
// not used for eCos profiling since it would require customizing the
// profiling package on a per-target basis, so unfortunately the
// memory is wasted.
//
// It is assumed that d0/d1/a0/a1 (the callee-save registers) are
// available. a0 is certainly available because that is used to hold 
// the above per-function word. If the assumption is false then this
// would have to save d0/d1/a1 because the C __profile_mcount() could
// zap them.                
//
// This code is conditional on CYGPKG_PROFILE_GPROF since that provides
// the implementation of __profile_mcount.
#ifdef CYGPKG_PROFILE_GPROF                
        FUNC_START(mcount)
        // __profile_mcount() should be called with interrupts disabled.
        move.w  %sr,%d0
        move.l  %d0,-(%sp)
        // The callee-pc corresponds to the return address on the stack
        move.l  4(%sp),%a0
        move.l  %a0,-(%sp)
        // The caller-pc is accessible relative to the frame pointer a6
        move.l  4(%a6),%a0
        move.l  %a0,-(%sp)
        
        move.w  #0x2700, %sr
        jbsr    __profile_mcount

        move.l  8(%sp),%d0
        add.l   #12, %sp
        move.w  %d0,%sr
        
        rts
#endif
                
        .end
