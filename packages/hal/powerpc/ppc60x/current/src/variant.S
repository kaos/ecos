##=============================================================================
##
##	variant.S
##
##	PowerPC PPC60x variant code
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
## Date:	2000-02-04
## Purpose:	PowerPC PPC60x variant code
## Description:	Variant specific code for PowerPC 60x CPUs.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/system.h>	
#include <pkgconf/hal.h>

#include <cyg/hal/variant.inc>

#---------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.

#if (CYG_ISR_TABLE_SIZE > 2)
        .data
#else
        .section ".sdata","aw"
#endif

        .extern hal_default_decrementer_isr
        .extern hal_default_isr

        .globl  hal_interrupt_handlers
hal_interrupt_handlers:
        .long   hal_default_decrementer_isr
        .rept   CYG_ISR_TABLE_SIZE-1
        .long   hal_default_isr
        .endr

        .globl  hal_interrupt_data
hal_interrupt_data:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

        .globl  hal_interrupt_objects
hal_interrupt_objects:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

##-----------------------------------------------------------------------------
## end of variant.S
	
