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
// Author(s):    nickg
// Contributors: nickg, jlarmour, pjo
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/plf_misc.h>

/*------------------------------------------------------------------------*/
// Array which stores the configured priority levels for the configured
// interrupts.

volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];

/*------------------------------------------------------------------------*/

void hal_variant_init(void)
{
#ifdef CYGPKG_KERNEL
	{	int i ;

// Write all of the interrupt vectors to point to us.
		for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++)
		{	hal_pc_interrupt_attach(i, (CYG_ADDRESS)cyg_hal_default_isr, 0, 0) ;
		}
	}
#endif /* CYGPKG_KERNEL */    
}


// pjo, 5 oct 1999

void hal_pc_clock_initialize(cyg_uint32 period)
{
/* Select mode 3: square wave.  Then we'll load LSB, and finally MSB. */
	pc_outb(PC_PIT_CONTROL, 0x36) ;
	pc_outb(PC_PIT_CLOCK_0, period);			/* Only the LSB makes it to the bus. */
	pc_outb(PC_PIT_CLOCK_0, period >> 8);		/* Now the second 8 bits. */
}


void hal_pc_clock_read(cyg_uint32 * count)
{
	cyg_uint32 curr ;
	cyg_uint32 interruptState ;

/* Hold off on interrupts for a bit. */
	HAL_DISABLE_INTERRUPTS(interruptState) ;

/* Latch counter 0. */
	pc_outb(PC_PIT_CONTROL, 0x00);

/* Now get the value. */
	curr = pc_inb(PC_PIT_CLOCK_0) & 0xFF ;
	curr |= ((pc_inb(PC_PIT_CLOCK_0) & 0xFF) << 8);

/* (Maybe) restore interrupts. */
	HAL_RESTORE_INTERRUPTS(interruptState) ;

	count[0] = CYGNUM_HAL_RTC_PERIOD - curr ;
}

#if 0
void hal_idle_thread_action(cyg_uint32 loop_count)
{	asm("hlt") ;
}
#endif

void hal_ctrlc_isr_init(void)
{
}


/*------------------------------------------------------------------------*/
/* End of var_misc.c                                                      */
