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
// Contributors:  
// Date:          2000-04-20
// Purpose:       Initialization of stderr stream for ISO C library
// Description:   We put all this in a separate file in the hope that if
//                no-one uses stderr, then it is not included in the image
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>          // Configuration header

// Don't let us get the stream definitions of stdin/out/err from stdio.h
// since we are going to break the type safety :-(
#define CYGPRI_LIBC_STDIO_NO_DEFAULT_STREAMS

// And we don't need the stdio inlines, which will otherwise complain if
// stdin/out/err are not available
#ifdef CYGIMP_LIBC_STDIO_INLINES
# undef CYGIMP_LIBC_STDIO_INLINES
#endif

// INCLUDES

#include <cyg/infra/cyg_type.h>           // Common type definitions and support
#include <cyg/libc/stdio/stream.hxx>      // Cyg_StdioStream class
#include <cyg/libc/stdio/stdiofiles.hxx>  // C library files
#include <cyg/libc/stdio/stdiosupp.hxx>   // Cyg_libc_stdio_find_filename()

// CONSTANTS

// add 2 to the priority to allow it to be the fd 2 if necessary
#define PRIO (CYG_INIT_LIBC+2)

// STATICS

Cyg_StdioStream
cyg_libc_stdio_stderr CYGBLD_ATTRIB_INIT_PRI(PRIO) = Cyg_StdioStream(
    Cyg_libc_stdio_find_filename(CYGDAT_LIBC_STDIO_DEFAULT_CONSOLE,
                                 Cyg_StdioStream::CYG_STREAM_WRITE,
                                 false, false),
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
                                             CYGBLD_ATTRIB_INIT_PRI(PRIO);

// and finally stderr itself
__externC Cyg_StdioStream * const stderr;
Cyg_StdioStream * const stderr=&cyg_libc_stdio_stderr;

// EOF stderr.cxx
