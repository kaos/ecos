#ifndef CYGONCE_LIBC_SIGNAL_INL
#define CYGONCE_LIBC_SIGNAL_INL
//========================================================================
//
//      signal.inl
//
//      Inline functions for implementation of ISO C and POSIX signals
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
// Date:          1999-02-17
// Purpose:       Implements required inline functions of ISO C and
//                POSIX 1003.1 signals
// Description:   
// Usage:         Do not include this file directly, instead use <signal.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

#ifdef CYGPKG_LIBC_SIGNALS

// INCLUDES

#include <signal.h>                // Header for this file, just in case
#include <cyg/infra/cyg_ass.h>     // Assertion infrastructure
#include <cyg/infra/cyg_trac.h>    // Tracing infrastructure

// GLOBALS

extern __sighandler_t cyg_libc_signal_handlers[];
#ifdef CYGDBG_USE_TRACING
extern cyg_uint8 cyg_libc_signals_raise_trace_level;
#endif

// DEFINES

// The following are overriden by the libc implementation to get a non-inline
// version to prevent duplication of code
#ifndef CYGPRI_LIBC_SIGNALS_RAISE_INLINE
# define CYGPRI_LIBC_SIGNALS_RAISE_INLINE extern __inline__
#endif

#ifndef CYGPRI_LIBC_SIGNALS_SIGNAL_INLINE
# define CYGPRI_LIBC_SIGNALS_SIGNAL_INLINE extern __inline__
#endif

// FUNCTION PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

// Default signal handler - SIG_DFL
extern void cyg_libc_signals_default_handler(int __sig);

#ifdef CYGSEM_LIBC_SIGNALS_THREAD_SAFE
extern cyg_bool cyg_libc_signals_lock_do_lock(void);
extern void cyg_libc_signals_lock_do_unlock(void);
#endif

// INLINE FUNCTIONS

/////////////////////////////
// cyg_libc_signals_lock() //
/////////////////////////////

static inline cyg_bool
cyg_libc_signals_lock(void)
{
#ifdef CYGSEM_LIBC_SIGNALS_THREAD_SAFE
    return cyg_libc_signals_lock_do_lock();
#else
    return true;
#endif
} // cyg_libc_signals_lock()

///////////////////////////////
// cyg_libc_signals_unlock() //
///////////////////////////////

static inline void
cyg_libc_signals_unlock(void)
{
#ifdef CYGSEM_LIBC_SIGNALS_THREAD_SAFE
    cyg_libc_signals_lock_do_unlock();
#else
    return;
#endif
} // cyg_libc_signals_unlock()

// ISO C functions

//////////////////////////////
// signal() - ISO C 7.7.1   //
//////////////////////////////


#ifdef CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE

#ifdef CYGSEM_LIBC_SIGNALS_SIGNAL_SETS_ERRNO
# include <errno.h>                 // errno
#endif

CYGPRI_LIBC_SIGNALS_SIGNAL_INLINE __sighandler_t
signal(int __sig, __sighandler_t __handler)
{
    __sighandler_t __old_handler;
    CYG_REPORT_FUNCNAMETYPE( "__signal", "returning %08x" );
    
    CYG_REPORT_FUNCARG2( "signal number = %d, requested handler is at %08x",
                         __sig, __handler );

    // check valid signal - raise should not raise the null signal either
    if ( (__sig >= CYGNUM_LIBC_SIGNALS) || (__sig <= 0) ) {

#ifdef CYGSEM_LIBC_SIGNALS_BAD_SIGNAL_FATAL
        CYG_FAIL("__signal() passed bad signal number");
#else
# ifdef CYGSEM_LIBC_SIGNALS_SIGNAL_SETS_ERRNO
        errno = EINVAL;
# endif
        return SIG_ERR;
#endif
    }

    // paranoia
    CYG_CHECK_DATA_PTR( cyg_libc_signal_handlers,
                        "signal handler array is invalid!" );
    if ( (__handler != SIG_IGN) && (__handler != SIG_DFL) )
        CYG_CHECK_FUNC_PTR( __handler, "__signal() passed invalid handler");
    
    if (!cyg_libc_signals_lock()) {
#ifdef CYGSEM_LIBC_SIGNALS_SIGNAL_SETS_ERRNO
        errno = EINTR;
#endif
        return SIG_ERR;
    } // if

    __old_handler = cyg_libc_signal_handlers[__sig];
    cyg_libc_signal_handlers[__sig] = __handler;
    
    cyg_libc_signals_unlock();

    CYG_REPORT_RETVAL( __old_handler );
   
    return __old_handler;
} // signal()
#endif // ifdef CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE


///////////////////////////
// raise() - ISO C 7.7.2 //
///////////////////////////

#ifdef CYGIMP_LIBC_SIGNALS_RAISE_INLINE

#ifdef CYGSEM_LIBC_SIGNALS_RAISE_SETS_ERRNO
# include <errno.h>                 // errno
#endif

CYGPRI_LIBC_SIGNALS_RAISE_INLINE int
raise(int __sig)
{
    int __ret=0;
    __sighandler_t __sigfun;
    
    CYG_REPORT_FUNCNAMETYPE( "__raise", "returning %d" );
    
    CYG_REPORT_FUNCARG1( "signal number = %d", __sig );

    // check valid signal - raise should not raise the null signal either
    if ( (__sig >= CYGNUM_LIBC_SIGNALS) || (__sig <= 0) ) {

#ifdef CYGSEM_LIBC_SIGNALS_BAD_SIGNAL_FATAL
        CYG_FAIL("__raise() passed bad signal number");
#else
# ifdef CYGSEM_LIBC_SIGNALS_RAISE_SETS_ERRNO
        errno = EINVAL;
# endif
        return -1;
#endif
    }

    // paranoia
    CYG_CHECK_DATA_PTR( cyg_libc_signal_handlers,
                        "signal handler array is invalid!" );
    
    if (!cyg_libc_signals_lock()) {
#ifdef CYGSEM_LIBC_SIGNALS_RAISE_SETS_ERRNO
        errno = EINTR;
#endif
        return -1;
    } // if

    __sigfun = cyg_libc_signal_handlers[__sig];

    switch ( (CYG_ADDRESS)__sigfun ) {
        
    case (CYG_ADDRESS)SIG_DFL:
        CYG_TRACE0(cyg_libc_signals_raise_trace_level,
                   "signal handler returned is SIG_DFL");
        cyg_libc_signals_unlock();
        cyg_libc_signals_default_handler(__sig);
        break;

    case (CYG_ADDRESS)SIG_IGN:
        CYG_TRACE0(cyg_libc_signals_raise_trace_level,
                   "signal handler returned is SIG_IGN");
        cyg_libc_signals_unlock();

        break;

    default:
        CYG_TRACE1(cyg_libc_signals_raise_trace_level,
                   "signal handler returned is at %08x", __sigfun);
        // call the signal handler directly
        cyg_libc_signal_handlers[__sig] = SIG_DFL;

        cyg_libc_signals_unlock();
        CYG_CHECK_FUNC_PTR( __sigfun, "returned signal handler invalid!");

        (*__sigfun)(__sig);
        break;
    }

    CYG_REPORT_RETVAL( __ret );

    return __ret;
} // raise()
#endif // ifdef CYGIMP_LIBC_SIGNALS_RAISE_INLINE


#ifdef __cplusplus
} // extern "C"
#endif 

#endif // ifdef CYGPKG_LIBC_SIGNALS

#endif // CYGONCE_LIBC_SIGNAL_INL multiple inclusion protection

// EOF signal.inl
