//==========================================================================
//
//      devs/wallclock/sh3.cxx
//
//      Wallclock emulation implementation
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
// Date:          2000-03-17
// Purpose:       Wallclock driver for SH3 CPU RTC module
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>             // Kernel config
#include <pkgconf/wallclock.h>          // Wallclock device config

#include <cyg/infra/cyg_type.h>         // Common type definitions and support

#define __CYGBLD_DRIVER_PRIVATE
#include <cyg/io/wallclock.hxx>         // The WallClock API

#include <cyg/hal/sh_regs.h>            // RTC register definitions

#include <cyg/hal/hal_io.h>             // IO macros

#include <cyg/infra/diag.h>             // For debugging



//-----------------------------------------------------------------------------
// Functions for setting and getting the hardware clock counters

// Year must be last two digits of "western calendar year". Leap year when
// divisible by four.
static void
set_sh3_hwclock(cyg_uint32 year, cyg_uint32 month, cyg_uint32 mday,
                cyg_uint32 hour, cyg_uint32 minute, cyg_uint32 second)
{
    // Stop RTC
    HAL_WRITE_UINT8(CYGARC_REG_RCR2, CYGARC_REG_RCR2_RESET);

    // Program it
    HAL_WRITE_UINT8(CYGARC_REG_RYRCNT,  TO_BCD(year));
    HAL_WRITE_UINT8(CYGARC_REG_RMONCNT, TO_BCD(month));
    HAL_WRITE_UINT8(CYGARC_REG_RDAYCNT, TO_BCD(mday));
    HAL_WRITE_UINT8(CYGARC_REG_RHRCNT,  TO_BCD(hour));
    HAL_WRITE_UINT8(CYGARC_REG_RMINCNT, TO_BCD(minute));
    HAL_WRITE_UINT8(CYGARC_REG_RSECCNT, TO_BCD(second));

    // Start RTC
    HAL_WRITE_UINT8(CYGARC_REG_RCR1, CYGARC_REG_RCR1_CIE);
    HAL_WRITE_UINT8(CYGARC_REG_RCR2, 
                    CYGARC_REG_RCR2_RTCEN | CYGARC_REG_RCR2_START);

}

static void
get_sh3_hwclock(cyg_uint32* year, cyg_uint32* month, cyg_uint32* mday,
                cyg_uint32* hour, cyg_uint32* minute, cyg_uint32* second)
{
    cyg_uint8 tmp;

    do {
        // Clear carry flag
        HAL_WRITE_UINT8(CYGARC_REG_RCR1, 0);
        
        // Read time
        HAL_READ_UINT8(CYGARC_REG_RYRCNT, tmp);
        *year = TO_DEC(tmp);
        HAL_READ_UINT8(CYGARC_REG_RMONCNT, tmp);
        *month = TO_DEC(tmp);
        HAL_READ_UINT8(CYGARC_REG_RDAYCNT, tmp);
        *mday = TO_DEC(tmp);
        HAL_READ_UINT8(CYGARC_REG_RHRCNT, tmp);
        *hour = TO_DEC(tmp);
        HAL_READ_UINT8(CYGARC_REG_RMINCNT, tmp);
        *minute = TO_DEC(tmp);
        HAL_READ_UINT8(CYGARC_REG_RSECCNT, tmp);
        *second = TO_DEC(tmp);

        // Read carry flag
        HAL_READ_UINT8(CYGARC_REG_RCR1, tmp);
    } while (CYGARC_REG_RCR1_CF & tmp); // loop if carry set
}

//-----------------------------------------------------------------------------
// Functions required for the hardware-driver API.

// Returns the number of seconds elapsed since 1970-01-01 00:00:00.
cyg_uint32 
Cyg_WallClock::get_hw_seconds(void)
{
    cyg_uint32 year, month, mday, hour, minute, second;

    get_sh3_hwclock(&year, &month, &mday, &hour, &minute, &second);

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

#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
    // We know what we initialized the hardware for : 1970, so by doing this
    // the returned time should be OK for 30 years uptime.
    year += 1900;
#else
    // Need to use sliding window or similar to figure out what the
    // century should be... Patent issue is unclear, and since there's
    // no battery backup of the clock, there's little point in
    // investigating.
# error "Need some magic here to figure out century counter"
#endif

    cyg_uint32 now = _simple_mktime(year, month, mday, hour, minute, second);
    return now;
}

#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE

void
Cyg_WallClock::init_hw_seconds(void)
{
    // This is our base: 1970-01-01 00:00:00
    // Set the HW clock - if for nothing else, just to be sure it's in a
    // legal range. Any arbitrary base could be used.
    // After this the hardware clock is only read.
    set_sh3_hwclock(70,1,1,0,0,0);
}

#endif // CYGSEM_WALLCLOCK_SET_GET_MODE

//-----------------------------------------------------------------------------
// End of devs/wallclock/sh3.cxx
