//========================================================================
//
//      stderr.cxx
//
//      Initialization of stderr stream for ISO C library
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-15
// Purpose:       Initialization of stderr stream for ISO C library
// Description:   We put all this in a separate file in the hope that if
//                no-one uses stderr, then it is not included in the image
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
cyg_libc_stdio_stderr CYG_INIT_PRIORITY(LIBC) = Cyg_StdioStream( 
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE),
    Cyg_StdioStream::CYG_STREAM_WRITE, false, false, _IONBF, 0, NULL );

// CLASSES

// This is a dummy class just so we can execute arbitrary code when
// stderr is requested

class cyg_libc_dummy_stderr_init_class {
public:
    cyg_libc_dummy_stderr_init_class(void) { 
        Cyg_libc_stdio_files::set_file_stream(2, &cyg_libc_stdio_stderr);
    }
};

// And here's an instance of the class just to make the code run
static cyg_libc_dummy_stderr_init_class cyg_libc_dummy_stderr_init
                                         CYG_INIT_PRIORITY(LIBC);

#endif // ifdef CYGPKG_LIBC_STDIO

// EOF stderr.cxx
