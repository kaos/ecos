##=============================================================================
##
##      linux.S
##
##      Linux setup
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
## Author(s):   jskov
## Contributors:jskov
## Date:        1999-03-11
## Purpose:     Linux setup
## Description: This file contains any code needed to initialize the
##              CPU on an i386/Linux synthetic target.
##
######DESCRIPTIONEND####
##
##=============================================================================

        .globl  cyg_hal_hardware_init
cyg_hal_hardware_init:
        push   %ebp
        mov    %esp,%ebp
        sub    $0x4,%esp

        ## FIXME: The below turns out to have no effect. At some point
        ## before the cause_fpe test of signal2.c gets executes, the
        ## flags have been reverted. Don't know what's causing it. Are
        ## magic Linux calls required perhaps?
        ## Maybe it is because a segv happens before the FPE..

        ## Enable floating point exceptions. Bit mask:
        ##  1 - invalid operation
        ##  2 - denormalized operand
        ##  4 - zero divide
        ##  8 - overflow
        ## 16 - underflow
        ## 32 - precision
        fstcw  -4(%ebp)
        movl   -4(%ebp),%eax
        andb   $(~0x04),%al
        movl   %eax,-4(%ebp)
        fldcw  -4(%ebp)

        mov    %ebp,%esp
        pop    %ebp
        ret

#------------------------------------------------------------------------------
# end of linux.S
