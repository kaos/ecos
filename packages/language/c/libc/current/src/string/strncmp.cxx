//===========================================================================
//
//      strncmp.cxx
//
//      ANSI standard strncmp() routine
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

externC int 
strncmp( const char *s1, const char *s2, size_t n ) \
    CYGPRI_LIBC_WEAK_ALIAS("_strncmp");

// FUNCTIONS

int 
_strncmp( const char *s1, const char *s2, size_t n )
{
    int retval;

    CYG_REPORT_FUNCNAMETYPE( "_strncmp", "returning %d" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    if (n == 0)
        return 0;

    while (n-- != 0 && *s1 == *s2)
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }
    
    retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);

    CYG_REPORT_RETVAL( retval );

    return retval;
#else
    const CYG_WORD *aligned_s1;
    const CYG_WORD *aligned_s2;
    
    if (n == 0)
    {
        CYG_REPORT_RETVAL( 0 );
        return 0;
    } // if
    
    // If s1 or s2 are unaligned, then compare bytes.
    if (CYG_LIBC_STR_UNALIGNED2 (s1, s2))
    {  
        while (n-- != 0 && *s1 == *s2)
        {
            // If we've run out of bytes or hit a null, return zero
            // since we already know *s1 == *s2.
            if (n == 0 || *s1 == '\0')
                return 0;
            s1++;
            s2++;
        }
        retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);

        CYG_REPORT_RETVAL( retval );

        return retval;
    } //if
    
    // If s1 and s2 are word-aligned, compare them a word at a time.
    aligned_s1 = (const CYG_WORD *)s1;
    aligned_s2 = (const CYG_WORD *)s2;
    while (n >= sizeof(CYG_WORD) && *aligned_s1 == *aligned_s2)
    {
        n -= sizeof (CYG_WORD);
        
        // If we've run out of bytes or hit a null, return zero
        // since we already know *aligned_s1 == *aligned_s2.
        if (n == 0 || CYG_LIBC_STR_DETECTNULL (*aligned_s1))
        {
            CYG_REPORT_RETVAL( 0 );
            return 0;
        } // if
        
        aligned_s1++;
        aligned_s2++;
    } // while
    
    // A difference was detected in last few bytes of s1, so search bytewise
    s1 = (const char *)aligned_s1;
    s2 = (const char *)aligned_s2;
    
    while (n-- > 0 && *s1 == *s2)
    {
        // If we've run out of bytes or hit a null, return zero
        // since we already know *s1 == *s2.
        if (n == 0 || *s1 == '\0')
        {
            CYG_REPORT_RETVAL( 0 );
            return 0;
        } // if
        s1++;
        s2++;
    } // while

    retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);

    CYG_REPORT_RETVAL( retval );
    
    return retval;

#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _strncmp()

#endif // ifdef CYGPKG_LIBC     

// EOF strncmp.cxx
