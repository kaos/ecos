|==========================================================================
|
|      vectors.S
|
|      ColdFire exception vectors
|
|==========================================================================
|###ECOSGPLCOPYRIGHTBEGIN####
| -------------------------------------------
| This file is part of eCos, the Embedded Configurable Operating System.
| Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
| Copyright (C) 2006 eCosCentric Ltd.
|
| eCos is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free
| Software Foundation; either version 2 or (at your option) any later version.
|
| eCos is distributed in the hope that it will be useful, but WITHOUT ANY
| WARRANTY; without even the implied warranty of MERCHANTABILITY or
| FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
| for more details.
|
| You should have received a copy of the GNU General Public License along
| with eCos; if not, write to the Free Software Foundation, Inc.,
| 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
|
| As a special exception, if other files instantiate templates or use macros
| or inline functions from this file, or you compile this file and link it
| with other works to produce a work based on this file, this file does not
| by itself cause the resulting work to be covered by the GNU General Public
| License. However the source code for this file must still be made available
| in accordance with section (3) of the GNU General Public License.
|
| This exception does not invalidate any other reasons why a work based on
| this file might be covered by the GNU General Public License.
| -------------------------------------------
|###ECOSGPLCOPYRIGHTEND####
|=============================================================================
|#####DESCRIPTIONBEGIN####
|
| Author(s):    Enrico Piria
| Contributors: Wade Jensen
| Date:         2005-25-06
| Purpose:      ColdFire exception vectors
| Description:  This file contains the first level default VSRs
|               that save and restore state for both exceptions and
|               interrupts.
|
|####DESCRIPTIONEND####
|==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/hal/cf_offsets.inc>
#include <cyg/hal/arch.inc>
#include <cyg/hal/variant.inc>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

| ----------------------------------------------------------------------------
| Hardware reset vector

        .section ".boot","x"
        .balign 4
        .globl cyg_hal_reset_vsr
cyg_hal_reset_vsr:

        | Define the entry point for the linker.
        .globl _start
_start:

        | Make sure that all interrupts are masked.
        hal_cpu_int_disable

        | Initial setup. Just do the minimum to be able to perform
        | initialization in C.

        | Initialize CPU variant
        hal_cpu_init

        | Platform specific hardware initialization.
        | This may include memory controller initialization.
        hal_hardware_init

        | Setup boot stack
        hal_boot_stack_init
        
        | Set up the initial frame pointer.
        lea     0,%fp
        link    %fp,#0

        | Call the C routine to complete the reset process.
        .extern hal_reset
        jsr    hal_reset

        | If we return, stop.
9:
        stop    #0x2000
        bra     9b


| ----------------------------------------------------------------------------
| Default exception vector handler
|
| The default handler for all machine exceptions. We save the
| machine state and call the default C exception handler. This routine passes a
| pointer to the saved state to the C exception handler. The stack pointer in
| the saved state points to the the sp before the exception.
| The format/vector word in the exception stack contains the vector
| number.

| void hal_exception_handler(CYG_WORD vector, HAL_SavedRegisters *regs);
        
        .text
        .balign 4
        .globl cyg_hal_default_exception_vsr
cyg_hal_default_exception_vsr:

        | Disable all interrupts
        hal_cpu_int_disable

        | Preserve the entire state.
        | Allocate space for all registers (including the stack pointer).
        | Write all registers to the stack space.
        lea.l   -CYGARC_CF_EXCEPTION_DECREMENT(%sp),%sp
        movem.l %d0-%d7,CYGARC_CFREG_DREGS(%sp)
        movem.l %a0-%a6,CYGARC_CFREG_AREGS(%sp)

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        save_mac_registers %d0
#endif
                                            
        | Write the original stack pointer value to the stack.
        | The format/vector word, sr, and pc are already on the stack.
        find_original_sp %d0
        move.l   %d0,CYGARC_CFREG_SP(%sp)
                                           
        | Calculate the vector number. The format/vector word on the stack
        | contains the vector number.
        move.w  CYGARC_CF_FMTVECWORD(%sp),%d0
        and.l   #0x000003fc,%d0
        lsr.l   #2,%d0

        | Pass a pointer to the saved state to the exception handler.
        pea.l   (%sp)
                                            
        | Push the vector number parameter.
        move.l  %d0,-(%sp)

        | Call the default exception handler. This routine may modify
        | the exception context.
        .extern hal_exception_handler
        jsr    hal_exception_handler

        | Remove the vector number and the state pointer from the stack.
        addq.l  #2*4,%sp

        | Get a pointer to the location following the exception context.
        find_original_sp %d0
                                            
        | Restore all of the registers that we do not need in the following
        | code. We will copy all registers that are not restored here
        | to the new stack before restoring them.

#ifdef CYGHWR_HAL_COLDFIRE_MAC
        restore_mac_registers %d0
#endif
        
        movem.l CYGARC_CFREG_D2(%sp),%d2-%d7
        movem.l CYGARC_CFREG_A1(%sp),%a1-%a6
                                            
        | Load the address of the new SP.
        move.l  CYGARC_CFREG_SP(%sp),%d1
                                            
        | We now have:
        | d0.l : original stack pointer
        | d1.l : final stack pointer

        | ColdFire programmer's manual doesn't tell if rte instruction expects
        | the stack frame to be aligned at 32-bit boundaries.
        | So, we align the new stack value, and adjust the format field
        | accordingly. At the end of rte instruction the stack will thus point
        | to the desired location.

        | Compare the new stack address to the end of the exception context.
        | This will tell us the order that we need to copy the exception
        | stack and the remaining registers from the old exception context to
        | the new one. The order is important because the stack frames might
        | overlap.
        cmp.l   %d0,%d1

        | If the new SP and the old one coincide.
        beq     2f

        | If the new SP is at a higher address than the old one. 
        bgt     1f
                                            
        | The new SP is at a lower address than the old one. Copy from the
        | lowest address to the highest address.

        | Align stack at longword boundary
        move.l  %d1,%d0
        and.l   #0xfffffffc,%d0
        move.l  %d0,%a0

        | Allocate new frame
        sub.l   #CYGARC_CF_CONTEXT_SIZE,%a0

        | Copy D0, D1, A0, FVW, SR, and PC from the old stack to the new stack.
        | Note that we copy in ascending order.

        | Copy D0, D1, A0
        move.l  CYGARC_CFREG_D0(%sp),CYGARC_CFREG_D0(%a0)
        move.l  CYGARC_CFREG_D1(%sp),CYGARC_CFREG_D1(%a0)
        move.l  CYGARC_CFREG_A0(%sp),CYGARC_CFREG_A0(%a0)
        
        | Based on target SP address, construct new format field
        and.l   #0x00000003,%d1
        or.l    #0x4,%d1
        lsl.l   #8,%d1
        lsl.l   #4,%d1

        | Load old format field
        move.w  CYGARC_CF_FMTVECWORD(%sp),%d0

        | Clear old format field
        and.l   #0x0fff,%d0

        | Write the new one
        or.l    %d1,%d0
        move.w  %d0,CYGARC_CF_FMTVECWORD(%a0)

        | Copy SR and PC
        move.w  CYGARC_CF_SR(%sp),CYGARC_CF_SR(%a0)
        move.l  CYGARC_CFREG_PC(%sp),CYGARC_CFREG_PC(%a0)

        | A0 points to the top of the new stack
        move.l  %a0,%sp

        | Restore remaining registers and exit
        jmp 2f

1:

        | The new SP is at a higher address than the old one. Copy from the
        | highest address to the lowest address.
        
        | Align stack at longword boundary
        move.l  %d1,%d0
        and.l   #0xfffffffc,%d0
        move.l  %d0,%a0
        
        | Allocate new frame
        sub.l   #CYGARC_CF_CONTEXT_SIZE,%a0

        | Copy D0, D1, A0, FVW, SR, and PC from the old stack to the new stack.
        | Note that we copy in descending order.

        | Copy PC and SR
        move.l  CYGARC_CFREG_PC(%sp),CYGARC_CFREG_PC(%a0)
        move.w  CYGARC_CF_SR(%sp),CYGARC_CF_SR(%a0)

        | Based on target SP address, construct new format field
        and.l   #0x00000003,%d1
        or.l    #0x4,%d1
        lsl.l   #8,%d1
        lsl.l   #4,%d1

        | Load old format field
        move.w  CYGARC_CF_FMTVECWORD(%sp),%d0

        | Clear old format field
        and.l   #0x0fff,%d0

        | Write the new one
        or.l    %d1,%d0
        move.w  %d0,CYGARC_CF_FMTVECWORD(%a0)

        | Copy A0, D1, D0
        move.l  CYGARC_CFREG_A0(%sp),CYGARC_CFREG_A0(%a0)
        move.l  CYGARC_CFREG_D1(%sp),CYGARC_CFREG_D1(%a0)
        move.l  CYGARC_CFREG_D0(%sp),CYGARC_CFREG_D0(%a0)

        | A0 points to the top of the new stack
        move.l  %a0,%sp

2:
        | Restore remaining registers
        move.l  CYGARC_CFREG_D0(%sp),%d0
        move.l  CYGARC_CFREG_D1(%sp),%d1
        move.l  CYGARC_CFREG_A0(%sp),%a0
        add.l   #CYGARC_CF_EXCEPTION_DECREMENT,%sp
        
        | Return from exception
        rte


| ----------------------------------------------------------------------------
| Spurious interrupt vector handler
|
| Used for spurious and uninitialized interrupts.
| It is unknown at which priority spurious interrupts are generated. So, the
| safest thing to do is to disable all interrupts while processing spurious
| ones.

        .text
        .balign 4
        .globl cyg_hal_default_spurious_vsr
cyg_hal_default_spurious_vsr:

#ifndef CYGIMP_HAL_COMMON_INTERRUPTS_IGNORE_SPURIOUS

        | Disable all interrupts. On the first instruction, interrupt sampling
        | is always disabled.
        hal_cpu_int_disable

        | Preserve all registers that this handler needs to preserve.
        | The C code will preserve all other registers.
        int_pres_regs

        | Pass a pointer to the saved state to the interrupt handler.
        pea.l   (%sp)
        
        | Call spurious interrupt handler
        .extern hal_spurious_interrupt
        jsr hal_spurious_interrupt

        | Remove the arguments from the stack.
        addq.l  #4,%sp

        | Restore the preserved registers for the current thread.        
        int_rest_regs

#endif /* ifndef CYGIMP_HAL_COMMON_INTERRUPTS_IGNORE_SPURIOUS */
        
        | Just return from interrupt.
        rte


| ----------------------------------------------------------------------------
| User interrupt vector handler
|
| Control is transferred here from a user interrupt vector (#64-255).
| Before branching to common code, load a value to translate the
| vector table offset to the ISR table offset.

        .text
        .balign 4
        .globl cyg_hal_default_interrupt_vsr
cyg_hal_default_interrupt_vsr:

        | Disable all interrupts. On the first instruction, interrupt sampling
        | is always disabled.
        hal_cpu_int_disable

        | Preserve all registers that this handler needs to preserve.
        | The C code will preserve all other registers.
        int_pres_regs

        | It is safe to use breakpoints below this point.
        .globl _cyg_hal_default_interrupt_vsr_bp_safe
_cyg_hal_default_interrupt_vsr_bp_safe:
        
        | Adding this value to the vector table offset will result in the
        | corresponding offset into the ISR table.
        move.l #(-CYGNUM_HAL_ISR_MIN)*4,%d2

        | d2.l: Contains a value to translate the vector table offset to
        | the ISR table offset.

        | Calculate the vector offset. The format/vector word on the stack
        | contains the vector number. Mask off all unused bits. The bit
        | position of the vector number field makes it automatically multiplied
        | by four.
        move.w  CYGARC_CF_FMTVECWORD(%sp),%d1
        and.l   #0x000003fc,%d1

        | Calculate the ISR table offset. Add the vector table offset to the
        | translation value.
        add.l   %d1,%d2

        | Calculate the vector number using the vector table offset.
        asr.l   #2,%d1

        | d2.l: Contains the offset into the ISR table.
        | d1.l: Contains the vector number.

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)

        | If we are supporting Ctrl-C interrupts from GDB, we must squirrel
        | away a pointer to the saved interrupt state here so that we can
        | plant a breakpoint at some later time.

        .extern hal_saved_interrupt_state
        move.l %sp,(hal_saved_interrupt_state)

#endif

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

        | Lock the scheduler if we are using the kernel.
        .extern cyg_scheduler_sched_lock
        addq.l  #1,cyg_scheduler_sched_lock

#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
        
        | a0 = sp. We'll need it later
        move.l %sp,%a0

        cmp.l #__interrupt_stack_base,%sp
        
        | If sp < base : not on istack
        blt 1f
        
        cmp.l #__interrupt_stack,%sp
        
        | If sp <= top : already on istack
        ble 2f

1:
        | Switch to istack
        lea __interrupt_stack,%sp

2:
        | Save old SP on istack
        pea (%a0)

#endif

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

        .extern cyg_instrument

        | Save d1
        move.l %d1,-(%sp)

        | arg2 = 0
        move.l #0,-(%sp)

        | arg1 = vector number
        move.l %d1,-(%sp)

        | type = INTR,RAISE
        move.l #0x0301,-(%sp)

        | Call instrumentation
        jsr cyg_instrument

        | Remove args from stack
        add.l #12,%sp

        | Restore %d1
        move.l (%sp)+,%d1

#endif


#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

        | If interrupt nesting is enabled, we have to determine the IPL of the
        | current interrupt. We inline the following macro, which is defined
        | by ColdFire variants. The vector number of the current interrupt
        | is passed in d0, and the return value is in d0.
        | Registers a0-a1/d0-d1 are for use by the macro, other registers
        | must be saved explicitly before being used.

        | Save %d1
        move.l %d1,-(%sp)

        | Pass d1 as argument to macro
        move.l %d1,%d0

        | Retrieve IPL, which will be contained in d0
        hal_variant_retrieve_ipl

        | Shift IPL up to the same position occupied in sr
        lsl.l #8,%d0

        | Transform d0 in a mask to be applied to sr
        or.l #0xfffff0ff,%d0

        | Update sr. Use d1 as working register
        move.w %sr,%d1
        and.l %d0,%d1
        move.w %d1,%sr

        | Restore d1
        move.l (%sp)+,%d1

#endif

        | We need to call the following routines. The isr address, data, and
        | intr are all from the ISR table. The interrupt_end routine is
        | only called if we are using the kernel. regs points to the saved
        | registers on the stack. isr_ret is the return value from the ISR.
        | vector is the vector number.

        | static cyg_uint32 isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)

        | externC void interrupt_end(cyg_uint32 isr_ret, Cyg_Interrupt *intr,
        |     HAL_SavedRegisters  *regs)


        | Push the data value from the table.
        .extern cyg_hal_interrupt_data
        lea     cyg_hal_interrupt_data,%a0
        move.l  (%a0,%d2.l),-(%sp)

        | Get the address of the ISR from the table.
        .extern cyg_hal_interrupt_handlers
        lea     cyg_hal_interrupt_handlers,%a0
        move.l  (%a0,%d2.l),%a0

        | Push the vector number parameter.
        move.l  %d1,-(%sp)

        | Call the ISR.
        jsr   (%a0)

        | Remove the isr parameters from the stack.
        addq.l  #4*2,%sp
                                            
        | d0.l now contains the return value from the ISR.

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

        | If we are returning from the last nested interrupt, move back
        | to the thread stack. interrupt_end() must be called on the
        | thread stack since it potentially causes a context switch.
        | Since we have arranged for the top of stack location to
        | contain the sp we need to go back to here, just pop it off
        | and put it in SP.

        move.l (%sp),%sp                           | sp = *sp
        
#endif

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

        | We only need to call interrupt_end() when there is a kernel
        | present to do any tidying up. To keep the following code simple,
        | we enable all interrupts before calling DSRs only if a common
        | interrupt stack is in use.

        | Push the regs pointer.
        pea     (%sp)

        | Push the intr object pointer from the table.        
        .extern cyg_hal_interrupt_objects
        lea     cyg_hal_interrupt_objects,%a0
        move.l  (%a0,%d2.l),-(%sp)

        | Push ISR return value
        move.l %d0,-(%sp)

        | Even when this is not the last nested interrupt, we must call
        | interrupt_end() to post the DSR and decrement the scheduler
        | lock.

        | Call the interrupt_end C routine.
        .extern interrupt_end
        jsr    interrupt_end

        | Remove the isr_ret, intr, and regs parameters from the stack.
        lea     (4*3)(%sp),%sp
                                            
#endif /* ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

        | Restore the preserved registers for the current thread.        
        int_rest_regs

        | Restore the SR and PC.
        rte


| ----------------------------------------------------------------------------
| Execute pending DSRs on the interrupt stack with interrupts enabled.
| Note: this can only be called from code running on a thread stack

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

        .extern cyg_interrupt_call_pending_DSRs

FUNC_START(hal_interrupt_stack_call_pending_DSRs)
        | Change to interrupt stack, save state and set up stack for
        | calls to C code.
        | By virtue of GNU C calling conventions, we are free to use registers
        | %d0-%d1 and %a0-%a1 without saving them.

        | a0 = sp
        move.l %sp, %a0

        | Switch to istack
        lea __interrupt_stack,%sp

        | Save old SP on istack
        pea (%a0)

        | Save sr
        move.w %sr,%d0
        move.l %d0,-(%sp)

        | Enable interrupts
        hal_cpu_int_enable %d0
        
        | Call into kernel which will execute DSRs
        jsr cyg_interrupt_call_pending_DSRs

        move.l (%sp)+,%d0

        | Restore previous interrupt state
        hal_cpu_int_merge %d0,%d1

        | Restore sp
        move.l (%sp),%sp

        | return to caller
        rts

#endif


| ----------------------------------------------------------------------------
| Interrupt and reset stack
|
| WARNING: Do not put this in any memory section that gets initialized.
| Doing so may cause the C code to initialize its own stack.

        .section ".uninvar","aw",@nobits

        .balign 16
        .global cyg_interrupt_stack_base
cyg_interrupt_stack_base:
__interrupt_stack_base:
        .skip   CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
        .balign 16
        .global cyg_interrupt_stack
cyg_interrupt_stack:
__interrupt_stack:
        .skip   0x10

