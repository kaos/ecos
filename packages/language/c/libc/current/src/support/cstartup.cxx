//=======================================================================
//
//      cstartup.cxx
//
//      Starting C library support
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-08-27
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>       // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing support
#include <cyg/infra/cyg_ass.h>     // Common assertion support
#include "clibincl/stdlibsupp.hxx" // __libc_exit() ( i.e. exit() )

#ifdef CYGPKG_KERNEL
# include <cyg/kernel/thread.hxx>   // eCos thread support
# include <cyg/kernel/thread.inl>
#endif


// FUNCTION PROTOTYPES

// We provide a weakly named main to allow this to link if the user
// doesn't provide their own main. This isn't strictly good behaviour,
// but if the user wants good performance then of _course_ they should
// play with the config options and this won't be called. Or it might
// be "giving them enough rope" etc. :-)

// The definition of the normal user-supplied main() is here aliased
// to a dummy main function. It isn't called main itself in order to
// throw GDB off the scent, which otherwise gets confused. These
// attributes have no effect on a user-supplied main() if one is supplied

externC int
cyg_libc_dummy_main( int argc, char *argv[] );

externC int
main( int argc, char *argv[] )
    CYGPRI_LIBC_WEAK_ALIAS("cyg_libc_dummy_main");

static inline void
invoke_main( CYG_ADDRWORD );

// STATICS

#ifdef CYGPKG_KERNEL
static cyg_uint8 main_stack[ CYGNUM_LIBC_MAIN_STACK_SIZE ];


// GLOBALS

// let the main thread be global so people can play with it (e.g. suspend
// or resume etc.) if that's what they want to do
Cyg_Thread cyg_libc_main_thread( &invoke_main, (CYG_ADDRWORD) 0,
                                 CYGNUM_LIBC_MAIN_STACK_SIZE,
                                 (CYG_ADDRESS) &main_stack );
#endif


// FUNCTIONS

externC int
cyg_libc_dummy_main( int argc, char *argv[] )
{
    CYG_REPORT_FUNCNAMETYPE("main", "returning %d" );

    // try to be helpful by diagnosing malformed arguments
    CYG_PRECONDITION( argv != NULL, "argv is NULL!" );
    CYG_PRECONDITION( argv[argc] == NULL, "argv[argc] isn't NULL!" );

    CYG_REPORT_FUNCARG2("argc=%d, argv[0]=%s", argc,
                        (argv[0]==NULL) ? "NULL" : argv[0] );

    CYG_TRACE0( true, "This is the system-supplied default main()" );

    CYG_REPORT_RETVAL(0);
    return 0; // some CPUs have 0 hard-wired - faster than a reg
}

// Making invoke_main() static inline has no effect if the kernel is
// present, and makes it inline if not! Cor.
static inline void
invoke_main( CYG_ADDRWORD )
{
    CYG_REPORT_FUNCNAME( "invoke_main" );
    CYG_REPORT_FUNCARG1( "argument is %s", "ignored" );

    // argv[argc] must be NULL according to the ISO C standard 5.1.2.2.1
    char *temp_argv[] = CYGDAT_LIBC_ARGUMENTS ; 
    int rc;
    
    rc = main( (sizeof(temp_argv)/sizeof(char *)) - 1, &temp_argv[0] );

    CYG_TRACE1( true, "main() has returned with code %d. Calling exit()",
                rc );

    __libc_exit(rc);

    CYG_FAIL( "__libc_exit() returned!!!" );

    CYG_REPORT_RETURN();
    
} // invoke_main();

externC void
cyg_iso_c_start( void )
{
    CYG_REPORT_FUNCNAME( "cyg_iso_c_start" );
    CYG_REPORT_FUNCARGVOID();
    
#ifdef CYGPKG_KERNEL
    CYG_TRACE0( true, "Resuming cyg_libc_main_thread" );
    cyg_libc_main_thread.resume();
#else
    invoke_main(0);
#endif

    CYG_REPORT_RETURN();
} // cyg_iso_c_start()


#endif // ifdef CYGPKG_LIBC

// EOF cstartup.cxx
