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
    HAL_UCACHE_INVALIDATE_ALL();    

    // Set cache modes
    HAL_UCACHE_WRITE_MODE_SH(CACHE_MODE_P0|CACHE_MODE_P1);

#ifdef CYGHWR_HAL_SH_CACHE_ENABLE
    // Enable cache.
    HAL_UCACHE_ENABLE();
#endif
}

//---------------------------------------------------------------------------
void
hal_variant_init(void)
{
}

//---------------------------------------------------------------------------
// Interrupt function support

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
externC cyg_uint8 cyg_hal_ILVL_table[];
#define HAL_UPDATE_ILVL_TABLE(_vector_, _level_)                         \
    /* Update the ILVL table, so it is easy for the interrupt entry */   \
    /* code to find out the level of a given interrupt source.      */   \
    cyg_hal_ILVL_table[(_vector_)] = (_level_)
#else
#define HAL_UPDATE_ILVL_TABLE(_vector_, _level_)
#endif

void
hal_interrupt_set_level(int vector, int level)
{
    cyg_uint16 iprX;                                                     
                                                                         
    CYG_ASSERT((0 <= (level) && 15 >= (level)), "Illegal level");    
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (vector)                         
                && CYGNUM_HAL_ISR_MAX >= (vector)), "Illegal vector"); 
                                                                         
    HAL_UPDATE_ILVL_TABLE(vector, level);                            
                                                                         
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
                                                                         
#if (CYGARC_SH_MOD_INTC >= 2)
#ifndef CYGHWR_HAL_SH_IRQ_USE_IRQLVL
        /* IPRC */                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          
        iprX &= ~CYGARC_REG_IPRC_IRQ0_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRC_IRQ0_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          
        iprX &= ~CYGARC_REG_IPRC_IRQ1_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRC_IRQ1_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          
        iprX &= ~CYGARC_REG_IPRC_IRQ2_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRC_IRQ2_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          
        iprX &= ~CYGARC_REG_IPRC_IRQ3_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRC_IRQ3_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         
        break;                                                           
#endif
        /* IPRD */                                                           
    case CYGNUM_HAL_INTERRUPT_PINT_PINT07:                               
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          
        iprX &= ~CYGARC_REG_IPRD_PINT07_MASK;                            
        iprX |= (level)*CYGARC_REG_IPRD_PINT07_PRI1;                   
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_PINT_PINT8F:                               
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          
        iprX &= ~CYGARC_REG_IPRD_PINT8F_MASK;                            
        iprX |= (level)*CYGARC_REG_IPRD_PINT8F_PRI1;                   
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          
        iprX &= ~CYGARC_REG_IPRD_IRQ5_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRD_IRQ5_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          
        iprX &= ~CYGARC_REG_IPRD_IRQ4_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRD_IRQ4_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         
        break;                                                           
                                                                         
        /* IPRE */                                                           
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI0:                                 
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI1:                                 
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI2:                                 
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI3:                                 
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          
        iprX &= ~CYGARC_REG_IPRE_DMAC_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRE_DMAC_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_IRDA_ERI1:                                 
    case CYGNUM_HAL_INTERRUPT_IRDA_RXI1:                                 
    case CYGNUM_HAL_INTERRUPT_IRDA_BRI1:                                 
    case CYGNUM_HAL_INTERRUPT_IRDA_TXI1:                                 
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          
        iprX &= ~CYGARC_REG_IPRE_IRDA_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRE_IRDA_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_SCIF_ERI2:                                 
    case CYGNUM_HAL_INTERRUPT_SCIF_RXI2:                                 
    case CYGNUM_HAL_INTERRUPT_SCIF_BRI2:                                 
    case CYGNUM_HAL_INTERRUPT_SCIF_TXI2:                                 
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          
        iprX &= ~CYGARC_REG_IPRE_SCIF_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRE_SCIF_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_ADC_ADI:                                   
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          
        iprX &= ~CYGARC_REG_IPRE_ADC_MASK;                               
        iprX |= (level)*CYGARC_REG_IPRE_ADC_PRI1;                      
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         
        break;                                                           
#endif // (CYGARC_SH_MOD_INTC >= 2)

#if (CYGARC_SH_MOD_INTC >= 3)
        /* IPRF */                                                           
    case CYGNUM_HAL_INTERRUPT_LCDC_LCDI:                                 
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          
        iprX &= ~CYGARC_REG_IPRF_LCDI_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRF_LCDI_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_PCC_PCC0:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          
        iprX &= ~CYGARC_REG_IPRF_PCC0_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRF_PCC0_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         
        break;                                                           
    case CYGNUM_HAL_INTERRUPT_PCC_PCC1:                                  
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          
        iprX &= ~CYGARC_REG_IPRF_PCC1_MASK;                              
        iprX |= (level)*CYGARC_REG_IPRF_PCC1_PRI1;                     
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         
        break;                                                           
#endif // (CYGARC_SH_MOD_INTC >= 3)

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
hal_interrupt_mask(int vector)                                    
{
    switch( (vector) ) {                                                
    case CYGNUM_HAL_INTERRUPT_NMI:                                        
        /* fall through */                                                
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:          
        /* Can only be masked by fiddling Imask in SR. */                 
        break;                                                            
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0...CYGNUM_HAL_ISR_MAX:            
        HAL_INTERRUPT_SET_LEVEL((vector), 0);                           
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
        HAL_INTERRUPT_SET_LEVEL((vector), 1);                           
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
#if (CYGARC_SH_MOD_INTC >= 2)
    if ( (vector) >= CYGNUM_HAL_INTERRUPT_IRQ_IRQ0                      
         && (vector) <= CYGNUM_HAL_INTERRUPT_IRQ_IRQ5) {                
                                                                          
        cyg_uint8 irr0;                                                   
                                                                          
        HAL_READ_UINT8(CYGARC_REG_IRR0, irr0);       
        switch ( vector ) {                          
#ifndef CYGHWR_HAL_SH_IRQ_USE_IRQLVL                 
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ0;           
            break;                                   
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ1;           
            break;                                   
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ2;           
            break;                                   
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ3;           
            break;                                   
#endif                                               
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ4;           
            break;                                   
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:          
            irr0 &= ~CYGARC_REG_IRR0_IRQ5;           
            break;                                   
        default:                                     
            CYG_FAIL("Unhandled interrupt vector");  
        }                                            
        HAL_WRITE_UINT8(CYGARC_REG_IRR0, irr0);      
    }                                                                     
#endif
}

// Note: The PINTs can be masked and configured individually, even
// though there are only two vectors. Maybe add some fake vectors just
// for masking/configuring?
void 
hal_interrupt_configure(int vector, int level, int up)
{
#if (CYGARC_SH_MOD_INTC >= 2)
    if ( (vector) >= CYGNUM_HAL_INTERRUPT_IRQ_IRQ0                     
         && (vector) <= CYGNUM_HAL_INTERRUPT_IRQ_IRQ5) {               
                                                                         
        cyg_uint16 icr1, ss, mask;                                       
        ss = 0;                                                          
        mask = CYGARC_REG_ICR1_SENSE_UP|CYGARC_REG_ICR1_SENSE_LEVEL;     
        if (up) ss |= CYGARC_REG_ICR1_SENSE_UP;                        
        if (level) ss |= CYGARC_REG_ICR1_SENSE_LEVEL;                  
        CYG_ASSERT(!(up && level), "Cannot trigger on high level!"); 
                                                                         
        switch( (vector) ) {                                           
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ5_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ5_shift;                   
            break;                                                       
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ4_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ4_shift;                   
            break;                                                       
#ifndef CYGHWR_HAL_SH_IRQ_USE_IRQLVL
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ3_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ3_shift;                   
            break;                                                       
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ2_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ2_shift;                   
            break;                                                       
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ1_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ1_shift;                   
            break;                                                       
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:                              
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ0_shift;                     
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ0_shift;                   
            break;                                                       
#endif
        default:
            CYG_FAIL("Unhandled interrupt vector");
        }
                                                                         
        HAL_READ_UINT16(CYGARC_REG_ICR1, icr1);                          
        icr1 &= ~mask;                                                   
        icr1 |= ss;                                                      
        HAL_WRITE_UINT16(CYGARC_REG_ICR1, icr1);                         
    }                                                                    
#endif
}

//---------------------------------------------------------------------------
// End of hal_misc.c
