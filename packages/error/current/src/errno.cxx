//===========================================================================
//
//      errno.cxx
//
//      ISO C and POSIX error code
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          2000-04-14
// Purpose:       Provide the errno variable
// Description:   This file either provides the errno variable directly,
//                or if thread-safe, using a kernel per-thread data
//                access function
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/system.h>
#include <pkgconf/error.h>   // Configuration header

#ifdef CYGPKG_ERROR_ERRNO

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_trac.h>   // Common tracing functions
#include <cyg/error/errno.h>      // Header for this file

#ifdef CYGSEM_ERROR_PER_THREAD_ERRNO
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // per-thread data
# include <cyg/kernel/thread.inl>  // per-thread data
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// GLOBAL VARIABLES

#ifndef CYGSEM_ERROR_PER_THREAD_ERRNO

// errno is initialised to 0 at program startup - ANSI 7.1.4
Cyg_ErrNo errno = 0;

#else // ifndef CYGSEM_ERROR_PER_THREAD_ERRNO

# if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_ERROR_ERRNO_TRACE_LEVEL)
static int errno_trace = CYGNUM_ERROR_ERRNO_TRACE_LEVEL;
#  define TL1 (0 < errno_trace)
# else
#  define TL1 (0)
# endif

// FUNCTIONS

Cyg_ErrNo *
cyg_error_get_errno_p( void )
{
    Cyg_ErrNo *errno_p;

    CYG_REPORT_FUNCNAMETYPE( "cyg_error_get_errno_p", "&errno is %d");

    // set up the thread data, allocating if necessary (even though the
    // user _shouldn't_ read errno before its set, we can't stop them - and
    // ANSI prescribes it has a sensible value (0) before its set too anyway.

    Cyg_Thread *self = Cyg_Thread::self();

    errno_p = (Cyg_ErrNo *)self->get_data_ptr(CYGNUM_KERNEL_THREADS_DATA_ERRNO);

    CYG_TRACE1( TL1, "errno is %d", *errno_p );

    CYG_REPORT_RETVAL( errno_p );
    
    // return the internal data's errno
    return errno_p;
} // cyg_error_get_errno_p()

#endif // ifdef CYGSEM_ERROR_PER_THREAD_ERRNO

#endif // ifdef CYGPKG_ERROR_ERRNO

// EOF errno.cxx
