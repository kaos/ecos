#ifndef CYGONCE_LIBC_SYS_TIMEUTIL_H
#define CYGONCE_LIBC_SYS_TIMEUTIL_H
//========================================================================
//
//      sys/timeutil.h
//
//      ISO C date and time implementation support functions
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-03-03
// Purpose:      Provide declarations for support functions used by the
//               ISO C date and time implementation
// Description:   
// Usage:        #include <sys/timeutil.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support

#ifdef __cplusplus
extern "C" {
#endif

// GLOBALS

// Strings of the days and month names
// FIXME: uncomment "const" when PR 19440 is fixed
extern /*const*/ char cyg_libc_time_day_name[7][10];
extern const cyg_uint8 cyg_libc_time_day_name_len[7];
extern /*const*/ char cyg_libc_time_month_name[12][10];
extern const cyg_uint8 cyg_libc_time_month_name_len[12];

// number of days in each month, defined for normal and leap years
extern const cyg_uint8 cyg_libc_time_month_lengths[2][12];


// FUNCTION PROTOTYPES

// cyg_libc_time_normalize_structtm() will adjust the fields of a struct tm
// so that they are within the normal ranges expected.
// tm_wday, tm_yday, and tm_isdst are ignored

extern void
cyg_libc_time_normalize_structtm( struct tm *__timeptr );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_LIBC_SYS_TIMEUTIL_H multiple inclusion protection

// EOF sys/timeutil.h
