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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jlarmour
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
#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
        movhi   hi(V850_REGS),r0,r6       

        // set bus control signals
        mov     0x01,r1                        
        st.b    r1,lo(V850_REG_SYC)[r6]

        // Internal RAM, internal ROM and I/O - no wait states, regardless
        // of the setting of DWC
#if CYGHWR_HAL_V85X_CPU_FREQ > 17000000
        // External ROM      - 3 wait states
        // External RAM      - 3 wait states
        movea   0xFFFF,r0,r1
#elif CYGHWR_HAL_V85X_CPU_FREQ > 8000000
        // External ROM      - 2 wait states
        // External RAM      - 1 wait states
        movea   0x7FFE,r0,r1
#elif CYGHWR_HAL_V85X_CPU_FREQ > 4194304
        // External ROM      - 1 wait states
        // External RAM      - 0 wait states
        movea   0x3FFD,r0,r1
#else

#endif
        st.h    r1,lo(V850_REG_DWC)[r6]

        // Internal RAM, ROM, I/O - always 0 idle regardless of the setting
        // of BCC
        // External RAM      - 0 idle
        // External ROM      - 0 idle
        movea   0x2AA8,r0,r1
        st.h    r1,lo(V850_REG_BCC)[r6]

        // No INTs on rising edge
        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_EGP0)[r6]

        // enable INTP0 (serial) IRQ only, set for falling edge
        mov     0x01,r1                        
        st.b    r1,lo(V850_REG_EGN0)[r6]

        // Port 1 mode: set serial DSR, RXD and CTS as inputs, and others
        // as outputs
        movea   0x2C,r0,r1                        
        st.b    r1,lo(V850_REG_PM1)[r6]

        // Port 3 mode: SW2 read port: set to all input
        movea   0xFF,r0,r1                        
        st.b    r1,lo(V850_REG_PM3)[r6]

        // Set serial port control inputs (DSR, RXD, CTS) to 1
        // FIXME Why? Also I don't get why the top two bits are set.
        movea   0xEC,r0,r1                        
        st.b    r1,lo(V850_REG_P1)[r6]

        // Enable all outputs for 7-segment LED
        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_PM10)[r6]

        // Set LED to 0
        mov     0x00,r1                        
        st.b    r1,lo(V850_REG_P10)[r6]

        // Init serial port 0 baud rate to divide clock down to 9600 baud
        // by setting baud count here
        // This may seem unnecessary, but setting up the serial allows
        // us to do diag output before HAL diag is initialized. The values
        // are clock dependent however, but this is only for debug so we
        // don't care.
        movea   0xDD,r0,r1                        
        st.b    r1,lo(V850_REG_BRGC0)[r6]

        // and divisor here
        mov     0x03,r1                        
        st.b    r1,lo(V850_REG_BRGMC00)[r6]

        // set serial 0 to enable tx/rx and 8-N-1
        movea   0xC8,r0,r1                        
        st.b    r1,lo(V850_REG_ASIM0)[r6]

        // disable reception of serial interrupts, and set serial interrupt
        // priority to level 7 (lowest)
        movea   0x47,r0,r1                        
        st.b    r1,lo(V850_REG_STIC0)[r6]

        // Memory expansion mode - set to 4MB
        // We could probably set this to 256K (MM==0x4), but there seems
        // no advantage
        mov     0x07,r1                        
        st.b    r1,lo(V850_REG_MM)[r6]

        // Setting the PCC register is tricky - it is a "specific register"
        // We set the CPU clock to full speed
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
