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

//---------------------------------------------------------------------------
// Use MMU resources to map memory regions.  
// Takes and returns an int used to ID the MMU resource to use. This ID
// is increased as resources are used and should be used for subsequent
// invocations.
//
// The MPC8xx CPUs do not have BATs. Fortunately we don't currently
// use the MMU, so we can simulate BATs by using the TLBs.
int
cyg_hal_map_memory (int id,CYG_ADDRESS virt, CYG_ADDRESS phys, 
                    cyg_int32 size, cyg_uint8 flags)
{
    cyg_uint32 epn, rpn, ctr, twc;
    int max_tlbs;

#if defined(CYGPKG_HAL_POWERPC_MPC860)
    // There are 32 TLBs.
    max_tlbs = 32;
#elif defined(CYGPKG_HAL_POWERPC_MPC823) || defined(CYGPKG_HAL_POWERPC_MPC850)
    // There are 8 TLBs.
    max_tlbs = 8;
#endif

    epn = (virt & MI_EPN_EPNMASK) | MI_EPN_EV;
    rpn = ((phys & MI_RPN_RPNMASK) 
           | MI_RPN_PPRWRW | MI_RPN_LPS | MI_RPN_SH | MI_RPN_V);
    if (flags & CYGARC_MEMDESC_CI) 
        rpn |= MI_RPN_CI;

    twc = MI_TWC_PS8MB | MI_TWC_V;
    if (flags & CYGARC_MEMDESC_GUARDED) 
        twc |= MI_TWC_G;

    // Ignore attempts to use more than max_tlbs.
    while (id < max_tlbs && size > 0) {
        ctr = id << MI_CTR_INDX_SHIFT;
        
        // Instruction TLB.
        CYGARC_MTSPR (MI_TWC, twc);
        CYGARC_MTSPR (MI_CTR, ctr);
        CYGARC_MTSPR (MI_EPN, epn);
        CYGARC_MTSPR (MI_RPN, rpn);

        // Data TLB.
        {
            cyg_uint32 drpn;

            // Need to mark data page as changed or an exception
            // will be generated on first write to the page.
            drpn = rpn | MD_RPN_CHANGED;

            CYGARC_MTSPR (MD_TWC, twc);
            CYGARC_MTSPR (MD_CTR, ctr);
            CYGARC_MTSPR (MD_EPN, epn);
            CYGARC_MTSPR (MD_RPN, drpn);
        }

        // Move to next 8MB block.
        size -= 8*1024*1024;
        epn  += 8*1024*1024;
        rpn  += 8*1024*1024;
        id++;
    }

    return id;
}


// Initialize MMU to a sane (NOP) state.
//
// Initialize TLBs with 0, Valid bits unset.
void
cyg_hal_clear_MMU (void)
{
    cyg_uint32 ctr;
    int id;
    int max_tlbs;

#if defined(CYGPKG_HAL_POWERPC_MPC860)
    // There are 32 TLBs.
    max_tlbs = 32;
#elif defined(CYGPKG_HAL_POWERPC_MPC823) || defined(CYGPKG_HAL_POWERPC_MPC850)
    // There are 8 TLBs.
    max_tlbs = 8;
#endif

    CYGARC_MTSPR (M_CASID, 0);

    for (id = 0; id < max_tlbs; id++) {
        ctr = id << MI_CTR_INDX_SHIFT;

        // Instruction TLBs.
        CYGARC_MTSPR (MI_TWC, 0);
        CYGARC_MTSPR (MI_CTR, ctr);
        CYGARC_MTSPR (MI_EPN, 0);
        CYGARC_MTSPR (MI_RPN, 0);
        // Data TLBs.
        CYGARC_MTSPR (MD_TWC, 0);
        CYGARC_MTSPR (MD_CTR, ctr);
        CYGARC_MTSPR (MD_EPN, 0);
        CYGARC_MTSPR (MD_RPN, 0);
    }
}

//--------------------------------------------------------------------------
// End of var_misc.c
