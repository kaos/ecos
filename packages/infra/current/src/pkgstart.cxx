//========================================================================
//
//      pkgstart.cxx
//
//      General startup code for the target machine
//
//========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:       This provides generic startup code for the eCos system.
// Description:   We start the system with the entry point cyg_start()
//                which is called from the eCos HALs. This in turn invokes
//                cyg_prestart(), cyg_package_start() and cyg_user_start().
//                All these can be overriden by the user.
// Usage:         Override the defaults to use your own startup code.
//                cyg_package_start() is used to call into individual
//                packages' startup code. Alternatively leave it be and
//                use the configuration options to set the appropriate
//                compatibility environment e.g. uItron, ISO C, etc.
//                This may be automatically generated in the future.
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/infra.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Default tracing support

#ifdef CYGSEM_START_ISO_C_COMPATIBILITY
# include <sys/cstartup.h>
#endif

#ifdef CYGSEM_START_UITRON_COMPATIBILITY
# include <cyg/compat/uitron/uit_func.h>
#endif

// FUNCTION PROTOTYPES

externC void
cyg_package_start( void ) CYGBLD_ATTRIB_WEAK;

// FUNCTIONS

void
cyg_package_start( void )
{
    CYG_REPORT_FUNCTION();
    CYG_REPORT_FUNCARGVOID();

    CYG_TRACE0( true, "This is the system default cyg_package_start()" );

#ifdef CYGSEM_START_UITRON_COMPATIBILITY
    cyg_uitron_start();
#endif

#ifdef CYGSEM_START_ISO_C_COMPATIBILITY
    // This works even if the kernel is there, but if not, then it
    // calls the user main() and doesn't return until it does!
    cyg_iso_c_start();
#endif

    CYG_EMPTY_STATEMENT; // don't let it complain about doing nothing

    CYG_REPORT_RETURN();
} // cyg_package_start()

// EOF pkgstart.cxx
