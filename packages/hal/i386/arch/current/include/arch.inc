#ifndef CYGONCE_HAL_ARCH_INC
#define CYGONCE_HAL_ARCH_INC
##=============================================================================
##
##	arch.inc
##
##	i386 assembler header file
##
##=============================================================================
######COPYRIGHTBEGIN####
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
######COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	 nickg
## Contributors: nickg, pjo
## Date:	1999-10-15
## Purpose:	Architecture definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the i386 CPU family.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/i386.inc>

#include <cyg/hal/variant.inc>

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		
	
#ifndef CYGPKG_HAL_I386_CPU_INIT_DEFINED
	# Initialize CPU
	.macro	hal_cpu_init
	.endm
#endif /* !CYGPKG_HAL_I386_CPU_INIT_DEFINED */

##-----------------------------------------------------------------------------

#ifndef CYGPKG_HAL_I386_INTC_DEFINED

#ifndef CYGPKG_HAL_I386_INTC_INIT_DEFINED
	# initialize all interrupts to disabled
	.macro	hal_intc_init
	.endm
#endif

	.macro	hal_intc_decode vnum
	.endm

#endif


#------------------------------------------------------------------------------
# FPU macros.
	
#ifndef CYGPKG_HAL_I386_FPU_DEFINED

#ifdef CYGHWR_HAL_I386_FPU

	.macro	hal_fpu_init
	.endm
		
	.macro	hal_fpu_save regs
	.endm

	.macro	hal_fpu_save_caller regs
	.endm

	.macro	hal_fpu_save_callee regs
	.endm

	.macro	hal_fpu_load_caller regs
	.endm
	
	.macro	hal_fpu_load_callee regs
	.endm
	
	.macro	hal_fpu_load regs
	.endm
	
#else /* !CYGHWR_HAL_I386_FPU */

	.macro	hal_fpu_init
	.endm
		
	.macro	hal_fpu_save regs
	.endm

	.macro	hal_fpu_save_caller regs
	.endm

	.macro	hal_fpu_save_callee regs
	.endm

	.macro	hal_fpu_load_caller regs
	.endm
	
	.macro	hal_fpu_load_callee regs
	.endm
	
	.macro	hal_fpu_load regs
	.endm
	
#endif
			
#endif	

#------------------------------------------------------------------------------
# MMU macros.
	
#ifndef CYGPKG_HAL_I386_MMU_DEFINED

	.macro	hal_mmu_init
	.endm

#endif	

#------------------------------------------------------------------------------
# MEMC macros.
	
#ifndef CYGPKG_HAL_I386_MEMC_DEFINED

	.macro	hal_memc_init
	.endm

#endif	
	
#------------------------------------------------------------------------------
# Cache macros.
	
#ifndef CYGPKG_HAL_I386_CACHE_DEFINED

	.macro	hal_cache_init
	.endm

#endif	

#------------------------------------------------------------------------------
# Diagnostics macros.
	
#ifndef CYGPKG_HAL_I386_DIAG_DEFINED

	.macro	hal_diag_init
	.endm

	.macro	hal_diag_excpt_start
	.endm

	.macro	hal_diag_intr_start
	.endm

	.macro	hal_diag_restore
	.endm
#endif	

#------------------------------------------------------------------------------
# Timer initialization.
	
#ifndef CYGPKG_HAL_I386_TIMER_DEFINED

	.macro	hal_timer_init
	.endm

#endif	

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_I386_MON_DEFINED

	.macro	hal_mon_init
	.endm

#endif	

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_ARCH_INC
# end of arch.inc
