#ifndef CYGONCE_DEVS_PLF_FLASH_INC
#define CYGONCE_DEVS_PLF_FLASH_INC

##=============================================================================
##
##	plf_flash.inc
##
##	MIPS assembler header file
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
## Date:	2000-12-05
## Purpose:	MIPS definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the MIPS CPU family.
## Usage:
##		#include <cyg/hal/plf_flash.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#define FUNC_START(name)	\
        .type name,@function;	\
        .globl name;		\
        .ent   name;		\
name:

#define FUNC_END(name)		\
        .globl name;		\
name:


#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_DEVS_PLF_FLASH_INC
# end of plf_flash.inc
