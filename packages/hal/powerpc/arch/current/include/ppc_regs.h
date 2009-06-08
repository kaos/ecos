#ifndef CYGONCE_HAL_PPC_REGS_H
#define CYGONCE_HAL_PPC_REGS_H

//==========================================================================
//
//      ppc_regs.h
//
//      PowerPC CPU definitions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
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

#if !defined(CYGHWR_HAL_POWERPC_BOOK_E)

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

#else

// The MSR on BOOK E processors has some bits in common with the AIM
// architecture, but also has some differences.

#define CYGARC_REG_MSR_PMM      0x00000004   // performance monitor mark
#define CYGARC_REG_MSR_DS       0x00000010   // data address space
#define CYGARC_REG_MSR_IS       0x00000020   // instruction address space
#define CYGARC_REG_MSR_FE1      0x00000100   // floating-point exception mode 1
#define CYGARC_REG_MSR_DE       0x00000200   // debug interrupt enable
#define CYGARC_REG_MSR_UBLE     0x00000400   // User BTB lock enable (e
#define CYGARC_REG_MSR_FE0      0x00000800   // floating-point exception mode 0
#define CYGARC_REG_MSR_ME       0x00001000   // machine check enable
#define CYGARC_REG_MSR_FP       0x00002000   // floating-point available
#define CYGARC_REG_MSR_PR       0x00004000   // privilege level
#define CYGARC_REG_MSR_EE       0x00008000   // external interrupt enable
#define CYGARC_REG_MSR_CE       0x00020000   // critical interrupt enable
#define CYGARC_REG_MSR_WE       0x00040000   // wait state enable
#define CYGARC_REG_MSR_SPE      0x02000000   // SPE enable
#define CYGARC_REG_MSR_UCLE     0x04000000   // user cache lock enable

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define MSR_PMM         CYGARC_REG_MSR_PMM
#define MSR_DS          CYGARC_REG_MSR_DS
#define MSR_IS          CYGARC_REG_MSR_IS 
#define MSR_FE1         CYGARC_REG_MSR_FE1
#define MSR_DE          CYGARC_REG_MSR_DE 
#define MSR_UBLE        CYGARC_REG_MSR_UBLE
#define MSR_FE0         CYGARC_REG_MSR_FE0
#define MSR_ME          CYGARC_REG_MSR_ME 
#define MSR_FP          CYGARC_REG_MSR_FP 
#define MSR_PR          CYGARC_REG_MSR_PR 
#define MSR_EE          CYGARC_REG_MSR_EE 
#define MSR_CE          CYGARC_REG_MSR_CE 
#define MSR_WE          CYGARC_REG_MSR_WE 
#define MSR_SPE         CYGARC_REG_MSR_SPE
#define MSR_UCLE        CYGARC_REG_MSR_UCLE

// The following bits are not defined by BOOK E processors. However
// they are referenced in the architecture HAL. By defining them as
// zero we neutralize their effect.
#define MSR_RI          0
#define MSR_DR          0
#define MSR_IR          0

#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

#endif

//--------------------------------------------------------------------------
// Time Base Registers
// READ and WRITE are different addresses on some variants!

#define CYGARC_REG_TBL_W   284
#define CYGARC_REG_TBU_W   285

#if !defined(CYGHWR_HAL_POWERPC_BOOK_E)
#define CYGARC_REG_TBL_R   268
#define CYGARC_REG_TBU_R   269
#else
#define CYGARC_REG_TBL_R   284
#define CYGARC_REG_TBU_R   285
#endif

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define TBL_W      CYGARC_REG_TBL_W
#define TBU_W      CYGARC_REG_TBU_W
#define TBL_R      CYGARC_REG_TBL_R
#define TBU_R      CYGARC_REG_TBU_R
#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

#if defined(CYGHWR_HAL_POWERPC_BOOK_E)

#define CYGARC_REG_DECAR    54
#define CYGARC_REG_TCR     340
#define CYGARC_REG_TSR     336

#define CYGARC_REG_TCR_WP(__x)          ((((__x)&3)<<30)|(((__x)&0x3C)<<15))
#define CYGARC_REG_TCR_WRC_NONE         (0<<28)
#define CYGARC_REG_TCR_WRC_CHECKSTOP    (1<<28)
#define CYGARC_REG_TCR_WRC_RESET        (2<<28)
#define CYGARC_REG_TCR_WIE              (1<<27)
#define CYGARC_REG_TCR_DIE              (1<<26)
#define CYGARC_REG_TCR_FP(__x)          ((((__x)&3)<<24)|(((__x)&0x3C)<<11))
#define CYGARC_REG_TCR_FIE              (1<<23)
#define CYGARC_REG_TCR_ARE              (1<<22)


#define CYGARC_REG_TSR_ENW              (1<<31)
#define CYGARC_REG_TSR_WIS              (1<<30)
#define CYGARC_REG_TSR_WRS_NONE         (0<<28)
#define CYGARC_REG_TSR_WRS_CHECKSTOP    (1<<28)
#define CYGARC_REG_TSR_WRS_RESET        (2<<28)
#define CYGARC_REG_TSR_DIS              (1<<27)
#define CYGARC_REG_TSR_FIS              (1<<26)

#endif

//-----------------------------------------------------------------------------
// Exception Syndrome Register

#if defined(CYGHWR_HAL_POWERPC_BOOK_E)

#define CYGARC_REG_ESR          62

#define CYGARC_REG_ESR_PIL      (1<<27)
#define CYGARC_REG_ESR_PPR      (1<<26)
#define CYGARC_REG_ESR_PTR      (1<<25)
#define CYGARC_REG_ESR_FP       (1<<24)

#endif

//-----------------------------------------------------------------------------
#endif // ifdef CYGONCE_HAL_PPC_REGS_H
// End of ppc_regs.h
