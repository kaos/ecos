//=======================================================================
//
//      main.cxx
//
//      Default main() function
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-26
// Purpose:       Provide a default empty main() function
// Description:   This file provides a default empty main() function so
//                that things don't fall apart if the user starts the
//                ISO C environment (the default setting) but doesn't
//                provide their own main(). This is taken advantage of
//                in some tests (for example)
// Usage:         Obviously simply override main() in your own program to
//                prevent the use of the one below
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/system.h>        // for CYGPKG_KERNEL

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support

// FUNCTION PROTOTYPES

// We provide a weakly named main to allow this to link if the user
// doesn't provide their own main. This isn't strictly good behaviour,
// but if the user wants good performance then of _course_ they should
// play with the config options and this won't be called. Or it might
// be "giving them enough rope" etc. :-)

externC int
main( int argc, char *argv[] ) CYGBLD_ATTRIB_WEAK;

externC void
cyg_user_start(void);

// FUNCTIONS

externC int
main( int argc, char *argv[] )
{
    CYG_REPORT_FUNCNAMETYPE("main", "returning %d" );

    // try to be helpful by diagnosing malformed arguments
    CYG_PRECONDITION( argv != NULL, "argv is NULL!" );
    CYG_PRECONDITION( argv[argc] == NULL, "argv[argc] isn't NULL!" );

    CYG_REPORT_FUNCARG2("argc=%d, argv[0]=%s", argc,
                        (CYG_ADDRWORD)((argv[0]==NULL) ? "NULL" : argv[0]) );

    CYG_TRACE0( true, "This is the system-supplied default main()" );

    // If the kernel isn't present then cyg_lib_invoke_main() will have assumed
    // main() is present and will call it. But if we're here, then
    // evidently the user didn't supply one - which can't be right. So we
    // assume that they have useful code to run in cyg_user_start() instead.
    // Its better than just exiting
#ifndef CYGPKG_KERNEL
    cyg_user_start();
#endif

    CYG_REPORT_RETVAL(0);
    return 0; // some CPUs have 0 hard-wired - faster than a reg
} // main()

// EOF main.cxx
