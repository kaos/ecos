#ifndef CYGONCE_HAL_ARCH_INC
#define CYGONCE_HAL_ARCH_INC
##=============================================================================
##
##	arch.inc
##
##	MN10300 assembler header file
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
## Date:	1997-10-16
## Purpose:	Architecture definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the MN10300 CPU family.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
	
#include <cyg/hal/variant.inc>

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		

#ifndef CYGPKG_HAL_MN10300_CPU_INIT_DEFINED
	# Initialize CPU
	.macro	hal_cpu_init
	# Set up the PSW
	mov	0x0700,d0
	mov	d0,psw
	.endm
#endif

	# Enable interrupts
	.macro hal_cpu_int_enable
	or	0x0800,psw
	.endm		

	# Disable interrupts
	.macro hal_cpu_int_disable
	and	0xF7ff,psw
	.endm	

	# Merge the interrupt enable state of the status register in
	# \sr with the current sr.
	.macro	hal_cpu_int_merge sr
	movm	[d3],(sp)
	and	0x00000F00,\sr		# isolate IE and IM bits
	hal_cpu_get_psw d3
	and	0xfffff0ff,d3
	or	\sr,d3
	hal_cpu_set_psw d3
	movm	(sp),[d3]
	.endm

	# Enable further exception processing, and disable
	# interrupt processing.
	.macro hal_cpu_except_enable
	.endm		
	
	# Return from exception.
	.macro	hal_cpu_eret pc,sr
	.endm
	
##-----------------------------------------------------------------------------
# Default interrupt decoding macros.

#ifndef CYGPKG_HAL_MN10300_INTC_DEFINED

#ifndef CYGPKG_HAL_MN10300_INTC_INIT_DEFINED
	# initialize all interrupts to disabled
	.macro	hal_intc_init
	.endm
#endif

	.macro	hal_intc_decode vnum
	.endm

#endif

#------------------------------------------------------------------------------
# MMU macros.
	
#ifndef CYGPKG_HAL_MN10300_MMU_DEFINED

	.macro	hal_mmu_init
	.endm

#endif	

#------------------------------------------------------------------------------
# MEMC macros.
	
#ifndef CYGPKG_HAL_MN10300_MEMC_DEFINED

	.macro	hal_memc_init
	.endm

#endif	
	
#------------------------------------------------------------------------------
# Cache macros.
	
#ifndef CYGPKG_HAL_MN10300_CACHE_DEFINED

	.macro	hal_cache_init
	.endm

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

	.macro	hal_diag_data
	.endm
#endif	

#------------------------------------------------------------------------------
# Timer initialization.
	
#ifndef CYGPKG_HAL_MN10300_TIMER_DEFINED

	.macro	hal_timer_init
	.endm

#endif	

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_MN10300_MON_DEFINED

	.macro	hal_mon_init
	.endm

#endif	

#------------------------------------------------------------------------------
# temporary trace macros

#if 1

#ifdef CYGPKG_HAL_MN10300_AM31_STDEVAL1
#define TRACE_BASE 0x48100000
#else
#error TRACE_BASE not defined for platform
#endif

	.macro traceinit
	mov	TRACE_BASE,a2
	mov	TRACE_BASE+0x10,d0
	mov	d0,(a2)
	.endm
	
	.macro trace,tag,arg1=0,arg2=0
	movm	[d2,a2,a3],(sp)
	mov	TRACE_BASE,a2
	mov	(a2),a3
	mov	\tag,d2
	mov	d2,(a3)
	mov	(12,a2),d2
	mov	d2,(4,a3)
	add	1,d2
	mov	d2,(12,a2)
	mov	\arg1,d2
	mov	d2,(8,a3)
	mov	\arg2,d2
	mov	d2,(12,a3)
	add	16,a3
	mov	a3,(a2)
	movm	(sp),[d2,a2,a3]
	.endm
#else
	.macro traceinit
	.endm

	.macro trace,tag,arg1=0,arg2=0
	.endm
#endif


#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_ARCH_INC
# end of arch.inc
