//=========================================================================
//
//      exit.cxx
//
//      Implementation of the exit() function
//
//=========================================================================
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
//=========================================================================
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
//=========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <stdio.h>                 // fflush()
#include <errno.h>                 // errno
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions

// SYMBOLS

externC void
exit( int ) CYGBLD_ATTRIB_WEAK_ALIAS(__libc_exit) CYGBLD_ATTRIB_NORET;

// FUNCTIONS

externC void
__libc_exit( int status )
{
    CYG_REPORT_FUNCTION(); // shouldn't return, but CYG_FAIL will catch it
    CYG_REPORT_FUNCARG1DV( status );
    
    // Strictly the only thing exit() does is invoke the atexit handlers
    // and flush stdio buffers. Anything else is for _exit() (aka
    // __libc__exit() within the implementation)

#ifdef CYGFUN_LIBC_ATEXIT
    // we start with the atexit handlers
    cyg_libc_invoke_atexit_handlers();
#endif

#ifdef CYGSEM_LIBC_EXIT_CALLS_FFLUSH

    int rc;

    CYG_TRACE0( true, "Calling fflush( NULL )" );
    rc = fflush( NULL );

    CYG_TRACE2( rc != 0, "fflush() returned non-zero. It returned %d and "
                "errno indicates the error: %s", rc, strerror(errno) );
#endif

    __libc__exit( status );

    CYG_FAIL( "__libc_exit() returning!!!" );

    CYG_REPORT_RETURN();
} // __libc_exit()


#endif // ifdef CYGPKG_LIBC     

// EOF exit.cxx
