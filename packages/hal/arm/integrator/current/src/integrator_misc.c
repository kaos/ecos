//==========================================================================
//
//      integrator_misc.c
//
//      HAL misc board support code for ARM INTEGRATOR7
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
// Author(s):    David A Rusling
// Contributors: Philippe Robin
// Date:         November 7, 2000
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_integrator.h>

/*------------------------------------------------------------------------*/
// On-board timer
/*------------------------------------------------------------------------*/

// forward declarations
void hal_if_init(void);

// declarations
static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
    //diag_init();  diag_printf("%s(%d)\n", __PRETTY_FUNCTION__, period);
    //diag_printf("psr = %x\n", psr());
    HAL_WRITE_UINT32(CYG_DEVICE_TIMER_CONTROL, CTL_DISABLE);    // Turn off
    HAL_WRITE_UINT32(CYG_DEVICE_TIMER_LOAD, period);
    HAL_WRITE_UINT32(CYG_DEVICE_TIMER_CONTROL,
		     CTL_ENABLE | CTL_PERIODIC | CTL_SCALE_16);
    _period = period;
}

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    //diag_init();  diag_printf("%s\n", __PRETTY_FUNCTION__);
    HAL_WRITE_UINT32(CYG_DEVICE_TIMER_CLEAR, 0);
    _period = period;
}

void hal_clock_read(cyg_uint32 *pvalue)
{
    cyg_uint32 value;
//    diag_init();  diag_printf("%s\n", __PRETTY_FUNCTION__);
    HAL_READ_UINT32(CYG_DEVICE_TIMER_CURRENT, value);
    value &= 0xFFFF;
    *pvalue = _period - (value & 0xFFFF);   // Note: counter is only 16 bits
                                            //       and decreases
}

void hal_hardware_init(void)
{
    // Any hardware/platform initialization that needs to be done.

    // Clear all interrupt sources
    HAL_WRITE_UINT32(CYG_DEVICE_IRQ_EnableClear, 0xFFFF); 

    // FIXME: The line with the thumb check is a hack, allowing
    // the farm to run test. Problem is that virtual vector table
    // API needs to be ARM/Thumb consistent. Will fix later.
#ifndef __thumb__
    // Set up eCos/ROM interfaces
    hal_if_init();
#endif
}

//
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.

int hal_IRQ_handler(void)
{
    // Do hardware-level IRQ handling
    int irq_status, vector;
    HAL_READ_UINT32(CYG_DEVICE_IRQ_Status, irq_status);
    //diag_init();  diag_printf("IRQ status: 0x%x\n", irq_status); 
    for (vector = 1;  vector <= 16;  vector++) {
        if (irq_status & (1<<vector)) return vector;
    }
    return -1 ; // This shouldn't happen!
}

//
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    //diag_init();  diag_printf("hal_interrupt_mask(%d)\n", vector);
    HAL_WRITE_UINT32(CYG_DEVICE_IRQ_EnableClear, 1<<vector);
}

#if 0
void hal_interrupt_status(void)
{
    int irq_status, irq_enable, timer_status, timer_value, timer_load;
    HAL_READ_UINT32(CYG_DEVICE_IRQ_Status, irq_status);
    HAL_READ_UINT32(CYG_DEVICE_IRQ_Enable, irq_enable);
    HAL_READ_UINT32(CYG_DEVICE_TIMER_LOAD, timer_load);
    HAL_READ_UINT32(CYG_DEVICE_TIMER_CURRENT, timer_value);
    HAL_READ_UINT32(CYG_DEVICE_TIMER_CONTROL, timer_status);    
    diag_printf("Interrupt: IRQ: %x.%x, TIMER: %x.%x.%x, psr: %x\n",
                irq_status, irq_enable, timer_status, timer_value,
                timer_load, psr());
}
#endif

void hal_interrupt_unmask(int vector)
{
    //diag_init();  diag_printf("hal_interrupt_unmask(%d)\n", vector);
    HAL_WRITE_UINT32(CYG_DEVICE_IRQ_EnableSet, 1<<vector);
}

void hal_interrupt_acknowledge(int vector)
{
    //diag_init();  diag_printf("%s(%d)\n", __PRETTY_FUNCTION__, vector);
}

void hal_interrupt_configure(int vector, int level, int up)
{
    //diag_init();  diag_printf("%s(%d,%d,%d)\n", __PRETTY_FUNCTION__, vector, level, up);
}

void hal_interrupt_set_level(int vector, int level)
{
    //diag_init();  diag_printf("%s(%d,%d)\n", __PRETTY_FUNCTION__, vector, level);
}

void hal_show_IRQ(int vector, int data, int handler)
{
    //    diag_printf("IRQ - vector: %x, data: %x, handler: %x\n", vector, data, handler);
}
 
/*---------------------------------------------------------------------------*/
/* End of hal_misc.c */
