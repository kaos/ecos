//========================================================================
//
//      atexit.cxx
//
//      Implementation of the atexit() function
//
//========================================================================
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

#include <pkgconf/libc.h>   // Configuration header

// Include atexit() ?
#ifdef CYGFUN_LIBC_ATEXIT

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions

// SYMBOLS

externC int
atexit( Cyg_libc_atexit_fn_t ) CYGBLD_ATTRIB_WEAK_ALIAS(_atexit);


// STATICS

// cyg_libc_atexit_handlers contains the functions to call.
// cyg_libc_atexit_handlers_count contains the number of valid handlers
// or you can consider the next free handler slot in
// cyg_libc_atexit_handlers.

static Cyg_libc_atexit_fn_t
cyg_libc_atexit_handlers[ CYGNUM_LIBC_ATEXIT_HANDLERS ];

static cyg_ucount32 cyg_libc_atexit_handlers_count;


// FUNCTIONS

externC void
cyg_libc_invoke_atexit_handlers( void )
{
    CYG_REPORT_FUNCNAME( "cyg_libc_invoke_atexit_handlers");
    CYG_REPORT_FUNCARGVOID();
    
    cyg_ucount32 i;

    for (i=cyg_libc_atexit_handlers_count; i>0; --i) {

        CYG_TRACE1( true,
                    "Calling function registered with atexit at addr %08x",
                    cyg_libc_atexit_handlers[i-1] );
        CYG_CHECK_FUNC_PTR( cyg_libc_atexit_handlers[i-1],
                            "Function to call in atexit handler list "
                            "isn't valid! Even though it was when "
                            "entered!" );

        (*cyg_libc_atexit_handlers[i-1])();

    } // for
        
    CYG_REPORT_RETURN();
} // cyg_libc_invoke_atexit_handlers()


externC int
_atexit( Cyg_libc_atexit_fn_t func_to_register )
{
    CYG_REPORT_FUNCNAMETYPE( "_atexit", "returning %d" );
    CYG_REPORT_FUNCARG1XV( func_to_register );

    CYG_CHECK_FUNC_PTR( func_to_register,
                       "_atexit() not passed a valid function argument!" );

    // have we any slots left?
    if (cyg_libc_atexit_handlers_count >=
        sizeof(cyg_libc_atexit_handlers)/sizeof(Cyg_libc_atexit_fn_t) ) {

        CYG_REPORT_RETVAL( 1 ); 
        return 1; // failure
    } // if

    cyg_libc_atexit_handlers[cyg_libc_atexit_handlers_count++] =
        func_to_register;

    CYG_REPORT_RETVAL(0);
    return 0;
} // _atexit()


#endif // ifdef CYGFUN_LIBC_ATEXIT

// EOF atexit.cxx
