#ifndef CYGONCE_IO_WALLCLOCK_HXX
#define CYGONCE_IO_WALLCLOCK_HXX

//==========================================================================
//
//      wallclock.hxx
//
//      Wallclock interface declaration
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          1999-03-05
// Purpose:       Wall Clock declarations
// Description:   This file defines the interface to the wall clock device
//                that provides real time stamps. Actual implementations of
//                this driver can be found in devs/wallclock.
// Usage:         #include <cyg/io/wallclock.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>

// -------------------------------------------------------------------------
// Wallclock class

class Cyg_WallClock
{

public:

    Cyg_WallClock();

    // Returns the number of seconds elapsed since 1970-01-01 00:00:00.
    // This may involve reading the hardware, so it may take anything
    // up to a second to complete.
    cyg_uint32 get_current_time();

    // Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
    // This may involve writing to the hardware, so it may take
    // anything up to a second to complete.
    void set_current_time( cyg_uint32 time_stamp );

    // A static pointer to the single system defined wall clock device.
    static Cyg_WallClock *wallclock;

private:

    // Functions implemented by the hardware-specific part of the
    // driver.

    // Note: While the API specifies elapsed time since 1970-01-01
    // 00:00:00 this is only an actual requirement if the driver is
    // used in set-get mode - in init-get mode, the upper layer of the
    // driver only needs to know the number of elapsed seconds between
    // the init_hw_seconds and get_hw_seconds calls; the hardware
    // epoch base is factored out, so the driver can use any base
    // desired (see the emulated device).

#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
    // Called to initialize the hardware clock to a known sane state.
    void init_hw_seconds( void );
#endif

    // Returns the number of seconds elapsed since 1970-01-01 00:00:00.
    // (or any desired epoch when in init-get mode)
    cyg_uint32 get_hw_seconds( void );

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    // Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
    void set_hw_seconds( cyg_uint32 secs );
#endif
};

#ifdef __CYGBLD_DRIVER_PRIVATE

// -------------------------------------------------------------------------
// Some helper functions

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

//-----------------------------------------------------------------------------
// BCD helper macros
#define TO_BCD(x) (((x/10)<<4) | (x%10))
#define TO_DEC(x) (((x>>4)*10) + (x&0xf))


// -------------------------------------------------------------------------
#endif // __CYGBLD_DRIVER_PRIVATE

#endif // ifndef CYGONCE_DEVS_WALLCLOCK_HXX
// EOF wallclock.hxx
