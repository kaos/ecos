#ifndef CYGONCE_HAL_PPC_REGS_H
#define CYGONCE_HAL_PPC_REGS_H

//==========================================================================
//
//      ppc_regs.h
//
//      PowerPC CPU definitions
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         1999-02-19
// Purpose:      Provide PPC register definitions
// Description:  Provide PPC register definitions
//               The short difinitions (sans CYGARC_REG_) are exported only
//               if CYGARC_HAL_COMMON_EXPORT_CPU_MACROS is defined.
// Usage:
//               #include <cyg/hal/ppc_regs.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/var_regs.h>


//--------------------------------------------------------------------------
// SPR access macros.
#define CYGARC_MTSPR(_spr_, _v_) \
    asm volatile ("mtspr %0, %1;" :: "I" (_spr_), "r" (_v_));
#define CYGARC_MFSPR(_spr_, _v_) \
    asm volatile ("mfspr %0, %1;" : "=r" (_v_) : "I" (_spr_));

//--------------------------------------------------------------------------
// TB access macros.
#define CYGARC_MTTB(_tbr_, _v_) \
    asm volatile ("mttb %0, %1;" :: "I" (_tbr_), "r" (_v_));
#define CYGARC_MFTB(_tbr_, _v_) \
    asm volatile ("mftb %0, %1;" : "=r" (_v_) : "I" (_tbr_));

//--------------------------------------------------------------------------
// Generic PowerPC Family Definitions
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Some SPRs
#define CYGARC_REG_DSISR    18
#define CYGARC_REG_DAR      19
#define CYGARC_REG_DEC      22
#define CYGARC_REG_SRR0     26
#define CYGARC_REG_SRR1     27
#define CYGARC_REG_SPRG0   272
#define CYGARC_REG_SPRG1   273
#define CYGARC_REG_SPRG2   274
#define CYGARC_REG_SPRG3   275
#define CYGARC_REG_PVR     287

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define DSISR      CYGARC_REG_DSISR
#define DAR        CYGARC_REG_DAR
#define DEC        CYGARC_REG_DEC
#define SRR0       CYGARC_REG_SRR0
#define SRR1       CYGARC_REG_SRR1
#define SPRG0      CYGARC_REG_SPRG0
#define SPRG1      CYGARC_REG_SPRG1
#define SPRG2      CYGARC_REG_SPRG2
#define SPRG3      CYGARC_REG_SPRG3
#define PVR        CYGARC_REG_PVR
#endif

//--------------------------------------------------------------------------
// MSR bits
#define CYGARC_REG_MSR_LE       0x00000001   // little-endian mode enable
#define CYGARC_REG_MSR_RI       0x00000002   // recoverable exception
#define CYGARC_REG_MSR_DR       0x00000010   // data address translation
#define CYGARC_REG_MSR_IR       0x00000020   // instruction address translation
#define CYGARC_REG_MSR_IP       0x00000040   // exception prefix
#define CYGARC_REG_MSR_FE1      0x00000100   // floating-point exception mode 1
#define CYGARC_REG_MSR_BE       0x00000200   // branch trace enable
#define CYGARC_REG_MSR_SE       0x00000400   // single-step trace enable
#define CYGARC_REG_MSR_FE0      0x00000800   // floating-point exception mode 0
#define CYGARC_REG_MSR_ME       0x00001000   // machine check enable
#define CYGARC_REG_MSR_FP       0x00002000   // floating-point available
#define CYGARC_REG_MSR_PR       0x00004000   // privilege level
#define CYGARC_REG_MSR_EE       0x00008000   // external interrupt enable
#define CYGARC_REG_MSR_ILE      0x00010000   // exception little-endian mode
#define CYGARC_REG_MSR_POW      0x00040000   // power management enable

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define MSR_LE          CYGARC_REG_MSR_LE 
#define MSR_RI          CYGARC_REG_MSR_RI 
#define MSR_DR          CYGARC_REG_MSR_DR 
#define MSR_IR          CYGARC_REG_MSR_IR 
#define MSR_IP          CYGARC_REG_MSR_IP 
#define MSR_FE1         CYGARC_REG_MSR_FE1
#define MSR_BE          CYGARC_REG_MSR_BE 
#define MSR_SE          CYGARC_REG_MSR_SE 
#define MSR_FE0         CYGARC_REG_MSR_FE0
#define MSR_ME          CYGARC_REG_MSR_ME 
#define MSR_FP          CYGARC_REG_MSR_FP 
#define MSR_PR          CYGARC_REG_MSR_PR 
#define MSR_EE          CYGARC_REG_MSR_EE 
#define MSR_ILE         CYGARC_REG_MSR_ILE
#define MSR_POW         CYGARC_REG_MSR_POW
#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

//--------------------------------------------------------------------------
// Time Base Registers
// READ and WRITE are different addresses!
#define CYGARC_REG_TBL_W   284
#define CYGARC_REG_TBU_W   285
#define CYGARC_REG_TBL_R   268
#define CYGARC_REG_TBU_R   269

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define TBL_W      CYGARC_REG_TBL_W
#define TBU_W      CYGARC_REG_TBU_W
#define TBL_R      CYGARC_REG_TBL_R
#define TBU_R      CYGARC_REG_TBU_R
#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

//-----------------------------------------------------------------------------
#endif // ifdef CYGONCE_HAL_PPC_REGS_H
// End of ppc_regs.h
