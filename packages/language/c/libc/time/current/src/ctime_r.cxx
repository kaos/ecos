//========================================================================
//
//      ctime_r.cxx
//
//      POSIX 1003.1 date and time implementation for ctime_r()
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
// Purpose:       Provide implementation of POSIX 1003.1 ctime()
// Description:   This file provides the linkable symbol version of ctime_r()
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_time.h>          // C library configuration

// The following lines allow ctime_r() to be included non-inline which
// actually defines it as a function here
#define CYGPRI_LIBC_TIME_CTIME_R_INLINE
#ifndef CYGIMP_LIBC_TIME_CTIME_R_INLINE
# define CYGIMP_LIBC_TIME_CTIME_R_INLINE
#endif

#include <time.h>                  // Main date and time definitions

// EOF ctime_r.cxx
