//========================================================================
//
//      asctime.cxx
//
//      ISO C date and time implementation for asctime()
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-26
// Purpose:       Provide implementation of ISO C asctime()
// Description:   This file provides the linkable symbol version of asctime()
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support

// The following lines allow __asctime() to be included non-inline which
// actually defines it as a function here
#define CYGPRI_LIBC_TIME_ASCTIME_INLINE
#ifndef CYGIMP_LIBC_TIME_ASCTIME_INLINE
# define CYGIMP_LIBC_TIME_ASCTIME_INLINE
#endif

#include <time.h>                  // Main date and time definitions

// GLOBALS

// 26 isn't such a magic value if you work out what asctime is returning
// It should do until 23:59:59 31st Dec 9999 anyway :-)
char cyg_libc_time_asctime_buf[26];

// SYMBOL DEFINITIONS

#undef asctime

externC char *
asctime( const struct tm *__timeptr ) CYGBLD_ATTRIB_WEAK_ALIAS(__asctime);

// EOF asctime.cxx
