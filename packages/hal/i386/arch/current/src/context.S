##=============================================================================
##
##      context.S
##
##      i386 context switch code
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
## Author(s):   jskov
## Contributors:jskov, pjo, nickg
## Date:        1999-01-20
## Purpose:     i386 context switch code
## Description: This file contains implementations of the thread context 
##              switch routines. It also contains the longjmp() and setjmp()
##              routines.
##              Based on PowerPC context.S, using data from SYSV ABI4, i386 
##              supplement (page 37-38)
##         http://www.sco.com/products/layered/develop/devspecs/abi386-4.pdf
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/i386.inc>

#include <cyg/hal/arch.inc>
	
#------------------------------------------------------------------------------
# function declaration macro

#define FUNC_START(name)                        \
        .globl name;                            \
name:   

#------------------------------------------------------------------------------
# hal_thread_switch_context
# Switch thread contexts
# :     0(%esp) :     return address
# :     4(%esp) :     address of sp of next thread to execute
# :     8(%esp) :     address of sp save location of current thread
#
# %eax, %ecx, and %edx are ours to abuse.
        
FUNC_START(hal_thread_switch_context)

	# Pop the return address from the stack, but leave the
	# arguments there so that the caller can remove them
	# itself when we return.
	
	popl	%ecx		# pop return eip
	movl	0(%esp),%eax	# get next context ptr
	movl	4(%esp),%edx	# get this context ptr

	# Save context in the same format as an
	# exception
		
	pushfl			# save eflags
	pushl	%cs		# save cs
	pushl	%ecx		# save eip 
	pushl	$-1		# push fake vector
	pushal			# push general registers
	hal_fpu_push_ctx	# push FPU state
		
        # Store the context ptr
        movl    %esp,(%edx)

hal_thread_switch_context_load:
	
	# The pointer to the next context is in EAX
	
	movl	(%eax),%esp	# Point ESP at new state

	# Merge the IF bit in the saved EFLAGS with the rest of the 
	# bits currently in EFLAGS.

	movl	i386reg_eflags(%esp),%ebx # EBX = saved EFLAGS
	andl	$0x0200,%ebx	# isolate IF bit
	pushfl			# push current flags
	popl	%ecx		# pop into ECX
	btrl	$9,%ecx		# clear IF flag in current EFLAGS
	orl	%ebx,%ecx	# Or in saved IF bit
	movl	%ecx,i386reg_eflags(%esp) # Restore to saved state for use by iret

	# Now we can load the state and enter next thread

	hal_fpu_pop_ctx		# Pop FPU state
	popal			# unstack general registers
	add	$4,%esp		# skip vector number
        
        iret			# And return
        
#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
# : 4(%esp) = address of sp of thread to execute
#
# %eax, %ecx, and %edx are ours to abuse.
        
FUNC_START(hal_thread_load_context)

        movl    4(%esp),%eax	# get new context ptr

	# Jump into hal_thread_switch_context at the right
	# point to load this context.
	
	jmp	hal_thread_switch_context_load

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
        movl    0(%esp),%edx
        movl    %edx,CYGARC_JMP_BUF_PC*4(%eax)

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

#-----------------------------------------------------------------------------
# End of context.S
