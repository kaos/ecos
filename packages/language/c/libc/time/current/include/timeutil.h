#ifndef CYGONCE_LIBC_TIME_TIMEUTIL_H
#define CYGONCE_LIBC_TIME_TIMEUTIL_H
//========================================================================
//
//      timeutil.h
//
//      ISO C date and time implementation support functions
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-03-03
// Purpose:      Provide declarations for support functions used by the
//               ISO C date and time implementation
// Description:   
// Usage:        #include <cyg/libc/time/timeutil.h>
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
// FIXME: should comment out "const" in multi-dimensional arrays until
// PR 19440 is fixed
extern const char cyg_libc_time_day_name[7][10];
extern const cyg_uint8 cyg_libc_time_day_name_len[7];
extern const char cyg_libc_time_month_name[12][10];
extern const cyg_uint8 cyg_libc_time_month_name_len[12];

// number of days in each month, defined for normal and leap years
extern const cyg_uint8 cyg_libc_time_month_lengths[2][12];


// FUNCTION PROTOTYPES

////////////////////////////////////////
// cyg_libc_time_normalize_structtm() //
////////////////////////////////////////
//
// cyg_libc_time_normalize_structtm() will adjust the fields of a struct tm
// so that they are within the normal ranges expected.
// tm_wday, tm_yday, and tm_isdst are ignored

extern void
cyg_libc_time_normalize_structtm( struct tm *__timeptr );

//////////////////////////
// cyg_libc_time_itoa() //
//////////////////////////
//
// This converts num to a string and puts it into s padding with
// "0"'s if padzero is set, or spaces otherwise if necessary.
// The number of chars written to s is returned
//

extern cyg_uint8
cyg_libc_time_itoa( cyg_uint8 *__s, cyg_int32 __num, cyg_uint8 __width,
                    cyg_bool __padzero );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_LIBC_TIME_TIMEUTIL_H multiple inclusion protection

// EOF timeutil.h
