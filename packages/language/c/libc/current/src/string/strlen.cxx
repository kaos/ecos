//===========================================================================
//
//      strlen.cxx
//
//      ANSI standard strlen() routine
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
// Author(s):   jlarmour
// Contributors:  jlarmour
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

externC size_t
strlen( const char *s ) CYGPRI_LIBC_WEAK_ALIAS("_strlen");

// FUNCTIONS

size_t
_strlen( const char *s )
{
    int retval;
    
    CYG_REPORT_FUNCNAMETYPE( "_strlen", "returning length %d" );
    CYG_REPORT_FUNCARG1( "s=%08x", s );

    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    const char *start = s;
    
    while (*s)
        s++;
    
    retval = s - start;

    CYG_REPORT_RETVAL( retval );

    return retval;

#else

    const char *start = s;
    CYG_WORD *aligned_addr;
    
    if (CYG_LIBC_STR_UNALIGNED (s))
    {
        while (*s)
            s++;
        retval = s - start;

        CYG_REPORT_RETVAL( retval );
    
        return retval;
    }
    
    // If the string is word-aligned, we can check for the presence of 
    // a null in each word-sized block.
    
    aligned_addr = (CYG_WORD *)s;
    while (!CYG_LIBC_STR_DETECTNULL (*aligned_addr))
        aligned_addr++;
    
    // Once a null is detected, we check each byte in that block for a
    // precise position of the null.
    s = (char*)aligned_addr; 
    while (*s)
        s++;
    retval = s - start;

    CYG_REPORT_RETVAL( retval );
    
    return retval;
#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _strlen()

#endif // ifdef CYGPKG_LIBC     

// EOF strlen.cxx
