#ifndef CYGONCE_IO_WALLCLOCK_INL
#define CYGONCE_IO_WALLCLOCK_INL

//==========================================================================
//
//      wallclock.inl
//
//      Wallclock internal helper functions
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
// Date:          2000-05-26
// Purpose:       Wall Clock internal helper functions
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>          // assertions

// -------------------------------------------------------------------------
// Some helper functions

#define is_leap(_y_) (((0==(_y_)%4 && 0!=(_y_)%100) || 0==(_y_)%400) ? 1 : 0)
#define year_days(_y_) (is_leap(_y_) ? 366 : 365)

static cyg_int32 days_per_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

#ifndef time_t
#define time_t cyg_uint32
#endif

static time_t
_simple_mktime(cyg_uint32 year, cyg_uint32 mon,
               cyg_uint32 day, cyg_uint32 hour,
               cyg_uint32 min, cyg_uint32 sec)
{
    time_t secs;
    cyg_uint32 y, m, days;

    CYG_ASSERT(year <= 3124, "Year is unreasonably large");
    CYG_ASSERT(mon <= 12, "Month is invalid");
    CYG_ASSERT(day <= 31, "Day is invalid");
    CYG_ASSERT(hour <= 23, "Hour is invalid");
    CYG_ASSERT(min <= 59, "Minutes is invalid");
    CYG_ASSERT(sec <= 61, "Seconds is invalid");

    // Number of days due to years
    days = 0;
    for (y = 1970; y < year; y++)
        days += year_days(y);

    // Due to months
    for (m = 0; m < mon-1; m++)
        days += days_per_month[is_leap(year)][m];
    // Add days
    days += day - 1;

    // Add hours, minutes, and seconds
    secs = ((days * 24 + hour) * 60 + min) * 60 + sec;

    return secs;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE


static void
_simple_mkdate(time_t time,
               cyg_uint32* year, cyg_uint32* mon,
               cyg_uint32* day, cyg_uint32* hour,
               cyg_uint32* min, cyg_uint32* sec)
{
    cyg_int32 days, hms, y, m, *dpm;

    days = (cyg_int32) (time / (24*60*60));
    hms  = (cyg_int32) (time % (24*60*60));

    // Nothing fancy about the time - no leap year magic involved
    *sec = hms % 60;
    *min = (hms % (60*60)) / 60;
    *hour = hms / (60*60);

    // Find year
    for (y = 1970; days >= year_days(y); y++)
        days -= year_days(y);
    *year = y;
    dpm = &days_per_month[is_leap(y)][0];

    // Find month
    for (m = 0; days >= dpm[m]; m++)
        days -= dpm[m];
    m++;
    *mon = m;

    *day = days+1;
}

#endif

//-----------------------------------------------------------------------------
// BCD helper macros
#define TO_BCD(x) (((x/10)<<4) | (x%10))
#define TO_DEC(x) (((x>>4)*10) + (x&0xf))

#endif // ifndef CYGONCE_DEVS_WALLCLOCK_INL
// EOF wallclock.inl
