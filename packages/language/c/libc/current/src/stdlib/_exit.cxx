//========================================================================
//
//      _exit.cxx
//
//      _exit() as from POSIX 1003.1 section 3.2.2 "Terminate a process"
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
// Purpose:       Provides POSIX 1003.1 _exit() function
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // eCos kernel for thread termination
# include <cyg/kernel/thread.inl>  // and
# include <cyg/kernel/sched.hxx>   // for stopping the scheduler
# include <cyg/kernel/sched.inl>
#endif


// SYMBOLS

externC void
_exit( int ) CYGBLD_ATTRIB_WEAK_ALIAS(__libc__exit) CYGBLD_ATTRIB_NORET;

// FUNCTIONS

externC void
__libc__exit( int status )
{
    CYG_REPORT_FUNCTION(); // shouldn't return, but CYG_FAIL will catch it
    CYG_REPORT_FUNCARG1DV( status );
    
#ifdef CYGPKG_KERNEL

# ifdef CYGSEM_LIBC_EXIT_STOPS_SYSTEM

    Cyg_Scheduler::lock(); // prevent rescheduling

    for (;;)
        CYG_EMPTY_STATEMENT;

# else

    Cyg_Thread::exit();

# endif
#endif

    // loop forever
    for (;;)
        CYG_EMPTY_STATEMENT;

    CYG_FAIL( "__libc__exit() returning!!!" );

    CYG_REPORT_RETURN();
} // _exit()


// EOF _exit.cxx
