#ifndef CYGONCE_ISO_STRING_H
#define CYGONCE_ISO_STRING_H
/*========================================================================
//
//      string.h
//
//      ISO string functions
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
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides the string macros, types and functions
//                required by ISO C and POSIX 1003.1. It may also provide
//                other compatibility functions like BSD-style string
//                functions
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <string.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get NULL and size_t from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#define __need_size_t
#include <stddef.h>

#if CYGINT_ISO_STRERROR
# include CYGBLD_ISO_STRERROR_HEADER
#endif

/* memcpy() and memset() are special as they are used internally by
 * the compiler, so they *must* exist. So if no-one claims to implement
 * them, prototype them anyway */

#ifdef CYGBLD_ISO_MEMCPY_HEADER
# include CYGBLD_ISO_MEMCPY_HEADER
#else

extern 
# ifdef __cplusplus
"C"
# endif
void *
memcpy( void *, const void *, size_t );

#endif

#ifdef CYGBLD_ISO_MEMSET_HEADER
# include CYGBLD_ISO_MEMSET_HEADER
#else

extern 
# ifdef __cplusplus
"C"
# endif
void *
memset( void *, int, size_t );

#endif

#if CYGINT_ISO_STRTOK_R
# include CYGBLD_ISO_STRTOK_R_HEADER
#endif

#if CYGINT_ISO_STRING_LOCALE_FUNCS
# include CYGBLD_ISO_STRING_LOCALE_FUNCS_HEADER
#endif

#if CYGINT_ISO_STRING_BSD_FUNCS
# include CYGBLD_ISO_STRING_BSD_FUNCS_HEADER
#endif

#if CYGINT_ISO_STRING_MEMFUNCS
# include CYGBLD_ISO_STRING_MEMFUNCS_HEADER
#endif

#if CYGINT_ISO_STRING_STRFUNCS
# include CYGBLD_ISO_STRING_STRFUNCS_HEADER
#endif

#endif /* CYGONCE_ISO_STRING_H multiple inclusion protection */

/* EOF string.h */
