//========================================================================
//
//      stdiofiles.cxx
//
//      ISO C library stdio central file data
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
// Purpose:       Allocate storage for central file data object
// Description:   This file allocates the actual objects used by the
//                Cyg_libc_stdio_files class defined in
//                "clibincl/stdiofiles.hxx"
// Usage:       
//
//####DESCRIPTIONEND####
//
//=========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

#ifdef CYGPKG_LIBC_STDIO

// INCLUDES

#include <stddef.h>                // NULL
#include "clibincl/stream.hxx"     // Cyg_StdioStream
#include "clibincl/stdiofiles.hxx" // Class definition for
                                   // Cyg_libc_stdio_files

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
# include <cyg/infra/cyg_type.h>   // CYG_INIT_PRIORITY
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif


// GLOBAL VARIABLES

Cyg_StdioStream *Cyg_libc_stdio_files::files[FOPEN_MAX] = { NULL };

# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
Cyg_Mutex Cyg_libc_stdio_files::files_lock CYG_INIT_PRIORITY(LIBC);
# endif

#endif // ifdef CYGPKG_LIBC_STDIO

// EOF stdiofiles.cxx
