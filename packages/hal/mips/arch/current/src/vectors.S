##=============================================================================
##
##	vectors.S
##
##	MIPS exception vectors
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
## Date:	1998-02-04
## Purpose:	MIPS exception vectors
## Description:	This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##		that save and restore state for both exceptions and
##		interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================

#	.file	"vectors.S"

#include <pkgconf/system.h>	
#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>	
#endif
			
#include <cyg/hal/mips.inc>
		
	.extern cyg_instrument

##-----------------------------------------------------------------------------

#define MAX_SAVE_REG	31	
			
#if defined(CYG_HAL_STARTUP_RAM)
# if defined(CYG_HAL_MIPS_SIM) || !defined(CYG_HAL_USE_ROM_MONITOR)
#  define INITIAL_SR	0x1000ff00	/* CP0 usable, Ints enabled	*/
//#define INITIAL_SR	0x10000000	/* CP0 usable, Ints disabled	*/
# else
#  define INITIAL_SR	0x1040ff00	/* as above + ROM vectors used	*/
# endif	
#elif defined(CYG_HAL_STARTUP_ROM)
# define INITIAL_SR	0x1040ff00	/* as above + ROM vectors used	*/
#endif

## ROM timing characteristics dependent on the clock speed.
		
#if (CYGHWR_HAL_MIPS_CPU_FREQ == 50)
#define ROM_CCR0_INIT	0x00000420
#define DRAM_DREFC_INIT	0x00000180
#elif (CYGHWR_HAL_MIPS_CPU_FREQ == 66)
#define ROM_CCR0_INIT	0x00000520
#define DRAM_DREFC_INIT	0x00000200
#else
#error Unsupported clock frequency		
#endif

## DRAM configuration dependent on the DRAM device used.
      ## for 16MByte (4MBit (x4bit) x 8)           0x08024030
      ## for 4MByte (1MBit (x4bit) x 8)            0x08013020
      ## for 8MByte (1MBit (x4bit) x 8 x 2 banks)  0x08013020

#if defined CYGHWR_HAL_TX39_JMR3904_DRAM_CONFIG_INIT
#define DRAM_CONFIG_INIT CYGHWR_HAL_TX39_JMR3904_DRAM_CONFIG_INIT
#else
#define DRAM_CONFIG_INIT 0x08024030
#endif

##-----------------------------------------------------------------------------
## Hardware supplied vectors
	
	.set	noreorder
	.set	noat
 
	.section ".reset_vector","ax"

	# Reset vector at 0xBFC00000
	
	.globl	reset_vector
reset_vector:
	la	v0,_start		# jump to start
	jr	v0
	nop				# (delay slot)

	.section ".debug_vector","ax"
		
	# Debug vector at 0xBFC00200
	
	.globl	debug_vector
debug_vector:
	la	k0,32
	la	k1,hal_vsr_table	# Get VSR table
	lw	k1,32*4(k1)		# load debug vector
	jr	k1			# go there
	nop				# (delay slot)

	.section ".other_vector","ax"

	# Common vector at 0x80000080 or 0xBFC00180
	
	.globl	other_vector	
other_vector:
	mfc0	k0,cause		# K0 = exception cause
	nop
	andi	k0,k0,0x7F		# isolate exception code
	la	k1,hal_vsr_table	# address of VSR table
	add	k1,k1,k0		# offset of VSR entry
	lw	k1,0(k1)		# k1 = pointer to VSR
	jr	k1			# go there
	nop				# (delay slot)
		
	.section ".utlb_vector","ax"

	.globl	utlb_vector
utlb_vector:	
	la	k0,33
	la	k1,hal_vsr_table	# Get VSR table
	lw	k1,33*4(k1)		# load utlb vector
	jr	k1			# go there
	nop				# (delay slot)

	
##-----------------------------------------------------------------------------
## Startup code
	
	.text
	
	.globl	_start
	.ent	_start
_start:
	# init CPU registers
	mtc0	zero,cause		# zero cause reg
	la	v0,INITIAL_SR
	mtc0	v0,sr
	mfc0	v0,config
	nop
	.set	at
	and	v0,v0,0xffffffcf
#	la	v0,0
	.set	noat
	mtc0	v0,config

	
	# Initialize hardware

#ifdef CYG_HAL_MIPS_JMR3904
	
	# These mappings need to be set up before we
	# can use the stack and make calls to other
	# functions

	# If we have been started from Cygmon, it should have
	# already done a lot of this, but it should do no harm
	# to reinitialize the following registers.
			
	# SCS0,1 base addr of ISA & PCI
	la	a0,0xffffe010
	la	a1,0x20201410
	sw	a1,0(a0)	

	la	a0,0xffffe014
	la	a1,0xfffffcfc
	sw	a1,0(a0)	

	# ROM configuration
	.set	at
	la	a0,0xffff9000
	lw	a1,0(a0)
	and	a1,a1,0xffff0004	# keep hardware defaults
	or	a1,ROM_CCR0_INIT	# install our values
	sw	a1,0(a0)	
	.set	noat

	# SRAM config
	la	a0,0xffff9100
	la	a1,0x00000000
	sw	a1,0(a0)	

	# ISA bus setup
	la	a0,0xb2100000
	la	a1,4
	sb	a1,0(a0)	
	
	# Clear IMR (to cope with JMON)
	la	a0,0xffffc004
	la	a1,0x00000000
	sw	a1,0(a0)

#if defined(CYG_HAL_STARTUP_ROM)

	# Only do this in ROM configurations.
	
	# DRAM Configuration
	la	a0, 0xffff8000
	la	a1, DRAM_CONFIG_INIT
	sw	a1, 0(a0)
	
	# DBMR0
	la	a0, 0xffff8004
	la	a1, 0x00000000	
	sw	a1, 0(a0)
	
	# DWR0
	la	a0, 0xffff8008
	la	a1, 0x00000000
	sw	a1, 0(a0)
	
        # DREFC - Depends on clock requency
	la	a0, 0xffff8800
	la	a1, DRAM_DREFC_INIT
	sw	a1, 0(a0)

#endif

	# Set up PIO0 for output
	la	a0,0xfffff500
	la	a1,0xff
	sb	a1,0(a0)
	la	a1,0
	sb	a1,4(a0)	
				
#endif	
	
	# Load Global Pointer register.
	la	gp,_gp

	# load initial stack pointer
	la	a0,__interrupt_stack
	move	sp,a0

#if defined(CYG_HAL_STARTUP_ROM) || \
    (defined(CYG_HAL_STARTUP_RAM) && !defined(CYG_HAL_USE_ROM_MONITOR))
	
	# If we are starting up from ROM, or we are starting in
	# RAM and NOT using a ROM monitor, initialize the VSR table.

	la	a0,__default_interrupt_vsr
	la	a1,__default_exception_vsr
	la	a3,hal_vsr_table
	sw	a0,0(a3)
	sw	a1,1*4(a3)
	sw	a1,2*4(a3)
	sw	a1,3*4(a3)
	sw	a1,4*4(a3)
	sw	a1,5*4(a3)
	sw	a1,6*4(a3)
	sw	a1,7*4(a3)
	sw	a1,8*4(a3)
	sw	a1,9*4(a3)
	sw	a1,10*4(a3)
	sw	a1,11*4(a3)
	sw	a1,12*4(a3)
	sw	a1,13*4(a3)
	sw	a1,14*4(a3)
	sw	a1,15*4(a3)

	sw	a1,32*4(a3)
	sw	a1,33*4(a3)
	
#ifdef CYG_HAL_ROM_MONITOR

	# Install vectors to debug read and write functions
	
	.extern	hal_diag_write_char
	.extern	hal_diag_read_char
	.extern	breakpoint	

	la	a0,hal_diag_write_char
	sw	a0,63*4(a3)
	la	a1,hal_diag_read_char
	sw	a1,62*4(a3)
	la	a0,breakpoint
	sw	a0,61*4(a3)	
	
#endif		
		
#endif	
#if defined(CYG_HAL_STARTUP_RAM) && defined(CYG_HAL_USE_ROM_MONITOR)

	# initialize the VSR table
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	la	a0,__default_interrupt_vsr
	la	a3,hal_vsr_table
	sw	a0,0(a3)
		
#endif
			
#ifdef CYG_HAL_STARTUP_ROM
	# Copy data from ROM to RAM

	.extern	hal_copy_data
	jal	hal_copy_data
	nop
#endif
		
	# Zero BSS

	.extern hal_zero_bss
	jal	hal_zero_bss
	nop

	# Call constructors

	.extern cyg_hal_invoke_constructors
	jal     cyg_hal_invoke_constructors
	nop

	.extern cyg_hal_enable_caches
	jal     cyg_hal_enable_caches
	nop

	.extern cyg_hal_debug_init
	jal     cyg_hal_debug_init
	nop

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern initialize_stub
        jal     initialize_stub
        nop
#endif

	# Call cyg_start	
	
	.extern	cyg_start
	jal	cyg_start
	nop

9:	
	j	9b		# Loop if we return
	nop			# (delay slot)
	
	.end	_start	

##-----------------------------------------------------------------------------
## Default exception VSR.
## Saves machine state and calls external handling code.
	
	.globl	__default_exception_vsr
	.ent	__default_exception_vsr
__default_exception_vsr:

	# We enter here with all of the CPU state still
	# in its registers except:
	# K0 = vector index
	# K1 = address of this function

#ifdef CYG_HAL_ROM_MONITOR
	# Switch to interrupt stack to handle exception
	move	k1,sp
	la	sp,__interrupt_stack
#endif
			
	addi	sp,sp,-mips_exception_decrement
				# space for registers + safety margin

	sw	k0,mipsreg_vector(sp)	# store vector

	.set	noat
	# store GPRs
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
	sw	$16,(mipsreg_regs+16*4)(sp)
	sw	$17,(mipsreg_regs+17*4)(sp)
	sw	$18,(mipsreg_regs+18*4)(sp)
	sw	$19,(mipsreg_regs+19*4)(sp)
	sw	$20,(mipsreg_regs+20*4)(sp)
	sw	$21,(mipsreg_regs+21*4)(sp)
	sw	$22,(mipsreg_regs+22*4)(sp)
	sw	$23,(mipsreg_regs+23*4)(sp)
	sw	$24,(mipsreg_regs+24*4)(sp)
	sw	$25,(mipsreg_regs+25*4)(sp)
#	sw	$26,(mipsreg_regs+26*4)(sp)	# == K0
#	sw	$27,(mipsreg_regs+27*4)(sp)	# == K1
	sw	$28,(mipsreg_regs+28*4)(sp)	# == GP
#	sw	$29,(mipsreg_regs+29*4)(sp)	# == SP
	sw	$30,(mipsreg_regs+30*4)(sp)	# == FP
	sw	$31,(mipsreg_regs+31*4)(sp)	# == RA

	mfhi	a0
	mflo	a1
	sw	a0,mipsreg_hi(sp)
	sw	a1,mipsreg_lo(sp)

#ifdef CYG_HAL_ROM_MONITOR
	# K1 contains original SP
	sw	k1,(mipsreg_regs+29*4)(sp)	# store in reg dump	
#else		
	# synthesize original SP value
	move	a2,sp				# a2 = sp
	addi	a2,a2,mips_exception_decrement	# a2 = original sp
	sw	a2,(mipsreg_regs+29*4)(sp)	# store in reg dump	
#endif
	
	# save remaining machine state registers	
	mfc0	t0,cause
	mfc0	t1,sr
#	mfc0	t2,cache
	mfc0	t3,badvr
	mfc0	t4,config
	mfc0	t5,prid
	mfc0	t6,epc

#ifndef CYG_HAL_MIPS_SIM 
	mfc0	t2,cache
#endif
	
	sw	t0,mipsreg_cause(sp)
	sw	t1,mipsreg_sr(sp)
	sw	t2,mipsreg_cache(sp)
	sw	t3,mipsreg_badvr(sp)
	sw	t4,mipsreg_config(sp)
	sw	t5,mipsreg_prid(sp)
	sw	t6,mipsreg_pc(sp)

	
	# The machine state is now all saved on the stack.

	# Load Global Pointer register.
	la	gp,_gp
	
	move	s0,sp				# save pointer to saved state

	addi	sp,sp,-mips_stack_frame_size	# make a null frame	

	# Need to set up back pointers etc. ???

	la	ra,restore_state		# load return address
	
	.extern	exception_handler
	j	exception_handler		# call C code
	move	a0,s0				# arg0 = register dump (delay slot)

	# When the exception handler returns, it will
	# go back to restore_state, below.
			
	.end	__default_exception_vsr
	
##------------------------------------------------------------------------------
## Default interrupt VSR.
## Saves machine state and calls appropriate ISR. When done, calls
## interrupt_end() to finish up and possibly reschedule.	

	.globl	__default_interrupt_vsr
	.ent	__default_interrupt_vsr
__default_interrupt_vsr:

	
	# We enter here with all of the CPU state still
	# in its registers except:
	# K0 = vector index
	# K1 = address of this function

	addi	sp,sp,-mips_exception_decrement
				# space for registers + safety margin

	sw	k0,mipsreg_vector(sp)	# store vector

	# store GPRs
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
	sw	$16,(mipsreg_regs+16*4)(sp)
	sw	$17,(mipsreg_regs+17*4)(sp)
	sw	$18,(mipsreg_regs+18*4)(sp)
	sw	$19,(mipsreg_regs+19*4)(sp)
	sw	$20,(mipsreg_regs+20*4)(sp)
	sw	$21,(mipsreg_regs+21*4)(sp)
	sw	$22,(mipsreg_regs+22*4)(sp)
	sw	$23,(mipsreg_regs+23*4)(sp)
	sw	$24,(mipsreg_regs+24*4)(sp)
	sw	$25,(mipsreg_regs+25*4)(sp)
#	sw	$26,(mipsreg_regs+26*4)(sp)	# == K0
#	sw	$27,(mipsreg_regs+27*4)(sp)	# == K1
	sw	$28,(mipsreg_regs+28*4)(sp)	# == GP
#	sw	$29,(mipsreg_regs+29*4)(sp)	# == SP
	sw	$30,(mipsreg_regs+30*4)(sp)	# == FP
	sw	$31,(mipsreg_regs+31*4)(sp)	# == RA

	mfhi	a0
	mflo	a1
	sw	a0,mipsreg_hi(sp)
	sw	a1,mipsreg_lo(sp)

	# synthesize original SP value
	move	a2,sp				# a2 = sp
	addi	a2,a2,mips_exception_decrement	# a2 = original sp
	sw	a2,(mipsreg_regs+29*4)(sp)	# store in reg dump	
	
	mfc0	t1,sr
	mfc0	t3,epc
	
#ifndef CYG_HAL_MIPS_SIM
	mfc0	t2,cache
#endif
	sw	t1,mipsreg_sr(sp)
	sw	t2,mipsreg_cache(sp)
	sw	t3,mipsreg_pc(sp)

	# The machine state is now all saved on the stack.

	# Load Global Pointer register.
	la	gp,_gp
	
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT			
	.extern	cyg_scheduler_sched_lock
	la	v0,cyg_scheduler_sched_lock
	lw	a0,0(v0)
	addi	a0,a0,1
	sw	a0,0(v0)
#endif

	move	s0,sp				# save pointer to saved state
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK	

	la	a0,__interrupt_stack		# a0 = stack top
	la	a1,__interrupt_stack_base	# a1 = stack base
	sub	a3,sp,a1			# a3 = sp - base
	bltz	a3,1f				# if < 0 - not on istack
	nop					# delay slot
	sub	t0,a0,sp			# t0 = top - sp
	bgtz	t0,8f				# if > 0 - already on istack
	nop					# delay slot
1:	
	move	sp,a0				# switch to istack
8:
	addi	sp,sp,-4			# space for old SP
	sw	s0,0(sp)			# save old SP on stack
	
#endif
					
	subu	sp,sp,mips_stack_frame_size	# make a null frame	

	# Need to set up back pointers etc. ???

	# Decode external interrupt via interrupt controller

	mfc0	a1,cause			# get cause register
	nop
	srl	a1,a1,10			# shift IP bits to ls bits
	andi	a1,a1,0x7F			# isolate IP bits
	la	t0,interrupt_translation_table	# address of translation table
	add	t0,t0,a1			# offset of index byte
	lb	s2,0(t0)			# load it

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
	move	s1,zero				# Just vector zero is supported
#else			
	move	s1,s2				# Vector == interrupt number
#endif

	# Here s1 is the number of the vector to be called and s2 is
	# the number of the interrupt being serviced. 
	
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

	# Call cyg_instrument to record that this interrupt is being raised.
		
	li	a0,0x0301			# a0 = type = INTR,RAISE
	move	a1,s1				# a1 = vector number
	jal	cyg_instrument			# call instrument function
	xor	a2,a2,a2			# arg2 = 0 (delay slot)
#endif

	sll	s1,s1,2				# s1 = byte offset of vector
	
	la	t2,hal_interrupt_handlers	# handler table
	add	t2,t2,s1			# address of ISR ptr
	lw	t2,0(t2)			# ISR pointer

	la	a1,hal_interrupt_data		# data table
	add	a1,a1,s1			# address of data ptr
	lw	a1,0(a1)			# Data pointer

	move	a0,s2				# pass interrupt number

	move	a2,s0				# arg3 = saved register dump
		
	jalr	t2				# call ISR via t2
	nop					# (delay slot)

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

	# If we are returning from the last nested interrupt, move back
	# to the thread stack. interrupt_end() must be called on the
	# thread stack since it potentially causes a context switch.
	# Since we have arranged for the top of stack location to
	# contain the sp we need to go back to here, just pop it off
	# and put it in SP.

	
	lw	sp,mips_stack_frame_size(sp)	# sp = *sp
	subu	sp,sp,mips_stack_frame_size	# make a null frame		
#endif	
	
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

	# We only need to call _interrupt_end() when there is a kernel
	# present to do any tidying up.
	
	# On return v0 bit 1 will indicate whether a DSR is
	# to be posted. Pass this together with a pointer to
	# the interrupt object we have just used to the
	# interrupt tidy up routine.

	# Note that s0, s1 and s2 are defined to be preserved across
	# calls by the calling convention, so they still contain
	# the register dump, the vector offset and the interrupt number
	# respectively.

	move	a0,v0				# put ISR result in arg0
		
	la	a1,hal_interrupt_objects	# interrupt object table
	add	a1,a1,s1			# address of object ptr
	lw	a1,0(a1)			# a1 = object ptr

	move	a2,s0				# arg3 = saved register dump
	
	# interrupt_end() must be called with interrupts enabled.
	
	rfe					# restore previous status bits
						# this may enable interrupts
			
	.extern	interrupt_end
	jal	interrupt_end			# call into C to finish off 
	nop					# (delay slot)

#endif
				
restore_state:	

	# All done, restore CPU state and continue

	addi	sp,sp,mips_stack_frame_size	# retrieve CPU state ptr
	
	# Load status register and cache control register.
	# This will put the interrupt, mode and cache control
	# bits back to their post-exception values, so we will
	# need to do another RFE before we finish.
		
	lw	t1,mipsreg_sr(sp)
	lw	t2,mipsreg_cache(sp)

	mtc0	t1,sr
#ifndef CYG_HAL_MIPS_SIM
	mtc0	t2,cache
#endif
		
	lw	a0,mipsreg_hi(sp)
	lw	a1,mipsreg_lo(sp)
	mthi	a0
	mtlo	a1
	
		# load GPRs
#	lw	$0,(mipsreg_regs+0*4)(sp)
	lw	$1,(mipsreg_regs+1*4)(sp)
	lw	$2,(mipsreg_regs+2*4)(sp)
	lw	$3,(mipsreg_regs+3*4)(sp)
	lw	$4,(mipsreg_regs+4*4)(sp)
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
	lw	$16,(mipsreg_regs+16*4)(sp)
	lw	$17,(mipsreg_regs+17*4)(sp)
	lw	$18,(mipsreg_regs+18*4)(sp)
	lw	$19,(mipsreg_regs+19*4)(sp)
	lw	$20,(mipsreg_regs+20*4)(sp)
	lw	$21,(mipsreg_regs+21*4)(sp)
	lw	$22,(mipsreg_regs+22*4)(sp)
	lw	$23,(mipsreg_regs+23*4)(sp)
	lw	$24,(mipsreg_regs+24*4)(sp)
	lw	$25,(mipsreg_regs+25*4)(sp)
#	lw	$26,(mipsreg_regs+26*4)(sp)	# == K0
#	lw	$27,(mipsreg_regs+27*4)(sp)	# == K1
	lw	$28,(mipsreg_regs+28*4)(sp)	# == GP
#	lw	$29,(mipsreg_regs+29*4)(sp)	# == SP
	lw	$30,(mipsreg_regs+30*4)(sp)	# == FP
	lw	$31,(mipsreg_regs+31*4)(sp)	# == RA

	lw	k0,mipsreg_pc(sp)		# K0 = return PC
	lw	sp,(mipsreg_regs+29*4)(sp)	# load SP
	sync
	jr	k0				# jump back to interrupted code
	rfe					# restore state		(delay slot)
		
	.end	__default_interrupt_vsr

# This table translates from the 6 bit value supplied in the IP bits
# of the cause register into a 0..16 offset into the ISR tables.
interrupt_translation_table:	
	.byte	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	.byte	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	.byte	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
	.byte	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
		
##-----------------------------------------------------------------------------
## Interrupt Stack.
## Used during intialization and for executing ISRs.
	
	.bss

	.balign 16
__interrupt_stack_base:
	.rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
	.byte 0
	.endr
	.balign 16
__interrupt_stack:

	.long	0,0,0,0,0,0,0,0	
	
##-----------------------------------------------------------------------------
## VSR table.
## The main interrupt code indirects through here to find the VSR
## to execute for each architecture defined interrupt.
## This is only used for simulated targets, on real targets a fixed location VSR
## table is now allocated at 0x80000100.

#if defined(CYG_HAL_MIPS_SIM) ||	\
    ( defined(CYG_HAL_MIPS_TX3904) &&	\
      defined(CYG_HAL_STARTUP_RAM) &&	\
      !defined(CYG_HAL_USE_ROM_MONITOR) \
    )
	.section ".vsr_table","a"
			
	.globl	hal_vsr_table

hal_vsr_table:	
	.long	__default_interrupt_vsr	
	.rept	63
	.long	__default_exception_vsr	
	.endr

#endif	
	
#------------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.
# hal_interrupt_level contains the interrupt level set by 
# HAL_INTERRUPT_CONFIGURE().
	

	.extern hal_default_isr
	
	.data

	.globl	hal_interrupt_handlers
hal_interrupt_handlers:
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr
	.long	hal_default_isr

	.globl	hal_interrupt_data
hal_interrupt_data:
	.rept	17
	.long	0
	.endr

	.globl	hal_interrupt_objects
hal_interrupt_objects:
	.rept	17
	.long	0
	.endr

	.globl	hal_interrupt_level
hal_interrupt_level:
	.rept	17
	.byte	7
	.endr

##-----------------------------------------------------------------------------
## end of vectors.S
