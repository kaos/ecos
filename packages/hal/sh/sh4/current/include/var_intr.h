#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//==========================================================================
//
//      var_intr.h
//
//      SH4 Interrupt and clock support
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

// More include statements below. First part of this file must be
// usable for both assembly and C files, so only use defines here.

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

//--------------------------------------------------------------------------
// Additional SH4 exception vectors. 
// These are identified with event values 0x800 and 0x820, so we need to
// extend the decode macros accordingly. Performance wise it's not an
// issue - these are exceptions, not interrupts. No critical fast path.
#define CYGNUM_HAL_VECTOR_FPU_EXCEPTION          9 // FPU exception
#define CYGNUM_HAL_VECTOR_TLB_MUTI_HIT          10 // mutible TLB hit
#define CYGNUM_HAL_VECTOR_FPU_DISABLE           17
#define CYGNUM_HAL_VECTOR_SLOT_FPU_DISABLE      18

#define CYG_VECTOR_IS_INTERRUPT(v)   \
    ((CYGNUM_HAL_VECTOR_INSTRUCTION_BP < (v)) && ((v) < CYGNUM_HAL_VECTOR_FPU_DISABLE))

#define CYGNUM_HAL_VSR_MAX                   CYGNUM_HAL_VECTOR_SLOT_FPU_DISABLE

#define CYGNUM_HAL_VSR_EXCEPTION_COUNT       (CYGNUM_HAL_VSR_MAX-CYGNUM_HAL_VECTOR_POWERON+1)

#define CYGNUM_HAL_INTERRUPT_RESERVED_5C0    32
#define CYGNUM_HAL_INTERRUPT_RESERVED_5E0    33
#define CYGNUM_HAL_INTERRUPT_HUDI            34
#define CYGNUM_HAL_INTERRUPT_GPIO            35
#define CYGNUM_HAL_INTERRUPT_DMAC_DMTE0      36
#define CYGNUM_HAL_INTERRUPT_DMAC_DMTE1      37
#define CYGNUM_HAL_INTERRUPT_DMAC_DMTE2      38
#define CYGNUM_HAL_INTERRUPT_DMAC_DMTE3      39
#define CYGNUM_HAL_INTERRUPT_DMAC_DMAE       40
#define CYGNUM_HAL_INTERRUPT_RESERVED_6E0    41
#define CYGNUM_HAL_INTERRUPT_SCIF_ERI        42
#define CYGNUM_HAL_INTERRUPT_SCIF_RXI        43
#define CYGNUM_HAL_INTERRUPT_SCIF_BRI        44
#define CYGNUM_HAL_INTERRUPT_SCIF_TXI        45

#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_SCIF_TXI

// Exception vectors. These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception()

// These are in addition to the exceptions defined by the architecture.

#define CYGNUM_HAL_EXCEPTION_FPU_EXCEPTION          9 // fpu exception
#define CYGNUM_HAL_EXCEPTION_TLB_MULTI_HIT         10 // TLB multi hit exception
#define CYGNUM_HAL_EXCEPTION_FPU_DISABLE           17
#define CYGNUM_HAL_EXCEPTION_SLOT_FPU_DISABLE      18

#define CYGNUM_HAL_EXCEPTION_MIN          CYGNUM_HAL_EXCEPTION_POWERON
#define CYGNUM_HAL_EXCEPTION_MAX          CYGNUM_HAL_EXCEPTION_SLOT_FPU_DISABLE


// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC             CYGNUM_HAL_INTERRUPT_TMU0_TUNI0

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
// End of hal_intr.h
