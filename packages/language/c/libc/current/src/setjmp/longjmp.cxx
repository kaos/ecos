//===========================================================================
//
//      longjmp.cxx
//
//      ANSI standard longjmp() function
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour@cygnus.co.uk
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: Implements ANSI standard non-local jump function longjmp()
//              per ANSI para 7.6.2.1. This is the "real" alternative to
//              the inline version of longjmp()
//             
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Common tracing code
#include <cyg/infra/cyg_ass.h>     // Common assertion code
#include <cyg/hal/hal_arch.h>      // HAL architecture specific implementation

// We don't want the inline version of longjmp() defined here
#ifdef CYGIMP_LIBC_SETJMP_INLINES
# undef CYGIMP_LIBC_SETJMP_INLINES
#endif

#include <setjmp.h>                // Header for setjmp/longjmp
#include "clibincl/setjmpsupp.hxx" // Support for setjmp/longjmp functions


// EXPORTED SYMBOLS

externC void
longjmp( jmp_buf cyg_buf, int cyg_val) CYGPRI_LIBC_WEAK_ALIAS("_longjmp")
                                       CYGPRI_LIBC_NORETURN;


// INLINE FUNCTIONS

void
_longjmp( jmp_buf cyg_buf, int cyg_val)
{
    CYG_REPORT_FUNCNAME( "longjmp" );
    CYG_REPORT_FUNCARG2( "&cyg_buf=%08x, cyg_val=%d", &cyg_buf, cyg_val );

    // ANSI says that if we are passed cyg_val==0, then we change it to 1
    if (cyg_val == 0)
        ++cyg_val;

    // we let the HAL do the work

    HAL_REORDER_BARRIER(); // prevent any chance of optimisation re-ordering
    hal_longjmp( cyg_buf, cyg_val );
    HAL_REORDER_BARRIER(); // prevent any chance of optimisation re-ordering

#ifdef CYGDBG_USE_ASSERTS
    CYG_ASSERT( 0, "longjmp should not have reached this point!" );
#else
    for (;;)
        CYG_EMPTY_STATEMENT;
#endif
} // _longjmp()


#endif // ifdef CYGPKG_LIBC     

// EOF longjmp.cxx
