//===========================================================================
//
//      memchr.cxx
//
//      ANSI standard memchr() routine
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
#include <stddef.h>           // Compiler definitions such as size_t, NULL etc.
#include "clibincl/stringsupp.hxx" // Useful string function support and
                                   // prototypes

// EXPORTED SYMBOLS

externC void *
memchr( const void *s, int c, size_t n ) CYGPRI_LIBC_WEAK_ALIAS("_memchr");

// FUNCTIONS

void *
_memchr( const void *s, int c, size_t n )
{
    CYG_REPORT_FUNCNAMETYPE( "_memchr", "returning addr %08x" );
    CYG_REPORT_FUNCARG3( "s=%08x, c=%d, n=%d", s, c, n );

    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );

#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) || defined(__OPTIMIZE_SIZE__)
    const unsigned char *src = (const unsigned char *) s;
    
    c &= 0xff;

    while (n--)
    {
        if (*src == c)
        {
            CYG_REPORT_RETVAL( src );
            return (void *) src;
        } // if
        src++;
    }
    CYG_REPORT_RETVAL( NULL );
    return NULL;
#else
    const unsigned char *src = (const unsigned char *) s;
    CYG_WORD *aligned_src;
    CYG_WORD buffer;
    CYG_WORD mask;
    cyg_ucount8 i;

    c &= 0xff;
    
    // If the size is small, or src is unaligned, then 
    // use the bytewise loop.  We can hope this is rare.
    if (CYG_LIBC_STR_OPT_TOO_SMALL (n) || CYG_LIBC_STR_UNALIGNED (src)) 
    {
        while (n--)
        {
            if (*src == c)
            {
                CYG_REPORT_RETVAL( src );
                return (void *) src;
            } // if
            src++;
        }
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    }
    
    // The fast code reads the ASCII one word at a time and only 
    // performs the bytewise search on word-sized segments if they
    // contain the search character, which is detected by XORing 
    // the word-sized segment with a word-sized block of the search
    // character and then detecting for the presence of NULL in the
    // result.

    aligned_src = (CYG_WORD *) src;
    mask = 0;
    for (i = 0; i < CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE; i++)
        mask = (mask << 8) + c;
    
    while (n > CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE)
    {
        buffer = *aligned_src;
        buffer ^=  mask;
        if (CYG_LIBC_STR_DETECTNULL (buffer))
        {
            src = (unsigned char*) aligned_src;
            for ( i = 0; i < CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE; i++ )
            {
                if (*src == c)
                {
                    CYG_REPORT_RETVAL( src );
                    return (void *) src;
                } // if
                src++;
            }
        }
        n -= CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE;
        aligned_src++;
    }
    
    // If there are fewer than CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE characters
    // left, then we resort to the bytewise loop.
    
    src = (const unsigned char *) aligned_src;
    while (n--)
    {
        if (*src == c)
        {
            CYG_REPORT_RETVAL( src );
            return (void *) src;
        } // if
        src++;
    } 

    CYG_REPORT_RETVAL( NULL );
    return NULL;
#endif // not defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST) ||
       //     defined(__OPTIMIZE_SIZE__)
} // _memchr()

#endif // ifdef CYGPKG_LIBC     

// EOF memchr.cxx
