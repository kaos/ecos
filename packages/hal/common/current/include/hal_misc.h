#ifndef CYGONCE_HAL_HAL_MISC_H
#define CYGONCE_HAL_HAL_MISC_H

//=============================================================================
//
//      hal_misc.h
//
//      HAL header for miscellaneous helper routines
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
// Date:        2000-06-08
// Purpose:     Miscellaneous routines shared between HALs
// Usage:       #include <cyg/hal/hal_misc.h>
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>         // types & externC

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

#include <cyg/hal/hal_intr.h>           // hal_interrupt_x tables
#include <cyg/hal/drv_api.h>            // CYG_ISR_HANDLED

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

//=============================================================================
externC cyg_bool cyg_hal_is_break(char *buf, int size);
externC void cyg_hal_user_break( CYG_ADDRWORD *regs );
#endif

//=============================================================================

#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000

#define SZ_1K           0x00000400
#define SZ_2K           0x00000800
#define SZ_4K           0x00001000
#define SZ_8K           0x00002000
#define SZ_16K          0x00004000
#define SZ_32K          0x00008000
#define SZ_64K          0x00010000
#define SZ_128K         0x00020000
#define SZ_256K         0x00040000
#define SZ_512K         0x00080000
#define SZ_1M           0x00100000
#define SZ_2M           0x00200000
#define SZ_4M           0x00400000
#define SZ_8M           0x00800000
#define SZ_16M          0x01000000
#define SZ_32M          0x02000000
#define SZ_64M          0x04000000
#define SZ_128M         0x08000000
#define SZ_256M         0x10000000
#define SZ_512M         0x20000000
#define SZ_1G           0x40000000

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_MISC_H
// End of hal_misc.h
