//===========================================================================
//
//      memmove.cxx
//
//      ANSI standard memmove() routine
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
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_string.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>         // Compiler definitions such as size_t, NULL etc.
#include <cyg/libc/string/stringsupp.hxx> // Useful string function support and
                                          // prototypes

// EXPORTED SYMBOLS

externC void *
memmove( void *s1, const void *s2, size_t n ) \
    CYGBLD_ATTRIB_WEAK_ALIAS(__memmove);

// FUNCTIONS

void *
__memmove( void *s1, const void *s2, size_t n )
{
    char *dst = (char *)s1;
    const char *src = (const char *)s2;
    
    CYG_REPORT_FUNCNAMETYPE( "__memmove", "returning %08x" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    if (n)
    {
        CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
        CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );
    }

    if ((src < dst) && (dst < (src + n)))
    {
        // Have to copy backwards
        src += n;
        dst += n;
        while (n--)
        {
            *--dst = *--src;
        }
    }
    else
    {
        while (n--)
        {
            *dst++ = *src++;
        }
    }

    CYG_REPORT_RETVAL( s1 );
    
    return s1;
} // __memmove()

// EOF memmove.cxx
