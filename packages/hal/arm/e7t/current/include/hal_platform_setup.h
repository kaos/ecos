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
// Author(s):   jskov
// Contributors:jskov
// Date:        2001-03-16
// Purpose:     E7T platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/plf_io.h>

#define CYGHWR_LED_MACRO                                                  \
        ldr     r0,=E7T_IOPDATA                                          ;\
        mov     r1, #((15 & (\x)) << 4)                                  ;\
        str     r1, [r0]                                                 ;

#if CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE==4096
// Override default to a more sensible value
#undef  CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
#define CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE 2048
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#define PLATFORM_SETUP1                                                 ;\
        ldr     r1,=E7T_IOPMOD                                          ;\
        ldr     r2,=0x0001fcf0 /* set led + seg to output */            ;\
        str     r2,[r1,#0x00]                                           ;\
        ldr     r1,=E7T_IOPDATA                                         ;\
        ldr     r2,=0x00000050 /* set leds */                           ;\
        str     r2,[r1,#0x00]                                           ;\
                                                                        ;\
20:     ldr     lr,=33f                                                 ;\
        ldr     r0,=12f                                                 ;\
        ldm     r0,{r1-r12}                                             ;\
        ldr     r0,=E7T_EXTDBWTH                                        ;\
        stm     r0,{r1-r12}                                             ;\
        mov     pc,lr                                                   ;\
                                                                        ;\
        /* The below are set with a store-multiple instruction */       ;\
        /* Flash is 16 bit, SRAM is 32 bit */                           ;\
        /* .long   E7T_EXTDBWTH */                                      ;\
12:     .long  ( (E7T_EXTDBWTH_16BIT<<E7T_EXTDBWTH_DSR0_shift)           \
                |(E7T_EXTDBWTH_32BIT<<E7T_EXTDBWTH_DSR1_shift)           \
                |(E7T_EXTDBWTH_32BIT<<E7T_EXTDBWTH_DSR2_shift) )        ;\
        /* Flash at 0x01800000-0x01880000, 5 cycles, 4 cycles */        ;\
        /* .long   E7T_ROMCON0 */                                       ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_4C)                                    \
                |((0x01800000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x01880000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\
        /* SRAM at 0x00000000-0x00400000, 5 cycles, 2 cycles */         ;\
        /* .long   E7T_ROMCON1 */                                       ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_2C)                                    \
                |((0x00000000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x00040000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\
        /* SRAM at 0x00400000-0x00800000, 5 cycles, 2 cycles */         ;\
        /* .long   E7T_ROMCON2 */                                       ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_2C)                                    \
                |((0x00040000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x00080000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\
        /* Below values are what Boot Monitor sets up */                ;\
        /* .long   E7T_ROMCON3 */                                       ;\
        .long   0x08018020                                              ;\
        /* .long   E7T_ROMCON4 */                                       ;\
        .long   0x0a020040                                              ;\
        /* .long   E7T_ROMCON5 */                                       ;\
        .long   0x0c028040                                              ;\
        /* .long   E7T_DRAMCON0 */                                      ;\
        .long   0x00000000                                              ;\
        /* .long   E7T_DRAMCON1 */                                      ;\
        .long   0x00000000                                              ;\
        /* .long   E7T_DRAMCON2 */                                      ;\
        .long   0x00000000                                              ;\
        /* .long   E7T_DRAMCON3 */                                      ;\
        .long   0x00000000                                              ;\
        /* .long   E7T_REFEXTCON */                                     ;\
        .long   0x9c218360                                              ;\
                                                                        ;\
33:
#else
#define PLATFORM_SETUP1
#endif

//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
