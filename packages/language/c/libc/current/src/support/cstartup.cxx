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
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>           // C library configuration

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

// let the main thread be global so people can play with it (e.g. suspend
// or resume etc.) if that's what they want to do
#ifdef CYGSEM_LIBC_STARTUP_MAIN_THREAD
extern Cyg_Thread cyg_libc_main_thread;
#endif

// FUNCTIONS

externC void
cyg_iso_c_start( void )
{
    CYG_REPORT_FUNCNAME( "cyg_iso_c_start" );
    CYG_REPORT_FUNCARGVOID();
    
#ifdef CYGSEM_LIBC_STARTUP_MAIN_THREAD
    CYG_TRACE0( true, "Resuming cyg_libc_main_thread" );
    cyg_libc_main_thread.resume();
#else
    cyg_libc_invoke_main(0);
#endif

    CYG_REPORT_RETURN();
} // cyg_iso_c_start()

// EOF cstartup.cxx
