//===========================================================================
//
//      strncat.cxx
//
//      ANSI standard strncat() routine
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
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>         // Compiler definitions such as size_t, NULL etc.
#include "clibincl/stringsupp.hxx" // Useful string function support and
                                   // prototypes

// EXPORTED SYMBOLS

externC char *
strncat( char *s1, const char *s2, size_t n ) \
    CYGPRI_LIBC_WEAK_ALIAS("_strncat");

// FUNCTIONS

char *
_strncat( char *s1, const char *s2, size_t n)
{
    CYG_REPORT_FUNCNAMETYPE( "_strncat", "returning %08x" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    char *s = s1;
    
    while (*s1)
        s1++;
    while (n-- != 0 && (*s1++ = *s2++))
    {
        if (n == 0)
            *s1 = '\0';
    }
    
    CYG_REPORT_RETVAL( s );

    return s;
#else
    char *s = s1;
    
    // Skip over the data in s1 as quickly as possible.
    if (!CYG_LIBC_STR_UNALIGNED (s1))
    {
        CYG_WORD *aligned_s1 = (CYG_WORD *)s1;
        while (!CYG_LIBC_STR_DETECTNULL (*aligned_s1))
            aligned_s1++;
        
        s1 = (char *)aligned_s1;
    }
    
    while (*s1)
        s1++;
    
    // s1 now points to the its trailing null character, now copy
    // up to N bytes from S2 into S1 stopping if a NULL is encountered
    // in S2.
    // 
    // It is not safe to use strncpy here since it copies EXACTLY N
    // characters, NULL padding if necessary.

    while (n-- != 0 && (*s1++ = *s2++))
    {
        if (n == 0)
            *s1 = '\0';
    }
    
    CYG_REPORT_RETVAL( s );

    return s;
#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _strncat()

#endif // ifdef CYGPKG_LIBC     

// EOF strncat.cxx
