//========================================================================
//
//      signal.cxx
//
//      ISO C and POSIX 1003.1 signals implementation
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
// Date:          1999-02-12
// Purpose:       Provide implementation of ISO C and POSIX 1003.1 signals
// Description:   This file provides the linkable symbol version of raise()
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

#ifdef CYGPKG_LIBC_SIGNALS

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support

// The following lines allow __raise() to be included non-inline which
// actually defines it as a function here
#define CYGPRI_LIBC_SIGNALS_SIGNAL_INLINE
#ifndef CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE
# define CYGIMP_LIBC_SIGNALS_SIGNAL_INLINE
#endif

#define signal(a, b) __signal(a, b)

#include <signal.h>                // Main signals definitions

// SYMBOL DEFINITIONS

#undef signal

externC __sighandler_t signal(int __sig, __sighandler_t __handler)
    CYGBLD_ATTRIB_WEAK_ALIAS(__signal);

#endif // ifdef CYGPKG_LIBC_SIGNALS

// EOF signal.cxx
