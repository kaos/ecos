##=============================================================================
##
##      fads.S
##
##      Fads board hardware setup
##
##=============================================================================
#==============================================================================
#####UNSUPPORTEDBEGIN####
#
# -------------------------------------------
# This source file has been contributed to eCos/Red Hat. It may have been
# changed slightly to provide an interface consistent with those of other 
# files.
#
# The functionality and contents of this file is supplied "AS IS"
# without any form of support and will not necessarily be kept up
# to date by Red Hat.
#
# All inquiries about this file, or the functionality provided by it,
# should be directed to the 'ecos-discuss' mailing list (see
# http://sourceware.cygnus.com/ecos/intouch.html for details).
#
# Contributed by: Kevin Hester <khester@opticworks.com>
# Maintained by:  <Unmaintained>
# -------------------------------------------
#
#####UNSUPPORTEDEND####
#==============================================================================
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
## Author(s):   nickg
## Contributors:nickg
## Date:        1997-11-11
## Purpose:     Fads board hardware setup
## Description: This file contains any code needed to initialize the
##              hardware on a Fads PowerPC board.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
        
#include <cyg/hal/ppc.inc>

#------------------------------------------------------------------------------
                
        .globl  hal_hardware_init
hal_hardware_init:

#if defined(CYGPKG_HAL_POWERPC_FADS) && defined(CYG_HAL_POWERPC_MP860)

        # kevinh disabled - we use the values which have already been setup
        # by the mpc8bug

#endif                
        blr

#------------------------------------------------------------------------------
# end of fads.S
