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

    // There are 64 TLBs.
    max_tlbs = 64;

    CYGARC_MTSPR (SPR_PID, 0);

    for (id = 0; id < max_tlbs; id++) {
        CYGARC_TLBWE(id, tlbhi, tlblo);
    }

    // Make caches default disabled when MMU is disabled.
}

//--------------------------------------------------------------------------
// Clock control - use the programmable (variable period) timer

static cyg_uint32 _period;
extern cyg_uint32 _hold_tcr;

externC void 
hal_ppc40x_clock_initialize(cyg_uint32 period)
{
    cyg_uint32 tcr;    

    // Enable auto-reload
    CYGARC_MFSPR(SPR_TCR, tcr);
    tcr = _hold_tcr;
    tcr |= TCR_ARE;
    CYGARC_MTSPR(SPR_TCR, tcr);
    _hold_tcr = tcr;

    // Set up the counter register
    _period = period;
    CYGARC_MTSPR(SPR_PIT, period);
}

// Returns the number of clocks since the last interrupt
externC void 
hal_ppc40x_clock_read(cyg_uint32 *val)
{
    cyg_uint32 period;

    CYGARC_MFSPR(SPR_PIT, period);
    *val = _period - period;
}

externC void 
hal_ppc40x_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    hal_ppc40x_clock_initialize(period);
}

externC void 
hal_ppc40x_delay_us(int us)
{
}

//--------------------------------------------------------------------------
// End of var_misc.c
