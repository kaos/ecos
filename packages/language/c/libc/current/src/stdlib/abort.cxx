//===========================================================================
//
//      abort.cxx
//
//      Implementation of the abort() function
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-18
// Purpose:       Implement the ISO C abort() function from 7.10.4.1
// Description:   This implements abort() simply by raising SIGABRT as
//                required by ISO C
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h> // Common type definitions and support
#include <cyg/infra/cyg_trac.h> // Tracing support

#include <stdlib.h>             // Header for all stdlib functions
                                // (like this one)
#ifdef CYGPKG_LIBC_SIGNALS
# include <signal.h>
#endif

// FUNCTIONS

externC void
__abort( void )
{
    CYG_REPORT_FUNCNAME( "__abort" );
    
#ifdef CYGPKG_LIBC_SIGNALS
    int rc;
    
    rc = raise(SIGABRT);
    
    CYG_TRACE1(1, "raise(SIGABRT) returned!!! rc=%d", rc);

    CYG_FAIL("raise(SIGABRT) returned");

#endif

    // ISO C clearly says that abort() cannot return to its caller

    // loop forever
    for (;;)
        CYG_EMPTY_STATEMENT;

    CYG_REPORT_RETURN();
} // abort()

// EXPORTED SYMBOLS

externC void abort(void) CYGBLD_ATTRIB_WEAK_ALIAS(__abort);

// EOF abort.cxx
