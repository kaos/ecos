#ifndef CYGONCE_HAL_HAL_ARBITER_H
#define CYGONCE_HAL_HAL_ARBITER_H

//=============================================================================
//
//      hal_arbiter.h
//
//      Functionality used by ISR arbiters
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
// Date:        2001-06-29
// Purpose:     Functionality used by ISR arbiters
// Usage:       #include <cyg/hal/hal_arbiter.h>
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_intr.h>           // hal_interrupt_x tables
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

//=============================================================================
// Function used to call ISRs from ISR arbiters
// An arbiter is hooked on the shared interrupt vector and looks like this:
//
//  cyg_uint32 _arbitration_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
//  {
//     cyg_uint32 isr_ret;
//     // decode interrupt source and for each active source call the ISR
//     if (source_A_active) {
//         isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SOURCE_A);
//  #ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
//         if (isr_ret & CYG_ISR_HANDLED)
//  #endif
//             return isr_ret;
//     }
//     if (source_B_active) {
//         isr_ret = hal_call_isr (CYGNUM_HAL_INTERRUPT_SOURCE_B);
//  #ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
//         if (isr_ret & CYG_ISR_HANDLED)
//  #endif
//             return isr_ret;
//     }
//  ...
//     return 0;
//  }
//
// Remember to attach and enable the arbiter source:
//    HAL_INTERRUPT_ATTACH(CYGNUM_HAL_INTERRUPT_ARBITER, &_arbitration_isr, 0, 0);
//    HAL_INTERRUPT_SET_LEVEL(CYGNUM_HAL_INTERRUPT_ARBITER, 1);
//    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_ARBITER);
//

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
    if (isr_ret & CYG_ISR_CALL_DSR) {
        cyg_interrupt_post_dsr (hal_interrupt_objects[vector]);
    }
#endif

    return isr_ret & ~CYG_ISR_CALL_DSR;
}

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ARBITER_H
// End of hal_arbiter.h
