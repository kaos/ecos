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
// Notes:         Currently only reads from the RTC. There should be a config
//                option for selecting whether to do init-read or read-write
//                to the clock. This will be useful on targets which have
//                battery backing on the RTC circuitry.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>             // Kernel config
#include <pkgconf/wallclock.h>          // Wallclock device config

#include <cyg/infra/cyg_type.h>         // Common type definitions and support
#include <cyg/kernel/sched.inl>

#include <cyg/devs/wallclock.hxx>

#include <cyg/hal/sh_regs.h>            // RTC register definitions

#include <cyg/infra/diag.h>         // Common type definitions and support


//-----------------------------------------------------------------------------
// Local static variables

static Cyg_WallClock wallclock_instance CYG_INIT_PRIORITY( CLOCK );

static cyg_uint32 hw_epoch_base;

static cyg_uint32 epoch_ticks;
static cyg_uint32 epoch_time_stamp;

Cyg_WallClock *Cyg_WallClock::wallclock;


//-----------------------------------------------------------------------------
// BCD helper macros
#define TO_BCD(x) (((x/10)<<4) | (x%10))
#define TO_DEC(x) (((x>>4)*10) + (x&0xf))

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
// Functions providing a simple read-elapsed-seconds interface to the
// hardware clock

// This function is from the Linux kernel.
//
// Converts Gregorian date to seconds since 1970-01-01 00:00:00.
// Assumes input in normal date format, i.e. 1980-12-31 23:59:59
// => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
//
// This algorithm was first published by Gauss (I think).
//
// WARNING: this function will overflow on 2106-02-07 06:28:16 on
// machines were long is 32-bit! (However, as time_t is signed, we
// will already get problems at other places on 2038-01-19 03:14:08)
static cyg_uint32
_simple_mktime(cyg_uint32 year, cyg_uint32 mon,
		     cyg_uint32 day, cyg_uint32 hour,
		     cyg_uint32 min, cyg_uint32 sec)
{
	if (0 >= (int) (mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}
	return (((
		(cyg_uint32)(year/4 - year/100 + year/400 + 367*mon/12 + day) +
		year*365 - 719499
		)*24 + hour /* now have hours */
		)*60 + min /* now have minutes */
		)*60 + sec; /* finally seconds */
}

// Returns number of elapsed ticks since the hw_epoch_base
static inline cyg_uint32 
get_hw_ticks(void)
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

    cyg_uint32 now = _simple_mktime(year, month, mday, hour, minute, second);
    return now - hw_epoch_base;
}

static inline void
init_hw_ticks(void)
{
    // This is our base: 1970-01-01 00:00:00
    // Set the HW clock - if for nothing else, just to be sure it's in a
    // legal range. Any arbitrary base could be used.
    // After this the hardware clock is only read.
    hw_epoch_base = _simple_mktime(1970,1,1,0,0,0);
    set_sh3_hwclock(70,1,1,0,0,0);
}

//-----------------------------------------------------------------------------
// Constructor

Cyg_WallClock::Cyg_WallClock()
{
    // install instance pointer
    wallclock = &wallclock_instance;

    init_hw_ticks();
}

//-----------------------------------------------------------------------------
// Returns the current timestamp. This may involve reading the
// hardware, so it may take anything up to a second to complete.

cyg_uint32 Cyg_WallClock::get_current_time()
{
    Cyg_Scheduler::lock();

    cyg_uint32 diff = get_hw_ticks() - epoch_ticks;

    Cyg_Scheduler::unlock();

    return epoch_time_stamp + diff;
}

//-----------------------------------------------------------------------------
// Sets the value of the timestamp relative to now. This may involve
// writing to the hardware, so it may take anything up to a second to
// complete.
void Cyg_WallClock::set_current_time( cyg_uint32 time_stamp )
{
    Cyg_Scheduler::lock();

    epoch_time_stamp    = time_stamp;
    epoch_ticks         = get_hw_ticks();

    Cyg_Scheduler::unlock();
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/sh3.cxx
