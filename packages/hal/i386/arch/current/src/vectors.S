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
## Date:        1999-01-07
## Purpose:     x86 exception vectors
## Description: This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##              that save and restore state for both exceptions and
##              interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#==============================================================================

    .file   "vectors.S"

# The Linux synthetic target defines its own startup.
#ifdef CYGPKG_HAL_I386_LINUX
    .extern _linux_entry
    .text
    .globl _start

_start:
    jmp   _linux_entry

#else  ## ifdef CYGPKG_HAL_I386_LINUX
#==============================================================================
# Real startup code. We jump here from the various reset vectors to set up the
# world.

    .text
    .globl _start

_start: 
#error "Not defined."

#endif ## ifndef CYGPKG_HAL_I386_LINUX

#------------------------------------------------------------------------------
# end of vectors.S
