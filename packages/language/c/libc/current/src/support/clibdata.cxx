//========================================================================
//
//      clibdata.cxx
//
//      C library internal data support module
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
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//=========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <stdio.h>                // FILE and fopen()
#include "clibincl/stream.hxx"    // Internal per-file class
#include "clibincl/clibdata.hxx"  // Header and class definition for this
                                  // file

#ifdef CYGPKG_LIBC_STDIO
# include "clibincl/stdiosupp.hxx" // Support for stdio
#endif

// GLOBAL VARIABLES

#ifndef CYGSEM_LIBC_THREAD_SAFETY
// No thread support means use a global structure
Cyg_CLibInternalData cyg_my_clib_internal_data;
#endif // ifndef CYGSEM_LIBC_THREAD_SAFETY


#ifdef CYGPKG_LIBC_STDIO
Cyg_StdioStream *Cyg_CLibInternalData::files[FOPEN_MAX-3];
# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
Cyg_Mutex Cyg_CLibInternalData::files_lock;
# endif
#endif // ifdef CYGPKG_LIBC_STDIO

#ifdef CYGFUN_LIBC_strtok
# ifndef CYGSEM_LIBC_PER_THREAD_STRTOK
char *Cyg_CLibInternalData::strtok_last;
# endif
#endif

#ifdef CYGPKG_LIBC_RAND
# ifndef CYGSEM_LIBC_PER_THREAD_RAND
Cyg_CLibInternalData::Cyg_rand_seed_t Cyg_CLibInternalData::rand_seed;
# endif
#endif

#ifdef CYGPKG_LIBC_ENVIRONMENT

// The global environment. The default can be configured in
// <pkgconf/libc.h>
externC char *environ[];

char *environ[] = CYGDAT_LIBC_DEFAULT_ENVIRONMENT ;

#endif


// STATIC INITIALISATION

#ifdef CYGPKG_LIBC_STDIO

static Cyg_StdioStream cyg_stdio_stdin( 
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE),
    Cyg_StdioStream::CYG_STREAM_READ, false, false, _IOLBF );

static Cyg_StdioStream cyg_stdio_stdout(
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE),
    Cyg_StdioStream::CYG_STREAM_WRITE, false, false, _IOLBF );


static Cyg_StdioStream cyg_stdio_stderr(
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE),
    Cyg_StdioStream::CYG_STREAM_WRITE, false, false, _IONBF, 0, NULL );


Cyg_StdioStream *Cyg_CLibInternalData::stdin  = &cyg_stdio_stdin;
Cyg_StdioStream *Cyg_CLibInternalData::stdout = &cyg_stdio_stdout;
Cyg_StdioStream *Cyg_CLibInternalData::stderr = &cyg_stdio_stderr;

FILE *stdin  = (FILE *)&cyg_stdio_stdin;
FILE *stdout = (FILE *)&cyg_stdio_stdout;
FILE *stderr = (FILE *)&cyg_stdio_stderr;

#endif // CYGPKG_LIBC_STDIO

#endif // ifdef CYGPKG_LIBC     

// EOF clibdata.cxx
