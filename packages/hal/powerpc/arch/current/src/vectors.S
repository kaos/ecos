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
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>     // CYGPKG_KERNEL_INSTRUMENT
#endif

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include "cyg/hal/ppc_regs.h"        
#include "cyg/hal/ppc.inc"

#===========================================================================
                
        .file   "vectors.S"
        
        .extern hal_interrupt_handlers
        .extern hal_interrupt_data
        .extern hal_interrupt_objects

        .extern cyg_instrument

        .extern hal_hardware_init

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
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_STUBS)
# ifdef CYG_HAL_POWERPC_COPY_VECTORS
#  define CYG_MSR CYG_MSR_COMMON
# else
#  define CYG_MSR (CYG_MSR_COMMON | MSR_IP)
# endif
#endif
#ifdef CYG_HAL_STARTUP_RAM      
#define CYG_MSR CYG_MSR_COMMON
#endif
#ifdef CYG_HAL_POWERPC_SIM
# When building for SIM, don~t enable MMU -- it~s not needed since caches
# are disabled, and there is a runtime simulation overhead.
#undef CYG_MSR
#define CYG_MSR (CYG_MSR_COMMON & ~(MSR_IR | MSR_DR))
#endif

#define CYG_MSR_NO_INTS (CYG_MSR & ~MSR_ME)        

#===========================================================================
# If the following option is enabled, we only save registers up to R12.
# The PowerPC ABI defines registers 13..31 as callee saved and thus we do
# not need to save them when calling C functions.

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT

#define MAX_SAVE_REG    12

#else

#define MAX_SAVE_REG    31
        
#endif                  
        
#===========================================================================
# Start by defining the exceptions vectors that must be placed at
# locations 0xFFF00000 and 0x00000000. The following code will normally
# be located at 0xFFF00000 in the ROM. It may optionally be copied out
# to 0x00000000 if we want to use the RAM vectors. For this reason this code
# MUST BE POSITION INDEPENDENT. In RAM loaded configurations, this code may
# be placed at 0x00000000 during loading.
        
        .section ".vectors","ax"

#---------------------------------------------------------------------------
# Unused first vector.

rom_vectors:    
        lwi     r3,_start
        mtlr    r3
        blr
                
        .byte   0x11
        .byte   0x22
        .byte   0x33
        .byte   0x44
        
#---------------------------------------------------------------------------
# Reset vector. 

        .p2align 8
        .globl  reset_vector    

reset_vector:
        lwi     r3,_start
        mtlr    r3
        blr
        
#---------------------------------------------------------------------------
# Macro for generating an exception vector service routine
        
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

        # These are the architecture defined vectors that
        # are always present.
                
        exception_vector        machine_check
        exception_vector        data_storage
        exception_vector        instruction_storage
        exception_vector        external
        exception_vector        alignment
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_STUBS) \
    || defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
        exception_vector        program
#else
        # Provide a crude hook to the GDB exception handler in ROM.
        # This allows the testing infrastructure to rely on breakpoints
        # to terminate tests, even when the tests are compiled without
        # the GDB stub.
        # It is _not_ a generic solution to the problem; in particular it
        # is not possible to continue after a breakpoint is hit.
        # 
        # Note that for platforms where StubROM vectors are copied into RAM
        # at 0x00000000, simply skipping the copy of vector 0x700 is a
        # better solution.  See CYGPRI_STUBROM_HAS_RAM_VECTORS below.

        .p2align 8
        .globl  __exception_program
__exception_program:
        bl      _init_CPU                       # disable caches.
        lwi     r3,0xfff00700                   # address of trap handler
        mtlr    r3                              # in ROM.
        blr                                     # jump to it.

#endif
        exception_vector        floatingpoint_unavailable
        exception_vector        decrementer
        exception_vector        reserved_00a00
        exception_vector        reserved_00b00
        exception_vector        system_call
        exception_vector        trace
        exception_vector        floatingpoint_assist
        exception_vector        reserved_00f00

        # There are some additional vectors defined
        # on various implementations.

#ifdef CYG_HAL_POWERPC_MPC8xx
        # MPC8xx vectors

        exception_vector        software_emu
        exception_vector        instruction_tlb_miss
        exception_vector        data_tlb_miss
        exception_vector        instruction_tlb_error
        exception_vector        data_tlb_error
        exception_vector        reserved_01500
        exception_vector        reserved_01600
        exception_vector        reserved_01700
        exception_vector        reserved_01800
        exception_vector        reserved_01900
        exception_vector        reserved_01A00
        exception_vector        reserved_01B00
        exception_vector        data_breakpoint
        exception_vector        instruction_breakpoint
        exception_vector        peripheral_breakpoint
        exception_vector        NMI_port

#endif
rom_vectors_end:        

#===========================================================================
# Real startup code. We jump here from the various reset vectors to set up
# the world.
        
        .text   
        .globl  _start
        
_start:
        # Initialize CPU
        bl      _init_CPU

#ifdef CYG_HAL_POWERPC_MPC8xx
        # Disable special MPC8xx "development support" which
	# suppresses trace exceptions. The CPU seems to hang, not 
        # executing from offset 0x1e00(?) as expected.

        lwi	r3,0x00000007
        mtspr	ICTRL,r3

        li      r3,0
        mtspr   DER, r3
#endif

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

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_STUBS)

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
        
        
#ifdef CYG_HAL_POWERPC_COPY_VECTORS // only for StubRom-supported startup

#ifdef CYG_HAL_POWERPC_MBX
# define CYGPRI_STUBROM_HAS_RAM_VECTORS
#endif

#ifdef CYGPRI_STUBROM_HAS_RAM_VECTORS
# ifdef CYG_HAL_STARTUP_RAM
#  ifndef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#   define CYGPRI_SKIP_BREAKPOINT_VECTOR
#  endif  // no stubs
# endif  // RAM startup
#endif  // Stubrom vectors are in RAM

        # Copy RAM exception vectors from ROM (or load place) to RAM
        lwi     r3,rom_vectors          # r3 = rom start
        lwi     r4,0                    # r4 = ram start
#ifdef CYGPRI_SKIP_BREAKPOINT_VECTOR
        lwi     r5,__exception_program	# r5 = end of first leg of copy
#else
        lwi     r5,rom_vectors_end      # r5 = rom end
#endif

        cmplw   r3,r5                   # skip if no vectors
        beq     2f
        
1:
        lwz     r0,0(r3)                # get word from ROM
        stw     r0,0(r4)                # store in RAM
        addi    r3,r3,4                 # increment by 1 word
        addi    r4,r4,4                 # increment by 1 word
        cmplw   r3,r5                   # compare
        blt     1b                      # loop if not yet done
2:
#ifdef CYGPRI_SKIP_BREAKPOINT_VECTOR
	cmplwi	r4,__exception_program@L
	bne	3f			# is it the breakpoint vector?
	addi	r4,r4,0x100		# if so, leave it well alone
	addi	r3,r3,0x100
	lwi     r5,rom_vectors_end      # r5 = rom end
	b	1b			# and do the rest of the copy
3:	
#endif // Skip copying the breakpoint vector
#endif

        # clear BSS
        .extern __bss_start
        .extern __bss_end
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
        .extern __sbss_start
        .extern __sbss_end
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
        .extern hal_MMU_init
        bl      hal_MMU_init

        # Enable MMU it so we can safely enable caches.
        lwi     r3,CYG_MSR_NO_INTS              # interrupts enabled later
        sync
        mtmsr   r3
        sync
	
        # Enable caches
        .extern hal_enable_caches
        bl      hal_enable_caches

        # call c++ constructors
        .extern cyg_hal_invoke_constructors
        bl      cyg_hal_invoke_constructors

        # set up platform specific interrupt environment
        .extern hal_IRQ_init
        bl      hal_IRQ_init

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern initialize_stub
        bl      initialize_stub
#endif

        .extern cyg_start
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

        .extern __default_exception_vsr
__default_exception_vsr:
        
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

#ifdef CYG_HAL_POWERPC_MPC8xx 

## First level decoding of MPC8xx SIU interrupt controller.

#define CYG_ISR_TABLE_SIZE    59

        # decode the interrupt
	.macro  decode_interrupt dreg,state
        lwz     \dreg,ppcreg_vector(\state)     # retrieve vector number,
        rlwinm. \dreg,\dreg,22,31,31            # isolate bit 21
        beq     0f                              # done if decrementer (vec 0)
        lwi     \dreg,CYGARC_REG_IMM_SIVEC      # if external, get SIU
        lbz     \dreg,0(\dreg)                  # vector.
        srwi    \dreg,\dreg,2
        addi    \dreg,\dreg,1                   # Skip decrementer vector
0:      stw     \dreg,ppcreg_vector(\state)     # update vector in state frame.
        slwi    \dreg,\dreg,2                   # convert to byte offset.
        .endm                              

#else

## This is the simple version. No interrupt controller, ppcreg_vector 
## is updated with the decoded interrupt vector. Isr tables/chaining
## use same interrupt decoder.
## Bit 21 biffers between decrementer (0) and external (1).

#define CYG_ISR_TABLE_SIZE    2

        # decode the interrupt
	.macro  decode_interrupt dreg,state
        lwz     \dreg,ppcreg_vector(\state)     # retrieve vector number,
        rlwinm  \dreg,\dreg,22,31,31            # isolate bit 21 and update
        stw     \dreg,ppcreg_vector(\state)     # vector in state frame.
        slwi    \dreg,\dreg,2                   # convert to word offset.
        .endm                              

#endif

#---------------------------------------------------------------------------
# Common interrupt handling code.

        .extern __default_interrupt_vsr
__default_interrupt_vsr:

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

        decode_interrupt r15,r14                # get table index

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

#ifdef CYG_HAL_POWERPC_MPC8xx
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
# Code for putting the CPU in a post-reset state; disabling MMU and caches.
_init_CPU:
#ifdef CYG_HAL_POWERPC_MPC8xx
        lwi     r3,CYGARC_REG_DC_CMD_CD
        sync
        mtspr   CYGARC_REG_DC_CST,r3
        lwi     r3,CYGARC_REG_IC_CMD_CD
        isync
        mtspr   CYGARC_REG_IC_CST,r3
        isync
#endif

        # Set up MSR (disable MMU for now)
        lwi     r3,(CYG_MSR & ~(MSR_IR | MSR_DR))
        sync
        mtmsr   r3
        sync
        blr

#---------------------------------------------------------------------------
# This table contains a VSR pointer for each defined exception.
# All of these except 5 and 9 point to the __default_exception_vsr
# above, 5 and 9 point to the __default_interrupt_vsr. Entries in
# this table may be changed using Cyg_Interrupt::set_vsr().
        
        .data

        .globl  hal_vsr_table
hal_vsr_table:
        # Architecture defined vectors
        .long   __default_exception_vsr         # reserved
        .long   __default_exception_vsr         # system reset
        .long   __default_exception_vsr         # machine check
        .long   __default_exception_vsr         # data storage
        .long   __default_exception_vsr         # instruction storage

        .long   __default_interrupt_vsr         # external interrupt

        .long   __default_exception_vsr         # alignment
        .long   __default_exception_vsr         # program
        .long   __default_exception_vsr         # floating point unavailable

        .long   __default_interrupt_vsr         # decrementer

        .long   __default_exception_vsr         # reserved 0x0a00
        .long   __default_exception_vsr         # reserved 0x0b00
        .long   __default_exception_vsr         # system call
        .long   __default_exception_vsr         # trace
        .long   __default_exception_vsr         # floating point assist
        .long   __default_exception_vsr         # reserved 0x0f00

#ifdef CYG_HAL_POWERPC_MPC8xx

        # Implementation defined vectors
        .long   __default_exception_vsr         # software emulation
        .long   __default_exception_vsr         # instruction TLB miss
        .long   __default_exception_vsr         # data TLB miss
        .long   __default_exception_vsr         # instruction TLB error
        .long   __default_exception_vsr         # data TLB error
        .long   __default_exception_vsr         # reserved 0x1500
        .long   __default_exception_vsr         # reserved 0x1600
        .long   __default_exception_vsr         # reserved 0x1700
        .long   __default_exception_vsr         # reserved 0x1800
        .long   __default_exception_vsr         # reserved 0x1900
        .long   __default_exception_vsr         # reserved 0x1a00
        .long   __default_exception_vsr         # reserved 0x1b00
        .long   __default_exception_vsr         # data breakpoint
        .long   __default_exception_vsr         # instruction breakpoint
        .long   __default_exception_vsr         # peripheral breakpoint
        .long   __default_exception_vsr         # non maskable development port
#endif

#---------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.

#if (CYG_ISR_TABLE_SIZE > 2)
        .data
#else
        .section ".sdata","aw"
#endif

        .extern hal_default_decrementer_isr
        .extern hal_default_isr

        .globl  hal_interrupt_handlers
hal_interrupt_handlers:
        .long   hal_default_decrementer_isr
        .rept   CYG_ISR_TABLE_SIZE-1
        .long   hal_default_isr
        .endr

        .globl  hal_interrupt_data
hal_interrupt_data:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

        .globl  hal_interrupt_objects
hal_interrupt_objects:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

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
