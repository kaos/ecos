//==========================================================================
//
//      hal_intr.c
//
//      PowerPC interrupt handlers
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
// Date:         1999-02-20
// Purpose:      PowerPC interrupt handlers
// Description:  This file contains code to handle interrupt related issues
//               on the PowerPC.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_intr.h>

externC void
hal_IRQ_init(void)
{
    // No architecture general initialization, but the variant may have
    // provided some.
    hal_variant_IRQ_init();

    // Initialize real-time clock (for delays, etc, even if kernel doesn't use it)
    HAL_CLOCK_INITIALIZE(CYGNUM_HAL_RTC_PERIOD);
}

// Delay for some number of useconds.
externC void 
hal_delay_us(int us)
{
    cyg_uint32 old_dec, new_dec;
    long ticks;
    // Note: the system constant CYGNUM_HAL_RTC_PERIOD corresponds to 10,000us
    // Scale the desired number of microseconds to be a number of decrementer ticks
    if (CYGNUM_HAL_RTC_PERIOD > 10000) {
        ticks = us * (CYGNUM_HAL_RTC_PERIOD * 100) / 1000000;
    } else {
        ticks = us / (CYGNUM_HAL_RTC_PERIOD * 100) / 1000000;
    }
    asm volatile("mfdec  %0;" : "=r"(old_dec) : );
    while (ticks-- > 0) {
        do {
            asm volatile("mfdec  %0;" : "=r"(new_dec) : );        
        } while (old_dec == new_dec);
        old_dec = new_dec;
    }
}

// -------------------------------------------------------------------------
// EOF hal_intr.c
