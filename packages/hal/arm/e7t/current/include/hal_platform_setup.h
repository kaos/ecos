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
4:	ldr	r1,=register_setups                                     ;\
10:	ldr	r2,[r1,#0]	/* register address */                  ;\
	cmp	r2,#0                                                   ;\
	beq	20f                                                     ;\
	ldr	r3,[r1,#4]	/* register value */                    ;\
	str	r3,[r2,#0x00]                                           ;\
	add	r1,r1,#8	/* next register  */                    ;\
	b	10b                                                     ;\
register_setups:                                                        ;\
        /* Clear LEDs */                                                ;\
        .long   E7T_IOPMOD                                              ;\
        .long   0x0001fcf0 /* set led + seg to output */                ;\
        .long   E7T_IOPDATA                                             ;\
        .long   0x00000000                                              ;\
        /* Flash is 16 bit, SRAM is 32 bit */                           ;\
        .long   E7T_EXTDBWTH                                            ;\
        .long  ( (E7T_EXTDBWTH_16BIT<<E7T_EXTDBWTH_DSR0_shift)           \
                |(E7T_EXTDBWTH_32BIT<<E7T_EXTDBWTH_DSR1_shift)           \
                |(E7T_EXTDBWTH_32BIT<<E7T_EXTDBWTH_DSR2_shift) )        ;\
        .long   E7T_IOPDATA                                             ;\
        .long   0x00000010                                              ;\
        /* Set LEDs */                                                  ;\
        .long   E7T_IOPDATA                                             ;\
        .long   0x00000050                                              ;\
	.long 0                                                         ;\
20:
#else
#define PLATFORM_SETUP1
#endif

#if 0
/*
 These are the values left by the boot monitor. That's what we want to
 program as well, but it seems changing the ROMCONs at this time is
 too late. So just leave it as is.

0x03FF0000 0x07FFFFA0
0x03FF3000 0x00000000
0x03FF3008 0x00000000
0x03FF300C 0x00000000
0x03FF3010 0x0000003E
0x03FF3014 0x18860030
0x03FF3018 0x00400010
0x03FF301C 0x00801010 */

        /* Flash at 0x01800000-0x01880000, 5 cycles, 4 cycles */        ;\
	.long   E7T_ROMCON0                                             ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_4C)                                    \
                |((0x01800000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x01880000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\
        /* SRAM at 0x00000000-0x00400000, 5 cycles, 2 cycles */         ;\
        .long   E7T_ROMCON1                                             ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_2C)                                    \
                |((0x00000000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x00040000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\
        /* SRAM at 0x00400000-0x00800000, 5 cycles, 2 cycles */         ;\
        .long   E7T_ROMCON2                                             ;\
        .long  ( (E7T_ROMCON_PMC_ROM)                                    \
                |(E7T_ROMCON_TPA_5C)                                     \
                |(E7T_ROMCON_TACC_2C)                                    \
                |((0x00040000 >> 16) << E7T_ROMCON_BASE_shift)           \
                |((0x00080000 >> 16) << E7T_ROMCON_NEXT_shift))         ;\

#endif
//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
