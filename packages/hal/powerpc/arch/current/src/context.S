##=============================================================================
##
##      context.S
##
##      PowerPC context switch code
##
##=============================================================================
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   nickg
## Contributors:        nickg
## Date:        1998-04-27
## Purpose:     PowerPC context switch code
## Description: This file contains implementations of the thread context 
##              switch routines. It also contains the longjmp() and setjmp()
##              routines.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
        
#include "cyg/hal/ppc.inc"

#------------------------------------------------------------------------------
# function declaration macro
                
#define FUNC_START(name) \
        .type name,@function; \
        .globl name; \
name:   

#------------------------------------------------------------------------------
# Configure to use either a minimal or maximal thread state
        
#ifdef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

#define MIN_SAVE_REG    13
#define MAX_SAVE_REG    31

        .macro  save_special
        .endm

        .macro  load_special
        .endm
#else

#define MIN_SAVE_REG     6
#define MAX_SAVE_REG    31

        .macro  save_special
        mfxer   r6
        mfctr   r7
        stw     r6,ppcreg_xer(sp)
        stw     r7,ppcreg_ctr(sp)
        .endm

        .macro  load_special
        lwz     r6,ppcreg_xer(sp)
        lwz     r7,ppcreg_ctr(sp)
        mtxer   r6
        mtctr   r7
        .endm
                
#endif                  
                
#------------------------------------------------------------------------------
# hal_thread_switch_context
# Switch thread contexts
# R3 = address of sp of next thread to execute
# R4 = address of sp save location of current thread

        
FUNC_START(hal_thread_switch_context)
        mr      r5,sp                   # R5 = saved stack pointer
        subi    sp,sp,ppcreg_context_size # space for state

        # Save registers MIN..MAX
        .set    _reg,MIN_SAVE_REG
        .rept   MAX_SAVE_REG+1-MIN_SAVE_REG
        stw     _reg,(ppcreg_regs+_reg*4)(sp)
        .set    _reg,_reg+1
        .endr

        stw     r0,ppcreg_regs+0*4(sp)  # R0
        stw     r5,ppcreg_regs+1*4(sp)  # R5 = real SP, save in R1 slot
        stw     r2,ppcreg_regs+2*4(sp)  # R2 = TOC

        # CR cannot be treated as a special register since GCC will only
        # do partial restore of CR at function exit, depending on which
        # of CR2, CR3, and CR4 have been used in the given function.
        mfcr    r5                      # save CR.
        stw     r5,ppcreg_cr(sp)

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
        # Make the thread context look like an exception context if thread-
        # aware debugging is required. This state does not need restoring.
        mflr    r5
        stw     r5,ppcreg_pc(sp)        # pc of caller
        mfmsr   r5
        stw     r5,ppcreg_msr(sp)       # msr (or PS)
#endif

        save_special                    # save special regs
        
        mflr    r5                      # save LR == return link for this fn
        stw     r5,ppcreg_lr(sp)
        
        stw     sp,0(r4)                # save SP into save location

        # Now load the destination thread by dropping through
        # to hal_thread_load_context
        
#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
# R3 = address of sp of next thread to execute
# Note that this function is also the second half of hal_thread_switch_context
# and is simply dropped into from it.
        
FUNC_START(hal_thread_load_context)
        
        lwz     sp,0(r3)                # load state directly into SP

        load_special                    # load special registers

        lwz     r5,ppcreg_cr(sp)        # restore CR
        mtcr    r5

        lwz     r5,ppcreg_lr(sp)        # get LR as set as return link
        mtlr    r5
                
        # Load registers MIN..MAX
        .set    _reg,MIN_SAVE_REG
        .rept   MAX_SAVE_REG+1-MIN_SAVE_REG
        lwz     _reg,(ppcreg_regs+_reg*4)(sp)
        .set    _reg,_reg+1
        .endr

        lwz     r0,ppcreg_regs+0*4(sp)  # R0
        lwz     r2,ppcreg_regs+2*4(sp)  # R2 = TOC
        lwz     r3,ppcreg_regs+3*4(sp)  # load r3
        
        lwz     sp,ppcreg_regs+1*4(sp)  # finally restore true SP
        
        blr                             # jump to LR
                        
#------------------------------------------------------------------------------
# HAL longjmp, setjmp implementations
# hal_setjmp saves only to callee save registers 13-31, r1[sp],r2, cr[2-4]
# and lr into buffer supplied in r3[arg0]
# Note: These definitions are repeated in hal_arch.h. If changes are required
# remember to update both sets.

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_R2        1
#define CYGARC_JMP_BUF_R13       2
#define CYGARC_JMP_BUF_R14       3
#define CYGARC_JMP_BUF_R15       4
#define CYGARC_JMP_BUF_R16       5
#define CYGARC_JMP_BUF_R17       6
#define CYGARC_JMP_BUF_R18       7
#define CYGARC_JMP_BUF_R19       8
#define CYGARC_JMP_BUF_R20       9
#define CYGARC_JMP_BUF_R21      10
#define CYGARC_JMP_BUF_R22      11
#define CYGARC_JMP_BUF_R23      12
#define CYGARC_JMP_BUF_R24      13
#define CYGARC_JMP_BUF_R25      14
#define CYGARC_JMP_BUF_R26      15
#define CYGARC_JMP_BUF_R27      16
#define CYGARC_JMP_BUF_R28      17
#define CYGARC_JMP_BUF_R29      18
#define CYGARC_JMP_BUF_R30      19
#define CYGARC_JMP_BUF_R31      20
#define CYGARC_JMP_BUF_LR       21
#define CYGARC_JMP_BUF_CR       22

#define CYGARC_JMP_BUF_SIZE     23

FUNC_START(hal_setjmp)
        mfcr    r5
        stw     r5, CYGARC_JMP_BUF_CR*4(r3)
        mflr    r5
        stw     r5, CYGARC_JMP_BUF_LR*4(r3)
        stw     r31,CYGARC_JMP_BUF_R31*4(r3)
        stw     r30,CYGARC_JMP_BUF_R30*4(r3)
        stw     r29,CYGARC_JMP_BUF_R29*4(r3)
        stw     r28,CYGARC_JMP_BUF_R28*4(r3)
        stw     r27,CYGARC_JMP_BUF_R27*4(r3)
        stw     r26,CYGARC_JMP_BUF_R26*4(r3)
        stw     r25,CYGARC_JMP_BUF_R25*4(r3)
        stw     r24,CYGARC_JMP_BUF_R24*4(r3)
        stw     r23,CYGARC_JMP_BUF_R23*4(r3)
        stw     r22,CYGARC_JMP_BUF_R22*4(r3)
        stw     r21,CYGARC_JMP_BUF_R21*4(r3)
        stw     r20,CYGARC_JMP_BUF_R20*4(r3)
        stw     r19,CYGARC_JMP_BUF_R19*4(r3)
        stw     r18,CYGARC_JMP_BUF_R18*4(r3)
        stw     r17,CYGARC_JMP_BUF_R17*4(r3)
        stw     r16,CYGARC_JMP_BUF_R16*4(r3)
        stw     r15,CYGARC_JMP_BUF_R15*4(r3)
        stw     r14,CYGARC_JMP_BUF_R14*4(r3)
        stw     r13,CYGARC_JMP_BUF_R13*4(r3)
        stw     r2, CYGARC_JMP_BUF_R2*4(r3)        # TOC, optimize out?
        stw     sp, CYGARC_JMP_BUF_SP*4(r3)
        li      r3,0            # return 0
        blr

# hal_longjmp loads state from r3[arg0] and returns
# and lr into buffer supplied in r3[arg0]

FUNC_START(hal_longjmp)
        lwz     r5,CYGARC_JMP_BUF_CR*4(r3)
        mtcr    r5
        lwz     r5,CYGARC_JMP_BUF_LR*4(r3)
        mtlr    r5
        lwz     r31,CYGARC_JMP_BUF_R31*4(r3)
        lwz     r30,CYGARC_JMP_BUF_R30*4(r3)
        lwz     r29,CYGARC_JMP_BUF_R29*4(r3)
        lwz     r28,CYGARC_JMP_BUF_R28*4(r3)
        lwz     r27,CYGARC_JMP_BUF_R27*4(r3)
        lwz     r26,CYGARC_JMP_BUF_R26*4(r3)
        lwz     r25,CYGARC_JMP_BUF_R25*4(r3)
        lwz     r24,CYGARC_JMP_BUF_R24*4(r3)
        lwz     r23,CYGARC_JMP_BUF_R23*4(r3)
        lwz     r22,CYGARC_JMP_BUF_R22*4(r3)
        lwz     r21,CYGARC_JMP_BUF_R21*4(r3)
        lwz     r20,CYGARC_JMP_BUF_R20*4(r3)
        lwz     r19,CYGARC_JMP_BUF_R19*4(r3)
        lwz     r18,CYGARC_JMP_BUF_R18*4(r3)
        lwz     r17,CYGARC_JMP_BUF_R17*4(r3)
        lwz     r16,CYGARC_JMP_BUF_R16*4(r3)
        lwz     r15,CYGARC_JMP_BUF_R15*4(r3)
        lwz     r14,CYGARC_JMP_BUF_R14*4(r3)
        lwz     r13,CYGARC_JMP_BUF_R13*4(r3)
        lwz     r2, CYGARC_JMP_BUF_R2*4(r3)
        lwz     sp, CYGARC_JMP_BUF_SP*4(r3)
        mr      r3,r4           # return r4[arg1]
        blr

#------------------------------------------------------------------------------
# end of context.S
