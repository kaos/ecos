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
int
cyg_hal_map_memory (int id,CYG_ADDRESS virt, CYG_ADDRESS phys, 
                    cyg_int32 size, cyg_uint8 flags)
{
    // Use BATs to map the memory.
    cyg_uint32 ubat, lbat;

    ubat = (virt & UBAT_BEPIMASK) | UBAT_VS | UBAT_VP;
    lbat = (phys & LBAT_BRPNMASK);
    if (flags & CYGARC_MEMDESC_CI) 
        lbat |= LBAT_I;
    if (flags & CYGARC_MEMDESC_GUARDED) 
        lbat |= LBAT_G;
        
    // There are 4 BATs, size is programmable.
    while (id < 4 && size > 0) {
        cyg_uint32 blk_size = 128*1024;
        cyg_uint32 bl = 0;
        while (blk_size < 256*1024*1024 && blk_size < size) {
            blk_size *= 2;
            bl = (bl << 1) | 1;
        }
        ubat = (ubat & ~UBAT_BLMASK) | (bl << 2);
        
        switch (id) {
        case 0:
            CYGARC_MTSPR (IBAT0U, ubat);
            CYGARC_MTSPR (IBAT0L, lbat);
            CYGARC_MTSPR (DBAT0U, ubat);
            CYGARC_MTSPR (DBAT0L, lbat);
            break;
        case 1:
            CYGARC_MTSPR (IBAT1U, ubat);
            CYGARC_MTSPR (IBAT1L, lbat);
            CYGARC_MTSPR (DBAT1U, ubat);
            CYGARC_MTSPR (DBAT1L, lbat);
            break;
        case 2:
            CYGARC_MTSPR (IBAT2U, ubat);
            CYGARC_MTSPR (IBAT2L, lbat);
            CYGARC_MTSPR (DBAT2U, ubat);
            CYGARC_MTSPR (DBAT2L, lbat);
            break;
        case 3:
            CYGARC_MTSPR (IBAT3U, ubat);
            CYGARC_MTSPR (IBAT3L, lbat);
            CYGARC_MTSPR (DBAT3U, ubat);
            CYGARC_MTSPR (DBAT3L, lbat);
            break;
        }
        
        size -= blk_size;
        id++;
    }

    return id;
}


// Initialize MMU to a sane (NOP) state.
void
cyg_hal_clear_MMU (void)
{
    cyg_uint32 ubat, lbat;
        
    // Initialize BATs with 0 -- VS&VP are unset, making all matches fail
    ubat = 0;
    lbat = 0;

    CYGARC_MTSPR (IBAT0U, ubat);
    CYGARC_MTSPR (IBAT0L, lbat);
    CYGARC_MTSPR (DBAT0U, ubat);
    CYGARC_MTSPR (DBAT0L, lbat);
    CYGARC_MTSPR (IBAT1U, ubat);
    CYGARC_MTSPR (IBAT1L, lbat);
    CYGARC_MTSPR (DBAT1U, ubat);
    CYGARC_MTSPR (DBAT1L, lbat);
    CYGARC_MTSPR (IBAT2U, ubat);
    CYGARC_MTSPR (IBAT2L, lbat);
    CYGARC_MTSPR (DBAT2U, ubat);
    CYGARC_MTSPR (DBAT2L, lbat);
    CYGARC_MTSPR (IBAT3U, ubat);
    CYGARC_MTSPR (IBAT3L, lbat);
    CYGARC_MTSPR (DBAT3U, ubat);
    CYGARC_MTSPR (DBAT3L, lbat);
}

//--------------------------------------------------------------------------
// End of var_misc.c
