//===========================================================================
//
//      strcoll.cxx
//
//      Real function definition for ANSI string strcoll() function - not
//      that there is an inline alternative
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

// We don't want the inline versions of string functions defined here

#ifdef CYGIMP_LIBC_STRING_INLINES
#undef CYGIMP_LIBC_STRING_INLINES
#endif

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <cyg/libc/string/stringsupp.hxx> // Useful string function support and
                                          // prototypes

// EXPORTED SYMBOLS

externC int
strcoll( const char *s1, const char *s2 ) CYGBLD_ATTRIB_WEAK_ALIAS(__strcoll);


int
__strcoll( const char *s1, const char *s2 )
{
    int retval;

    CYG_REPORT_FUNCNAMETYPE( "__strcoll", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%d", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    retval = strcmp(s1, s2);

    return retval;
} // __strcoll()


// EOF strcoll.cxx
