##=============================================================================
##
##	variant.S
##
##	MIPS JMR-TX3904 variant code
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
## Date:	1999-04-20
## Purpose:	MIPS JMR-TX3904 variant code
## Description:	Variant specific code for JMR-TX3904 board.
##              
##		
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/system.h>	
#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>	
#endif
			
#include <cyg/hal/arch.inc>	
	
##-----------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.
		
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

##-----------------------------------------------------------------------------
## end of variant.S
	