#ifndef CYGONCE_LIBC_TIME_H
#define CYGONCE_LIBC_TIME_H
//===========================================================================
//
//      time.h
//
//      Date and time routines from ISO C section 7.12 and POSIX 1003.1
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-02-25
// Purpose:      Provide definitions required by ISO C section 7.12 and
//               POSIX 1003.1 8.3.4-8.3.7
// Description: 
// Usage:        #include <time.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <stddef.h>                // <time.h> is required to provide NULL
                                   // and size_t, so we get them from compiler

// CONSTANTS

// What to divide clock_t by to get seconds (para 7.12.1)
// According to CAE XSH, Issue 4, V2, CLOCKS_PER_SEC must be 1 million on all
// XSI-conformant systems
#define CLOCKS_PER_SEC 1000000

// TYPE DEFINITIONS

// Define clock_t as per ISO C para 7.12.1
// We make it very big since CLOCKS_PER_SEC is so large - this should last
// for 584942 years :-) An int32 would only last a little over an hour :-(
typedef cyg_int64 clock_t;

// Define time_t as per ISO C para 7.12.1
// In the implementation we give it the meaning of seconds since the
// Epoch (00:00:00 GMT, 1970-01-01), but don't rely on this - always use
// the type opaquely
// A 32-bit value will overflow in the year 2038. Hopefully we will have
// moved forward from 32-bit CPUs by then, so making it a cyg_count* type
// may suffice ;-)
typedef cyg_count32 time_t;
#define __time_t_defined // FIXME: this is temporary to stop newlib's
                         // <sys/types.h> complaining. This should be resolved
                         // properly and a new <sys/types.h> provided

// Define struct tm as per ISO C para 7.12.1
struct tm {
    int tm_sec;      // seconds after the minute - [0..61] 
                     //   (note 61 allows for two leap seconds)
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

// The following struct is used by the implementation-defined functions
// to manipulate the Daylight Savings Time state
typedef enum {
    CYG_LIBC_TIME_DSTNA  = -1,
    CYG_LIBC_TIME_DSTOFF =  0,
    CYG_LIBC_TIME_DSTON  =  1
} Cyg_libc_time_dst;

// FUNCTION PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================
//
// POSIX 1003.1 functions

#ifdef CYGFUN_LIBC_TIME_POSIX

/////////////////////////////////
// asctime_r() - POSIX.1 8.3.4 //
/////////////////////////////////
//
// This returns a textual representation of a struct tm, and writes
// the string to return into __buf
//

extern char *
asctime_r( const struct tm *__timeptr, char *__buf );

///////////////////////////////
// ctime_r() - POSIX.1 8.3.5 //
///////////////////////////////
//
// This returns the equivalent of ctime() but writes to __buf
// to store the returned string
//

extern char *
ctime_r( const time_t *__timer, char *__buf );

////////////////////////////////
// gmtime_r() - POSIX.1 8.3.6 //
////////////////////////////////
//
// This converts a time_t into a struct tm expressed in UTC, and stores
// the result in the space occupied by __result
//

extern struct tm *
gmtime_r( const time_t *__timer, struct tm *__result );

///////////////////////////////////
// localtime_r() - POSIX.1 8.3.7 //
///////////////////////////////////
//
// This converts a time_t into a struct tm expressed in local time, and
// stores the result in the space occupied by __result
//

extern struct tm *
localtime_r( const time_t *__timer, struct tm *__result );


#endif // ifdef CYGFUN_LIBC_TIME_POSIX

//===========================================================================

// ISO C functions

// Time manipulation functions - ISO C 7.12.2

//////////////////////////////
// clock() - ISO C 7.12.2.1 //
//////////////////////////////
//
// Returns processor time used in "clock"s. For a single process,
// multi-threaded system this will just be the time since booting
//

extern clock_t
clock( void );

/////////////////////////////////
// difftime() - ISO C 7.12.2.2 //
/////////////////////////////////
//
// This returns (__time1 - __time0) in seconds
//

extern double
difftime( time_t __time1, time_t __time0 );

///////////////////////////////
// mktime() - ISO C 7.12.2.3 //
///////////////////////////////
//
// This converts a "struct tm" to a "time_t"
//

extern time_t
mktime( struct tm *__timeptr );

/////////////////////////////
// time() - ISO C 7.12.2.4 //
/////////////////////////////
//
// This returns calendar time as a time_t
//

extern time_t
time( time_t *__timer );

// Time conversion functions - ISO C 7.12.3

////////////////////////////////
// asctime() - ISO C 7.12.3.1 //
////////////////////////////////
//
// This returns a textual representation of a struct tm
//

extern char *
asctime( const struct tm *__timeptr );

//////////////////////////////
// ctime() - ISO C 7.12.3.2 //
//////////////////////////////
//
// This returns asctime(localtime(__timeptr))
//

extern char *
ctime( const time_t *__timer );

///////////////////////////////
// gmtime() - ISO C 7.12.3.3 //
///////////////////////////////
//
// This converts a time_t into a struct tm expressed in UTC
//

extern struct tm *
gmtime( const time_t *__timer );

//////////////////////////////////
// localtime() - ISO C 7.12.3.4 //
//////////////////////////////////
//
// This converts a time_t into a struct tm expressed in local time
//

extern struct tm *
localtime( const time_t *__timer );

/////////////////////////////////
// strftime() - ISO C 7.12.3.5 //
/////////////////////////////////
//
// This converts a string using format specifiers that signify various
// time-related quantities
//

extern size_t
strftime( char *__s, size_t __maxsize, const char *__format,
          const struct tm *__timeptr) CYGPRI_LIBC_ATTRIB_FORMAT_STRFTIME;

//===========================================================================

// Implementation-specific functions

////////////////////////////////////
// cyg_libc_time_setzoneoffsets() //
////////////////////////////////////
//
// This function sets the offsets used when Daylight Savings Time is enabled
// or disabled. The offsets are in time_t's
//

extern void
cyg_libc_time_setzoneoffsets( time_t __stdoffset, time_t __dstoffset );

////////////////////////////
// cyg_libc_time_setdst() //
////////////////////////////
//
// This function sets the state of Daylight Savings Time: on, off, or unknown
//

extern void
cyg_libc_time_setdst( Cyg_libc_time_dst __state );


////////////////////////////////////
// cyg_libc_time_getzoneoffsets() //
////////////////////////////////////
//
// This function retrieves the current state of the Daylight Savings Time
// and the offsets of both STD and DST
// The offsets are both in time_t's
//

extern Cyg_libc_time_dst
cyg_libc_time_getzoneoffsets( time_t *__stdoffset, time_t *__dstoffset );

/////////////////////////////
// cyg_libc_time_settime() //
/////////////////////////////
//
// This function sets the current time for the system. The time is
// specified as a time_t in UTC. It returns non-zero on error.
//

extern cyg_bool
cyg_libc_time_settime( time_t __utctime );


#ifdef __cplusplus
} // extern "C"
#endif

#include <time.inl>

#endif // CYGONCE_LIBC_TIME_H multiple inclusion protection

// EOF time.h
