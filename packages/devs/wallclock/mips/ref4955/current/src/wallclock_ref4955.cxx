//==========================================================================
//
//      devs/wallclock/mips/ref4955/ref4955.cxx
//
//      Wallclock implementation
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
// Author(s):     jskov
// Contributors:  jskov
// Date:          2000-05-25
// Purpose:       Wallclock driver for REF4955
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>             // Kernel config
#include <pkgconf/wallclock.h>          // Wallclock device config

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/infra/cyg_type.h>         // Common type definitions and support

#include <cyg/io/wallclock.hxx>         // The WallClock API
#include <cyg/io/wallclock/wallclock.inl> // Helpers

#include <cyg/infra/diag.h>             // For debugging

// REF4955 has a Dallas 1742W part.
#define DS_BASE 0xb5000000
#include <cyg/io/wallclock/ds1742.inl>

//-----------------------------------------------------------------------------
// Functions required for the hardware-driver API.

// Returns the number of seconds elapsed since 1970-01-01 00:00:00.
cyg_uint32 
Cyg_WallClock::get_hw_seconds(void)
{
    cyg_uint32 year, month, mday, hour, minute, second;

    get_ds_hwclock(&year, &month, &mday, &hour, &minute, &second);

#if 0
    // This will cause the test to eventually fail due to these printouts
    // causing timer interrupts to be lost...
    diag_printf("year %02d\n", year);
    diag_printf("month %02d\n", month);
    diag_printf("mday %02d\n", mday);
    diag_printf("hour %02d\n", hour);
    diag_printf("minute %02d\n", minute);
    diag_printf("second %02d\n", second);
#endif

    cyg_uint32 now = _simple_mktime(year, month, mday, hour, minute, second);
    return now;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE

// Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
void
Cyg_WallClock::set_hw_seconds( cyg_uint32 secs )
{
    cyg_uint32 year, month, mday, hour, minute, second;

    _simple_mkdate(secs, &year, &month, &mday, &hour, &minute, &second);

    set_ds_hwclock(year, month, mday, hour, minute, second);
}

#endif

void
Cyg_WallClock::init_hw_seconds(void)
{
#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    init_ds_hwclock();
#else
    // This is our base: 1970-01-01 00:00:00
    // Set the HW clock - if for nothing else, just to be sure it's in a
    // legal range. Any arbitrary base could be used.
    // After this the hardware clock is only read.
    set_ds_hwclock(1970,1,1,0,0,0);
#endif
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/mips/ref4955/wallclock_ref4955.cxx
