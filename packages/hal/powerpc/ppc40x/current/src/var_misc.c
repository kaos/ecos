//==========================================================================
//
//      var_misc.c
//
//      HAL implementation miscellaneous functions
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
// Date:         2000-02-04
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/ppc_regs.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_mem.h>

//--------------------------------------------------------------------------
void hal_variant_init(void)
{
}


//--------------------------------------------------------------------------
// Variant specific idle thread action.
bool
hal_variant_idle_thread_action( cyg_uint32 count )
{
    // Let architecture idle thread action run
    return true;
}

//---------------------------------------------------------------------------
// Use MMU resources to map memory regions.  
// Takes and returns an int used to ID the MMU resource to use. This ID
// is increased as resources are used and should be used for subsequent
// invocations.
//
// The PPC4xx CPUs do not have BATs. Fortunately we don't currently
// use the MMU, so we can simulate BATs by using the TLBs.
int
cyg_hal_map_memory (int id, CYG_ADDRESS virt, CYG_ADDRESS phys, 
                    cyg_int32 size, cyg_uint8 flags)
{
    cyg_uint32 epn, rpn;
    int sv, lv, max_tlbs;

    // There are 64 TLBs.
    max_tlbs = 64;

    // Use the smallest "size" value which is big enough (round up)
    for (sv = 0, lv = 0x400;  sv < 8;  sv++, lv <<= 2) {
        if (lv >= size) break;        
    }

    // Note: the process ID comes from the PID register (always 0)
    epn = (virt & M_EPN_EPNMASK) | M_EPN_EV | M_EPN_SIZE(sv);
    rpn = (phys & M_RPN_RPNMASK) | M_RPN_EX | M_RPN_WR;

    if (flags & CYGARC_MEMDESC_CI) {
        rpn |= M_RPN_I;
    }

    if (flags & CYGARC_MEMDESC_GUARDED) 
        rpn |= M_RPN_G;

    CYGARC_TLBWE(id, epn, rpn);
    id++;

    // Make caches default disabled when MMU is disabled.

    return id;
}


// Initialize MMU to a sane (NOP) state.
//
// Initialize TLBs with 0, Valid bits unset.
void
cyg_hal_clear_MMU (void)
{
    cyg_uint32 tlbhi = 0;
    cyg_uint32 tlblo = 0;
    int id, max_tlbs;
    cyg_uint32 tlb[64][2];

    // There are 64 TLBs.
    max_tlbs = 64;

    CYGARC_MTSPR (M_PID, 0);

    for (id = 0; id < max_tlbs; id++) {
        CYGARC_TLBRE(id, tlb[id][0], tlb[id][1]);
        CYGARC_TLBWE(id, tlbhi, tlblo);
    }

    // Make caches default disabled when MMU is disabled.
}

//--------------------------------------------------------------------------
// End of var_misc.c
