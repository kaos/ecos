#ifndef CYGONCE_LIBC_STDLIB_ABS_INL
#define CYGONCE_LIBC_STDLIB_ABS_INL
/*===========================================================================
//
//      abs.inl
//
//      Inline implementations for the ISO standard utility functions
//      abs() and labs() defined in section 7.10 of the standard
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

/* INCLUDES */

#include <cyg/infra/cyg_trac.h>    /* Tracing support */

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus
extern "C" {
#endif

extern int
abs( int /* val */ ) __attribute__((__const__));

extern long
labs( long /* val */ ) __attribute__((__const__));

#ifdef __cplusplus
} /* extern "C" */
#endif 

/* INLINE FUNCTIONS */

#ifndef CYGPRI_LIBC_STDLIB_ABS_INLINE
# define CYGPRI_LIBC_STDLIB_ABS_INLINE extern __inline__
#endif

CYGPRI_LIBC_STDLIB_ABS_INLINE int
abs( int __j )
{
    int __retval;

    CYG_REPORT_FUNCNAMETYPE( "abs", "returning %d" );
   
    __retval = (__j<0) ? -__j : __j;

    CYG_REPORT_RETVAL( __retval );

    return __retval;
} /* abs() */

CYGPRI_LIBC_STDLIB_ABS_INLINE long
labs( long __j )
{
    long __retval;

    CYG_REPORT_FUNCNAMETYPE( "abs", "returning %ld" );
   
    __retval = (__j<0) ? -__j : __j;

    CYG_REPORT_RETVAL( __retval );

    return __retval;
} /* labs() */


#endif /* CYGONCE_LIBC_STDLIB_ABS_INL multiple inclusion protection */

/* EOF abs.inl */
