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
# Version 1.0 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://sourceware.cygnus.com/ecos                                        
#                                                                          
# Software distributed under the License is distributed on an       
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
	.equ	i386reg_esp,		0
	.equ	i386reg_next_context,	4 # only used when dropping through...
	.equ	i386reg_ebp,		8 # ...from switch_ to load_context.
	.equ	i386reg_ebx,		12
	.equ	i386reg_esi,		16
	.equ	i386reg_edi,		20
	.equ	i386reg_context_size, 	24

#------------------------------------------------------------------------------
# end of i386.inc
