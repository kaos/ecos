//=======================================================================
//
//      invokemain.cxx
//
//      Support for startup of ISO C environment
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
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-30
// Purpose:       Provide entry point for thread which then calls main()
// Description:   cyg_libc_invoke_main() is used as the entry point for
//                the thread object that is created to call the
//                user-supplied main() function. It sets up the arguments
//                (if any) and invokes exit() if main() returns
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_startup.h>  // Configuration header
#include <pkgconf/isoinfra.h>

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <stdlib.h>                // exit()

// EXTERNS

externC cyg_bool cyg_hal_stop_constructors;

// FUNCTION PROTOTYPES

externC int
main( int argc, char *argv[] );

externC void
cyg_hal_invoke_constructors(void);

externC void
pthread_exit( void *value_ptr );

// STATICS

#ifdef CYGSEM_LIBC_INVOKE_DEFAULT_STATIC_CONSTRUCTORS
class cyg_libc_dummy_constructor_class {
public:
    cyg_libc_dummy_constructor_class(void) { ++cyg_hal_stop_constructors; }
};

static cyg_libc_dummy_constructor_class cyg_libc_dummy_constructor_obj
                                  CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_PREDEFAULT);
#endif

// FUNCTIONS

externC void
cyg_libc_invoke_main( CYG_ADDRWORD )
{
    CYG_REPORT_FUNCNAME( "cyg_libc_invoke_main" );
    CYG_REPORT_FUNCARG1( "argument is %s", "ignored" );

#ifdef CYGSEM_LIBC_INVOKE_DEFAULT_STATIC_CONSTRUCTORS
    // finish invoking constructors that weren't called by default
    cyg_hal_invoke_constructors();
#endif

    // argv[argc] must be NULL according to the ISO C standard 5.1.2.2.1
    char *temp_argv[] = CYGDAT_LIBC_ARGUMENTS ; 
    int rc;
    
    rc = main( (sizeof(temp_argv)/sizeof(char *)) - 1, &temp_argv[0] );

    CYG_TRACE1( true, "main() has returned with code %d. Calling exit()",
                rc );

#if CYGINT_ISO_PTHREAD_IMPL > 0
    // It is up to pthread_exit() to call exit() if needed
    pthread_exit( (void *)rc );
    CYG_FAIL( "pthread_exit() returned!!!" );
#else
    exit(rc);
    CYG_FAIL( "exit() returned!!!" );
#endif

    CYG_REPORT_RETURN();
    
} // cyg_libc_invoke_main()

// EOF invokemain.cxx
