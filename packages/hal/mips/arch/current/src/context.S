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

#include <cyg/hal/arch.inc>

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
	sw	$0,(mipsreg_regs+0*mips_regsize)(sp)
	sw	$1,(mipsreg_regs+1*mips_regsize)(sp)
	sw	$2,(mipsreg_regs+2*mips_regsize)(sp)
	sw	$3,(mipsreg_regs+3*mips_regsize)(sp)
	sw	$4,(mipsreg_regs+4*mips_regsize)(sp)
	sw	$5,(mipsreg_regs+5*mips_regsize)(sp)
	sw	$6,(mipsreg_regs+6*mips_regsize)(sp)
	sw	$7,(mipsreg_regs+7*mips_regsize)(sp)
	sw	$8,(mipsreg_regs+8*mips_regsize)(sp)
	sw	$9,(mipsreg_regs+9*mips_regsize)(sp)
	sw	$10,(mipsreg_regs+10*mips_regsize)(sp)
	sw	$11,(mipsreg_regs+11*mips_regsize)(sp)
	sw	$12,(mipsreg_regs+12*mips_regsize)(sp)
	sw	$13,(mipsreg_regs+13*mips_regsize)(sp)
	sw	$14,(mipsreg_regs+14*mips_regsize)(sp)
	sw	$15,(mipsreg_regs+15*mips_regsize)(sp)
	sw	$24,(mipsreg_regs+24*mips_regsize)(sp)
	sw	$25,(mipsreg_regs+25*mips_regsize)(sp)
	sw	$28,(mipsreg_regs+28*mips_regsize)(sp)	# == GP
#endif	
	sw	$16,(mipsreg_regs+16*mips_regsize)(sp)
	sw	$17,(mipsreg_regs+17*mips_regsize)(sp)	
	sw	$18,(mipsreg_regs+18*mips_regsize)(sp)
	sw	$19,(mipsreg_regs+19*mips_regsize)(sp)
	sw	$20,(mipsreg_regs+20*mips_regsize)(sp)
	sw	$21,(mipsreg_regs+21*mips_regsize)(sp)
	sw	$22,(mipsreg_regs+22*mips_regsize)(sp)
	sw	$23,(mipsreg_regs+23*mips_regsize)(sp)
#	sw	$26,(mipsreg_regs+26*mips_regsize)(sp)	# == K0
#	sw	$27,(mipsreg_regs+27*mips_regsize)(sp)	# == K1
#	sw	$29,(mipsreg_regs+29*mips_regsize)(sp)	# == SP
	sw	$30,(mipsreg_regs+30*mips_regsize)(sp)	# == FP
	sw	$31,(mipsreg_regs+31*mips_regsize)(sp)	# == RA
	sw	$31,(mipsreg_pc)(sp)		# == PC (to help with debugging)

#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

	mflo	t0				# save LO and HI regs
	mfhi	t1
	sw	t0,mipsreg_lo(sp)
	sw	t1,mipsreg_hi(sp)
	
#endif

	hal_fpu_save_callee	sp

#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM

	hal_fpu_save_caller	sp

#endif
	
	addi	t0,sp,mipsreg_size		# save SP in reg dump
	sw	t0,(mipsreg_regs+29*mips_regsize)(sp)

	mfc0	t1,status			# Save status register
	sw	t1,mipsreg_sr(sp)
	
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

	hal_fpu_load_caller sp	
	
#endif
			
	hal_fpu_load_callee sp
	
#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
	
	lw	t0,mipsreg_hi(sp)		# load HI and LO regs
	lw	t1,mipsreg_lo(sp)
	mthi	t0
	mtlo	t1

#endif

	# load GPRs
#	lw	$0,(mipsreg_regs+0*mips_regsize)(sp)
	lw	$4,(mipsreg_regs+4*mips_regsize)(sp)	# A0, must load for thread startup
#ifndef CYGDBG_HAL_COMMON_CONTEXT_SAVE_MINIMUM
	lw	$1,(mipsreg_regs+1*mips_regsize)(sp)
	lw	$2,(mipsreg_regs+2*mips_regsize)(sp)
	lw	$3,(mipsreg_regs+3*mips_regsize)(sp)
	lw	$5,(mipsreg_regs+5*mips_regsize)(sp)
	lw	$6,(mipsreg_regs+6*mips_regsize)(sp)
	lw	$7,(mipsreg_regs+7*mips_regsize)(sp)
	lw	$8,(mipsreg_regs+8*mips_regsize)(sp)
	lw	$9,(mipsreg_regs+9*mips_regsize)(sp)
	lw	$10,(mipsreg_regs+10*mips_regsize)(sp)
	lw	$11,(mipsreg_regs+11*mips_regsize)(sp)
	lw	$12,(mipsreg_regs+12*mips_regsize)(sp)
	lw	$13,(mipsreg_regs+13*mips_regsize)(sp)
	lw	$14,(mipsreg_regs+14*mips_regsize)(sp)
	lw	$15,(mipsreg_regs+15*mips_regsize)(sp)
	lw	$24,(mipsreg_regs+24*mips_regsize)(sp)
	lw	$25,(mipsreg_regs+25*mips_regsize)(sp)
#endif	
	lw	$16,(mipsreg_regs+16*mips_regsize)(sp)
	lw	$17,(mipsreg_regs+17*mips_regsize)(sp)
	lw	$18,(mipsreg_regs+18*mips_regsize)(sp)
	lw	$19,(mipsreg_regs+19*mips_regsize)(sp)
	lw	$20,(mipsreg_regs+20*mips_regsize)(sp)
	lw	$21,(mipsreg_regs+21*mips_regsize)(sp)
	lw	$22,(mipsreg_regs+22*mips_regsize)(sp)
	lw	$23,(mipsreg_regs+23*mips_regsize)(sp)
#	lw	$26,(mipsreg_regs+26*mips_regsize)(sp)	# == K0
#	lw	$27,(mipsreg_regs+27*mips_regsize)(sp)	# == K1
#	lw	$28,(mipsreg_regs+28*mips_regsize)(sp)	# == GP
#	lw	$29,(mipsreg_regs+29*mips_regsize)(sp)	# == SP
	lw	$30,(mipsreg_regs+30*mips_regsize)(sp)	# == FP
	lw	$31,(mipsreg_regs+31*mips_regsize)(sp)	# == RA

	lw	a2,mipsreg_sr(sp)		# A2 = saved SR
	lw	sp,(mipsreg_regs+29*mips_regsize)(sp)	# SP = saved SP

	hal_cpu_int_merge a2			# Merge with current SR
	
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
	sw	$31,CYGARC_JMP_BUF_R31*mips_regsize(a0)	# ra (link)
	sw	$30,CYGARC_JMP_BUF_R30*mips_regsize(a0)
	sw	$28,CYGARC_JMP_BUF_R28*mips_regsize(a0)	# gp, optimize out?
	sw	$23,CYGARC_JMP_BUF_R23*mips_regsize(a0)
	sw	$22,CYGARC_JMP_BUF_R22*mips_regsize(a0)
	sw	$21,CYGARC_JMP_BUF_R21*mips_regsize(a0)
	sw	$20,CYGARC_JMP_BUF_R20*mips_regsize(a0)
	sw	$19,CYGARC_JMP_BUF_R19*mips_regsize(a0)
	sw	$18,CYGARC_JMP_BUF_R18*mips_regsize(a0)
	sw	$17,CYGARC_JMP_BUF_R17*mips_regsize(a0)
	sw	$16,CYGARC_JMP_BUF_R16*mips_regsize(a0)
	sw	sp,CYGARC_JMP_BUF_SP*mips_regsize(a0)	# $29
	li	v0,0
	jr	ra
	nop			# delay slot
	.end hal_setjmp

	.globl	hal_longjmp
	.ent	hal_longjmp
hal_longjmp:	
	lw	$31,CYGARC_JMP_BUF_R31*mips_regsize(a0)	# ra (link)
	lw	$30,CYGARC_JMP_BUF_R30*mips_regsize(a0)
	lw	$28,CYGARC_JMP_BUF_R28*mips_regsize(a0)	# gp, optimize out?
	lw	$23,CYGARC_JMP_BUF_R23*mips_regsize(a0)
	lw	$22,CYGARC_JMP_BUF_R22*mips_regsize(a0)
	lw	$21,CYGARC_JMP_BUF_R21*mips_regsize(a0)
	lw	$20,CYGARC_JMP_BUF_R20*mips_regsize(a0)
	lw	$19,CYGARC_JMP_BUF_R19*mips_regsize(a0)
	lw	$18,CYGARC_JMP_BUF_R18*mips_regsize(a0)
	lw	$17,CYGARC_JMP_BUF_R17*mips_regsize(a0)
	lw	$16,CYGARC_JMP_BUF_R16*mips_regsize(a0)
	lw	sp,CYGARC_JMP_BUF_SP*mips_regsize(a0)	# $29
	move	v0,a1
	jr	ra
	nop			# delay slot
	.end hal_longjmp
	
	
#------------------------------------------------------------------------------
# end of context.S
