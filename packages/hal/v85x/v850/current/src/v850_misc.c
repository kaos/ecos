//==========================================================================
//
//      v850_misc.c
//
//      HAL CPU variant miscellaneous functions
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-03-15
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/v850_common.h>

externC void cyg_hal_platform_hardware_init(void);

//
// Interrupt management functions
//

static volatile unsigned char *interrupt_control_registers[] = {
    CYG_HAL_V85X_INTERRUPT_CONTROL_REGISTERS   // Defined in <plf_intr.h>
};

#define INT_CONTROL_PENDING   0x80
#define INT_CONTROL_DISABLE   0x40
#define INT_CONTROL_LEVEL(n)  n

#define INT_CONTROL_DEFAULT INT_CONTROL_DISABLE|INT_CONTROL_LEVEL(7)

//
// Mask, i.e. disable, interrupt #vector from occurring
//
void 
hal_interrupt_mask(int vector)
{
    volatile unsigned char *ctl;
    CYG_ASSERT(vector >= CYGNUM_HAL_ISR_MIN, "invalid interrupt vector [< MIN]");
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX, "invalid interrupt vector [> MAX]");
    ctl = interrupt_control_registers[vector-CYGNUM_HAL_ISR_MIN];
    CYG_ASSERT((void *)ctl != 0, "invalid interrupt vector [not defined]");
    if (ctl ) {
        *ctl |= INT_CONTROL_DISABLE;
    }
}

//
// Unmask, i.e. enable, interrupt #vector
//
void 
hal_interrupt_unmask(int vector)
{
    volatile unsigned char *ctl;
    CYG_ASSERT(vector >= CYGNUM_HAL_ISR_MIN, "invalid interrupt vector [< MIN]");
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX, "invalid interrupt vector [> MAX]");
    ctl = interrupt_control_registers[vector-CYGNUM_HAL_ISR_MIN];
    CYG_ASSERT((void *)ctl != 0, "invalid interrupt vector [not defined]");
    if (ctl ) {
        *ctl &= ~INT_CONTROL_DISABLE;
    }
}

//
// Acknowledge, i.e. clear, interrupt #vector
//
void 
hal_interrupt_acknowledge(int vector)
{
    volatile unsigned char *ctl;
    CYG_ASSERT(vector >= CYGNUM_HAL_ISR_MIN, "invalid interrupt vector [< MIN]");
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX, "invalid interrupt vector [> MAX]");
    ctl = interrupt_control_registers[vector-CYGNUM_HAL_ISR_MIN];
    CYG_ASSERT((void *)ctl != 0, "invalid interrupt vector [not defined]");
    if (ctl ) {
        *ctl &= ~INT_CONTROL_PENDING;
    }
}

static void
init_interrupts(void)
{
    int i;
    volatile unsigned char *ctl;
    for (i = CYGNUM_HAL_ISR_MIN;  i <= CYGNUM_HAL_ISR_MAX;  i++) {
        ctl = interrupt_control_registers[i-CYGNUM_HAL_ISR_MIN];
        if (ctl) {
            *ctl = INT_CONTROL_DEFAULT;
        }
    }
}

//
// Initialize the hardware.  This may involve platform specific code.
//
void
cyg_hal_hardware_init(void)
{
    init_interrupts();
    cyg_hal_platform_hardware_init();
}

/*------------------------------------------------------------------------*/
/* End of v850_misc.c                                                      */
