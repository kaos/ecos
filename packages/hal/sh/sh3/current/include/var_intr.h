#ifndef CYGONCE_VAR_INTR_H
#define CYGONCE_VAR_INTR_H

//==========================================================================
//
//      var_intr.h
//
//      HAL variant interrupt and clock support
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
// Contributors: jskov,
// Date:         1999-04-24
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//               #include <cyg/hal/hal_intr.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include CYGBLD_HAL_CPU_MODULES_H       // INTC module selection

//--------------------------------------------------------------------------
// Optional platform overrides and fallbacks
#include <cyg/hal/plf_intr.h>

#ifndef CYGPRI_HAL_INTERRUPT_UPDATE_LEVEL_PLF
# define CYGPRI_HAL_INTERRUPT_UPDATE_LEVEL_PLF(vec, level)                  \
    case CYGNUM_HAL_INTERRUPT_NMI:                                          \
        /* fall through */                                                  \
    case CYGNUM_HAL_INTERRUPT_LVL0 ... CYGNUM_HAL_INTERRUPT_LVL14:          \
        /* Cannot change levels */                                          \
        break;                                                           
#endif

#ifndef CYGPRI_HAL_INTERRUPT_ACKNOWLEDGE_PLF
# define CYGPRI_HAL_INTERRUPT_ACKNOWLEDGE_PLF(vec)
#endif

#ifndef CYGPRI_HAL_INTERRUPT_CONFIGURE_PLF
# define CYGPRI_HAL_INTERRUPT_CONFIGURE_PLF(vec, level, up)
#endif

//----------------------------------------------------------------------------
// Additional vectors provided by INTC V2

#if (CYGARC_SH_MOD_INTC >= 2)
#define CYGNUM_HAL_INTERRUPT_RESERVED_5C0    32
#define CYGNUM_HAL_INTERRUPT_HUDI_HUDI       33
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ0        34
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ1        35
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ2        36
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ3        37
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ4        38
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ5        39
#define CYGNUM_HAL_INTERRUPT_RESERVED_6C0    40
#define CYGNUM_HAL_INTERRUPT_RESERVED_6E0    41
#define CYGNUM_HAL_INTERRUPT_PINT_PINT07     42
#define CYGNUM_HAL_INTERRUPT_PINT_PINT8F     43
#define CYGNUM_HAL_INTERRUPT_RESERVED_740    44
#define CYGNUM_HAL_INTERRUPT_RESERVED_760    45
#define CYGNUM_HAL_INTERRUPT_RESERVED_780    46
#define CYGNUM_HAL_INTERRUPT_RESERVED_7A0    47
#define CYGNUM_HAL_INTERRUPT_RESERVED_7C0    48
#define CYGNUM_HAL_INTERRUPT_RESERVED_7E0    59
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI0       50
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI1       51
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI2       52
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI3       53
#define CYGNUM_HAL_INTERRUPT_IRDA_ERI1       54
#define CYGNUM_HAL_INTERRUPT_IRDA_RXI1       55
#define CYGNUM_HAL_INTERRUPT_IRDA_BRI1       56
#define CYGNUM_HAL_INTERRUPT_IRDA_TXI1       57
#define CYGNUM_HAL_INTERRUPT_SCIF_ERI2       58
#define CYGNUM_HAL_INTERRUPT_SCIF_RXI2       59
#define CYGNUM_HAL_INTERRUPT_SCIF_BRI2       60
#define CYGNUM_HAL_INTERRUPT_SCIF_TXI2       61
#define CYGNUM_HAL_INTERRUPT_ADC_ADI         62

#undef  CYGNUM_HAL_ISR_MAX
#ifdef CYGNUM_HAL_ISR_PLF_MAX
# define CYGNUM_HAL_ISR_MAX                  CYGNUM_HAL_ISR_PLF_MAX
#else
# define CYGNUM_HAL_ISR_MAX                  CYGNUM_HAL_INTERRUPT_ADC_ADI
#endif

#endif // CYGARC_SH_MOD_INTC >= 2

//----------------------------------------------------------------------------
// Additional vectors provided by INTC V3

#if (CYGARC_SH_MOD_INTC >= 3)
#define CYGNUM_HAL_INTERRUPT_LCDC_LCDI       63
#define CYGNUM_HAL_INTERRUPT_PCC_PCC0        64
#define CYGNUM_HAL_INTERRUPT_PCC_PCC1        65

#undef  CYGNUM_HAL_ISR_MAX
#ifdef CYGNUM_HAL_ISR_PLF_MAX
# define CYGNUM_HAL_ISR_MAX                  CYGNUM_HAL_ISR_PLF_MAX
#else
# define CYGNUM_HAL_ISR_MAX                  CYGNUM_HAL_INTERRUPT_PCC_PCC1
#endif

#endif // CYGARC_SH_MOD_INTC >= 3


//----------------------------------------------------------------------------
// Platform may provide extensions to the interrupt configuration functions
// via these macros. The first macro is put inside the functions's
// switch statements, the last two called as functions.
#ifndef CYGPRI_HAL_INTERRUPT_UPDATE_LEVEL_PLF
# define CYGPRI_HAL_INTERRUPT_UPDATE_LEVEL_PLF(vec, level)
# define CYGPRI_HAL_INTERRUPT_ACKNOWLEDGE_PLF(vec)
# define CYGPRI_HAL_INTERRUPT_CONFIGURE_PLF(vec)
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_INTR_H
// End of var_intr.h
