#ifndef CYGONCE_LIBC_TIME_H
#define CYGONCE_LIBC_TIME_H
//===========================================================================
//
//      time.h
//
//      ANSI standard time routines defined in section 7.12 of the standard
//
//===========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include <time.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>  // ECC common type definitions and support
#include <stddef.h>              // NULL and size_t from compiler

// CONSTANTS

// What to divide clock_t by to get seconds (para 7.12.1)
#define CLOCKS_PER_SEC 1000

// TYPE DEFINITIONS

// as per para 7.12.1
typedef cyg_int64 clock_t;

typedef cyg_count32 time_t;

struct tm {
    int tm_sec;      // seconds after the minute - [0..61]
    int tm_min;      // minutes after the hour - [0..59]
    int tm_hour;     // hours since midnight - [0..23]
    int tm_mday;     // days of the month - [1..31]
    int tm_mon;      // months since January - [0..11]
    int tm_year;     // years since 1900
    int tm_wday;     // days since Sunday - [0..6]
    int tm_yday;     // days since January 1 - [0..365]
    int tm_isdst;    // Daylight Saving Time flag - positive if DST is in
                     // effect, 0 if not in effect, and negative if the info
                     // is not available
};

// FUNCTION PROTOTYPES

//===========================================================================

// 7.12.2 Time manipulation functions

externC clock_t
clock( void );

#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_TIME_H multiple inclusion protection

// EOF time.h
