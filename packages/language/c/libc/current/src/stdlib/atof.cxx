//===========================================================================
//
//      atof.cxx
//
//      Real alternative for inline implementation of the ANSI standard
//      atof() utility function defined in section 7.10.1.1 of the standard
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

#include <pkgconf/libc.h>           // Configuration header

// Include the C library? And strtod() which we use?
#if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtod)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <cyg/infra/cyg_ass.h>      // Assertion support

// We don't want the inline versions of stdlib functions defined here

#ifdef CYGIMP_LIBC_STDLIB_INLINES
#undef CYGIMP_LIBC_STDLIB_INLINES
#endif

#include <stddef.h>                 // NULL, wchar_t and size_t from compiler
#include <stdlib.h>                 // Main header for stdlib functions
#include "clibincl/stdlibsupp.hxx"  // Support for stdlib functions


// EXPORTED SYMBOLS

externC double
atof( const char * ) CYGPRI_LIBC_WEAK_ALIAS("_atof");


// FUNCTIONS

double
_atof( const char *nptr )
{
    double retval;

    CYG_REPORT_FUNCNAMETYPE( "_atof", "returning %f" );

    CYG_CHECK_DATA_PTR( nptr, "nptr is an invalid pointer!" );
    
    retval = _strtod( nptr, (char **)NULL );

    CYG_REPORT_RETVAL( retval );

    return retval;
} // _atof()


#endif // if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtod)

// EOF atof.cxx
