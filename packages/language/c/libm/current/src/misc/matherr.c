//===========================================================================
//
//      matherr.c
//
//      Default matherr() error handler
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
// Description: Contains an empty default matherr() function which the user
//              can override if they want
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#if defined(CYGPKG_LIBM)

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Tracing macros

#include <math.h>                  // Main header for math library
#include "mathincl/fdlibm.h"       // Internal header for math library

int
matherr( struct exception *x ) __attribute__ ((weak));

int
matherr( struct exception *x )
{
    CYG_REPORT_FUNCNAMETYPE( "matherr", "returning %d" );

    CYG_UNUSED_PARAM( struct exception *, x );
    CYG_EMPTY_STATEMENT;

    CYG_REPORT_RETVAL( 0 );
    return 0;
} // matherr()


#endif // if defined(CYGPKG_LIBM)

// EOF matherr.c
