#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	AM33 assembler header file
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
## Author(s): 	nickg
## Contributors:	nickg, dmoseley
## Date:	1999-04-06
## Purpose:	AM33 definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the AM33 CPU variant.
## Usage:
##		#include <cyg/hal/variant.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

	.am33		# Enable AM33 instruction set

#include <cyg/hal/platform.inc>

#------------------------------------------------------------------------------
# Register definitions

#define NMICR	0xD4000000	// NMI control register
#define DCR	0xC0000030	// Debug control register
#define ISR	0xC0000034	// Interrupt control register
#define TBR	0xC0000024	// Trap Base Register
#define CHCTR	0xC0000070	// Cache Control Register

#define DCR_DE	0x0001		// DE bit in DCR

##-----------------------------------------------------------------------------
## Register addresses and initialization values

#define SDRAMCONFIG		0xDA000000

#define INIT_SDRAMCONFIG	0x0000002f

#------------------------------------------------------------------------------
# AM33 specific CPU initialization:

	# Initialize CPU
	.macro	hal_cpu_init
	# Set up the PSW
	mov	0x00000700,d0
	mov	d0,epsw
#ifndef CYGSEM_HAL_USE_ROM_MONITOR
	mov	reset_vector,a0		# set TBR to vector table
	mov	a0,(TBR)
#endif
	.endm

#define CYGPKG_HAL_MN10300_CPU_INIT_DEFINED

#------------------------------------------------------------------------------
# CPU state save and restore macros

	.macro	hal_cpu_save_all
	movm	[all],(sp)				# push all registers
	.endm

	.macro	hal_cpu_load_all
	movm	(sp),[all]				# pop regs
	.endm

	.macro	hal_cpu_get_psw reg
	mov	epsw,\reg
	.endm

	.macro	hal_cpu_set_psw reg
	mov	\reg,epsw
	.endm

# Location of PC in saved context (HAL_SavedRegisters)
#define SAVED_CONTEXT_PC_OFFSET          104

#-----------------------------------------------------------------------------
# Clear the NMID bit in the epsw to allow NMIs to be delivered again.

	.macro	hal_cpu_clear_nmid
	and	0xFFFDFFFF,epsw			# clear NMID bit
	.endm


#------------------------------------------------------------------------------
# MEMC macros.
	
#ifndef CYGPKG_HAL_MN10300_MEMC_DEFINED

	.macro	hal_memc_init
	mov	INIT_SDRAMCONFIG,d0
	mov	d0,(SDRAMCONFIG)
	.endm

#define CYGPKG_HAL_MN10300_MEMC_DEFINED

#endif	
	
#------------------------------------------------------------------------------
# Cache macros.
	
#ifndef CYGPKG_HAL_MN10300_CACHE_DEFINED

	.macro	hal_cache_init

#if defined(CYG_HAL_STARTUP_ROM)
    # Note that the hardware seems to come up with the
    # caches containing random data. Hence they must be
    # invalidated before being enabled.
    # However, we only do this if we are in ROM. If we are
    # in RAM, then we leave the caches in the state chosen
    # by the ROM monitor. If we enable them when the monitor
    # is not expecting it, we can end up breaking things if the
    # monitor is not doing cache flushes.
	mov	CHCTR,a0
	mov	0x0030,d0
	mov	d0,(a0)				# Invalidate the ICACHE and DCACHE
	mov	0x0003,d0
	mov	d0,(a0)				# Enable both caches
#else
	nop
#endif
	.endm

#define CYGPKG_HAL_MN10300_CACHE_DEFINED

#endif	

##-----------------------------------------------------------------------------
# Default interrupt decoding macros.

#ifndef CYGPKG_HAL_MN10300_INTC_DEFINED


	
	# initialize all interrupts to disabled
	.macro	hal_intc_init
	.endm

#define CYGPKG_HAL_MN10300_INTC_DEFINED

#endif

#ifndef CYGPKG_HAL_MN10300_INTC_DECODE_DEFINED

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN

#define CYG_ISR_TABLE_SIZE	10

	.macro	hal_intc_decode dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg1		# dreg1 = IAGR
	mov	(0,a2),\dreg			# dreg = vector priority
	mov	\dreg1,(0,a2)			# store real vector in saved state
	asl	2,\dreg				# dreg = byte index of isr
	add	12,\dreg			# skip NMI vectors
	.endm

#else

#define CYG_ISR_TABLE_SIZE	 57

	# decode the interrupt
	.macro  hal_intc_decode dreg,areg,dreg1
	mov	_mn10300_interrupt_control,\areg
	movhu	(0x100,\areg),\dreg		# dreg = IAGR
	mov	\dreg,(0,a2)			# store real vector in saved state
	add	12,\dreg			# skip NMI vectors
	.endm
		
#endif

#define CYGPKG_HAL_MN10300_INTC_DECODE_DEFINED

#endif


#------------------------------------------------------------------------------
# Diagnostics macros.
	
#ifndef CYGPKG_HAL_MN10300_DIAG_DEFINED

	.macro	hal_diag_init
	.endm

	.macro	hal_diag_excpt_start
	.endm

	.macro	hal_diag_intr_start
	.endm

	.macro	hal_diag_restore
	.endm

	.macro	hal_diag_led val
	movm	[d2,a2],(sp)
	mov	\val,d2
	movm	(sp),[d2,a2]
	.endm


#define CYGPKG_HAL_MN10300_DIAG_DEFINED

#endif	

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_MN10300_MON_DEFINED

	.macro	hal_mon_init
	hal_mon_init_vectors
	hal_mon_init_vsr
	.endm

#if defined(CYG_HAL_STARTUP_ROM)
	.macro	hal_mon_init_vectors
	mov	_mn10300_interrupt_vectors,a0
	mov	__hardware_vector_0,d0
	movhu	d0,(0,a0)
	mov	__hardware_vector_1,d0
	movhu	d0,(4,a0)
	mov	__hardware_vector_2,d0
	movhu	d0,(8,a0)
	mov	__hardware_vector_3,d0
	movhu	d0,(12,a0)
	mov	__hardware_vector_4,d0
	movhu	d0,(16,a0)
	mov	__hardware_vector_5,d0
	movhu	d0,(20,a0)
	mov	__hardware_vector_6,d0
	movhu	d0,(24,a0)
	.endm
#else
	.macro	hal_mon_init_vectors
	.endm
#endif

	.extern nmi_vsr_trampoline
	.extern nmi_sysef_trampoline

	# init vsr table in SRAM where the ROM
	# vectors the interrupts.

#if defined(CYG_HAL_STARTUP_ROM)
	.macro	hal_mon_init_vsr
	mov	_hal_vsr_table,a0
	mov	__default_interrupt_vsr,d0
	mov	d0,(0,a0)	
	mov	d0,(4,a0)
	mov	d0,(8,a0)
	mov	d0,(12,a0)	
	mov	d0,(16,a0)	
	mov	d0,(20,a0)	
	mov	d0,(24,a0)
	mov	nmi_vsr_trampoline,d0
	mov	d0,(28,a0)
	mov	__default_trap_vsr,d0
	mov	d0,(32,a0)
	mov	nmi_sysef_trampoline,d0
	mov	d0,(44,a0)
	mov	__default_nmi_vsr,d0
	mov	d0,(36,a0)
	mov	d0,(40,a0)
	mov	d0,(48,a0)
	mov	d0,(52,a0)
	mov	d0,(56,a0)
	mov	d0,(60,a0)
	mov	d0,(64,a0)
	mov	d0,(68,a0)
	mov	d0,(72,a0)
	mov	d0,(76,a0)
	mov	d0,(80,a0)
	mov	d0,(84,a0)
	mov	d0,(88,a0)
	mov	d0,(92,a0)
	mov	d0,(96,a0)
	mov	d0,(100,a0)
	mov	d0,(104,a0)
	mov	d0,(108,a0)
	.endm
#elif defined(CYG_HAL_STARTUP_RAM)
	.macro	hal_mon_init_vsr
	mov	_hal_vsr_table,a0
	mov	__default_interrupt_vsr,d0
	mov	d0,(0,a0)	
	mov	d0,(4,a0)
	mov	d0,(8,a0)
	mov	d0,(12,a0)	
	mov	d0,(16,a0)	
	mov	d0,(20,a0)	
	mov	d0,(24,a0)
#if !(defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs))
	mov	nmi_vsr_trampoline,d0
	mov	d0,(28,a0)
	mov	nmi_sysef_trampoline,d0
	mov	d0,(44,a0)
#endif
	mov	__default_trap_vsr,d0
	mov	d0,(32,a0)
	mov	__default_nmi_vsr,d0
	mov	d0,(36,a0)
	mov	d0,(40,a0)
	mov	d0,(48,a0)
	mov	d0,(52,a0)
	mov	d0,(56,a0)
	mov	d0,(60,a0)
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	mov	d0,(64,a0)	# breakpoint VSR only if stubs included
#endif
	mov	d0,(68,a0)
	mov	d0,(72,a0)
	mov	d0,(76,a0)
	mov	d0,(80,a0)
	mov	d0,(84,a0)
	mov	d0,(88,a0)
	mov	d0,(92,a0)
	mov	d0,(96,a0)
	mov	d0,(100,a0)
	mov	d0,(104,a0)
	mov	d0,(108,a0)
	.endm
#else
	.macro	hal_mon_init_vsr
	.endif
#endif

#define CYGPKG_HAL_MN10300_MON_DEFINED

#define CYG_HAL_MN10300_VSR_TABLE_DEFINED

#endif	



#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
