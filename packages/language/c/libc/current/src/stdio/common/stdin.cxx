//========================================================================
//
//      stdin.cxx
//
//      Initialization of stdin stream for ISO C library
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
// Date:          1999-01-15
// Purpose:       Initialization of stdin stream for ISO C library
// Description:   We put all this in a separate file in the hope that if
//                no-one uses stdin, then it is not included in the image
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // Configuration header

#ifdef CYGPKG_LIBC_STDIO

// Don't let us get the stream definitions of stdin/out/err from stdio.h
// since we are going to break the type safety :-(
#define CYGPRI_LIBC_STDIO_NO_DEFAULT_STREAMS

// And we don't need the stdio inlines, which will otherwise complain if
// stdin/out/err are not available
#ifdef CYGIMP_LIBC_STDIO_INLINES
# undef CYGIMP_LIBC_STDIO_INLINES
#endif

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include "clibincl/stream.hxx"      // Cyg_StdioStream class
#include "clibincl/stdiofiles.hxx"  // C library files
#include "clibincl/stdiosupp.hxx"   // Cyg_libc_stdio_find_filename()

// STATICS

Cyg_StdioStream
cyg_libc_stdio_stdin CYG_INIT_PRIORITY(LIBC) = Cyg_StdioStream( 
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE),
    Cyg_StdioStream::CYG_STREAM_READ, false, false, _IOLBF );


// CLASSES

// This is a dummy class just so we can execute arbitrary code when
// stdin is requested

class cyg_libc_dummy_stdin_init_class {
public:
    cyg_libc_dummy_stdin_init_class(void) { 
        Cyg_libc_stdio_files::set_file_stream(0, &cyg_libc_stdio_stdin);
    }
};

// And here's an instance of the class just to make the code run
static cyg_libc_dummy_stdin_init_class cyg_libc_dummy_stdin_init
                                         CYG_INIT_PRIORITY(LIBC);

#endif // ifdef CYGPKG_LIBC_STDIO

// EOF stdin.cxx
