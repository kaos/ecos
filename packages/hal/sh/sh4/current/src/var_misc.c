//==========================================================================
//
//      var_misc.c
//
//      HAL miscellaneous functions
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
// Contributors: jskov, jlarmour, nickg
// Date:         1999-04-03
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/infra/diag.h>             // diag_printf

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_cache.h>          // HAL cache
#include <cyg/hal/hal_intr.h>           // HAL interrupts/exceptions

//---------------------------------------------------------------------------
// Initial cache enabling

#ifdef CYGHWR_HAL_SH_CACHE_MODE_P0_WRITE_BACK
# define CACHE_MODE_P0 0
#else
# define CACHE_MODE_P0 CYGARC_REG_CCR_WT
#endif

#ifdef CYGHWR_HAL_SH_CACHE_MODE_P1_WRITE_BACK
# define CACHE_MODE_P1 CYGARC_REG_CCR_CB
#else
# define CACHE_MODE_P1 0
#endif

externC void
cyg_var_enable_caches(void)
{
    // Initialize cache.
    HAL_DCACHE_INVALIDATE_ALL();    
    HAL_ICACHE_INVALIDATE_ALL();    

    // Set cache modes
    HAL_DCACHE_WRITE_MODE_SH(CACHE_MODE_P0|CACHE_MODE_P1);

#ifdef CYGHWR_HAL_SH_CACHE_ENABLE
    // Enable cache.
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_ENABLE();
#endif
}

//---------------------------------------------------------------------------
void
hal_variant_init(void)
{
}

//---------------------------------------------------------------------------

externC cyg_uint8 cyg_hal_ILVL_table[];
externC cyg_uint8 cyg_hal_IMASK_table[];

static void
hal_interrupt_update_level(int vector)
{
    cyg_uint16 iprX;                                                     
    int level;

    level = cyg_hal_IMASK_table[vector] ? cyg_hal_ILVL_table[vector] : 0;

    switch( (vector) ) {
    case CYGNUM_HAL_INTERRUPT_NMI:
        /* fall through */
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:
        /* Cannot change levels */
        break;

    /* IPRA */
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0:
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);
        iprX &= ~CYGARC_REG_IPRA_TMU0_MASK;
        iprX |= (level)*CYGARC_REG_IPRA_TMU0_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_TMU1_TUNI1:
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);
        iprX &= ~CYGARC_REG_IPRA_TMU1_MASK;
        iprX |= (level)*CYGARC_REG_IPRA_TMU1_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_TMU2_TUNI2:
    case CYGNUM_HAL_INTERRUPT_TMU2_TICPI2:
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);
        iprX &= ~CYGARC_REG_IPRA_TMU2_MASK;
        iprX |= (level)*CYGARC_REG_IPRA_TMU2_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_RTC_ATI:
    case CYGNUM_HAL_INTERRUPT_RTC_PRI:
    case CYGNUM_HAL_INTERRUPT_RTC_CUI:
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);
        iprX &= ~CYGARC_REG_IPRA_RTC_MASK;
        iprX |= (level)*CYGARC_REG_IPRA_RTC_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);
        break;

    /* IPRB */
    case CYGNUM_HAL_INTERRUPT_SCI_ERI:
    case CYGNUM_HAL_INTERRUPT_SCI_RXI:
    case CYGNUM_HAL_INTERRUPT_SCI_TXI:
    case CYGNUM_HAL_INTERRUPT_SCI_TEI:
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);
        iprX &= ~CYGARC_REG_IPRB_SCI_MASK;
        iprX |= (level)*CYGARC_REG_IPRB_SCI_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_WDT_ITI:
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);
        iprX &= ~CYGARC_REG_IPRB_WDT_MASK;
        iprX |= (level)*CYGARC_REG_IPRB_WDT_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_REF_RCMI:
    case CYGNUM_HAL_INTERRUPT_REF_ROVI:
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);
        iprX &= ~CYGARC_REG_IPRB_REF_MASK;
        iprX |= (level)*CYGARC_REG_IPRB_REF_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);
        break;

    /* IPRC */
    case CYGNUM_HAL_INTERRUPT_HUDI:
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);
        iprX &= ~CYGARC_REG_IPRC_HUDI_MASK;
        iprX |= (level)*CYGARC_REG_IPRC_HUDI_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_GPIO:
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);
        iprX &= ~CYGARC_REG_IPRC_GPIO_MASK;
        iprX |= (level)*CYGARC_REG_IPRC_GPIO_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_DMAC_DMTE0:
    case CYGNUM_HAL_INTERRUPT_DMAC_DMTE1:
    case CYGNUM_HAL_INTERRUPT_DMAC_DMTE2:
    case CYGNUM_HAL_INTERRUPT_DMAC_DMTE3:
    case CYGNUM_HAL_INTERRUPT_DMAC_DMAE:
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);
        iprX &= ~CYGARC_REG_IPRC_DMAC_MASK;
        iprX |= (level)*CYGARC_REG_IPRC_DMAC_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_SCIF_ERI:
    case CYGNUM_HAL_INTERRUPT_SCIF_RXI:
    case CYGNUM_HAL_INTERRUPT_SCIF_BRI:
    case CYGNUM_HAL_INTERRUPT_SCIF_TXI:
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);
        iprX &= ~CYGARC_REG_IPRC_DMAC_MASK;
        iprX |= (level)*CYGARC_REG_IPRC_DMAC_PRI1;
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);
        break;
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_5C0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_5E0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_6E0:
        /* Do nothing for these reserved vectors. */
        break;
    }
}

void
hal_interrupt_set_level(int vector, int level)
{
    CYG_ASSERT((0 <= (level) && 15 >= (level)), "Illegal level");
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (vector)
                && CYGNUM_HAL_ISR_MAX >= (vector)), "Illegal vector");

    cyg_hal_ILVL_table[vector] = level;

    hal_interrupt_update_level(vector);
}

void
hal_interrupt_mask(int vector)                                    
{
    switch( vector ) {
    case CYGNUM_HAL_INTERRUPT_NMI:
        /* fall through */
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:
        /* Can only be masked by fiddling Imask in SR. */
        break;
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0...CYGNUM_HAL_ISR_MAX:
        cyg_hal_IMASK_table[vector] = 0;
        hal_interrupt_update_level(vector);
        break;
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:
        /* Do nothing for these reserved vectors. */
        break;
    default:
        CYG_FAIL("Unknown interrupt vector");
        break;
    }
}

void
hal_interrupt_unmask(int vector)                                  
{
    switch( (vector) ) {
    case CYGNUM_HAL_INTERRUPT_NMI:
        /* fall through */
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:
        /* Can only be unmasked by fiddling Imask in SR. */
        break;
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0...CYGNUM_HAL_ISR_MAX: 
        cyg_hal_IMASK_table[vector] = 1;
        hal_interrupt_update_level(vector);
        break;
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:
        /* Do nothing for these reserved vectors. */
        break;
    default:
        CYG_FAIL("Unknown interrupt vector");
        break;
    }
}

void
hal_interrupt_acknowledge(int vector)
{
}

void 
hal_interrupt_configure(int vector, int level, int up)
{
}
