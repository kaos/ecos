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

// FIXME: Let config control this
// Baud rate values calculated for peripheral clock = 15MHz (Pf = 15). 
// n is CKS setting (0-3)
// N = (Pf/(64^(n-1)*B))*10^6-1
// E(%) = ((Pf*10^6/((N+1)*B*(64^(n-1)))-1)*100
#define CYGARC_REG_SCBRR_50
#define CYGARC_REG_CKSx_50
#define CYGARC_REG_SCBRR_75
#define CYGARC_REG_CKSx_75
#define CYGARC_REG_SCBRR_110
#define CYGARC_REG_CKSx_110
#define CYGARC_REG_SCBRR_134_5
#define CYGARC_REG_CKSx_134_5
#define CYGARC_REG_SCBRR_150
#define CYGARC_REG_CKSx_150
#define CYGARC_REG_SCBRR_200
#define CYGARC_REG_CKSx_200
#define CYGARC_REG_SCBRR_300
#define CYGARC_REG_CKSx_300
#define CYGARC_REG_SCBRR_600
#define CYGARC_REG_CKSx_600
#define CYGARC_REG_SCBRR_1200
#define CYGARC_REG_CKSx_1200
#define CYGARC_REG_SCBRR_1800
#define CYGARC_REG_CKSx_1800
#define CYGARC_REG_SCBRR_2400
#define CYGARC_REG_CKSx_2400
#define CYGARC_REG_SCBRR_3600
#define CYGARC_REG_CKSx_3600
#define CYGARC_REG_SCBRR_4800           97
#define CYGARC_REG_CKSx_4800            0
#define CYGARC_REG_SCBRR_7200
#define CYGARC_REG_CKSx_7200
#define CYGARC_REG_SCBRR_9600           48
#define CYGARC_REG_CKSx_9600            0
#define CYGARC_REG_SCBRR_14400          32
#define CYGARC_REG_CKSx_14400           0
#define CYGARC_REG_SCBRR_19200          23
#define CYGARC_REG_CKSx_19200           0
#define CYGARC_REG_SCBRR_38400          11
#define CYGARC_REG_CKSx_38400           0
#define CYGARC_REG_SCBRR_57600          7
#define CYGARC_REG_CKSx_57600           0
#define CYGARC_REG_SCBRR_115200         3
#define CYGARC_REG_CKSx_115200          0
#define CYGARC_REG_SCBRR_234000
#define CYGARC_REG_CKSx_234000



#endif // ifndef CYGONCE_HAL_SH_REGS_H
