//==========================================================================
//
//      pcmb_misc.c
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
#include <pkgconf/hal_i386_pcmb.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>

/*------------------------------------------------------------------------*/
// Array which stores the configured priority levels for the configured
// interrupts.

volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];

/*------------------------------------------------------------------------*/
// Static variables

CYG_ADDRWORD cyg_hal_pcmb_memsize_base;
CYG_ADDRWORD cyg_hal_pcmb_memsize_extended;

/*------------------------------------------------------------------------*/
// Initializer

void hal_pcmb_init(void)
{
    cyg_uint8 lo,hi;
    
    HAL_READ_CMOS( 0x15, lo );
    HAL_READ_CMOS( 0x16, hi );

    cyg_hal_pcmb_memsize_base = ((hi<<8)+lo)*1024;

#ifndef CYG_HAL_STARTUP_ROM
    // If we started up under a BIOS, then it will have put
    // the discovered extended memory size in CMOS bytes 30/31.
    HAL_READ_CMOS( 0x30, lo );
    HAL_READ_CMOS( 0x31, hi );
#else
    // 
    HAL_READ_CMOS( 0x17, lo );
    HAL_READ_CMOS( 0x18, hi );
#endif

    cyg_hal_pcmb_memsize_extended = ((hi<<8)+lo)*1024;
}

/*------------------------------------------------------------------------*/

cyg_uint8 *hal_i386_mem_real_region_top( cyg_uint8 *regionend )
{
    CYG_ASSERT( cyg_hal_pcmb_memsize_base > 0 , "No base RAM size set!");
    CYG_ASSERT( cyg_hal_pcmb_memsize_extended > 0 , "No extended RAM size set!");

    if( (CYG_ADDRESS)regionend <= 0x000A0000 )
        regionend = (cyg_uint8 *)cyg_hal_pcmb_memsize_base;
    else if( (CYG_ADDRESS)regionend >= 0x00100000 )
        regionend = (cyg_uint8 *)cyg_hal_pcmb_memsize_extended+0x00100000;

    return regionend;
}

/*------------------------------------------------------------------------*/
// Clock initialization and access

void hal_pc_clock_initialize(cyg_uint32 period)
{
    /* Select mode 2: rate generator.  Then we'll load LSB, and finally MSB. */
    HAL_WRITE_UINT8( PC_PIT_CONTROL, 0x34 );
    HAL_WRITE_UINT8( PC_PIT_CLOCK_0, period & 0xFF );
    HAL_WRITE_UINT8( PC_PIT_CLOCK_0, period >> 8 );
}


void hal_pc_clock_read(cyg_uint32 * count)
{
    cyg_uint8 lo = 0,hi = 0;
    cyg_uint32 curr = 0;
    cyg_uint32 interruptState ;

    /* Hold off on interrupts for a bit. */
    HAL_DISABLE_INTERRUPTS(interruptState) ;

    /* Latch counter 0. */
    HAL_WRITE_UINT8(PC_PIT_CONTROL, 0x00);

    /* Now get the value. */
    HAL_READ_UINT8( PC_PIT_CLOCK_0, lo );
    HAL_READ_UINT8( PC_PIT_CLOCK_0, hi );

    curr = (hi<<8) | lo;
    
    /* (Maybe) restore interrupts. */
    HAL_RESTORE_INTERRUPTS(interruptState) ;

    *count = CYGNUM_HAL_RTC_PERIOD - curr ;
}

/*------------------------------------------------------------------------*/

#if 0
void hal_idle_thread_action(cyg_uint32 loop_count)
{	asm("hlt") ;
}
#endif

/*------------------------------------------------------------------------*/
/* End of pcmb_misc.c                                                      */
