#ifndef CYGONCE_HAL_SH_REGS_H
#define CYGONCE_HAL_SH_REGS_H
//=============================================================================
//
//      sh_regs.h
//
//      SH CPU definitions
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-04-24
// Purpose:     Define CPU memory mapped registers etc.
// Usage:       #include <cyg/hal/sh_regs.h>
// Notes:
//   This file describes registers for on-core modules found in all
//   the SH3 CPUs supported by the HAL. For each CPU is defined a
//   module specification file (mod_<CPU model number>.h) which lists
//   modules (and their version if applicable) included in that
//   particular CPU model.  Note that the versioning is ad hoc;
//   it doesn't reflect Hitachi internal versioning in any way.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

// Find out which modules are supported by the chosen CPU
// Note: At them moment only some modules are conditionalized (the ones
//       not in the 7708). For consistency, all modules should be treated
//       the same.
#include <pkgconf/system.h>
#include CYGBLD_HAL_CPU_MODULES_H

//==========================================================================
//                             CPU Definitions
//==========================================================================

//--------------------------------------------------------------------------
// Status register
#define CYGARC_REG_SR_MD                0x40000000
#define CYGARC_REG_SR_RB                0x20000000
#define CYGARC_REG_SR_BL                0x10000000
#define CYGARC_REG_SR_M                 0x00000200
#define CYGARC_REG_SR_Q                 0x00000100
#define CYGARC_REG_SR_IMASK             0x000000f0
#define CYGARC_REG_SR_I3                0x00000080
#define CYGARC_REG_SR_I2                0x00000040
#define CYGARC_REG_SR_I1                0x00000020
#define CYGARC_REG_SR_I0                0x00000010
#define CYGARC_REG_SR_S                 0x00000002
#define CYGARC_REG_SR_T                 0x00000001


//==========================================================================
//                          Memory Mapped Registers
//==========================================================================

//++++++ Module MMU ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// MMU control registers
#define CYGARC_REG_MMUCR                0xffffffe0

#define CYGARC_REG_MMUCR_SV             0x00000100
#define CYGARC_REG_MMUCR_RC_MASK        0x000000c0
#define CYGARC_REG_MMUCR_TF             0x00000004
#define CYGARC_REG_MMUCR_IX             0x00000002
#define CYGARC_REG_MMUCR_AT             0x00000001

//++++++ Module CAC ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Cache registers
#define CYGARC_REG_CCR                  0xffffffec

#define CYGARC_REG_CCR_RA               0x00000020
#define CYGARC_REG_CCR_CF               0x00000008
#define CYGARC_REG_CCR_CB               0x00000004
#define CYGARC_REG_CCR_WT               0x00000002
#define CYGARC_REG_CCR_CE               0x00000001

//--------------------------------------------------------------------------
// Cache registers
#define CYGARC_REG_CCR                  0xffffffec

#define CYGARC_REG_CCR_RA               0x00000020
#define CYGARC_REG_CCR_CF               0x00000008
#define CYGARC_REG_CCR_CB               0x00000004
#define CYGARC_REG_CCR_WT               0x00000002
#define CYGARC_REG_CCR_CE               0x00000001


//++++++ Module BSC ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Bus State Control
#define CYGARC_REG_BCR1                 0xffffff60
#define CYGARC_REG_BCR2                 0xffffff62
#define CYGARC_REG_WCR1                 0xffffff64
#define CYGARC_REG_WCR2                 0xffffff66
#define CYGARC_REG_MCR                  0xffffff68
#define CYGARC_REG_DCR                  0xffffff6a
#define CYGARC_REG_PCR                  0xffffff6c
#define CYGARC_REG_RTCSR                0xffffff6e
#define CYGARC_REG_RTCNT                0xffffff70
#define CYGARC_REG_RTCOR                0xffffff72
#define CYGARC_REG_RFCR                 0xffffff74

#ifdef CYGARC_SH_MOD_BCN_V2
# define CYGARC_REG_BCR3                0xffffff7e
#endif

#define CYGARC_REG_SDMR_AREA2_BASE      0xffffd000
#define CYGARC_REG_SDMR_AREA3_BASE      0xffffe000

#define CYGARC_REG_BCR1_DRAMTP2         0x0010
#define CYGARC_REG_BCR1_DRAMTP1         0x0008
#define CYGARC_REG_BCR1_DRAMTP0         0x0004

// Bus widths for areas
#define CYGARC_REG_BCR2_A6_8            0x1000
#define CYGARC_REG_BCR2_A6_16           0x2000
#define CYGARC_REG_BCR2_A6_32           0x3000
#define CYGARC_REG_BCR2_A5_8            0x0400
#define CYGARC_REG_BCR2_A5_16           0x0800
#define CYGARC_REG_BCR2_A5_32           0x0c00
#define CYGARC_REG_BCR2_A4_8            0x0100
#define CYGARC_REG_BCR2_A4_16           0x0200
#define CYGARC_REG_BCR2_A4_32           0x0300
#define CYGARC_REG_BCR2_A3_8            0x0040
#define CYGARC_REG_BCR2_A3_16           0x0080
#define CYGARC_REG_BCR2_A3_32           0x00c0
#define CYGARC_REG_BCR2_A2_8            0x0010
#define CYGARC_REG_BCR2_A2_16           0x0020
#define CYGARC_REG_BCR2_A2_32           0x0030

// Memory type selection and other IO behavior controls
#define CYGARC_REG_BCR1_PULA            0x8000 // Pin A25 to A0 Pull-Up
#define CYGARC_REG_BCR1_PULD            0x4000 // Pin D31 to D0 Pull-Up
#define CYGARC_REG_BCR1_HIZMEM          0x2000 // Hi-Z memory control
#define CYGARC_REG_BCR1_HIZCNT          0x1000 // High-Z Control
#define CYGARC_REG_BCR1_ENDIAN          0x0800 // Endian Flag
#define CYGARC_REG_BCR1_A0_BST_MASK     0x0600 // Area 0 Burst ROM Control
#define CYGARC_REG_BCR1_A0_BST_4        0x0200
#define CYGARC_REG_BCR1_A0_BST_8        0x0400
#define CYGARC_REG_BCR1_A0_BST_16       0x0600
#define CYGARC_REG_BCR1_A5_BST_MASK     0x0180 // Area 5 Burst ROM Control
#define CYGARC_REG_BCR1_A5_BST_4        0x0080
#define CYGARC_REG_BCR1_A5_BST_8        0x0100
#define CYGARC_REG_BCR1_A5_BST_16       0x0180
#define CYGARC_REG_BCR1_A6_BST_MASK     0x0060 // Area 6 Burst ROM Control
#define CYGARC_REG_BCR1_A6_BST_4        0x0020
#define CYGARC_REG_BCR1_A6_BST_8        0x0040
#define CYGARC_REG_BCR1_A6_BST_16       0x0060
#define CYGARC_REG_BCR1_DRAMTP_MASK     0x001c // Area 2, Area 3 Memory Type
#define CYGARC_REG_BCR1_A5PCM           0x0002 // Area 5 Bus Type
#define CYGARC_REG_BCR1_A6PCM           0x0001 // Area 6 Bus Type

// Intercycle wait states
#define CYGARC_REG_WCR1_WAITSEL         0x8000 // WAIT Sampling Timing Select
#define CYGARC_REG_WCR1_A6I_MASK        0x3000 // Intercycle Idle Specification
#define CYGARC_REG_WCR1_A6I_SHIFT       12
#define CYGARC_REG_WCR1_A5I_MASK        0x0c00
#define CYGARC_REG_WCR1_A5I_SHIFT       10
#define CYGARC_REG_WCR1_A4I_MASK        0x0300
#define CYGARC_REG_WCR1_A4I_SHIFT       8
#define CYGARC_REG_WCR1_A3I_MASK        0x00c0
#define CYGARC_REG_WCR1_A3I_SHIFT       6
#define CYGARC_REG_WCR1_A2I_MASK        0x0030
#define CYGARC_REG_WCR1_A2I_SHIFT       4
#define CYGARC_REG_WCR1_A0I_MASK        0x0003
#define CYGARC_REG_WCR1_A0I_SHIFT       0

#define CYGARC_REG_WCR1_1WS         1
#define CYGARC_REG_WCR1_2WS         2
#define CYGARC_REG_WCR1_3WS         3


// Wait states
#define CYGARC_REG_WCR2_A6_MASK         0xe000 // Wait states + burst pitch
#define CYGARC_REG_WCR2_A6_SHIFT        13
#define CYGARC_REG_WCR2_A5_MASK         0x1c00 // Wait states + burst pitch
#define CYGARC_REG_WCR2_A5_SHIFT        10
#define CYGARC_REG_WCR2_A4_MASK         0x0380 // Wait states
#define CYGARC_REG_WCR2_A4_SHIFT        7
#define CYGARC_REG_WCR2_A3_MASK         0x0060 // Wait states / CAS latency
#define CYGARC_REG_WCR2_A3_SHIFT        5
#define CYGARC_REG_WCR2_A2_MASK         0x0018 // Wait states / CAS latency
#define CYGARC_REG_WCR2_A2_SHIFT        3
#define CYGARC_REG_WCR2_A0_MASK         0x0007 // Wait states + burst pitch
#define CYGARC_REG_WCR2_A0_SHIFT        0

#define CYGARC_REG_WCR2_1WS             1
#define CYGARC_REG_WCR2_2WS             2
#define CYGARC_REG_WCR2_3WS             3
#define CYGARC_REG_WCR2_4WS             4
#define CYGARC_REG_WCR2_6WS             5
#define CYGARC_REG_WCR2_8WS             6
#define CYGARC_REG_WCR2_10WS            7



//++++++ Module UBC ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// User Break Control
#define CYGARC_REG_BRCR                 0xffffff98 // 16 bit

#define CYGARC_REG_BARA                 0xffffffb0 // 32 bit
#define CYGARC_REG_BAMRA                0xffffffb4 // 8 bit
#define CYGARC_REG_BBRA                 0xffffffb8 // 16 bit
#define CYGARC_REG_BASRA                0xffffffe4 // 8 bit

#define CYGARC_REG_BARB                 0xffffffa0 // 32 bit
#define CYGARC_REG_BAMRB                0xffffffa4 // 8 bit
#define CYGARC_REG_BASRB                0xffffffe8 // 8 bit
#define CYGARC_REG_BBRB                 0xffffffa8 // 16 bit
#define CYGARC_REG_BDRB                 0xffffff90 // 32 bit
#define CYGARC_REG_BDMRB                0xffffff94 // 32 bit


#define CYGARC_REG_BRCR_CMFA            0x8000 // condition match flag A
#define CYGARC_REG_BRCR_CMFB            0x4000 // condition match flag B
#define CYGARC_REG_BRCR_PCBA            0x0400 // PC break select A
#define CYGARC_REG_BRCR_DBEB            0x0080 // data break enable B
#define CYGARC_REG_BRCR_PCBB            0x0040 // PC break select B
#define CYGARC_REG_BRCR_SEQ             0x0008 // sequence condition select


//++++++ Module CPG ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Oscillator control registers
#define CYGARC_REG_FRQCR                0xffffff80
#define CYGARC_REG_WTCNT                0xffffff84 // read 8bit, write 16bit
#define CYGARC_REG_WTCSR                0xffffff86 // read 8bit, write 16bit

#define CYGARC_REG_WTCNT_WRITE          0x5a00     // top 8bit value for write

#define CYGARC_REG_WTCSR_WRITE          0xa500     // top 8bit value for write
#define CYGARC_REG_WTCSR_TME            0x80       // timer enable
#define CYGARC_REG_WTCSR_WT_IT          0x40       // watchdog(1)/interval(0)
#define CYGARC_REG_WTCSR_RSTS           0x20       // manual(1)/poweron(0)
#define CYGARC_REG_WTCSR_WOVF           0x10       // watchdog overflow
#define CYGARC_REG_WTCSR_IOVF           0x08       // interval overflow
#define CYGARC_REG_WTCSR_CKS2           0x04       // clock select 2
#define CYGARC_REG_WTCSR_CKS1           0x02       // clock select 1
#define CYGARC_REG_WTCSR_CKS0           0x01       // clock select 0
#define CYGARC_REG_WTCSR_CKSx_MASK      0x07       // clock select mask
// This is the period (in us) between watchdog reset and overflow.
// (used by the watchdog driver - board specific)
// Based on a 15MHz clock with max delay: 15Mhz/(4096 * 256)
#define CYGARC_REG_WTCSR_CKSx_SETTING   0x07       // max delay
#define CYGARC_REG_WTCSR_PERIOD         69905067   // ~69.9 ms

//++++++ Module TMU ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// TMU registers
#define CYGARC_REG_TOCR                 0xfffffe90 //  8 bit
#define CYGARC_REG_TSTR                 0xfffffe92 //  8 bit
#define CYGARC_REG_TCOR0                0xfffffe94 // 32 bit
#define CYGARC_REG_TCNT0                0xfffffe98 // 32 bit
#define CYGARC_REG_TCR0                 0xfffffe9c // 16 bit
#define CYGARC_REG_TCOR1                0xfffffea0 // 32 bit
#define CYGARC_REG_TCNT1                0xfffffea4 // 32 bit
#define CYGARC_REG_TCR1                 0xfffffea8 // 16 bit
#define CYGARC_REG_TCOR2                0xfffffeac // 32 bit
#define CYGARC_REG_TCNT2                0xfffffeb0 // 32 bit
#define CYGARC_REG_TCR2                 0xfffffeb4 // 16 bit
#define CYGARC_REG_TCPR2                0xfffffeb8 // 32 bit

// TSTR
#define CYGARC_REG_TSTR_STR0            0x0001
#define CYGARC_REG_TSTR_STR1            0x0002
#define CYGARC_REG_TSTR_STR2            0x0004

// TCR0/1/2
#define CYGARC_REG_TCR_TPSC0            0x0001
#define CYGARC_REG_TCR_TPSC1            0x0002
#define CYGARC_REG_TCR_TPSC2            0x0004
#define CYGARC_REG_TCR_CKEG0            0x0008
#define CYGARC_REG_TCR_CKEG1            0x0010
#define CYGARC_REG_TCR_UNIE             0x0020
#define CYGARC_REG_TCR_UNF              0x0100

// TCR2 additional bits
#define CYGARC_REG_TCR_ICPE0            0x0040
#define CYGARC_REG_TCR_ICPE1            0x0080
#define CYGARC_REG_TCR_ICPF             0x0200


//++++++ Module INTC +++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Interrupt registers
#define CYGARC_REG_EXCEVT               0xffffffd4
#define CYGARC_REG_INTEVT               0xffffffd8

#define CYGARC_REG_ICR                  0xfffffee0
#define CYGARC_REG_IPRA                 0xfffffee2
#define CYGARC_REG_IPRB                 0xfffffee4

//#ifdef CYGARC_SH_MOD_INTC_V2 // FIXME: HAL_INTERRUPT_SET_LEVEL needs these
# define CYGARC_REG_INTEVT2             0xa4000000
# define CYGARC_REG_IRR0                0xa4000004
# define CYGARC_REG_IRR1                0xa4000006
# define CYGARC_REG_IRR2                0xa4000008
# define CYGARC_REG_ICR0                CYGARC_REG_ICR
# define CYGARC_REG_ICR1                0xa4000010
# define CYGARC_REG_ICR2                0xa4000012
# define CYGARC_REG_INTER               0xa4000014
# define CYGARC_REG_IPRC                0xa4000016
# define CYGARC_REG_IPRD                0xa4000018
# define CYGARC_REG_IPRE                0xa400001a
//#endif
//#ifdef CYGARC_SH_MOD_INTC_V3 // FIXME: HAL_INTERRUPT_SET_LEVEL needs these
# define CYGARC_REG_IRR3                0xa400000a
# define CYGARC_REG_IRR4                0xa400000c
# define CYGARC_REG_IPRF                0xa400001c
//#endif


#define CYGARC_REG_IPRA_TMU0_MASK       0xf000
#define CYGARC_REG_IPRA_TMU0_PRI1       0x1000
#define CYGARC_REG_IPRA_TMU1_MASK       0x0f00
#define CYGARC_REG_IPRA_TMU1_PRI1       0x0100
#define CYGARC_REG_IPRA_TMU2_MASK       0x00f0
#define CYGARC_REG_IPRA_TMU2_PRI1       0x0010
#define CYGARC_REG_IPRA_RTC_MASK        0x000f
#define CYGARC_REG_IPRA_RTC_PRI1        0x0001

#define CYGARC_REG_IPRB_WDT_MASK        0xf000
#define CYGARC_REG_IPRB_WDT_PRI1        0x1000
#define CYGARC_REG_IPRB_REF_MASK        0x0f00
#define CYGARC_REG_IPRB_REF_PRI1        0x0100
#define CYGARC_REG_IPRB_SCI_MASK        0x00f0
#define CYGARC_REG_IPRB_SCI_PRI1        0x0010

//#ifdef CYGARC_SH_MOD_INTC_V2 // FIXME: HAL_INTERRUPT_SET_LEVEL needs these
#define CYGARC_REG_IPRC_IRQ3_MASK         0xf000
#define CYGARC_REG_IPRC_IRQ3_PRI1         0x1000
#define CYGARC_REG_IPRC_IRQ2_MASK         0x0f00
#define CYGARC_REG_IPRC_IRQ2_PRI1         0x0100
#define CYGARC_REG_IPRC_IRQ1_MASK         0x00f0
#define CYGARC_REG_IPRC_IRQ1_PRI1         0x0010
#define CYGARC_REG_IPRC_IRQ0_MASK         0x000f
#define CYGARC_REG_IPRC_IRQ0_PRI1         0x0001

#define CYGARC_REG_IPRD_PINT07_MASK       0xf000
#define CYGARC_REG_IPRD_PINT07_PRI1       0x1000
#define CYGARC_REG_IPRD_PINT8F_MASK       0x0f00
#define CYGARC_REG_IPRD_PINT8F_PRI1       0x0100
#define CYGARC_REG_IPRD_IRQ5_MASK         0x00f0
#define CYGARC_REG_IPRD_IRQ5_PRI1         0x0010
#define CYGARC_REG_IPRD_IRQ4_MASK         0x000f
#define CYGARC_REG_IPRD_IRQ4_PRI1         0x0001

#define CYGARC_REG_IPRE_DMAC_MASK         0xf000
#define CYGARC_REG_IPRE_DMAC_PRI1         0x1000
#define CYGARC_REG_IPRE_IRDA_MASK         0x0f00
#define CYGARC_REG_IPRE_IRDA_PRI1         0x0100
#define CYGARC_REG_IPRE_SCIF_MASK         0x00f0
#define CYGARC_REG_IPRE_SCIF_PRI1         0x0010
#define CYGARC_REG_IPRE_ADC_MASK          0x000f
#define CYGARC_REG_IPRE_ADC_PRI1          0x0001

#define CYGARC_REG_ICR1_MAI               0x8000
#define CYGARC_REG_ICR1_IRQLVL            0x4000
#define CYGARC_REG_ICR1_BLMSK             0x2000
#define CYGARC_REG_ICR1_IRLSEN            0x1000
#define CYGARC_REG_ICR1_SENSE_IRQ5_shift  10
#define CYGARC_REG_ICR1_SENSE_IRQ4_shift  8
#define CYGARC_REG_ICR1_SENSE_IRQ3_shift  6
#define CYGARC_REG_ICR1_SENSE_IRQ2_shift  4
#define CYGARC_REG_ICR1_SENSE_IRQ1_shift  2
#define CYGARC_REG_ICR1_SENSE_IRQ0_shift  0
#define CYGARC_REG_ICR1_SENSE_LEVEL       0x2
#define CYGARC_REG_ICR1_SENSE_UP          0x1

#define CYGARC_REG_IRR0_PINT07            0x80
#define CYGARC_REG_IRR0_PINT8F            0x40
#define CYGARC_REG_IRR0_IRQ5              0x20
#define CYGARC_REG_IRR0_IRQ4              0x10
#define CYGARC_REG_IRR0_IRQ3              0x08
#define CYGARC_REG_IRR0_IRQ2              0x04
#define CYGARC_REG_IRR0_IRQ1              0x02
#define CYGARC_REG_IRR0_IRQ0              0x01

//#endif

//#ifdef CYGARC_SH_MOD_INTC_V3 // FIXME: HAL_INTERRUPT_SET_LEVEL needs these
#define CYGARC_REG_IPRF_LCDI_MASK         0x0f00
#define CYGARC_REG_IPRF_LCDI_PRI1         0x0100
#define CYGARC_REG_IPRF_PCC0_MASK         0x00f0
#define CYGARC_REG_IPRF_PCC0_PRI1         0x0010
#define CYGARC_REG_IPRF_PCC1_MASK         0x000f
#define CYGARC_REG_IPRF_PCC1_PRI1         0x0001

//#endif

//++++++ Module RTC ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// RealTime Clock

#define CYGARC_REG_RC64CNT              0xfffffec0
#define CYGARC_REG_RSECCNT              0xfffffec2
#define CYGARC_REG_RMINCNT              0xfffffec4
#define CYGARC_REG_RHRCNT               0xfffffec6
#define CYGARC_REG_RWKCNT               0xfffffec8
#define CYGARC_REG_RDAYCNT              0xfffffeca
#define CYGARC_REG_RMONCNT              0xfffffecc
#define CYGARC_REG_RYRCNT               0xfffffece
#define CYGARC_REG_RSECAR               0xfffffed0
#define CYGARC_REG_RMINAR               0xfffffed2
#define CYGARC_REG_RHRAR                0xfffffed4
#define CYGARC_REG_RWKAR                0xfffffed6
#define CYGARC_REG_RDAYAR               0xfffffed8
#define CYGARC_REG_RMONAR               0xfffffeda
#define CYGARC_REG_RCR1                 0xfffffedc
#define CYGARC_REG_RCR2                 0xfffffede


#define CYGARC_REG_RCR1_CF              0x80 // carry flag
#define CYGARC_REG_RCR1_CIE             0x10 // carry interrupt enable
#define CYGARC_REG_RCR1_AIE             0x08 // alarm interrupt enable
#define CYGARC_REG_RCR1_AF              0x01 // alarm flag

#define CYGARC_REG_RCR2_PEF             0x80 // periodic interrupt flag
#define CYGARC_REG_RCR2_PES2            0x40 // periodic interrupt setting
#define CYGARC_REG_RCR2_PES1            0x20
#define CYGARC_REG_RCR2_PES0            0x10
#define CYGARC_REG_RCR2_RTCEN           0x08 // RTC enable
#define CYGARC_REG_RCR2_ADJ             0x04 // second adjustment
#define CYGARC_REG_RCR2_RESET           0x02 // reset
#define CYGARC_REG_RCR2_START           0x01 // start



//++++++ Module SCI ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Serial registers. All 8 bit registers.
#define CYGARC_REG_SCSMR                0xfffffe80 // serial mode register
#define CYGARC_REG_SCBRR                0xfffffe82 // bit rate register
#define CYGARC_REG_SCSCR                0xfffffe84 // serial control register
#define CYGARC_REG_SCTDR                0xfffffe86 // transmit data register
#define CYGARC_REG_SCSSR                0xfffffe88 // serial status register
#define CYGARC_REG_SCRDR                0xfffffe8a // receive data register

#ifdef CYGARC_SH_MOD_SCI_V2
# define CYGARC_REG_SCSPTR               0xfffffe7c // serial port register
#endif

// Serial Mode Register
#define CYGARC_REG_SCSMR_CA             0x80 // communication mode
#define CYGARC_REG_SCSMR_CHR            0x40 // character length (7 if set)
#define CYGARC_REG_SCSMR_PE             0x20 // parity enable
#define CYGARC_REG_SCSMR_OE             0x10 // parity mode
#define CYGARC_REG_SCSMR_STOP           0x08 // stop bit length
#define CYGARC_REG_SCSMR_MP             0x04 // multiprocessor mode
#define CYGARC_REG_SCSMR_CKS1           0x02 // clock select 1
#define CYGARC_REG_SCSMR_CKS0           0x01 // clock select 0
#define CYGARC_REG_SCSMR_CKSx_MASK      0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR_TIE            0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR_RIE            0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR_TE             0x20 // transmit enable
#define CYGARC_REG_SCSCR_RE             0x10 // receive enable
#define CYGARC_REG_SCSCR_MPIE           0x08 // multiprocessor interrupt enable
#define CYGARC_REG_SCSCR_TEIE           0x04 // transmit-end interrupt enable
#define CYGARC_REG_SCSCR_CKE1           0x02 // clock enable 1
#define CYGARC_REG_SCSCR_CKE0           0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR_TDRE           0x80 // transmit data register empty
#define CYGARC_REG_SCSSR_RDRF           0x40 // receive data register full
#define CYGARC_REG_SCSSR_ORER           0x20 // overrun error
#define CYGARC_REG_SCSSR_FER            0x10 // framing error
#define CYGARC_REG_SCSSR_PER            0x08 // parity error
#define CYGARC_REG_SCSSR_TEND           0x04 // transmit end
#define CYGARC_REG_SCSSR_MPB            0x02 // multiprocessor bit
#define CYGARC_REG_SCSSR_MPBT           0x01 // multiprocessor bit transfer

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR_CLEARMASK      0xf8

// Baud rate values calculated for peripheral clock = 15MHz (Pf = 15). 
// n is CKS setting (0-3)
// N = (Pf/(64*2^(2n-1)*B))*10^6-1
// E(%) = ((Pf*10^6/((N+1)*B*(64^(n-1)))-1)*100
//  So this macro is only valid when serial is configured for n=0:
//       N = (Pf/(32*B))*10^6-1
#define CYGARC_SCBRR_N(_b_)     \
    (((_b_) < 4800) ? 0 :       \
      ((_b_) > 115200) ? 0 :    \
       (CYGHWR_HAL_SH_BOARD_SPEED*1000000/(32*(_b_))-1))
#define CYGARC_SCBRR_CKSx(_b_) 0

//++++++ Module IRDA +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_IRDA

//--------------------------------------------------------------------------
// IrDA (infrared data association). Can also act as regular serial w FIFO

#define CYGARC_REG_SCSMR1               0xa4000140
#define CYGARC_REG_SCBRR1               0xa4000142
#define CYGARC_REG_SCSCR1               0xa4000144
#define CYGARC_REG_SCFTDR1              0xa4000146
#define CYGARC_REG_SCSSR1               0xa4000148
#define CYGARC_REG_SCFRDR1              0xa400014a
#define CYGARC_REG_SCFCR1               0xa400014c
#define CYGARC_REG_SCFDR1               0xa400014e

// Serial Mode Register
#define CYGARC_REG_SCSMR1_IRMOD          0x80 // IrDA Mode
#define CYGARC_REG_SCSMR1_ICK_MASK       0x78 // IR pulse width
#define CYGARC_REG_SCSMR1_PSEL           0x04 // IR pulse selector(?)
#define CYGARC_REG_SCSMR1_CKS1           0x02 // clock select 1
#define CYGARC_REG_SCSMR1_CKS0           0x01 // clock select 0
#define CYGARC_REG_SCSMR1_CKSx_MASK      0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR1_TIE            0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR1_RIE            0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR1_TE             0x20 // transmit enable
#define CYGARC_REG_SCSCR1_RE             0x10 // receive enable
#define CYGARC_REG_SCSCR1_CKE1           0x02 // clock enable 1
#define CYGARC_REG_SCSCR1_CKE0           0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR1_PER_MASK       0xf000 // number of parity errors
#define CYGARC_REG_SCSSR1_PER_shift      12
#define CYGARC_REG_SCSSR1_FER_MASK       0x0f00 // number of framing errors
#define CYGARC_REG_SCSSR1_FER_shift      8
#define CYGARC_REG_SCSSR1_ER             0x0080 // receive error
#define CYGARC_REG_SCSSR1_TEND           0x0040 // transmit end
#define CYGARC_REG_SCSSR1_TDFE           0x0020 // transmit fifo data empty
#define CYGARC_REG_SCSSR1_BRK            0x0010 // break detection
#define CYGARC_REG_SCSSR1_FER            0x0008 // framing error
#define CYGARC_REG_SCSSR1_PER            0x0004 // parity error
#define CYGARC_REG_SCSSR1_RDF            0x0002 // receive fifo data full
#define CYGARC_REG_SCSSR1_DR             0x0001 // receive data ready

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR2_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR1_CLEARMASK      0xf3

// Serial FIFO Control Register
#define CYGARC_REG_SCFCR1_RTRG_MASK      0xc0   // receive fifo data trigger
#define CYGARC_REG_SCFCR1_RTRG_1         0x00   // trigger on 1 char 
#define CYGARC_REG_SCFCR1_RTRG_4         0x40   // trigger on 4 chars
#define CYGARC_REG_SCFCR1_RTRG_8         0x80   // trigger on 8 chars
#define CYGARC_REG_SCFCR1_RTRG_14        0xc0   // trigger on 14 chars 

#define CYGARC_REG_SCFCR1_TTRG_MASK      0x30   // transmit fifo data trigger
#define CYGARC_REG_SCFCR1_TTRG_8         0x00   // trigger on 8 chars 
#define CYGARC_REG_SCFCR1_TTRG_4         0x10   // trigger on 4 chars
#define CYGARC_REG_SCFCR1_TTRG_2         0x20   // trigger on 2 chars
#define CYGARC_REG_SCFCR1_TTRG_1         0x30   // trigger on 1 char

#define CYGARC_REG_SCFCR1_MCE            0x08   // modem control enable
#define CYGARC_REG_SCFCR1_TFRST          0x04   // transmit fifo reset
#define CYGARC_REG_SCFCR1_RFRST          0x02   // receive fifo reset
#define CYGARC_REG_SCFCR1_LOOP           0x01   // loop back test


// Serial FIFO Data Count Set Register
#define CYGARC_REG_SCFDR1_RCOUNT_MASK    0x001f // number of chars in r fifo
#define CYGARC_REG_SCFDR1_RCOUNT_shift   0
#define CYGARC_REG_SCFDR1_TCOUNT_MASK    0x1f00 // number of chars in t fifo
#define CYGARC_REG_SCFDR1_TCOUNT_shift   8

// Baud rate values calculated for peripheral clock = 15MHz (Pf = 15). 
// n is CKS setting (0-3)
// N = (Pf/(64*2^(2n-1)*B))*10^6-1
// E(%) = ((Pf*10^6/((N+1)*B*(64^(n-1)))-1)*100
//  So this macro is only valid when serial is configured for n=0:
//       N = (Pf/(32*B))*10^6-1
#define CYGARC_SCBRR1_N(_b_)     \
    (((_b_) < 4800) ? 0 :       \
      ((_b_) > 115200) ? 0 :    \
       (CYGHWR_HAL_SH_BOARD_SPEED*1000000/(32*(_b_))-1))
#define CYGARC_SCBRR1_CKSx(_b_) 0

#endif // CYGARC_SH_MOD_IRDA

//++++++ Module SCIF +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_SCIF

//--------------------------------------------------------------------------
// Serial w FIFO registers

#define CYGARC_REG_SCSMR2               0xa4000150 // Serial mode
#define CYGARC_REG_SCBRR2               0xa4000152 // Bit rate
#define CYGARC_REG_SCSCR2               0xa4000154 // Serial control
#define CYGARC_REG_SCFTDR2              0xa4000156 // Transmit FIFO data
#define CYGARC_REG_SCSSR2               0xa4000158 // Serial status
#define CYGARC_REG_SCFRDR2              0xa400015a // Receive data FIFO
#define CYGARC_REG_SCFCR2               0xa400015c // FIFO control register
#define CYGARC_REG_SCFDR2               0xa400015e // FIFO data count set

// Serial Mode Register
#define CYGARC_REG_SCSMR2_CHR            0x40 // character length (7 if set)
#define CYGARC_REG_SCSMR2_PE             0x20 // parity enable
#define CYGARC_REG_SCSMR2_OE             0x10 // parity mode
#define CYGARC_REG_SCSMR2_STOP           0x08 // stop bit length
#define CYGARC_REG_SCSMR2_CKS1           0x02 // clock select 1
#define CYGARC_REG_SCSMR2_CKS0           0x01 // clock select 0
#define CYGARC_REG_SCSMR2_CKSx_MASK      0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR2_TIE            0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR2_RIE            0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR2_TE             0x20 // transmit enable
#define CYGARC_REG_SCSCR2_RE             0x10 // receive enable
#define CYGARC_REG_SCSCR2_CKE1           0x02 // clock enable 1
#define CYGARC_REG_SCSCR2_CKE0           0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR2_PER_MASK       0xf000 // number of parity errors
#define CYGARC_REG_SCSSR2_PER_shift      12
#define CYGARC_REG_SCSSR2_FER_MASK       0x0f00 // number of framing errors
#define CYGARC_REG_SCSSR2_FER_shift      8
#define CYGARC_REG_SCSSR2_ER             0x0080 // receive error
#define CYGARC_REG_SCSSR2_TEND           0x0040 // transmit end
#define CYGARC_REG_SCSSR2_TDFE           0x0020 // transmit fifo data empty
#define CYGARC_REG_SCSSR2_BRK            0x0010 // break detection
#define CYGARC_REG_SCSSR2_FER            0x0008 // framing error
#define CYGARC_REG_SCSSR2_PER            0x0004 // parity error
#define CYGARC_REG_SCSSR2_RDF            0x0002 // receive fifo data full
#define CYGARC_REG_SCSSR2_DR             0x0001 // receive data ready

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR2_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR2_CLEARMASK      0xf3

// Serial FIFO Control Register
#define CYGARC_REG_SCFCR2_RTRG_MASK      0xc0   // receive fifo data trigger
#define CYGARC_REG_SCFCR2_RTRG_1         0x00   // trigger on 1 char 
#define CYGARC_REG_SCFCR2_RTRG_4         0x40   // trigger on 4 chars
#define CYGARC_REG_SCFCR2_RTRG_8         0x80   // trigger on 8 chars
#define CYGARC_REG_SCFCR2_RTRG_14        0xc0   // trigger on 14 chars 

#define CYGARC_REG_SCFCR2_TTRG_MASK      0x30   // transmit fifo data trigger
#define CYGARC_REG_SCFCR2_TTRG_8         0x00   // trigger on 8 chars 
#define CYGARC_REG_SCFCR2_TTRG_4         0x10   // trigger on 4 chars
#define CYGARC_REG_SCFCR2_TTRG_2         0x20   // trigger on 2 chars
#define CYGARC_REG_SCFCR2_TTRG_1         0x30   // trigger on 1 char

#define CYGARC_REG_SCFCR2_MCE            0x08   // modem control enable
#define CYGARC_REG_SCFCR2_TFRST          0x04   // transmit fifo reset
#define CYGARC_REG_SCFCR2_RFRST          0x02   // receive fifo reset
#define CYGARC_REG_SCFCR2_LOOP           0x01   // loop back test


// Serial FIFO Data Count Set Register
#define CYGARC_REG_SCFDR2_RCOUNT_MASK    0x001f // number of chars in r fifo
#define CYGARC_REG_SCFDR2_RCOUNT_shift   0
#define CYGARC_REG_SCFDR2_TCOUNT_MASK    0x1f00 // number of chars in t fifo
#define CYGARC_REG_SCFDR2_TCOUNT_shift   8

// Baud rate values calculated for peripheral clock = 15MHz (Pf = 15). 
// n is CKS setting (0-3)
// N = (Pf/(64*2^(2n-1)*B))*10^6-1
// E(%) = ((Pf*10^6/((N+1)*B*(64^(n-1)))-1)*100
//  So this macro is only valid when serial is configured for n=0:
//       N = (Pf/(32*B))*10^6-1
#define CYGARC_SCBRR2_N(_b_)     \
    (((_b_) < 4800) ? 0 :       \
      ((_b_) > 115200) ? 0 :    \
       (CYGHWR_HAL_SH_BOARD_SPEED*1000000/(32*(_b_))-1))
#define CYGARC_SCBRR2_CKSx(_b_) 0


#endif // CYGARC_SH_MOD_SCIF

//++++++ Module DMAC +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_DMAC

//--------------------------------------------------------------------------
// DMA Controller registers

#define CYGARC_REG_SAR0                 0xa4000020
#define CYGARC_REG_DAR0                 0xa4000024
#define CYGARC_REG_DMATCR0              0xa4000028
#define CYGARC_REG_CHCR0                0xa400002c
#define CYGARC_REG_SAR1                 0xa4000030
#define CYGARC_REG_DAR1                 0xa4000034
#define CYGARC_REG_DMATCR1              0xa4000038
#define CYGARC_REG_CHCR1                0xa400003c
#define CYGARC_REG_SAR2                 0xa4000040
#define CYGARC_REG_DAR2                 0xa4000044
#define CYGARC_REG_DMATCR2              0xa4000048
#define CYGARC_REG_CHCR2                0xa400004c
#define CYGARC_REG_SAR3                 0xa4000050
#define CYGARC_REG_DAR3                 0xa4000054
#define CYGARC_REG_DMATCR3              0xa4000058
#define CYGARC_REG_CHCR3                0xa400005c
#define CYGARC_REG_DMAOR                0xa4000060

// DMA Channel Control Register 0
#define CYGARC_REG_CHCR0_RL             0x00040000 // request check level
#define CYGARC_REG_CHCR0_AM             0x00020000 // acknowledge mode
#define CYGARC_REG_CHCR0_AL             0x00010000 // acknowledge level
#define CYGARC_REG_CHCR0_DM1            0x00008000 // destination address mode
#define CYGARC_REG_CHCR0_DM0            0x00004000
#define CYGARC_REG_CHCR0_SM1            0x00002000 // source address mode
#define CYGARC_REG_CHCR0_SM2            0x00001000
#define CYGARC_REG_CHCR0_RS3            0x00000800 // resource select
#define CYGARC_REG_CHCR0_RS2            0x00000400
#define CYGARC_REG_CHCR0_RS1            0x00000200
#define CYGARC_REG_CHCR0_RS0            0x00000100
#define CYGARC_REG_CHCR0_DS             0x00000040 // DREQ select
#define CYGARC_REG_CHCR0_TM             0x00000020 // transmit mode
#define CYGARC_REG_CHCR0_TS1            0x00000010 // transmit size
#define CYGARC_REG_CHCR0_TS0            0x00000008
#define CYGARC_REG_CHCR0_IE             0x00000004 // interrupt enable
#define CYGARC_REG_CHCR0_TE             0x00000002 // transfer end
#define CYGARC_REG_CHCR0_DE             0x00000001 // DMAC enable

// DMA Channel Control Register 1
#define CYGARC_REG_CHCR1_RL             0x00040000 // request check level
#define CYGARC_REG_CHCR1_AM             0x00020000 // acknowledge mode
#define CYGARC_REG_CHCR1_AL             0x00010000 // acknowledge level
#define CYGARC_REG_CHCR1_DM1            0x00008000 // destination address mode
#define CYGARC_REG_CHCR1_DM0            0x00004000
#define CYGARC_REG_CHCR1_SM1            0x00002000 // source address mode
#define CYGARC_REG_CHCR1_SM2            0x00001000
#define CYGARC_REG_CHCR1_RS3            0x00000800 // resource select
#define CYGARC_REG_CHCR1_RS2            0x00000400
#define CYGARC_REG_CHCR1_RS1            0x00000200
#define CYGARC_REG_CHCR1_RS0            0x00000100
#define CYGARC_REG_CHCR1_DS             0x00000040 // DREQ select
#define CYGARC_REG_CHCR1_TM             0x00000020 // transmit mode
#define CYGARC_REG_CHCR1_TS1            0x00000010 // transmit size
#define CYGARC_REG_CHCR1_TS0            0x00000008
#define CYGARC_REG_CHCR1_IE             0x00000004 // interrupt enable
#define CYGARC_REG_CHCR1_TE             0x00000002 // transfer end
#define CYGARC_REG_CHCR1_DE             0x00000001 // DMAC enable

// DMA Channel Control Register 2
#define CYGARC_REG_CHCR2_RO             0x00080000 // source address reload
#define CYGARC_REG_CHCR2_DM1            0x00008000 // destination address mode
#define CYGARC_REG_CHCR2_DM0            0x00004000
#define CYGARC_REG_CHCR2_SM1            0x00002000 // source address mode
#define CYGARC_REG_CHCR2_SM2            0x00001000
#define CYGARC_REG_CHCR2_RS3            0x00000800 // resource select
#define CYGARC_REG_CHCR2_RS2            0x00000400
#define CYGARC_REG_CHCR2_RS1            0x00000200
#define CYGARC_REG_CHCR2_RS0            0x00000100
#define CYGARC_REG_CHCR2_TM             0x00000020 // transmit mode
#define CYGARC_REG_CHCR2_TS1            0x00000010 // transmit size
#define CYGARC_REG_CHCR2_TS0            0x00000008
#define CYGARC_REG_CHCR2_IE             0x00000004 // interrupt enable
#define CYGARC_REG_CHCR2_TE             0x00000002 // transfer end
#define CYGARC_REG_CHCR2_DE             0x00000001 // DMAC enable

// DMA Channel Control Register 3
#define CYGARC_REG_CHCR3_DI             0x00100000 // direct/indirect selection
#define CYGARC_REG_CHCR3_DM1            0x00008000 // destination address mode
#define CYGARC_REG_CHCR3_DM0            0x00004000
#define CYGARC_REG_CHCR3_SM1            0x00002000 // source address mode
#define CYGARC_REG_CHCR3_SM2            0x00001000
#define CYGARC_REG_CHCR3_RS3            0x00000800 // resource select
#define CYGARC_REG_CHCR3_RS2            0x00000400
#define CYGARC_REG_CHCR3_RS1            0x00000200
#define CYGARC_REG_CHCR3_RS0            0x00000100
#define CYGARC_REG_CHCR3_TM             0x00000020 // transmit mode
#define CYGARC_REG_CHCR3_TS1            0x00000010 // transmit size
#define CYGARC_REG_CHCR3_TS0            0x00000008
#define CYGARC_REG_CHCR3_IE             0x00000004 // interrupt enable
#define CYGARC_REG_CHCR3_TE             0x00000002 // transfer end
#define CYGARC_REG_CHCR3_DE             0x00000001 // DMAC enable     

// DMA Operation Register
#define CYGARC_REG_DMAOR_PR1            0x0200     // priority level
#define CYGARC_REG_DMAOR_PR0            0x0100
#define CYGARC_REG_DMAOR_AE             0x0004     // address error flag
#define CYGARC_REG_DMAOR_NMIF           0x0002     // NMI flag
#define CYGARC_REG_DMAOR_DME            0x0001     // DMA master enable


#endif // CYGARC_SH_MOD_DMAC

//++++++ Module PFC +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_PFC

#define CYGARC_REG_PACR                 0xa4000100 // port A control
#define CYGARC_REG_PBCR                 0xa4000102 // port B control
#define CYGARC_REG_PCCR                 0xa4000104 // port C control
#define CYGARC_REG_PDCR                 0xa4000106 // port D control
#define CYGARC_REG_PECR                 0xa4000108 // port E control
#define CYGARC_REG_PFCR                 0xa400010a // port F control
#define CYGARC_REG_PGCR                 0xa400010c // port G control
#define CYGARC_REG_PHCR                 0xa400010e // port H control
#define CYGARC_REG_PJCR                 0xa4000110 // port J control
#define CYGARC_REG_PKCR                 0xa4000112 // port K control
#define CYGARC_REG_PLCR                 0xa4000114 // port L control
#define CYGARC_REG_SCPCR                0xa4000116 // SC port control

#define CYGARC_REG_PADR                 0xa4000120 // port A data
#define CYGARC_REG_PBDR                 0xa4000122 // port B data
#define CYGARC_REG_PCDR                 0xa4000124 // port C data
#define CYGARC_REG_PDDR                 0xa4000126 // port D data
#define CYGARC_REG_PEDR                 0xa4000128 // port E data
#define CYGARC_REG_PFDR                 0xa400012a // port F data
#define CYGARC_REG_PGDR                 0xa400012c // port G data
#define CYGARC_REG_PHDR                 0xa400012e // port H data
#define CYGARC_REG_PJDR                 0xa4000130 // port J data
#define CYGARC_REG_PKDR                 0xa4000132 // port K data
#define CYGARC_REG_PLDR                 0xa4000134 // port L data
#define CYGARC_REG_SCPDR                0xa4000136 // SC port data


// Port C
#define CYGARC_REG_PCCR_PC7_MASK        0xc000
#define CYGARC_REG_PCCR_PC7_shift       14
#define CYGARC_REG_PCCR_PC6_MASK        0x3000
#define CYGARC_REG_PCCR_PC6_shift       12
#define CYGARC_REG_PCCR_PC5_MASK        0x0c00
#define CYGARC_REG_PCCR_PC5_shift       10
#define CYGARC_REG_PCCR_PC4_MASK        0x0300
#define CYGARC_REG_PCCR_PC4_shift       8
#define CYGARC_REG_PCCR_PC3_MASK        0x00c0
#define CYGARC_REG_PCCR_PC3_shift       6
#define CYGARC_REG_PCCR_PC2_MASK        0x0030
#define CYGARC_REG_PCCR_PC2_shift       4
#define CYGARC_REG_PCCR_PC1_MASK        0x000c
#define CYGARC_REG_PCCR_PC1_shift       2
#define CYGARC_REG_PCCR_PC0_MASK        0x0003
#define CYGARC_REG_PCCR_PC0_shift       0

#define CYGARC_REG_PCCR_MD_OTHER        0x0
#define CYGARC_REG_PCCR_MD_OUT          0x1
#define CYGARC_REG_PCCR_MD_IN_ON        0x2
#define CYGARC_REG_PCCR_MD_IN_OFF       0x3

#define CYGARC_REG_PCDR_PC7DT           0x80
#define CYGARC_REG_PCDR_PC6DT           0x40
#define CYGARC_REG_PCDR_PC5DT           0x20
#define CYGARC_REG_PCDR_PC4DT           0x10
#define CYGARC_REG_PCDR_PC3DT           0x08
#define CYGARC_REG_PCDR_PC2DT           0x04
#define CYGARC_REG_PCDR_PC1DT           0x02
#define CYGARC_REG_PCDR_PC0DT           0x01


#endif // CYGARC_SH_MOD_PFC

#endif // ifndef CYGONCE_HAL_SH_REGS_H
