//=======================================================================
//
//      cstartup.cxx
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
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/system.h>         // CYGPKG_KERNEL
#include <pkgconf/libc_startup.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_trac.h>     // Common tracing support

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>        // eCos kernel configuration
# include <cyg/kernel/thread.hxx>   // eCos thread support
# include <cyg/kernel/thread.inl>
#endif


// FUNCTION PROTOTYPES

externC void
cyg_libc_invoke_main( CYG_ADDRWORD );

// EXTERNS

#ifdef CYGSEM_LIBC_STARTUP_MAIN_THREAD
extern Cyg_Thread cyg_libc_main_thread;

// FUNCTIONS

externC void
cyg_iso_c_start( void )
{
    static int initialized=0;

    CYG_REPORT_FUNCNAME( "cyg_iso_c_start" );
    CYG_REPORT_FUNCARGVOID();

    if (initialized++ == 0) {
        CYG_TRACE0( true, "Resuming cyg_libc_main_thread" );
        cyg_libc_main_thread.resume();
    }
    CYG_REPORT_RETURN();

} // cyg_iso_c_start()

// define an object that will automatically call cyg_iso_c_start()

class cyg_libc_startup_dummy_constructor_class {
public:
    cyg_libc_startup_dummy_constructor_class() { cyg_iso_c_start(); }
};

static cyg_libc_startup_dummy_constructor_class cyg_libc_startup_obj
                                  CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_LIBC);

#elif defined( CYGSEM_LIBC_STARTUP_MAIN_INITCONTEXT )

// otherwise replace the default cyg_user_start(), but of course keep
// it a weak symbol in case the user wants to override

externC void
cyg_user_start(void) CYGBLD_ATTRIB_WEAK;

externC void
cyg_user_start(void)
{
    cyg_libc_invoke_main(0);
}

externC void
cyg_iso_c_start( void )
{
    static int initialized=0;

    CYG_REPORT_FUNCNAME( "cyg_iso_c_start" );
    CYG_REPORT_FUNCARGVOID();

    // In case they want to explicitly invoke the C library from
    // cyg_user_start() themselves
    if (initialized++ == 0) {
        cyg_libc_invoke_main(0);
    }
    CYG_REPORT_RETURN();
}
#else

externC void
cyg_iso_c_start( void ) {}

#endif

// EOF cstartup.cxx
