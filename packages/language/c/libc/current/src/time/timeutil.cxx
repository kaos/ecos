//========================================================================
//
//      timeutil.cxx
//
//      ISO C date and time implementation support functions
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-26
// Purpose:       Provide support functions used by the ISO C date and time
//                implementation
// Description:   This file provides the functions:
//                  cyg_libc_time_normalize_structtm()
//                  cyg_libc_time_getzoneoffsets()
//                  cyg_libc_time_setzoneoffsets()
//                  cyg_libc_time_setdst()
//                and the globals:
//                  cyg_libc_time_day_name
//                  cyg_libc_time_day_name_len
//                  cyg_libc_time_month_name
//                  cyg_libc_time_month_name_len
//                  cyg_libc_time_month_lengths
//                  cyg_libc_time_current_dst_stat
//                  cyg_libc_time_current_std_offset
//                  cyg_libc_time_current_dst_offset
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

// define these functions as outline, not inline, functions in this file
#define CYGPRI_LIBC_TIME_GETZONEOFFSETS_INLINE extern "C"
#define CYGPRI_LIBC_TIME_SETZONEOFFSETS_INLINE extern "C"
#define CYGPRI_LIBC_TIME_SETDST_INLINE extern "C"

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <sys/timeutil.h>          // Header for this file
#include <time.h>                  // Main date and time definitions
#include <stdlib.h>                // for div()


// GLOBALS

// FIXME: PR19440 - const char & -fwritable-strings don't mix
/*const*/ char cyg_libc_time_day_name[7][10] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
};
const cyg_uint8 cyg_libc_time_day_name_len[7] = { 6, 6, 7, 9, 8, 6, 8 };

// FIXME: PR19440 - const char & -fwritable-strings don't mix
/*const*/ char cyg_libc_time_month_name[12][10] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
const cyg_uint8 cyg_libc_time_month_name_len[12] = { 7, 8, 5, 5, 3, 4,
                                                     4, 6, 9, 7, 8, 8 };

const cyg_uint8 cyg_libc_time_month_lengths[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

Cyg_libc_time_dst cyg_libc_time_current_dst_stat =
     (Cyg_libc_time_dst)CYGNUM_LIBC_TIME_DST_DEFAULT_STATE;
time_t cyg_libc_time_current_std_offset =
     (time_t)CYGNUM_LIBC_TIME_STD_DEFAULT_OFFSET;
time_t cyg_libc_time_current_dst_offset =
     (time_t)CYGNUM_LIBC_TIME_DST_DEFAULT_OFFSET;


// FUNCTIONS

////////////////////////////////////////
// cyg_libc_time_normalize_structtm() //
////////////////////////////////////////
//
// cyg_libc_time_normalize_structtm() will adjust the fields of a struct tm
// so that they are within the normal ranges expected.
// tm_wday, tm_yday and tm_isdst are ignored

externC void
cyg_libc_time_normalize_structtm( struct tm *timeptr )
{
    div_t t;

    CYG_REPORT_FUNCNAME("cyg_libc_time_normalize_structtm");
    CYG_REPORT_FUNCARG1("timeptr is at address %08x", timeptr);

    // normalize seconds to 0..59
    if ((timeptr->tm_sec < 0) || (timeptr->tm_sec > 59)) {
        t = div(timeptr->tm_sec, 60);
        while (t.rem < 0) {
            t.rem += 60;
            --t.quot;
        }
        timeptr->tm_min += t.quot;
        timeptr->tm_sec = t.rem;
    } // if

    // normalize minutes to 0..59
    if ((timeptr->tm_min < 0) || (timeptr->tm_min > 59)) {
        t = div(timeptr->tm_min, 60);
        while (t.rem < 0) {
            t.rem += 60;
            --t.quot;
        }
        timeptr->tm_hour += t.quot;
        timeptr->tm_min = t.rem;
    } // if

    // normalize hours to 0..23
    if ((timeptr->tm_hour < 0) || (timeptr->tm_hour > 23)) {
        t = div(timeptr->tm_hour, 24);
        while (t.rem < 0) {
            t.rem += 24;
            --t.quot;
        }
        timeptr->tm_mday += t.quot;
        timeptr->tm_hour = t.rem;
    } // if
    
    // we wait before normalizing tm_mday as per ISO C 7.12.2.3 (although
    // actually it only makes sense if you think about it

    // normalize months to 0..11
    if ((timeptr->tm_mon < 0) || (timeptr->tm_mon > 11)) {
        t = div(timeptr->tm_mon, 12);
        while (t.rem < 0) {
            t.rem += 12;
            --t.quot;
        }
        timeptr->tm_year += t.quot;
        timeptr->tm_mon = t.rem;
    } // if

    // now tm_mday which needs to go to 1..31
    cyg_bool leap = cyg_libc_time_year_is_leap(timeptr->tm_year);

    while (timeptr->tm_mday < 1) {
        // move back a month

        if (--timeptr->tm_mon < 0) {
            --timeptr->tm_year;
            timeptr->tm_mon = 11;
            leap = cyg_libc_time_year_is_leap(timeptr->tm_year);
        } // if

        // we move backward the number of days in the _new_ current month
        timeptr->tm_mday += cyg_libc_time_month_lengths[leap][timeptr->tm_mon];

    } // while

    while (timeptr->tm_mday >
           cyg_libc_time_month_lengths[leap][timeptr->tm_mon]) {

        // move forward a month
        
        // we move forward the number of days in the _old_ current month
        timeptr->tm_mday -= cyg_libc_time_month_lengths[leap][timeptr->tm_mon];

        if (++timeptr->tm_mon > 11) {
            ++timeptr->tm_year;
            timeptr->tm_mon = 0;
            leap = cyg_libc_time_year_is_leap(timeptr->tm_year);
        } // if
        
    } // while

    CYG_REPORT_RETURN();

} // cyg_libc_time_normalize_structtm()


// EOF timeutil.cxx
