#==============================================================================
##
##      sim.S
##
##      Simulator hardware setup
##
##=============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   nickg
## Contributors:        nickg
## Date:        1997-11-11
## Purpose:     Simulator hardware setup
## Description: This file contains any code needed to initialize the
##              hardware on the PowerPC simulator.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
        
#include "cyg/hal/ppc.inc"

#------------------------------------------------------------------------------
                
        .globl  hal_hardware_init
hal_hardware_init:

#if 0   
        # Set up the BAT registers to cover same address spaces

        # ibat0
        # base addr 0xfff00000
        # length 1M
        # WIMG = 0000
        # covers ROM
        lwi     r4,0xfff0001f
        mtibatu 0,r4
        lwi     r4,0xfff00002
        mtibatl 0,r4
        
        # ibat1
        # base addr 0x00000000
        # length 256M
        # WIMG = 0000
        # covers DRAM and slots
        lwi     r4,0x00001fff
        mtibatu 1,r4
        lwi     r4,0x00000002
        mtibatl 1,r4
                
        # dbat0
        # base addr 0xfff00000
        # length 256M
        # WIMG = 0101
        # covers ROM for data access
        lwi     r4,0xfff01fff
        mtdbatu 0,r4
        lwi     r4,0xfff0002a
        mtdbatl 0,r4

        # dbat1
        # base addr 0x00000000
        # length 256M
        # WIMG = 0000
        # covers DRAM and slots
        lwi     r4,0x00001fff
        mtdbatu 1,r4
        lwi     r4,0x00000002
        mtdbatl 1,r4
                
        # Enable translation in the MSR
        mfmsr   r3
        lwi     r4,0x00000030
        or      r3,r3,r4
        mtmsr   r3
#endif          
        blr

#------------------------------------------------------------------------------
# end of sim.S          