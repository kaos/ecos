//===========================================================================
//
//      strchr.cxx
//
//      ANSI standard strchr() routine
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
strchr( const char *s, int c ) CYGPRI_LIBC_WEAK_ALIAS("_strchr");

// MACROS

// DETECTCHAR returns nonzero if X contains the byte used 
// to fill MASK.
#define DETECTCHAR(X,MASK) (CYG_LIBC_STR_DETECTNULL( (X) ^ (MASK) ))

// FUNCTIONS

char *
_strchr( const char *s, int c )
{
    CYG_REPORT_FUNCNAMETYPE( "_strchr", "returning %08x" );
    CYG_REPORT_FUNCARG2( "s=%08x, c=%d", s, c );

    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    char charc = c;
    
    while (*s && *s != charc)
    {
        s++;
    }
    
    if (*s != charc)
    {
        s = NULL;
    }
    
    CYG_REPORT_RETVAL( s );

    return (char *) s;
#else
    char charc = c;
    CYG_WORD mask;
    CYG_WORD j;
    CYG_WORD *aligned_addr;
    
    if (CYG_LIBC_STR_UNALIGNED (s))
    {
        while (*s && *s != charc)
            s++;
        if (*s != charc)
            s = NULL;

        CYG_REPORT_RETVAL( s );

        return (char *) s;
    }
    
    for (j = 0, mask = 0; j < sizeof(mask); j++)
        mask = (mask << 8) + charc;
    
    aligned_addr = (CYG_WORD *)s;
    while (!CYG_LIBC_STR_DETECTNULL (*aligned_addr) &&
           !DETECTCHAR (*aligned_addr, mask))
        aligned_addr++;
    
    // The block of bytes currently pointed to by aligned_addr
    // contains either a null or the target char, or both.  We
    // catch it using the bytewise search.
    
    s = (char *)aligned_addr;
    while (*s && *s != charc) 
        s++;
    if (*s != charc)
        s = NULL;

    CYG_REPORT_RETVAL( s );

    return (char *)s;
#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _strchr()

#endif // ifdef CYGPKG_LIBC     

// EOF strchr.cxx
