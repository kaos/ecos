//===========================================================================
//
//      strcmp.cxx
//
//      ANSI standard strcmp() routine
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
strcmp( const char *s1, const char *s2 ) CYGPRI_LIBC_WEAK_ALIAS("_strcmp");

// FUNCTIONS

int
_strcmp( const char *s1, const char *s2 )
{ 
    CYG_REPORT_FUNCNAMETYPE( "_strcmp", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    int retval;

    CYG_UNUSED_PARAM( int, retval ); // in case tracing is off

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    
    retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);

    CYG_REPORT_RETVAL( retval );

    return retval;
#else
    const CYG_WORD *aligned_s1;
    const CYG_WORD *aligned_s2;
    
    // If s1 or s2 are unaligned, then compare bytes.
    if (CYG_LIBC_STR_UNALIGNED2 (s1, s2))
    {  
        while (*s1 != '\0' && *s1 == *s2)
        {
            s1++;
            s2++;
        }
        retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);
        
        CYG_REPORT_RETVAL( retval );
        
        return retval;
    } 
    
    // If s1 and s2 are word-aligned, compare them a word at a time.
    aligned_s1 = (const CYG_WORD *)s1;
    aligned_s2 = (const CYG_WORD *)s2;
    while (*aligned_s1 == *aligned_s2)
    {
        // To get here, *aligned_s1 == *aligned_s2, thus if we find a
        // null in *aligned_s1, then the strings must be equal, so return
        // zero.
        if (CYG_LIBC_STR_DETECTNULL (*aligned_s1))
        {
            CYG_REPORT_RETVAL( 0 );
            return 0;
        } // if
        
        aligned_s1++;
        aligned_s2++;
    }
    
    // A difference was detected in last few bytes of s1, so search bytewise
    s1 = (const char *)aligned_s1;
    s2 = (const char *)aligned_s2;
    
    while (*s1 != '\0' && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    retval = (*(unsigned char *) s1) - (*(unsigned char *) s2);
    
    CYG_REPORT_RETVAL( retval );
        
    return retval;

#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _strcmp()

#endif // ifdef CYGPKG_LIBC     

// EOF strcmp.cxx
