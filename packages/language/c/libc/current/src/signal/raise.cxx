//========================================================================
//
//      raise.cxx
//
//      ISO C and POSIX 1003.1 signals implementation for raise()
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
// Date:          1999-02-26
// Purpose:       Provide implementation of ISO C and POSIX 1003.1 raise()
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
#define CYGPRI_LIBC_SIGNALS_RAISE_INLINE
#ifndef CYGIMP_LIBC_SIGNALS_RAISE_INLINE
# define CYGIMP_LIBC_SIGNALS_RAISE_INLINE
#endif

#define raise(a) __raise(a)

#include <signal.h>                // Main signals definitions

// SYMBOL DEFINITIONS

#undef raise

externC int raise(int __sig) CYGBLD_ATTRIB_WEAK_ALIAS(__raise);

// GLOBALS

#ifdef CYGDBG_USE_TRACING
cyg_uint8
cyg_libc_signals_raise_trace_level = CYGNUM_LIBC_SIGNALS_RAISE_TRACE_LEVEL;
#endif

#endif // ifdef CYGPKG_LIBC_SIGNALS

// EOF raise.cxx
