#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2005 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//===========================================================================*/
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Kurt Stremerch
// Contributors: Gary Thomas
// Date:         2003-02-23
//
//####DESCRIPTIONEND####

#include <pkgconf/system.h>          // System-wide configuration info
#include CYGBLD_HAL_VARIANT_H        // Variant specific configuration
#include CYGBLD_HAL_PLATFORM_H       // Platform specific configuration
#include <cyg/hal/hal_pxa2x0.h>      // Variant specific hardware definitions
#include <cyg/hal/hal_mmu.h>         // MMU definitions
#include <cyg/hal/hal_mm.h>          // more MMU definitions
#include <cyg/hal/xsengine.h>        // Platform specific hardware definitions
#include <cyg/hal/hal_spd.h>

#define MDCNFG_VALUE   0x000009C9
#define MDMRS_VALUE    0x00220022
#define MDREFR_VALUE_1 0x00018018
#define MDREFR_VALUE_2 0x00000000
#define MDREFR_VALUE_3 0x00000000

#define GPCR0_VALUE 0xFFFFFFFF
#define GPCR1_VALUE 0xFFFFFFFF
#define GPCR2_VALUE 0xFFFFFFFF

#define GPSR0_VALUE 0x0000A000
#define GPSR1_VALUE 0x00020000
#define GPSR2_VALUE 0x0000C000

#define GPDR0_VALUE 0x0000A000
#define GPDR1_VALUE 0x00022A80
#define GPDR2_VALUE 0x0000C000

#define GAFR0_L_VALUE 0x80000000
#define GAFR0_U_VALUE 0x00000010
#define GAFR1_L_VALUE 0x09988050
#define GAFR1_U_VALUE 0x00000008
#define GAFR2_L_VALUE 0xA0000000
#define GAFR2_U_VALUE 0x00000000

#define PSSR_VALUE          0x20
#define MSC0_VALUE_NONBURST 0x25F425F0
#define MSC0_VALUE_BURST    0x25F425F0

#define MDCNFG_DE0 0x00000001
#define MDCNFG_DE1 0x00000002
#define MDCNFG_DE2 0x00010000
#define MDCNFG_DE3 0x00020000

#undef XSDEBUG

#if defined(CYG_HAL_STARTUP_ROM)
#define PLATFORM_SETUP1  _platform_setup1
#define CYGHWR_HAL_ARM_HAS_MMU
#define CYGSEM_HAL_ROM_RESET_USES_JUMP

// This macro represents the initial startup code for the platform        
  .macro _platform_setup1
  // This is where we wind up immediately after reset. At this point, we
  // are executing from the boot address (0x00000000), not the eventual
  // flash address. Do some basic setup using position independent code
  // then switch to real flash address

// FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 
// This is a quick and dirty workaround to an apparent gas/ld
// bug. The computed UNMAPPED_PTR(reset_vector) is off by 0x20.
  .rept 0x20/4
  nop
  .endr
// FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME 

#if defined(XSDEBUG)
  ldr     r0,=PXA2X0_GPDR1    /* GPIO as output */
  ldr     r1,=0x80
  str     r1,[r0]
  ldr     r0,=PXA2X0_GAFR1_L  /* GPIO as decated BTUART */
  ldr     r1,=0x0088000
  str     r1,[r0]
  ldr     r0,=PXA2X0_FFLCR    /* 8bit char + DLAB(Divisor access) */
  ldr     r1,=0x83
  str     r1,[r0]
  ldr     r0,=PXA2X0_FFDLL    /* Divisor to 115200 */
  ldr     r1,=8
  str     r1,[r0]
  ldr     r0,=PXA2X0_FFLCR    /* DLAB off */
  ldr     r1,=0x3
  str     r1,[r0]
  ldr     r0,=PXA2X0_FFIER    /* enable UART */
  ldr     r1,=0x40
  str     r1,[r0]
  ldr     r0,=PXA2X0_FFTHR    /* send char A */
  ldr     r1,=0x41
  str     r1,[r0]
#endif

  // Disable interrupts, by setting the Interrupt Mask Registers to all 0's
  ldr     r1,=PXA2X0_ICMR
  mov     r0,#0
  str     r0,[r1]
   
  // disable MMU
  mov     r0, #0x0
  mcr     p15, 0, r0, c1, c0, 0

  // flush TLB
  mov     r0, #0x0
  mcr     p15, 0, r0, c8, c7, 0   //  Flush TLB

  // flush I&D caches and BTB
  mov     r0, #0x0
  mcr     p15, 0, r0, c7, c7, 0   //  Flush caches

  CPWAIT r0 

  // Enables access to coprocessor 0 (The only extra coprocessor on the PXA250)
  ldr     r0, =0x00000001
  mcr     p15, 0, r0, c15, c1, 0

  // Disable the IRQ's and FIQ's in the program status register and 
  // enable supervisor mode 
  ldr     r0,=(CPSR_IRQ_DISABLE|CPSR_FIQ_DISABLE|CPSR_SUPERVISOR_MODE)
  msr     cpsr, r0

  // Set TURBO mode

  ldr     r2, =0x00000321
  ldr     r1, =PXA2X0_CCCR
  str     r2,[r1]
  
  ldr     r1, =0x00000003
  mcr     p14, 0, r1, c6, c0, 0

  // Set-up memory according to Eyxs specs
  // Set Refresh Values
  ldr     r1,=PXA2X0_MDREFR
  ldr     r2,=MDREFR_VALUE_1   
  str     r2,[r1]

  //fetch platform value SDRAM
  ldr     r1,=PXA2X0_MDCNFG
  ldr     r2,=MDCNFG_VALUE
  
  bic     r2,r2,#(MDCNFG_DE0|MDCNFG_DE1)
  bic     r2,r2,#(MDCNFG_DE2|MDCNFG_DE3)
  
  str     r2,[r1]

  // pause for 200 uSecs
  ldr r3, =PXA2X0_OSCR
  mov r2, #0
  str r2, [r3]
  ldr r4, =0x300
1:
  ldr r2, [r3]
  cmp r4, r2
  bgt 1b

  ldr     r2,=PXA2X0_RAM_BANK0_BASE
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]
  str     r2,[r2]

  ldr     r1,=PXA2X0_MDCNFG
  ldr     r2,=MDCNFG_VALUE
  str     r2,[r1]

  ldr     r1,=PXA2X0_MDMRS
  ldr     r2,=MDMRS_VALUE
  str     r2,[r1]

#if defined(XSDEBUG)
  ldr     r0,=PXA2X0_FFTHR    /* send char B */
  ldr     r1,=0x42
  str     r1,[r0]
#endif

  // Set Static memory registers
  ldr     r1,=PXA2X0_MSC0
  ldr     r2,=MSC0_VALUE_NONBURST
  str     r2,[r1]

  // Setup GPIO clear registers
  ldr r1, =PXA2X0_GPCR0
  ldr r2, =(GPCR0_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPCR1
  ldr r2, =(GPCR1_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPCR2
  ldr r2, =(GPCR2_VALUE)
  str r2, [r1]

  // Setup GPIO set registers
  ldr r1, =PXA2X0_GPSR0
  ldr r2, =(GPSR0_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPSR1
  ldr r2, =(GPSR1_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPSR2
  ldr r2, =(GPSR2_VALUE)
  str r2, [r1]

  // Setup GPIO direction registers
  ldr r1, =PXA2X0_GPDR0
  ldr r2, =(GPDR0_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPDR1
  ldr r2, =(GPDR1_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GPDR2
  ldr r2, =(GPDR2_VALUE)
  str r2, [r1]

  // Setup GPIO alternate function registers
  ldr r1, =PXA2X0_GAFR0_L
  ldr r2, =(GAFR0_L_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GAFR0_U
  ldr r2, =(GAFR0_U_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GAFR1_L
  ldr r2, =(GAFR1_L_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GAFR1_U
  ldr r2, =(GAFR1_U_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GAFR2_L
  ldr r2, =(GAFR2_L_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_GAFR2_U
  ldr r2, =(GAFR2_U_VALUE)
  str r2, [r1]

  ldr r1, =PXA2X0_PSSR
  ldr r2, =(PSSR_VALUE)
  str r2, [r1]

  // Enable the Icache
  mrc     p15, 0, r0, c1, c0, 0
  orr     r0, r0, #MMU_Control_I
  mcr     p15, 0, r0, c1, c0, 0
  CPWAIT  r0

#if defined(XSDEBUG)
  ldr     r0,=PXA2X0_FFTHR    /* send char C */
  ldr     r1,=0x43
  str     r1,[r0]
#endif

  // Set up a stack [for calling C code]
  ldr     r1,=__startup_stack
  ldr     r2,=PXA2X0_RAM_BANK0_BASE
  orr     sp,r1,r2

  // Create MMU tables
  bl      hal_mmu_init

#if defined(XSDEBUG)
  ldr     r0,=PXA2X0_FFTHR    /* send char D */
  ldr     r1,=0x44
  str     r1,[r0]
#endif

  // Enable MMU
  ldr     r2,=10f
  ldr     r1,=MMU_Control_Init|MMU_Control_M
  mcr     MMU_CP,0,r1,MMU_Control,c0
  mov     pc, r2
  nop
  nop
  nop

10:
#if defined(XSDEBUG)
  ldr     r0,=PXA2X0_FFTHR    /* send char E */
  ldr     r1,=0x45
  str     r1,[r0]
#endif

  .endm

#else // defined(CYG_HAL_STARTUP_ROM)
#define PLATFORM_SETUP1
#endif

#define PLATFORM_VECTORS         _platform_vectors
  .macro  _platform_vectors
  .globl  hal_pcsr_cfg_retry
  hal_pcsr_cfg_retry:   .long   0  // Boot-time value of PCSR Retry bit.
  .endm                                        
 
/*---------------------------------------------------------------------------*/
/* end of hal_platform_setup.h                                               */
#endif /* CYGONCE_HAL_PLATFORM_SETUP_H */
