//===========================================================================
//
//      errno.cxx
//
//      ANSI error code
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
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
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

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_trac.h>   // Common tracing functions
#include <errno.h>                // Header for this file

#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO
# include "clibincl/clibdata.hxx"  // C library internal data
#endif

// GLOBAL VARIABLES

#ifndef CYGSEM_LIBC_PER_THREAD_ERRNO

// errno is initialised to 0 at program startup - ANSI 7.1.4
Cyg_ErrNo errno = 0;

#else // ifndef CYGSEM_LIBC_PER_THREAD_ERRNO

# if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_ERRNO_TRACE_LEVEL)
static int errno_trace = CYGNUM_LIBC_ERRNO_TRACE_LEVEL;
#  define TL1 (0 < errno_trace)
# else
#  define TL1 (0)
# endif

// FUNCTIONS

Cyg_ErrNo *
cyg_get_errno_p( void )
{
    Cyg_ErrNo *errno_p;

    CYG_REPORT_FUNCNAMETYPE( "cyg_get_errno_p", "&errno is %d");

    // set up the thread data, allocating if necessary (even though the
    // user _shouldn't_ read errno before its set, we can't stop them - and
    // ANSI prescribes it has a sensible value (0) before its set too anyway.

    CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE;

    errno_p = CYGPRI_LIBC_INTERNAL_DATA.get_errno_p();

    CYG_TRACE1( TL1, "errno is %d", *errno_p );

    CYG_REPORT_RETVAL( errno_p );
    
    // return the internal data's errno
    return errno_p;
} // cyg_get_errno_p()


#endif // ifndef CYGSEM_LIBC_PER_THREAD_ERRNO

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF errno.cxx
