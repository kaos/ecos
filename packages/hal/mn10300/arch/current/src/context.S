##=============================================================================
##
##	context.S
##
##	MN10300 context switch code
##
##=============================================================================
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1998-04-27
## Purpose:	MN10300 context switch code
## Description:	This file contains implementations of the thread context 
##		switch routines. It also contains the longjmp() and setjmp()
##              routines.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#------------------------------------------------------------------------------
# Configure to use either a minimal or maximal thread state
	
#ifdef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

#define SAVE_REGS [d2,d3,a2,a3,other]	
	
#else

#define SAVE_REGS [d2,d3,a2,a3,other]		

#endif
	
#------------------------------------------------------------------------------
# hal_thread_switch_context
# Switch thread contexts
# D0 = address of sp of next thread to execute
# D1 = address of sp save location of current thread

	.global	_hal_thread_switch_context
_hal_thread_switch_context:
	add	-8,sp		# space for SP and PSW
	movm	SAVE_REGS,(sp)	# dump all registers onto stack
	mov	psw,d2		# put PSW into saved state
	mov	d2,(52,sp)
	mov	d1,a2		# move save loc addr to A2
	mov	sp,(0,a2)	# store SP in save location

	# Now load the destination thread by dropping through
	# to hal_thread_load_context
	
#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
# D0 = address of sp of next thread to execute
# Note that this function is also the second half of hal_thread_switch_context
# and is simply dropped into from it.
	
	.global	_hal_thread_load_context
_hal_thread_load_context:

	mov	d0,a2		# move next sp loc to a2
	mov	(0,a2),sp	# load SP with new value
	movm	(sp),SAVE_REGS	# load all registers
	add	8,sp		# skip SP & PSW, we never restore them.
	ret	[],0
	
##-----------------------------------------------------------------------------
## HAL longjmp(), setjmp() implementations
## These implementations omit the usual movm [d2,d3,a2,a3],(sp)
## Which is the first instruction of all C compiled functions.	
## Note: These definitions are repeated in hal_arch.h. If changes are required
## remember to update both sets.

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_D2        1
#define CYGARC_JMP_BUF_D3        2
#define CYGARC_JMP_BUF_A2        3
#define CYGARC_JMP_BUF_A3        4
#define CYGARC_JMP_BUF_LR        5

#define CYGARC_JMP_BUF_SIZE      6

        
	# This just preserves the callee save registers 
	# namely a2,a3,d2,d3
	# setjmp cannot use movm to do this as we need to keep 
	# the sp underneath all live data at all times.
	.globl _hal_setjmp
_hal_setjmp:			# d0=env
	mov	d0,a0		# env
	mov	d2,(CYGARC_JMP_BUF_D2*4,a0)
	mov	d3,(CYGARC_JMP_BUF_D3*4,a0)
	mov	a2,(CYGARC_JMP_BUF_A2*4,a0)
	mov	a3,(CYGARC_JMP_BUF_A3*4,a0)
	mov	mdr,d1		# link saved by call (also in (0,sp))
	mov	d1,(CYGARC_JMP_BUF_LR*4,a0)	
	mov	sp,a1
	mov	a1,(CYGARC_JMP_BUF_SP*4,a0) # a1==(caller''s sp)
	clr	d0		# return 0
	ret	[],0

	# longjmp returns to caller of setjmp
	# after restoring callee save registers
	.globl _hal_longjmp
_hal_longjmp:
	mov	d0,a0
	mov	(CYGARC_JMP_BUF_D2*4,a0),d2
	mov	(CYGARC_JMP_BUF_D3*4,a0),d3
	mov	(CYGARC_JMP_BUF_A2*4,a0),a2
	mov	(CYGARC_JMP_BUF_A3*4,a0),a3
	mov	(CYGARC_JMP_BUF_LR*4,a0),d0
##	mov	d0,mdr
	mov	(CYGARC_JMP_BUF_SP*4,a0),a1 # stashed (caller''s sp)
	mov	a1,sp
	mov	d0,(0,sp)	# put link on stack
	mov	d1,d0		# return arg1
	ret	[],0		# this ignores (0,sp) and does pc=mdr

	
#------------------------------------------------------------------------------
# end of context.S

	