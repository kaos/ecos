#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-03-10
// Purpose:      NEC CEB/V850 platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>           // System-wide configuration info
#include <pkgconf/hal.h>              // Architecture independent configuration
#include <cyg/hal/v850_common.h>        
#include CYGBLD_HAL_PLATFORM_H        // Platform specific configuration

        .macro  lea     addr,reg
        movhi   hi(\addr),r0,\reg
        movea   lo(\addr),\reg,\reg
        .endm
        
        .macro  PLATFORM_SETUP1
#ifdef CYG_HAL_STARTUP_ROM         
        movhi   hi(V850_REGS),r0,r6       

        mov     0x01,r1                        
        st.b    r1,lo(V850_REG_SYC)[r6]

        // Internal RAM, I/O - 1 wait state
        // Externam ROM      - 3 wait states
        movea   0x7FFE,r0,r1
        st.h    r1,lo(V850_REG_DWC)[r6]

        // Internal RAM, I/O - 0 idle
        // Externam ROM      - 0 idle
        movea   0x2AA8,r0,r1
        st.h    r1,lo(V850_REG_BCC)[r6]

        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_EGP0)[r6]

        mov     0x01,r1                        
        st.b    r1,lo(V850_REG_EGN0)[r6]

        movea   0x2C,r0,r1                        
        st.b    r1,lo(V850_REG_PM1)[r6]

        movea   0xFF,r0,r1                        
        st.b    r1,lo(V850_REG_PM3)[r6]

        movea   0xEC,r0,r1                        
        st.b    r1,lo(V850_REG_P1)[r6]

        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_PM10)[r6]

        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_P10)[r6]

        movea   0xDD,r0,r1                        
        st.b    r1,lo(V850_REG_BRGC0)[r6]

        mov     0x03,r1                        
        st.b    r1,lo(V850_REG_BRGM0)[r6]

        movea   0xC8,r0,r1                        
        st.b    r1,lo(V850_REG_ASIM0)[r6]

        movea   0x47,r0,r1                        
        st.b    r1,lo(V850_REG_STIC0)[r6]

        mov     0x07,r1                        
        st.b    r1,lo(V850_REG_MM)[r6]

        // Setting the PCC register is tricky
        stsr    PSW,r7
        ori     CYGARC_PSW_NP,r7,r8
        ldsr    r8,PSW
        mov     0x00,r1
        st.b    r1,lo(V850_REG_PRCMD)[r6]
        st.b    r1,lo(V850_REG_PCC)[r6]
        ldsr    r7,PSW
        nop
        nop
        nop
        nop
        nop
#endif
        .endm

/*---------------------------------------------------------------------------*/
/* end of hal_platform_setup.h                                               */
#endif /* CYGONCE_HAL_PLATFORM_SETUP_H */
