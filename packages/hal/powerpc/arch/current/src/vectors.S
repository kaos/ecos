##==========================================================================
##
##      vectors.S
##
##      PowerPC exception vectors
##
##==========================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.1 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://www.redhat.com/                                                   
#                                                                          
# Software distributed under the License is distributed on an "AS IS"      
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
##==========================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):    nickg, jskov
## Contributors: nickg, jskov
## Date:         1999-02-20
## Purpose:      PowerPC exception vectors
## Description:  This file defines the code placed into the exception
##               vectors. It also contains the first level default VSRs
##               that save and restore state for both exceptions and
##               interrupts.
##
######DESCRIPTIONEND####
##
##==========================================================================

#===========================================================================
#
#       The PowerPC exception handling has changed as of version 1.3.1.
#       The primary motivation for rewriting the code was to bring it more
#       in line with the other HALs, in particular to allow a RAM application
#       to cleanly take over only a subset of vectors from a running ROM
#	monitor.
#
#       GDB stubs (and CygMon, should it be ported to PPC) copies
#       exception vector entry handler code to address 0. These vector entry
#       handlers (defined by the exception_vector macro below) compute
#       a vector index into the hal_vsr_table, fetch the pointer, and
#       jump to the HAL vector service routine (VSR).
#
#       The hal_vsr_table is located immediately after the vector
#       handlers (at address 0x3000), allowing RAM applications to
#       change VSRs as necessary, while still keeping desired ROM
#       monitor functionality available for debugging.
#
#       ROM applications can still be configured to leave the vector entry
#       handlers at 0xff000000, but there is at the moment no
#       provision for reclaiming the reserved vector space in RAM to
#       application usage.
#
#       RAM applications can also be configured to provide exception
#       handlers which are copied to address 0 on startup, thus taking
#       full control of the target.
#
#
#       Default configuration is for RAM applications to rely on an
#       existing ROM monitor to provide debugging functionality, and
#       for ROM applications to copy vectors to address 0.
#
#
#       Unfortunately the new exception scheme is not compatible with the
#       old scheme. Stubs and applications must be compiled using the same
#       scheme (i.e., old binaries will not run with new stubs, and new
#       binaries will not run with old stubs).
#
#===========================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>     // CYGPKG_KERNEL_INSTRUMENT
#endif

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include "cyg/hal/ppc_regs.h"        
#include "cyg/hal/ppc.inc"
        
#===========================================================================
                
        .file   "vectors.S"
        
        .extern hal_interrupt_data
        .extern hal_interrupt_handlers
        .extern hal_interrupt_objects
        .extern hal_vsr_table

        .extern cyg_hal_invoke_constructors
        .extern cyg_instrument
        .extern cyg_start
        .extern hal_IRQ_init
        .extern hal_MMU_init
        .extern hal_enable_caches
        .extern hal_hardware_init
        .extern initialize_stub

        .extern __bss_start
        .extern __bss_end
        .extern __sbss_start
        .extern __sbss_end

#===========================================================================
# MSR initialization value
# zero all bits except:
# FP = floating point available
# ME = machine check enabled
# IP = vectors at 0xFFFxxxxx (ROM startup only)
# IR = instruction address translation
# DR = data address translation
# RI = recoverable interrupt

#define CYG_MSR_COMMON (MSR_FP | MSR_ME | MSR_IR | MSR_DR | MSR_RI)
#if defined(CYG_HAL_STARTUP_ROM)
# ifdef CYGSEM_HAL_POWERPC_COPY_VECTORS
#  define CYG_MSR CYG_MSR_COMMON
# else
#  define CYG_MSR (CYG_MSR_COMMON | MSR_IP)
# endif
#elif defined(CYG_HAL_STARTUP_RAM)
# define CYG_MSR CYG_MSR_COMMON
#endif

#ifdef CYGPKG_HAL_POWERPC_SIM
# When building for SIM, don~t enable MMU -- it~s not needed since caches
# are disabled, and there is a runtime simulation overhead.
#undef CYG_MSR
#define CYG_MSR (CYG_MSR_COMMON & ~(MSR_IR | MSR_DR))
#endif

#define CYG_MSR_NO_INTS (CYG_MSR & ~MSR_ME)        

# Include variant macros after MSR definition.        
#include "cyg/hal/variant.inc"


#===========================================================================
# If the following option is enabled, we only save registers up to R12.
# The PowerPC ABI defines registers 13..31 as callee saved and thus we do
# not need to save them when calling C functions.

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
# define MAX_SAVE_REG    12
#else
# define MAX_SAVE_REG    31
#endif                  
        
#===========================================================================
# Start by defining the exceptions vectors that must be placed at
# locations 0xFFF00000 or 0x00000000. The following code will normally
# be located at 0xFFF00000 in the ROM. It may optionally be copied out
# to 0x00000000 if we want to use the RAM vectors. For this reason this code
# MUST BE POSITION INDEPENDENT.
        
        .section ".vectors","ax"

#---------------------------------------------------------------------------
# Macros for generating an exception vector service routine

# Reset vector macro

        .macro  reset_vector name
        .p2align 8
        .globl  __exception_\name
__exception_\name:
        lwi     r3,_start
        mtlr    r3
        blr
        .endm
        
# Generic vector macro
                
        .macro  exception_vector name
        .p2align 8
        .globl  __exception_\name
__exception_\name:
        mtspr   SPRG1,r3                        # stash some work registers away
        mtspr   SPRG2,r4
        mtspr   SPRG3,r5
        mfcr    r4                              # stash CR
        li      r5,__exception_\name@L          # load low half of vector address
        srwi    r5,r5,6                         # shift right by 6      
        lwi     r3,hal_vsr_table                # table base
        lwzx    r3,r3,r5                        # address of vsr
        mflr    r5                              # save link register
        mtlr    r3                              # put vsr address into it
        li      r3,__exception_\name@L          # reload low half of vector address     
        blr                                     # go to common code
        .endm
        
#---------------------------------------------------------------------------
# Define the exception vectors.

rom_vectors:
        # These are the architecture defined vectors that
        # are always present.
        exception_vector        reserved_00000
        reset_vector            reset
        exception_vector        machine_check
        exception_vector        data_storage
        exception_vector        instruction_storage
        exception_vector        external
        exception_vector        alignment
        exception_vector        program
        exception_vector        floatingpoint_unavailable
        exception_vector        decrementer
        exception_vector        reserved_00a00
        exception_vector        reserved_00b00
        exception_vector        system_call
        exception_vector        trace
        exception_vector        floatingpoint_assist
        exception_vector        reserved_00f00

        # Variants may define extra vectors.
        hal_extra_vectors

rom_vectors_end:   

#===========================================================================
# Real startup code. We jump here from the various reset vectors to set up
# the world.
        
        .text   
        .globl  _start
_start:
        # Initialize CPU to a post-reset state, ensuring the ground doesn''t
        # shift under us while we try to set things up.
        hal_cpu_init

        # Set up global offset table
        lwi     r2,_GLOBAL_OFFSET_TABLE_

        # set up time base register to zero
        xor     r3,r3,r3
        mtspr   TBL_W,r3
        xor     r4,r4,r4
        mtspr   TBU_W,r4

        # Call platform specific hardware initialization
        # This may include memory controller initialization. It is not
        # safe to access RAM until after this point.
        bl      hal_hardware_init	# this is platform dependent

        # set up stack
        lwi     sp,__interrupt_stack
        mtspr   SPRG0,sp        # save in sprg0 for later use

        # Set up exception handlers and VSR table, taking care not to
        # step on any ROM monitor''s toes.
        hal_mon_init

#if defined(CYG_HAL_STARTUP_ROM)

        # Copy data from ROM to ram
        lwi     r3,__rom_data_start     # r3 = rom start
        lwi     r4,__ram_data_start     # r4 = ram start
        lwi     r5,__ram_data_end       # r5 = ram end

        cmplw   r4,r5                   # skip if no data
        beq     2f
        
1:
        lwz     r0,0(r3)                # get word from ROM
        stw     r0,0(r4)                # store in RAM
        addi    r3,r3,4                 # increment by 1 word
        addi    r4,r4,4                 # increment by 1 word
        cmplw   r4,r5                   # compare
        blt     1b                      # loop if not yet done
2:
#endif

        # clear BSS
        lwi     r3,__bss_start  # r3 = start
        lwi     r4,__bss_end    # r4 = end
        li      r0,0            # r0 = 0
        cmplw   r3,r4           # skip if no bss
        beq     2f
        
1:      stw     r0,0(r3)        # store zero
        addi    r3,r3,4         # increment by 1 word
        cmplw   r3,r4           # compare
        blt     1b              # loop if not yet done
2:

        # clear SBSS
        lwi     r3,__sbss_start # r3 = start
        lwi     r4,__sbss_end   # r4 = end
        cmplw   r3,r4           # skip if no sbss
        beq     2f
        
1:      stw     r0,0(r3)        # store zero
        addi    r3,r3,4         # increment by 1 word
        cmplw   r3,r4           # compare
        blt     1b              # loop if not yet done
2:

        # It is now safe to call C functions which may rely on initialized
        # data.
        
        # Set up stack for calls to C code.
        subi    sp,sp,12                        # make space on stack
        li      r0,0
        stw     r0,0(sp)                        # clear back chain
        stw     r0,8(sp)                        # zero return pc
        stwu    sp,-ppc_stack_frame_size(sp)    # create new stack frame

        # Initialize MMU.
        bl      hal_MMU_init

        # Enable MMU so we can safely enable caches.
        lwi     r3,CYG_MSR_NO_INTS              # interrupts enabled later
        sync
        mtmsr   r3
        sync
	
        # Enable caches
        bl      hal_enable_caches

        # call c++ constructors
        bl      cyg_hal_invoke_constructors

        # set up platform specific interrupt environment
        bl      hal_IRQ_init

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        bl      initialize_stub
#endif

        bl      cyg_start                       # call cyg_start
9:      
        b       9b              # if we return, loop

#---------------------------------------------------------------------------
# This code handles the common part of all exception handlers.
# It saves the machine state onto the stack  and then calls
# a "C" routine to do the rest of the work. This work may result
# in thread switches, and changes to the saved state. When we return
# here the saved state is restored and execution is continued.
        
        .text

        .globl cyg_hal_default_exception_vsr
cyg_hal_default_exception_vsr:
        
        # We come here with all register containing their
        # pre-exception values except:
        # R3    = ls 16 bits of vector address
        # R4    = saved CR
        # R5    = saved LR
        # LR    = VSR address
        # SPRG1 = old R3
        # SPRG2 = old R4
        # SPRG3 = old R5
        # SRR0  = old PC
        # SRR1  = old MSR and the exception cause (the POW state is lost!)

        subi    r1,r1,ppc_exception_decrement
                                        # leave space for registers and
                                        # a safety margin

        # First, save away some registers
        stw     r3,ppcreg_vector(r1)    # stash vector
        stw     r4,ppcreg_cr(r1)        # stash CR
        stw     r5,ppcreg_lr(r1)        # stash LR

        # Enable MMU.
        lwi     r3,CYG_MSR_NO_INTS
        sync
        mtmsr   r3
        sync
                
        mfspr   r3,SPRG1                # save original R3
        stw     r3,ppcreg_regs+3*4(r1)
        mfspr   r4,SPRG2                # save original R4
        stw     r4,ppcreg_regs+4*4(r1)
        mfspr   r5,SPRG3                # save original R5
        stw     r5,ppcreg_regs+5*4(r1)

        stw     r0,ppcreg_regs(r1)      # save R0
        stw     r2,ppcreg_regs+2*4(r1)  # save R2
        
        mr      r3,r1                   # recreate original R1
        addi    r3,r3,ppc_exception_decrement
        stw     r3,ppcreg_regs+1*4(r1)  # and save it in state
        
        # Save registers r6..r12/r31
        .set    _reg,6
        .rept   MAX_SAVE_REG+1-6
        stw     _reg,(ppcreg_regs+_reg*4)(r1)
        .set    _reg,_reg+1
        .endr

        # Save registers used in vsr (r14+r15)
        stw     r14,(ppcreg_regs+14*4)(r1)
        stw     r15,(ppcreg_regs+15*4)(r1)
        
        # get remaining CPU registers
        mfxer   r3
        mfctr   r5
        mfdar   r6
        mfdsisr r7
        mfpvr   r8
        mfsrr0  r9
        mfsrr1  r10

        # and store them
        stw     r3,ppcreg_xer(r1)
        stw     r5,ppcreg_ctr(r1)
        stw     r6,ppcreg_dar(r1)
        stw     r7,ppcreg_dsisr(r1)
        stw     r8,ppcreg_pvr(r1)
        stw     r9,ppcreg_pc(r1)
        stw     r10,ppcreg_msr(r1)
        
        # The entire CPU state is now stashed on the stack,
        # call into C to do something with it.

        mr      r3,sp                           # R3 = register dump
        
        subi    sp,sp,ppc_stack_frame_size      # make a null frame

        li      r0,0                            # R0 = 0
        stw     r0,0(sp)                        # backchain = 0
        stw     r0,8(sp)                        # return pc = 0
        
        stwu    sp,-ppc_stack_frame_size(sp)    # create new stack frame
                                                # where C code can save LR

        lwi     r5,restore_state                # get return link
        mtlr    r5                              # to link register

        .extern cyg_hal_exception_handler
        b       cyg_hal_exception_handler       # call C code, r3 = registers

        # When the call returns it will go to restore_state below.


##--------------------------------------------------------------------------
## The following macros are defined depending on whether the Interrupt
## system is using isr tables or chaining, and depending on the interrupt
## controller in the system.

## Note: CYG_ISR_TABLE_SIZE must match CYG_ISR_COUNT defined in hal_intr.h.

#ifndef CYGPKG_HAL_POWERPC_INTC_DEFINED

## This is the simple version. No interrupt controller, ppcreg_vector 
## is updated with the decoded interrupt vector. Isr tables/chaining
## use same interrupt decoder.
## Bit 21 biffers between decrementer (0) and external (1).

#define CYG_ISR_TABLE_SIZE    2

        # decode the interrupt
	.macro  hal_intc_decode dreg,state
        lwz     \dreg,ppcreg_vector(\state)     # retrieve vector number,
        rlwinm  \dreg,\dreg,22,31,31            # isolate bit 21 and update
        stw     \dreg,ppcreg_vector(\state)     # vector in state frame.
        slwi    \dreg,\dreg,2                   # convert to word offset.
        .endm                              

#endif // CYGPKG_HAL_POWERPC_MPC8xx

#---------------------------------------------------------------------------
# Common interrupt handling code.

        .globl cyg_hal_default_interrupt_vsr
cyg_hal_default_interrupt_vsr:

        # We come here with all register containing their
        # pre-exception values except:
        # R3    = ls 16 bits of vector address
        # R4    = saved CR
        # R5    = saved LR
        # LR    = VSR address
        # SPRG1 = old R3
        # SPRG2 = old R4
        # SPRG3 = old R5
        # SRR0  = old PC
        # SRR1  = old MSR


        subi    r1,r1,ppc_exception_decrement
                                        # leave space for registers and
                                        # a safety margin

        stw     r3,ppcreg_vector(r1)    # stash vector
        stw     r4,ppcreg_cr(r1)        # stash CR
        stw     r5,ppcreg_lr(r1)        # stash LR

        # Enable MMU.
        lwi     r3,CYG_MSR_NO_INTS
        sync
        mtmsr   r3
        sync

        mfspr   r3,SPRG1                # save original R3
        stw     r3,ppcreg_regs+3*4(r1)
        mfspr   r4,SPRG2                # save original R4
        stw     r4,ppcreg_regs+4*4(r1)
        mfspr   r5,SPRG3                # save original R5
        stw     r5,ppcreg_regs+5*4(r1)

        stw     r0,ppcreg_regs(r1)      # save R0
        stw     r2,ppcreg_regs+2*4(r1)  # save R2

        mr      r3,r1                   # recreate original R1
        addi    r3,r3,ppc_exception_decrement
        stw     r3,ppcreg_regs+1*4(r1)  # and save it in state
        
        # Save registers r6..r12/r31
        .set    _reg,6
        .rept   MAX_SAVE_REG+1-6
        stw     _reg,(ppcreg_regs+_reg*4)(r1)
        .set    _reg,_reg+1
        .endr

        # Save registers used in vsr (r14+r15)
        stw     r14,(ppcreg_regs+14*4)(r1)
        stw     r15,(ppcreg_regs+15*4)(r1)

        # get remaining CPU registers
        mfxer   r3
        mfctr   r5
        mfsrr0  r6
        mfsrr1  r7
        
        # and store them
        stw     r3,ppcreg_xer(r1)
        stw     r5,ppcreg_ctr(r1)
        stw     r6,ppcreg_pc(r1)
        stw     r7,ppcreg_msr(r1)
                
        # The entire CPU state is now stashed on the stack,
        # increment the scheduler lock and call the ISR
        # for this vector.

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT                 
        .extern cyg_scheduler_sched_lock
        lwi     r3,cyg_scheduler_sched_lock
        lwz     r4,0(r3)
        addi    r4,r4,1
        stw     r4,0(r3)
#endif
        
        mr      r14,sp                          # r14 = register dump
        
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 
        lwi     r3,__interrupt_stack            # stack top
        lwi     r4,__interrupt_stack_base       # stack base
        sub.    r5,sp,r4                        # sp - base
        blt     1f                              # if < 0 - not on istack
        sub.    r5,r3,sp                        # top - sp
        bgt     2f                              # if > 0 - already on istack

1:      mr      sp,r3                           # switch to istack

2:      stwu    r14,-4(sp)                      # save old SP on stack

#endif
        
        subi    sp,sp,ppc_stack_frame_size      # make a null frame

        li      r0,0                            # R0 = 0
        stw     r0,0(sp)                        # backchain = 0
        stw     r0,8(sp)                        # return pc = 0
        
        stwu    sp,-ppc_stack_frame_size(sp)    # create new stack frame
                                                # where C code can save LR
        
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

        lwi     r3,0x0301                       # r3 = type = INTR,RAISE
        lwz     r4,ppcreg_vector(r14)           # arg1 = vector address
        srwi    r4,r4,8                         # arg1 = vector number
        xor     r5,r5,r5                        # arg2 = 0
        bl      cyg_instrument                  # call instrument function
        
#endif

        hal_intc_decode r15,r14                # get table index

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        .extern cyg_hal_gdb_isr
        lwz     r3,ppcreg_pc(r14)               # for serial receive irq.
        bl      cyg_hal_gdb_isr                 # (arg1 is PC)
        cmpwi   r3,0x0000                       # Call ISR proper?
        beq     2f                              # (r3 is 0 when skipping
                                                #  to avoid DSR call)
1:
#endif

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

#ifdef CYGPKG_HAL_POWERPC_MPC8xx
        # The CPM controller allows nested interrupts. However,
        # it sits on the back of the SIU controller which has no
        # HW support for this. In effect, SW masking of lower
        # priority IRQs in the SIU would be required for this to work.
#endif

#endif

        lwz     r3,ppcreg_vector(r14)           # retrieve decoded vector #

        lwi     r6,hal_interrupt_handlers       # get interrupt handler table
        lwzx    r6,r6,r15                       # load routine pointer

        lwi     r4,hal_interrupt_data           # get interrupt data table
        lwzx    r4,r4,r15                       # load data pointer
                                                # R4 = data argument
        
        mtctr   r6                              # put isr address in ctr

        bctrl                                   # branch to ctr reg and link

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        # If interrupt was caused by GDB, the ISR call above
        # is skipped by jumping here.
2:
#endif
        
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

        # If we are returning from the last nested interrupt, move back
        # to the thread stack. interrupt_end() must be called on the
        # thread stack since it potentially causes a context switch.
        # Since we have arranged for the top of stack location to
        # contain the sp we need to go back to here, just pop it off
        # and put it in SP.

        
        lwz     sp,ppc_stack_frame_size*2(sp)   # sp = *sp

        subi    sp,sp,ppc_stack_frame_size      # make a null frame

        li      r0,0                            # R0 = 0
        stw     r0,0(sp)                        # backchain = 0
        stw     r0,8(sp)                        # return pc = 0
        
        stwu    sp,-ppc_stack_frame_size(sp)    # create new stack frame
                                                # where C code can save LR
#endif  

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

        # We only need to call _interrupt_end() when there is a kernel
        # present to do any tidying up.
        
        # on return r3 bit 1 will indicate whether a DSR is
        # to be posted. Pass this together with a pointer to
        # the interrupt object we have just used to the
        # interrupt tidy up routine.

        # Note that r14 and r15 are defined to be preserved across
        # calls by the calling convention, so they still contain
        # the register dump and the vector number respectively.

        # Note: The instructions restoring the msr mask out the POW bit.
        # When eCos gets power management aware, this bit will have to be
        # set to the correct state rather than just cleared.
        
        lwi     r4,hal_interrupt_objects        # get interrupt object table
        lwzx    r4,r4,r15                       # load object pointer

        lwz     r6,ppcreg_msr(r14)              # restore msr, including
        lis     r5,0xfffe                       # interrupt enable.
        andc    r6,r6,r5                        # mask out reserved bits
        sync                                    # (and POW!)
        mtmsr   r6
        sync

        mr      r5,r14                          # arg3 = saved register dump
                        
        .extern interrupt_end
        bl      interrupt_end                   # call into C to finish off 
#endif

restore_state:  
        # All done, restore CPU state and continue

        addi    sp,sp,ppc_stack_frame_size*2    # retrieve CPU state pointer

        # get sprs we want to restore
        lwz     r3,ppcreg_cr(sp)
        lwz     r4,ppcreg_pc(sp)
        lwz     r5,ppcreg_msr(sp)
        lwz     r6,ppcreg_xer(sp)
        lwz     r7,ppcreg_lr(sp)
        lwz     r8,ppcreg_ctr(sp)


        # stuff some of them into the CPU
        mtxer   r6
        mtlr    r7
        mtctr   r8

        # restore R0 and R2
        lwz     r0,ppcreg_regs(sp)
        lwz     r2,ppcreg_regs+2*4(sp)
        
        # Restore registers used in vsr (r14+r15)
        lwz     r14,(ppcreg_regs+14*4)(r1)
        lwz     r15,(ppcreg_regs+15*4)(r1)

        # restore registers r6..r12/r31
        .set    _reg,6
        .rept   MAX_SAVE_REG+1-6
        lwz     _reg,(ppcreg_regs+_reg*4)(r1)
        .set    _reg,_reg+1
        .endr

        # Here all the registers are loaded except
        # r1 = ppcregs
        # r3 = ccr
        # r4 = srr0 = pc
        # r5 = srr1 = msr

        # We have to disable interrupts while srr0 and
        # srr1 are loaded, since another interrupt will
        # destroy them.

        mtcr    r3                      # set ccr
        
        lwi     r3,CYG_MSR_NO_INTS      # do rest with ints disabled
        sync
        mtmsr   r3
        sync
        
        mtsrr0  r4                      # load old pc
        mtsrr1  r5                      # load old msr
        
        lwz     r3,ppcreg_regs+3*4(r1)  # load r3 value
        lwz     r4,ppcreg_regs+4*4(r1)  # load r4 value
        lwz     r5,ppcreg_regs+5*4(r1)  # load r5 value
        lwz     r1,ppcreg_regs+1*4(r1)  # restore r1
                
        sync                            # settle things down
        isync   
        rfi                             # and return



##-----------------------------------------------------------------------------
## Execute pending DSRs on the interrupt stack with interrupts enabled.
## Note: this can only be called from code running on a thread stack
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	.extern cyg_interrupt_call_pending_DSRs

FUNC_START(hal_interrupt_stack_call_pending_DSRs)
        # Disable interrupts
        mfmsr   r4
        lis     r3,0
        ori     r3,r3,0x8000
        andc    r5,r4,r3
        mtmsr   r5

        # Change to interrupt stack
        mr      r3,sp
        lwi     sp,__interrupt_stack
        stwu    r3,-4(sp)                       # store old stackptr on stack
        mflr    r3
        stwu    r3,-4(sp)                       # store lr on stack
        stwu    r4,-4(sp)                       # store msr values on stack
        stwu    r5,-4(sp)
        
        # Set up stack for calls to C code.
        subi    sp,sp,12                        # make space on stack
        li      r0,0
        stw     r0,0(sp)                        # clear back chain
        stw     r0,8(sp)                        # zero return pc
        stwu    sp,-ppc_stack_frame_size(sp)    # create new stack frame

        # Restore interrupt state
        mtmsr   r4

        # Call into kernel which will execute DSRs
        bl      cyg_interrupt_call_pending_DSRs

        # Unwind stack and get msr values
        lwzu    r5,12+ppc_stack_frame_size(sp)
        lwz     r4,4(sp)
        lwz     r3,8(sp)
        mtlr    r3

        # Disable interrupts
        mtmsr   r5

        # Restore original stack pointer, original interrupt state and return
        lwz     sp,12(sp)
        mtmsr   r4

        blr
#endif		
        
#---------------------------------------------------------------------------
## Temporary interrupt stack
        
        .section ".bss"

	.balign 16
	.global cyg_interrupt_stack_base
cyg_interrupt_stack_base:
__interrupt_stack_base:
	.rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
	.byte 0
	.endr
	.balign 16
	.global cyg_interrupt_stack
cyg_interrupt_stack:
__interrupt_stack:
        
        .long   0,0,0,0,0,0,0,0 

#---------------------------------------------------------------------------
# end of vectors.S
