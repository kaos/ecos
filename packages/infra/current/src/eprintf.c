//========================================================================
//
//      eprintf.c
//
//      __eprintf() used by libgcc
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
// Copyright (C) 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-08-21
// Purpose:       Provide __eprintf() as used by libgcc.
// Description:   libgcc calls __eprintf() to display errors and abort.
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/system.h>

#ifdef CYGPKG_ISOINFRA 
# include <pkgconf/isoinfra.h>     // Configuration header
#endif

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_ass.h>     // Default assertion
#include <cyg/infra/diag.h>

#ifdef CYGPKG_ISOINFRA
# if defined(CYGINT_ISO_STDIO_FORMATTED_IO) || \
     defined(CYGINT_ISO_STDIO_FILEACCESS)
#  include <stdio.h>
# endif
# if CYGINT_ISO_EXIT
#  include <stdlib.h>
# endif
#endif

// FUNCTIONS

__externC void
__eprintf (const char *string, const char *expression,
           unsigned int line, const char *filename)
{
#ifdef CYGINT_ISO_STDIO_FORMATTED_IO
    fprintf(stderr, string, expression, line, filename);
#else
    diag_printf(string, expression, line, filename);
#endif
#ifdef CYGINT_ISO_STDIO_FILEACCESS
    fflush (stderr);
#endif
#if CYGINT_ISO_EXIT
    abort();
#else
    CYG_FAIL( "Aborting" );
    for (;;);
#endif
} // __eprintf()

// EOF eprintf.c
