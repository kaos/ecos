##=============================================================================
##
##      cogent.S
##
##      Cogent board hardware setup
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
## Purpose:     Cogent board hardware setup
## Description: This file contains any code needed to initialize the
##              hardware on a Cogent PowerPC board.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
        
#include <cyg/hal/ppc.inc>

#------------------------------------------------------------------------------
                
        .globl  hal_hardware_init
hal_hardware_init:

#if defined(CYG_HAL_POWERPC_COGENT) && defined(CYG_HAL_POWERPC_MP860)

        # Set up MP860 mapping registers for cogent
        
        # MP860 time base control register
        lwi     r3,0xff000200
        li      r4,0x0001
        sth     r4,0(r3)


        lwi     r3,0xff000000   # base address of control registers

        # FIXME: I do not know if the timeout is correct. works with the
        #        below memory mappings/wait states and the PromICE.
        lwi     r4,0x0000ff80   # Disable watchdog, enable ...
        stw     r4,0x0004(r3)   # ...Bus Monitor, timeout after 255*8 cycles

        # Set CS0 to
        # base addr 0xfff00000
        # wait states 4
        # size 1M
        # Covers ROM
        lwi     r4,0xfff00801
        stw     r4,0x0100(r3)
        lwi     r4,0xfff00140
        stw     r4,0x0104(r3)

        # Set CS1 to
        # base addr 0x00000000
        # external ACK
        # size 64M
        # Covers DRAM and slot0
        lwi     r4,0x00000001
        stw     r4,0x0108(r3)
        lwi     r4,0xfc000008
        stw     r4,0x010c(r3)

        # Set CS2 to
        # base addr 0x04000000
        # external ACK
        # size 64M
        # slot1 and slot2
        lwi     r4,0x04000001
        stw     r4,0x0110(r3)
        lwi     r4,0xfc000008
        stw     r4,0x0114(r3)

        # Set CS3 to
        # base addr 0x0e000000
        # exernal ack
        # size 32M
        # covers IO registers
        lwi     r4,0x0e000001
        stw     r4,0x0118(r3)
        lwi     r4,0xfe000108
        stw     r4,0x011c(r3)

#if 0   
        # Set up the BAT registers to cover same address spaces

        # The 860 does not appear to have BAT registers.
        
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
                
#endif  
                
        blr

#------------------------------------------------------------------------------
# end of cogent.S
