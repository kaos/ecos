##=============================================================================
##
##	variant.S
##
##	MIPS 64 variant code
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
## Author(s): 	dmoseley
## Contributors:	dmoseley
## Date:	2000-06-07
## Purpose:	MIPS 64 variant code
## Description:	Variant specific code for MIPS64 architecture.
##              
##		
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/system.h>	
#include <pkgconf/hal.h>

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/mips-regs.h>

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>	
#endif
			
#include <cyg/hal/arch.inc>	

#include <cyg/hal/var_arch.h>
#include <cyg/hal/hal_arch.h>

##-----------------------------------------------------------------------------
# Variant Initialization.
# This code performs variant specific initialization.

##-----------------------------------------------------------------------------
## end of variant.S
	
