##=============================================================================
##
##      context.S
##
##      x86 thread context manipulation.
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
# Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
# All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   jskov
## Contributors:jskov, pjo, nickg, bartv
## Date:        1999-03-11
## Purpose:     CPU-specific code
## Description: This file contains the code needed to manage the
##              CPU on an i386/Linux synthetic target.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/arch.inc>
        
#------------------------------------------------------------------------------
# function declaration macro

#define FUNC_START(name)                        \
        .globl name;                            \
name:   

#------------------------------------------------------------------------------
# Context switch and setjmp/longjmp support.        
# Based on PowerPC context.S, using data from SYSV ABI4, i386 
# supplement (page 37-38)
# http://www.sco.com/products/layered/develop/devspecs/abi386-4.pdf
#
# There is no need to worry about saving floating point context. If
# a switch occurs because of an interrupt/signal, the system will have
# saved the fpu state already before the signal handler was invoked,
# and the state will be returned when the signal handler returns i.e.
# when the interrupted thread is reactivated. If a context switch is
# voluntary, for example a call to cyg_thread_yield(), then according
# to the calling conventions all fpu registers are CALL_USED_REGISTERS
# (gcc/config/i386/i386.h) and will have been saved by the calling
# code.
#                
# FIXME: it may be appropriate to match eCos thread contexts
# more closely onto Linux sigcontexts - that might facilitate
# thread-aware debugging.
                                        
# hal_thread_switch_context
# Switch thread contexts
# :     0(%esp) :     return address
# :     4(%esp) :     address of sp of next thread to execute
# :     8(%esp) :     address of sp save location of current thread
#
# %eax, %ecx, and %edx are ours to abuse.
        
FUNC_START(hal_thread_switch_context)
        movl    4(%esp),%eax            # next context ptr
        movl    8(%esp),%edx            # this context ptr

        # Make room on the stack for the context
        movl    %esp,%ecx               # keep original SP
        sub	$i386reg_context_size,%esp

        # Save next context ptr in this context. Necessary because
        # hal_thread_load_context expects to find the ptr on the stack,
        # not in a register as on PPC.
        movl    %eax,i386reg_next_context(%esp)

        # Save registers
        movl    %ecx,i386reg_esp(%esp)  # original esp
        movl    %ebp,i386reg_ebp(%esp)
        movl    %ebx,i386reg_ebx(%esp)
        movl    %esi,i386reg_esi(%esp)
        movl    %edi,i386reg_edi(%esp)
        
        # Store the context ptr
        movl    %esp,(%edx)

        # Now fall through to hal_thread_load_context
        
        
#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
# : 4(%esp) = i386reg_next_context(%esp) = address of sp of thread to execute
# Note that this function is also the second half of hal_thread_switch_context
# and is simply dropped into from it.
#
# %eax, %ecx, and %edx are ours to abuse.
        
FUNC_START(hal_thread_load_context)

        movl    i386reg_next_context(%esp),%eax # get new context ptr
        movl    (%eax),%eax
        
        # Restore registers
        movl    i386reg_ebp(%eax),%ebp
        movl    i386reg_ebx(%eax),%ebx
        movl    i386reg_esi(%eax),%esi
        movl    i386reg_edi(%eax),%edi
        movl    i386reg_esp(%eax),%esp

        ret


#------------------------------------------------------------------------------
# HAL longjmp, setjmp implementations
# hal_setjmp saves only to callee save registers ebp, ebx, esi, edi and
# and esp+pc into buffer supplied in 4(esp)
# Note: These definitions are repeated in hal_arch.h. If changes are required
# remember to update both sets.

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_EBP       1
#define CYGARC_JMP_BUF_EBX       2
#define CYGARC_JMP_BUF_ESI       3
#define CYGARC_JMP_BUF_EDI       4
#define CYGARC_JMP_BUF_PC        5

#define CYGARC_JMP_BUF_SIZE      6

FUNC_START(hal_setjmp)
        # Get jmpbuf pointer
        movl    4(%esp),%eax

        # Save regular registers
        movl    %ebp,CYGARC_JMP_BUF_EBP*4(%eax)
        movl    %ebx,CYGARC_JMP_BUF_EBX*4(%eax)
        movl    %esi,CYGARC_JMP_BUF_ESI*4(%eax)
        movl    %edi,CYGARC_JMP_BUF_EDI*4(%eax)

        # Stack and PC
        movl    %esp,CYGARC_JMP_BUF_SP*4(%eax)
        movl    0(%esp),%ebx
        movl    %ebx,CYGARC_JMP_BUF_PC*4(%eax)

        # Return 0
        xor     %eax,%eax
        ret

        
# hal_longjmp loads state from 4(esp) and returns to PC stored in state

FUNC_START(hal_longjmp)
        # Get return value
        movl    8(%esp),%eax

        # Get jmpbuf pointer
        movl    4(%esp),%ecx
        
        # Restore regular registers
        movl    CYGARC_JMP_BUF_EBP*4(%ecx),%ebp
        movl    CYGARC_JMP_BUF_EBX*4(%ecx),%ebx
        movl    CYGARC_JMP_BUF_ESI*4(%ecx),%esi
        movl    CYGARC_JMP_BUF_EDI*4(%ecx),%edi
        
        # Restore stack pointer
        movl    CYGARC_JMP_BUF_SP*4(%ecx),%esp

        # Put return address on stack        
        movl    CYGARC_JMP_BUF_PC*4(%ecx),%edx
        movl    %edx,0(%esp)

        ret

#------------------------------------------------------------------------------
# end of linux.S
