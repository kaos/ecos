##=============================================================================
##
##	i386.inc
##
##	i386 assembler header file
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
## Author(s): 	jskov
## Contributors:jskov
## Date:	1999-01-20
## Purpose:	i386 definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the i386
## Usage:
##		#include <cyg/hal/i386.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#------------------------------------------------------------------------------
# Exception, interrupt and thread context save area layout
# The layout of this structure is also defined in "hal_arch.h", for C
# code. Do not change this without changing that (or vice versa).

	# See SYSV ABI4, i386 supplement (page 37-38)
        # http://www.sco.com/products/layered/develop/devspecs/abi386-4.pdf

	# Callee save registers (eax, ecx, and edx are scratch registers)

#ifdef CYGHWR_HAL_I386_FPU
#ifdef CYGHWR_HAL_I386_FPU_SWITCH_LAZY
	.equ	i386reg_fpucontext,	0
	.equ	i386reg_edi,		i386reg_fpucontext+4
#else
	.equ	i386reg_fpstate_valid,	0
	.equ	i386reg_fpstate,	i386reg_fpstate_valid+4
	.equ	i386reg_fpstate_size,	i386reg_fpstate+108
	.equ	i386reg_edi,		i386reg_fpstate_size
#endif
#else
	.equ	i386reg_edi,		0
#endif
	.equ	i386reg_esi,		i386reg_edi+4
	.equ	i386reg_ebp,		i386reg_esi+4
	.equ	i386reg_esp,		i386reg_ebp+4
	.equ	i386reg_ebx,		i386reg_esp+4
	.equ	i386reg_edx,		i386reg_ebx+4
	.equ	i386reg_ecx,		i386reg_edx+4
	.equ	i386reg_eax,		i386reg_ecx+4
	.equ	i386reg_vector,		i386reg_eax+4
	.equ	i386reg_eip,		i386reg_vector+4
	.equ	i386reg_cs,		i386reg_eip+4
	.equ	i386reg_eflags,		i386reg_cs+4
	.equ	i386reg_context_size, 	i386reg_eflags+4


#if defined(CYGHWR_HAL_I386_FPU) && defined(CYGHWR_HAL_I386_FPU_SWITCH_LAZY)
	.equ	i386reg_fpucontext_valid,	0
	.equ	i386reg_fpucontext_state,	i386reg_fpucontext_valid+4
	.equ	i386reg_fpucontext_size,	i386reg_fpucontext_state+108
#endif

#------------------------------------------------------------------------------
# end of i386.inc
