##=============================================================================
##
##      vectors.S
##
##      x86 exception vectors
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
## Author(s):   bartv
## Contributors:bartv
## Date:        2001-03-16
## Purpose:     Synthetic target startup
## Description: When running on real hardware vectors.S contains
##	        initialization code and usually the low-level interrupt and
##              exception support. On the synthetic target the latter is
##              handled by C code. The only initialization that is needed is
##              to jump into the C startup.        
##
######DESCRIPTIONEND####
##
##=============================================================================


#==============================================================================

        .file   "vectors.S"

	.extern	_linux_entry
	.text
	.globl	_start
_start:
	jmp	_linux_entry
