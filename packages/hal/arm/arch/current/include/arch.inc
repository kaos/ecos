##=============================================================================
##
##	arch.inc
##
##	ARM architecture assembler header file
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
## Date:	2000-11-15
## Purpose:	ARM definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the ARM
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

##-----------------------------------------------------------------------------
## ARM entry definitions. This allows _ prefixing to change by modifying
## the CYG_LABEL_DEFN macro.

#define FUNC_START(name)        \
        .type CYG_LABEL_DEFN(name),@function;   \
        .globl CYG_LABEL_DEFN(name);            \
CYG_LABEL_DEFN(name):

#define FUNC_END(name)  \
        .globl CYG_LABEL_DEFN(name);            \
CYG_LABEL_DEFN(name):
        
#define SYM_DEF(name)   \
        .globl CYG_LABEL_DEFN(name);            \
CYG_LABEL_DEFN(name):
	
#------------------------------------------------------------------------------
# end of arch.inc
