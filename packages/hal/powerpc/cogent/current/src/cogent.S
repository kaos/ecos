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
        
#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/ppc.inc>

#------------------------------------------------------------------------------
                
        .globl  hal_hardware_init
hal_hardware_init:

#if defined(CYG_HAL_POWERPC_COGENT) && defined(CYG_HAL_POWERPC_MPC8xx)

        # Set up MPC8xx mapping registers for cogent

        lwi     r3,CYGARC_REG_IMM_BASE  # base address of control registers
        
        # Set timebase divisor to busclock/16, pitrtc divisor to 512.
        lwz     r4,(CYGARC_REG_IMM_SCCR - CYGARC_REG_IMM_BASE)(r3)
        lwi     r5,(CYGARC_REG_IMM_SCCR_RTDIV | CYGARC_REG_IMM_SCCR_RTSEL | CYGARC_REG_IMM_SCCR_TBS)
        or      r4,r4,r5
        stw     r4,(CYGARC_REG_IMM_SCCR - CYGARC_REG_IMM_BASE)(r3)

        # Enable TimeBase
        li      r4,CYGARC_REG_IMM_TBSCR_TBE
        sth     r4,(CYGARC_REG_IMM_TBSCR - CYGARC_REG_IMM_BASE)(r3)

        # Note:  I do not know if the timeout is correct/optimal. 
        #        Works with the above memory mappings/wait states and 
        #        the PromICE.
        # Disable watchdog, enable bus monitor, timeout after 255*8 cycles
        lwi     r4,(CYGARC_REG_IMM_SYPCR_BMT_MASK | CYGARC_REG_IMM_SYPCR_BME)
        stw     r4,(CYGARC_REG_IMM_SYPCR - CYGARC_REG_IMM_BASE)(r3)

        # Set CS0 to
        # base addr 0xfff00000
        # wait states 4
        # size 1M
        # Covers ROM
        # Note: After a reset code is currently executing via the default
	# mapping defined in bank 0. This bank must remain valid while
	# it is being configured.
        lwi     r4,(0xfff00000 | CYGARC_REG_IMM_BR_PS_16 | CYGARC_REG_IMM_BR_V)
        stw     r4,(CYGARC_REG_IMM_BR0 - CYGARC_REG_IMM_BASE)(r3)
        lwi     r4,(0xfff00000 | CYGARC_REG_IMM_OR_BI | 4 << CYGARC_REG_IMM_OR_SCY_SHIFT)
        stw     r4,(CYGARC_REG_IMM_OR0 - CYGARC_REG_IMM_BASE)(r3)

        # Set CS1 to
        # base addr 0x00000000
        # external ACK
        # size 64M
        # Covers DRAM and slot0
        lwi     r4,(0xfc000000 | CYGARC_REG_IMM_OR_SETA)
        stw     r4,(CYGARC_REG_IMM_OR1 - CYGARC_REG_IMM_BASE)(r3)
        lwi     r4,(0x00000000 | CYGARC_REG_IMM_BR_PS_32 | CYGARC_REG_IMM_BR_V)
        stw     r4,(CYGARC_REG_IMM_BR1 - CYGARC_REG_IMM_BASE)(r3)

        # Set CS2 to
        # base addr 0x04000000
        # external ACK
        # size 64M
        # slot1 and slot2
        lwi     r4,(0xfc000000 | CYGARC_REG_IMM_OR_SETA)
        stw     r4,(CYGARC_REG_IMM_OR2 - CYGARC_REG_IMM_BASE)(r3)
        lwi     r4,(0x04000000 | CYGARC_REG_IMM_BR_PS_32 | CYGARC_REG_IMM_BR_V)
        stw     r4,(CYGARC_REG_IMM_BR2 - CYGARC_REG_IMM_BASE)(r3)

        # Set CS3 to
        # base addr 0x0e000000
        # exernal ack
        # size 32M
        # covers IO registers
        lwi     r4,(0xfe000000 | CYGARC_REG_IMM_OR_BI | CYGARC_REG_IMM_OR_SETA)
        stw     r4,(CYGARC_REG_IMM_OR3 - CYGARC_REG_IMM_BASE)(r3)
        lwi     r4,(0x0e000000 | CYGARC_REG_IMM_BR_PS_32 | CYGARC_REG_IMM_BR_V)
        stw     r4,(CYGARC_REG_IMM_BR3 - CYGARC_REG_IMM_BASE)(r3)

#endif  
                
        sync
        blr

#------------------------------------------------------------------------------
# end of cogent.S
