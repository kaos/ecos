//========================================================================
//
//      environ.cxx
//
//      C library definition and allocation of POSIX 1003.1 environ
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
// Purpose:       Provide extern char **environ as required by POSIX 1003.1
// Description:   C library definition and allocation of POSIX 1003.1 environ
// Usage:         You can override environ at startup if you wish simply
//                by redefining yourself. Setting the libc.h configuration
//                option is the intended method though.
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support

#ifdef CYGPKG_LIBC_ENVIRONMENT

// GLOBALS

// The global environment. The default can be configured in
// <pkgconf/libc.h>

static char * const
cyg_libc_default_environ[] = CYGDAT_LIBC_DEFAULT_ENVIRONMENT; 

char **environ CYGBLD_ATTRIB_WEAK = (char **)cyg_libc_default_environ;

#endif

// EOF environ.cxx
