//===========================================================================
//
//      fclose.cxx
//
//      Implementation of C library file close function as per ANSI 7.9.5.1
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-20
// Purpose:       Implements ISO C fclose() function
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header


// We can't have fclose() without fopen()
#if defined(CYGPKG_LIBC_STDIO_OPEN)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <stdio.h>                  // header for fclose()
#include <stdlib.h>                 // free()
#include <cyg/libc/stdio/stdiofiles.hxx>  // C library files
#include <cyg/libc/stdio/stream.hxx>      // C library streams

#include <cyg/libc/stdio/io.inl>     // I/O system inlines

// FUNCTIONS


externC int
fclose( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    int i;
    Cyg_ErrNo err;
    
    Cyg_libc_stdio_files::lock();

    // find the stream in the table
    for (i=0; i < FOPEN_MAX; i++)
    {
        if (real_stream == Cyg_libc_stdio_files::get_file_stream(i))
            break;
    } // for

    if (i == FOPEN_MAX) // didn't find it
    {
        errno = EBADF;

        return EOF;
    } // if

    err = real_stream->close();

    if( err != ENOERR )
    {
        errno = err;
        return EOF;
    }
    
    // FIXME: should use delete?
    // Explicitly call destructor - this flushes the output too
    real_stream->~Cyg_StdioStream();

    // and free it
    free(real_stream);

    // and mark the stream available for use
    Cyg_libc_stdio_files::set_file_stream(i, NULL);
        
    Cyg_libc_stdio_files::unlock();

    return 0;

} // fclose()
        
#endif // if defined(CYGPKG_LIBC_STDIO_OPEN)

// EOF fclose.cxx
