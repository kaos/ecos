//===========================================================================
//
//      strrchr.cxx
//
//      ANSI standard strrchr() routine
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

externC char *
strrchr( const char *s, int c ) CYGPRI_LIBC_WEAK_ALIAS("_strrchr");

// FUNCTIONS

char *
_strrchr( const char *s, int c )
{
    const char *last = NULL;
    const char charc = c;
    
    CYG_REPORT_FUNCNAMETYPE( "_strrchr", "returning %08x" );
    CYG_REPORT_FUNCARG2( "s=%08x, c=%d", s, c );

    CYG_CHECK_DATA_PTR( s, "s is not a valid pointer!" );

    while (*s)
    {
        if (*s == charc)
        {
            last = s;
        }
        s++;
    }
    
    if (*s == charc)
    {
        last = s;
    }

    CYG_REPORT_RETVAL( last );
    
    return (char *) last;
} // _strrchr()

#endif // ifdef CYGPKG_LIBC     

// EOF strrchr.cxx
