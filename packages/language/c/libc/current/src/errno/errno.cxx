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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-20
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

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_trac.h>   // Common tracing functions
#include <errno.h>                // Header for this file

#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO
# include <pkgconf/kernel.h>       // kernel configuration
# include <cyg/kernel/thread.hxx>  // per-thread data
# include <cyg/kernel/thread.inl>  // per-thread data
# include <cyg/kernel/mutex.hxx>   // mutexes
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

// STATICS

static cyg_ucount32 errno_data_index=CYGNUM_KERNEL_THREADS_DATA_MAX;
static Cyg_Mutex errno_data_mutex CYG_INIT_PRIORITY(LIBC);

// FUNCTIONS

Cyg_ErrNo * const
cyg_libc_get_errno_p( void )
{
    Cyg_ErrNo *errno_p;

    CYG_REPORT_FUNCNAMETYPE( "cyg_libc_get_errno_p", "&errno is %d");

    // set up the thread data, allocating if necessary (even though the
    // user _shouldn't_ read errno before its set, we can't stop them - and
    // ANSI prescribes it has a sensible value (0) before its set too anyway.

    Cyg_Thread *self = Cyg_Thread::self();

    // Get a per-thread data slot if we haven't got one already
    // Do a simple test before locking and retrying test, as this is a
    // rare situation
    if (CYGNUM_KERNEL_THREADS_DATA_MAX==errno_data_index) {
        errno_data_mutex.lock();
        if (CYGNUM_KERNEL_THREADS_DATA_MAX==errno_data_index) {

            // the kernel just throws an assert if this doesn't work
            // FIXME: Should use real CDL to pre-allocate a slot at compile
            // time to ensure there are enough slots
            errno_data_index = self->new_data_index();
            
            // errno is initialised to 0 at program startup - ANSI 7.1.4
            self->set_data(errno_data_index, 0);
        }
        errno_data_mutex.unlock();
    } // if

    // we have a valid index now

    errno_p = (Cyg_ErrNo *)self->get_data_ptr(errno_data_index);

    CYG_TRACE1( TL1, "errno is %d", *errno_p );

    CYG_REPORT_RETVAL( errno_p );
    
    // return the internal data's errno
    return errno_p;
} // cyg_libc_get_errno_p()

#endif // ifdef CYGSEM_LIBC_PER_THREAD_ERRNO

// EOF errno.cxx
