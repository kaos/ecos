##=============================================================================
##
##	arch.inc
##
##	SH architecture assembler header file
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
## Date:	2000-02-28
## Purpose:	SH definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the SH
##              It also includes the variant/platform assembly header file.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/basetype.h>
#include <cyg/hal/sh_regs.h>

##-----------------------------------------------------------------------------
## SH entry definitions. This allows _ prefixing to change by modifying
## the CYG_LABEL_DEFN macro.

#define FUNC_START(name)	\
        .type CYG_LABEL_DEFN(name),@function;	\
        .globl CYG_LABEL_DEFN(name);		\
CYG_LABEL_DEFN(name):

#define FUNC_END(name)	\
        .globl CYG_LABEL_DEFN(name);		\
CYG_LABEL_DEFN(name):
	
#define SYM_DEF(name)	\
        .globl CYG_LABEL_DEFN(name);		\
CYG_LABEL_DEFN(name):

#define SYM_PTR_REF(name)	\
        .globl CYG_LABEL_NAME(name);		\
$##name:	.long	CYG_LABEL_NAME(name)

# We need this macro because the SH assembler is lacking the nice
# inline immediate constant feature of the ARM assembler. So we have
# to define constants ourselves, and sometimes we need more than one of
# the same type. This allows the symbols used to change.
#define SYM_PTR_REFn(name,n)	\
        .globl CYG_LABEL_NAME(name);		\
$##name##n:	.long	CYG_LABEL_NAME(name)

#include <cyg/hal/platform.inc>
	        
##-----------------------------------------------------------------------------
## SH FPU state handling
## Empty for now.

        .macro	hal_fpu_save regs
        .endm

        .macro	hal_fpu_load regs
        .endm

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		

	# Enable interrupts
	.macro hal_cpu_int_enable t1,t2
        stc     sr,\t1
        mov     #CYGARC_REG_SR_IMASK>>1,\t2
        shll    \t2
        not     \t2,\t2
        and     \t2,\t1
        ldc     \t1,sr
	.endm		

	# Disable interrupts
	.macro hal_cpu_int_disable t1,t2
        stc     sr,\t1
        mov     #CYGARC_REG_SR_IMASK>>1,\t2
        shll    \t2
        or      \t2,\t1
        ldc     \t1,sr
	.endm		

	# Merge the interrupt enable state of the status register in
	# \sr with the current sr.
	.macro	hal_cpu_int_merge sr,t1,t2
        stc     sr,\t1
        mov     #CYGARC_REG_SR_IMASK>>1,\t2
        shll    \t2
        and     \t2,\sr
        not     \t2,\t2
        and     \t2,\t1
        or      \sr,\t1
        ldc     \t1,sr
	.endm

#------------------------------------------------------------------------------
# end of arch.inc
