//========================================================================
//
//      getenv.cxx
//
//      Implementation of the ISO C standard getenv() function
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-08-31
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>           // Configuration header

// Include the C library?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_ass.h>      // Tracing support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <stdlib.h>                 // Main header for stdlib functions
#include <string.h>                 // strchr(), strlen() and strncmp()
#include "clibincl/stdlibsupp.hxx"  // Support for stdlib functions


// VARIABLES

externC char **environ;   // standard definition of environ

// EXPORTED SYMBOLS

externC char *
getenv( const char *name ) CYGPRI_LIBC_WEAK_ALIAS("_getenv");


// FUNCTIONS

extern char *
_getenv( const char *name )
{
    // This function assumes the POSIX 1003.1 layout of the environment
    // in the environ variable, i.e. extern char **environ is a pointer
    // to an array of character pointers to the environment strings 
    // (POSIX 3.1.2.2 and 2.6).
    //
    // getenv() searches the environment list for a string of the form
    // "name=value" (POSIX 4.6.1). The strings must have this form
    // (POSIX 2.6)

    CYG_REPORT_FUNCNAMETYPE( "_getenv", "returning %08x" );

    CYG_PRECONDITION( name != NULL, "getenv() called with NULL string!" );

    CYG_REPORT_FUNCARG1( "name=%s", name );

    CYG_PRECONDITION( environ != NULL,
                      "environ not set i.e. environ == NULL" );

    CYG_CHECK_DATA_PTR( environ, "environ isn't a valid pointer!" );

    CYG_CHECK_DATA_PTR( name, "name isn't a valid pointer!" );

    // check name doesn't contain '='
    CYG_PRECONDITION( strchr(name, '=') == NULL,
                      "name contains '='!" );

    char **env_str_p;
    cyg_ucount32 len = strlen( name );

    for (env_str_p = environ; *env_str_p != NULL; ++env_str_p ) {

        CYG_CHECK_DATA_PTR( env_str_p,
                            "current environment string isn't valid!" );
        
        CYG_CHECK_DATA_PTR( *env_str_p,
                            "current environment string isn't valid!" );
        
        // do we have a match?
        if ( !strncmp(*env_str_p, name, len) ) {
            // but it could be just a prefix i.e. we could have
            // matched MYNAMEFRED when we're looking for just MYNAME, so:

            if ( (*env_str_p)[len] == '=' ) {
                // we got a match
                CYG_REPORT_RETVAL( *env_str_p + len + 1 );

                return (*env_str_p + len + 1);
            } // if
        } // if

        // check the next pointer isn't NULL, as we're about to dereference
        // it
        CYG_ASSERT( env_str_p+1 != NULL,
                    "environment contains a NULL pointer!" );
    } // for

    CYG_REPORT_RETVAL( NULL );
    return NULL;
} // getenv()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)

// EOF getenv.cxx
