#ifndef CYGONCE_LIBC_STDLIB_ATOX_INL
#define CYGONCE_LIBC_STDLIB_ATOX_INL
/*===========================================================================
//
//      atox.inl
//
//      Inline implementations for the ISO standard utility functions
//      atoi(), atol() and atof() defined in section 7.10 of the standard
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-04-28
// Purpose:     
// Description: 
// Usage:        Do not include this file directly - include <stdlib.h> instead
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

/* CONFIGURATION */

#include <pkgconf/libc_stdlib.h>    /* Configuration header */

/* INCLUDES */

#include <stddef.h>                 /* NULL */
#include <cyg/infra/cyg_trac.h>     /* Tracing support */

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CYGFUN_LIBC_strtod
extern double
atof( const char * /* double_str */ );
#endif

extern int
atoi( const char * /* int_str */ );

extern long
atol( const char * /* long_str */ );

#ifdef CYGFUN_LIBC_strtod
extern double
strtod( const char * /* double_str */, char ** /* endptr */ );
#endif

extern long
strtol( const char * /* long_str */, char ** /* endptr */,
        int /* base */ );

extern unsigned long
strtoul( const char * /* ulong_str */, char ** /* endptr */,
         int /* base */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

/* INLINE FUNCTIONS */

/* 7.10.1 String conversion functions */

#ifndef CYGPRI_LIBC_STDLIB_ATOX_INLINE
# define CYGPRI_LIBC_STDLIB_ATOX_INLINE extern __inline__
#endif


#ifdef CYGFUN_LIBC_strtod
CYGPRI_LIBC_STDLIB_ATOX_INLINE double
atof( const char *__nptr )
{
    double __retval;

    CYG_REPORT_FUNCNAMETYPE( "atof", "returning %f" );

    CYG_CHECK_DATA_PTR( __nptr, "__nptr is an invalid pointer!" );
    
    __retval = strtod( __nptr, (char **)NULL );

    CYG_REPORT_RETVAL( __retval );

    return __retval;
} /* atof() */
#endif

CYGPRI_LIBC_STDLIB_ATOX_INLINE int
atoi( const char *__nptr )
{
    int __retval;

    CYG_REPORT_FUNCNAMETYPE( "atoi", "returning %d" );

    CYG_CHECK_DATA_PTR( __nptr, "__nptr is an invalid pointer!" );
    
    __retval = (int)strtol( __nptr, (char **)NULL, 10 );

    CYG_REPORT_RETVAL( __retval );

    return __retval;
} /* atoi() */


CYGPRI_LIBC_STDLIB_ATOX_INLINE long
atol( const char *__nptr )
{
    long __retval;

    CYG_REPORT_FUNCNAMETYPE( "atol", "returning %ld" );

    CYG_CHECK_DATA_PTR( __nptr, "__nptr is an invalid pointer!" );
    
    __retval = strtol( __nptr, (char **)NULL, 10 );

    CYG_REPORT_RETVAL( __retval );

    return __retval;
} /* atol() */

#endif /* CYGONCE_LIBC_STDLIB_ATOX_INL multiple inclusion protection */

/* EOF atox.inl */
