//==========================================================================
//
//      var_intr.c
//
//      PowerPC variant interrupt handlers
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
// Date:         2000-02-11
// Purpose:      PowerPC variant interrupt handlers
// Description:  This file contains code to handle interrupt related issues
//               on the PowerPC variant.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/ppc_regs.h>

#include <cyg/hal/hal_intr.h>           // hal_interrupt_x tables

// These are the Cyg_Interrupt::HANDLED/CALL_DSR values.
#define Cyg_InterruptHANDLED  1
#define Cyg_InterruptCALL_DSR 2


typedef cyg_uint32 cyg_ISR(cyg_uint32 vector, CYG_ADDRWORD data);

extern void cyg_interrupt_post_dsr( CYG_ADDRWORD intr_obj );

static inline cyg_uint32
hal_call_isr (cyg_uint32 vector)
{
    cyg_ISR *isr;
    CYG_ADDRWORD data;
    cyg_uint32 isr_ret;

    isr = (cyg_ISR*) hal_interrupt_handlers[vector];
    data = hal_interrupt_data[vector];

    isr_ret = (*isr) (vector, data);

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    if (isr_ret & Cyg_InterruptCALL_DSR) {
        cyg_interrupt_post_dsr (hal_interrupt_objects[vector]);
    }
#endif

    return isr_ret & ~Cyg_InterruptCALL_DSR;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef CYGPKG_HAL_POWERPC_MPC860

// Since the interrupt sources do not have fixed vectors on the 860
// SIU, some arbitration is required.

// More than one interrupt source can be programmed to use the same
// vector, so all sources on the same vector have to be queried to
// find the one raising the interrupt. This functionality has not been
// implemented, but the arbiter functions for each of the SIU
// interrupt sources can be called in sequence without change.



// Timebase interrupt can be caused by match on either reference A
// or B.  
// Note: If only one interrupt source is assigned per vector, and only
// reference interrupt A or B is used, this ISR is not
// necessary. Attach the timerbase reference A or B ISR directly to
// the LVLx vector instead.
externC cyg_uint32
hal_arbitration_isr_tb (CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 isr_ret;
    cyg_uint16 tbscr;

    HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
    if (tbscr & CYGARC_REG_IMM_TBSCR_REFA) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SIU_TB_A);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    if (tbscr & CYGARC_REG_IMM_TBSCR_REFB) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SIU_TB_B);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    return 0;
}

// Periodic interrupt.
// Note: If only one interrupt source is assigned per vector, this ISR
// is not necessary. Attach the periodic interrupt ISR directly to the
// LVLx vector instead.
externC cyg_uint32
hal_arbitration_isr_pit (CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 isr_ret;
    cyg_uint16 piscr;

    HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
    if (piscr & CYGARC_REG_IMM_PISCR_PS) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SIU_PIT);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    return 0;
}

// Real time clock interrupts can be caused by the alarm or
// once-per-second.
// Note: If only one interrupt source is assigned per vector, and only
// the alarm or once-per-second interrupt is used, this ISR is not
// necessary. Attach the alarm or once-per-second ISR directly to the
// LVLx vector instead.
externC cyg_uint32
hal_arbitration_isr_rtc (CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 isr_ret;
    cyg_uint16 rtcsc;

    HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
    if (rtcsc & CYGARC_REG_IMM_RTCSC_SEC) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    if (rtcsc & CYGARC_REG_IMM_RTCSC_ALR) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    return 0;
}

// Communication Processor Module interrupt can be caused by any of
// the CPM sources.
externC cyg_uint32
hal_arbitration_isr_cpm (CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 isr_ret;
    cyg_uint16 civr;

    HAL_WRITE_UINT16 (CYGARC_REG_IMM_CIVR, CYGARC_REG_IMM_CIVR_IACK);
    HAL_READ_UINT16 (CYGARC_REG_IMM_CIVR, civr);
    civr >>= CYGARC_REG_IMM_CIVR_VECTOR_SHIFT;
    if (civr) {
        isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_CPM_LAST - civr);

        CYG_ASSERT (isr_ret & Cyg_InterruptHANDLED, 
                    "Interrupt not handled");

#ifndef CYGSEM_HAL_COMMON_INTERRUPTS_CHECK_ALL_SOURCES
        return isr_ret;
#endif
    }

    return 0;
}
#endif // ifdef CYGPKG_HAL_POWERPC_MPC860

externC void
hal_variant_IRQ_init(void)
{
#ifdef CYGSEM_HAL_POWERPC_MPC860_CPM_ENABLE
    // Attach first-level CPM arbiter to the configured SIU level and
    // enable CPM interrupts.
#define ID_CPM 0xDEAD
#define CYGPRI_SIU_LVL (CYGNUM_HAL_INTERRUPT_SIU_LVL0 \
                        +CYGHWR_HAL_POWERPC_MPC860_CPM_LVL*2)

    HAL_INTERRUPT_ATTACH (CYGPRI_SIU_LVL, &hal_arbitration_isr_cpm, ID_CPM, 0);
    HAL_INTERRUPT_UNMASK (CYGPRI_SIU_LVL);
    HAL_INTERRUPT_SET_LEVEL (CYGNUM_HAL_INTERRUPT_SIU_CPM, 
                             CYGHWR_HAL_POWERPC_MPC860_CPM_LVL);
    HAL_INTERRUPT_UNMASK (CYGNUM_HAL_INTERRUPT_SIU_CPM);
#endif
}

// -------------------------------------------------------------------------
// EOF var_intr.c

