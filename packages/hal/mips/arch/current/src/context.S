##=============================================================================##
##	context.S
##
##	MIPS context switch code
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
## Purpose:	MIPS context switch code
## Description:	This file contains implementations of the thread context 
##		switch routines. It also contains the longjmp() and setjmp()
##              routines.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/mips.inc>

	.set noat
	
#------------------------------------------------------------------------------
# hal_thread_switch_context
# Switch thread contexts
# A0 = address of sp of next thread to execute
# A1 = address of sp save location of current thread

	.global	hal_thread_switch_context
hal_thread_switch_context:		

	addi	sp,sp,-mipsreg_size	# space for registers

	# store GPRs
#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
	sw	$0,(mipsreg_regs+0*4)(sp)
	sw	$1,(mipsreg_regs+1*4)(sp)
	sw	$2,(mipsreg_regs+2*4)(sp)
	sw	$3,(mipsreg_regs+3*4)(sp)
	sw	$4,(mipsreg_regs+4*4)(sp)
	sw	$5,(mipsreg_regs+5*4)(sp)
	sw	$6,(mipsreg_regs+6*4)(sp)
	sw	$7,(mipsreg_regs+7*4)(sp)
	sw	$8,(mipsreg_regs+8*4)(sp)
	sw	$9,(mipsreg_regs+9*4)(sp)
	sw	$10,(mipsreg_regs+10*4)(sp)
	sw	$11,(mipsreg_regs+11*4)(sp)
	sw	$12,(mipsreg_regs+12*4)(sp)
	sw	$13,(mipsreg_regs+13*4)(sp)
	sw	$14,(mipsreg_regs+14*4)(sp)
	sw	$15,(mipsreg_regs+15*4)(sp)
	sw	$24,(mipsreg_regs+24*4)(sp)
	sw	$25,(mipsreg_regs+25*4)(sp)
	sw	$28,(mipsreg_regs+28*4)(sp)	# == GP
#endif	
	sw	$16,(mipsreg_regs+16*4)(sp)
	sw	$17,(mipsreg_regs+17*4)(sp)	
	sw	$18,(mipsreg_regs+18*4)(sp)
	sw	$19,(mipsreg_regs+19*4)(sp)
	sw	$20,(mipsreg_regs+20*4)(sp)
	sw	$21,(mipsreg_regs+21*4)(sp)
	sw	$22,(mipsreg_regs+22*4)(sp)
	sw	$23,(mipsreg_regs+23*4)(sp)
#	sw	$26,(mipsreg_regs+26*4)(sp)	# == K0
#	sw	$27,(mipsreg_regs+27*4)(sp)	# == K1
#	sw	$29,(mipsreg_regs+29*4)(sp)	# == SP
	sw	$30,(mipsreg_regs+30*4)(sp)	# == FP
	sw	$31,(mipsreg_regs+31*4)(sp)	# == RA
	sw	$31,(mipsreg_pc)(sp)		# == PC (to help with debugging)

#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

	mflo	t0				# save LO and HI regs
	mfhi	t1
	sw	t0,mipsreg_lo(sp)
	sw	t1,mipsreg_hi(sp)
	
#endif

	addi	t0,sp,mipsreg_size		# save SP in reg dump
	sw	t0,(mipsreg_regs+29*4)(sp)

	sw	sp,0(a1)			# save sp in save loc
	
	# Now load the destination thread by dropping through
	# to hal_thread_load_context
	
#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
# A0 = address of sp of next thread to execute
# Note that this function is also the second half of hal_thread_switch_context
# and is simply dropped into from it.
	
	.global	hal_thread_load_context
hal_thread_load_context:

	lw	sp,0(a0)			# load new SP directly

#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
	
	lw	t0,mipsreg_hi(sp)		# load HI and LO regs
	lw	t1,mipsreg_lo(sp)
	mthi	t0
	mtlo	t1

#endif
		
	# load GPRs
#	lw	$0,(mipsreg_regs+0*4)(sp)
	lw	$4,(mipsreg_regs+4*4)(sp)	# A0, must load for thread startup
#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
	lw	$1,(mipsreg_regs+1*4)(sp)
	lw	$2,(mipsreg_regs+2*4)(sp)
	lw	$3,(mipsreg_regs+3*4)(sp)
	lw	$5,(mipsreg_regs+5*4)(sp)
	lw	$6,(mipsreg_regs+6*4)(sp)
	lw	$7,(mipsreg_regs+7*4)(sp)
	lw	$8,(mipsreg_regs+8*4)(sp)
	lw	$9,(mipsreg_regs+9*4)(sp)
	lw	$10,(mipsreg_regs+10*4)(sp)
	lw	$11,(mipsreg_regs+11*4)(sp)
	lw	$12,(mipsreg_regs+12*4)(sp)
	lw	$13,(mipsreg_regs+13*4)(sp)
	lw	$14,(mipsreg_regs+14*4)(sp)
	lw	$15,(mipsreg_regs+15*4)(sp)
	lw	$24,(mipsreg_regs+24*4)(sp)
	lw	$25,(mipsreg_regs+25*4)(sp)
#endif	
	lw	$16,(mipsreg_regs+16*4)(sp)
	lw	$17,(mipsreg_regs+17*4)(sp)
	lw	$18,(mipsreg_regs+18*4)(sp)
	lw	$19,(mipsreg_regs+19*4)(sp)
	lw	$20,(mipsreg_regs+20*4)(sp)
	lw	$21,(mipsreg_regs+21*4)(sp)
	lw	$22,(mipsreg_regs+22*4)(sp)
	lw	$23,(mipsreg_regs+23*4)(sp)
#	lw	$26,(mipsreg_regs+26*4)(sp)	# == K0
#	lw	$27,(mipsreg_regs+27*4)(sp)	# == K1
#	lw	$28,(mipsreg_regs+28*4)(sp)	# == GP
#	lw	$29,(mipsreg_regs+29*4)(sp)	# == SP
	lw	$30,(mipsreg_regs+30*4)(sp)	# == FP
	lw	$31,(mipsreg_regs+31*4)(sp)	# == RA

	# Note that the following lw  must go here and not
	# in the jr delay slot, since it has a delay slot
	# of its own.
	lw	sp,(mipsreg_regs+29*4)(sp)	# load SP
	jr	ra				# return via ra
	nop					# delay slot - must be nop

#------------------------------------------------------------------------------
# HAL longjmp, setjmp implementations
# hal_setjmp saves only to callee save registers 16-23, 28, 30, 31[ra], 29[sp]
# into buffer supplied in a0[arg0]
# Note: These definitions are repeated in hal_arch.h. If changes are required
# remember to update both sets.

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_R16       1
#define CYGARC_JMP_BUF_R17       2
#define CYGARC_JMP_BUF_R18       3
#define CYGARC_JMP_BUF_R19       4
#define CYGARC_JMP_BUF_R20       5
#define CYGARC_JMP_BUF_R21       6
#define CYGARC_JMP_BUF_R22       7
#define CYGARC_JMP_BUF_R23       8
#define CYGARC_JMP_BUF_R28       9
#define CYGARC_JMP_BUF_R30      10
#define CYGARC_JMP_BUF_R31      11

#define CYGARC_JMP_BUF_SIZE     12

	.globl	hal_setjmp
	.ent	hal_setjmp
hal_setjmp:
	sw	$31,CYGARC_JMP_BUF_R31*4(a0)	# ra (link)
	sw	$30,CYGARC_JMP_BUF_R30*4(a0)
	sw	$28,CYGARC_JMP_BUF_R28*4(a0)	# gp, optimize out?
	sw	$23,CYGARC_JMP_BUF_R23*4(a0)
	sw	$22,CYGARC_JMP_BUF_R22*4(a0)
	sw	$21,CYGARC_JMP_BUF_R21*4(a0)
	sw	$20,CYGARC_JMP_BUF_R20*4(a0)
	sw	$19,CYGARC_JMP_BUF_R19*4(a0)
	sw	$18,CYGARC_JMP_BUF_R18*4(a0)
	sw	$17,CYGARC_JMP_BUF_R17*4(a0)
	sw	$16,CYGARC_JMP_BUF_R16*4(a0)
	sw	sp,CYGARC_JMP_BUF_SP*4(a0)	# $29
	li	v0,0
	jr	ra
	nop			# delay slot
	.end hal_setjmp

	.globl	hal_longjmp
	.ent	hal_longjmp
hal_longjmp:	
	lw	$31,CYGARC_JMP_BUF_R31*4(a0)	# ra (link)
	lw	$30,CYGARC_JMP_BUF_R30*4(a0)
	lw	$28,CYGARC_JMP_BUF_R28*4(a0)	# gp, optimize out?
	lw	$23,CYGARC_JMP_BUF_R23*4(a0)
	lw	$22,CYGARC_JMP_BUF_R22*4(a0)
	lw	$21,CYGARC_JMP_BUF_R21*4(a0)
	lw	$20,CYGARC_JMP_BUF_R20*4(a0)
	lw	$19,CYGARC_JMP_BUF_R19*4(a0)
	lw	$18,CYGARC_JMP_BUF_R18*4(a0)
	lw	$17,CYGARC_JMP_BUF_R17*4(a0)
	lw	$16,CYGARC_JMP_BUF_R16*4(a0)
	lw	sp,CYGARC_JMP_BUF_SP*4(a0)	# $29
	move	v0,a1
	jr	ra
	nop			# delay slot
	.end hal_longjmp
	
	
#------------------------------------------------------------------------------
# end of context.S
