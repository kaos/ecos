//===========================================================================
//
//      fopen.cxx
//
//      Implementation of C library file open function as per ANSI 7.9.5.3
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:       Implements ISO C fopen() function
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Do we want the stdio stuff? And fopen()?
#if defined(CYGPKG_LIBC_STDIO) && defined(CYGPKG_LIBC_STDIO_OPEN)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <cyg/io/io.h>              // I/O system 
#include <stdio.h>                  // header for fopen()
#include "clibincl/stdlibsupp.hxx"  // _malloc()
#include "clibincl/stringsupp.hxx"  // _strncmp() and _strcmp()
#include "clibincl/stdiofiles.hxx"  // C library files
#include "clibincl/stream.hxx"      // C library streams


// EXPORTED SYMBOLS

externC FILE *
fopen( const char *, const char * ) CYGBLD_ATTRIB_WEAK_ALIAS(_fopen);


// FUNCTIONS

// process the mode string. Return true on error
static cyg_bool
process_mode( const char *mode, Cyg_StdioStream::OpenMode *rw,
              cyg_bool *binary, cyg_bool *append )
{
    *binary = *append = false; // default

    switch (mode[0]) {
    case 'r':
        *rw = Cyg_StdioStream::CYG_STREAM_READ;
        break;

    case 'w':
    case 'a':
        *rw = Cyg_StdioStream::CYG_STREAM_WRITE;
        *append = true;
        break;
        
    default:
        return true;
    } // switch

    // ANSI says additional characters may follow the sequences, that we
    // don't necessarily recognise so we just ignore them, and pretend that
    // its the end of the string

    switch (mode[1]) {
    case 'b':
        *binary = true;
        break;
    case '+':
        *rw = Cyg_StdioStream::CYG_STREAM_READWRITE;
        break;
    default:
        return false;
    } // switch

    switch (mode[2]) {
    case 'b':
        *binary = true;
        break;
    case '+':
        *rw = Cyg_StdioStream::CYG_STREAM_READWRITE;
        break;
    default:
        return false;
    } // switch
    
    return false;
} // process_mode()


externC FILE *
_fopen( const char *filename, const char *mode )
{
    cyg_io_handle_t dev;
    Cyg_StdioStream *curr_stream;
    int i;
    Cyg_ErrNo err;
    Cyg_StdioStream::OpenMode open_mode;
    cyg_bool binary, append;

    err = cyg_io_lookup( filename, &dev );

    // if not found
    if (err != ENOERR) {
        errno = ENOENT;
        return NULL;
    } // if

    // process_mode returns true on error
    if (process_mode( mode, &open_mode, &binary, &append )) {
        errno = EINVAL;
        return NULL;
    } // if

    Cyg_libc_stdio_files::lock();

    // find an empty slot
    for (i=0; i < FOPEN_MAX; i++) {
        curr_stream = Cyg_libc_stdio_files::get_file_stream(i);
        if (curr_stream == NULL)
            break;
    } // for

    if (i == FOPEN_MAX) { // didn't find an empty slot
        errno = EMFILE;
        return NULL;
    } // if

    // allocate it some memory and construct it. Currently we only have
    // potentially interactive devices, so as per ANSI 7.9.3 it must not
    // be buffered
    curr_stream = new Cyg_StdioStream( dev, open_mode, append, binary,
                                       _IONBF, 0 );

    if (curr_stream == NULL) {
        errno = ENOMEM;
        return NULL;
    } // if


    // it puts any error in its own error flag
    if (( err=curr_stream->get_error() )) {

        Cyg_libc_stdio_files::unlock();
        
        _free( curr_stream );

        errno = err;

        return NULL;

    } // if

    Cyg_libc_stdio_files::set_file_stream(i, curr_stream);
        
    Cyg_libc_stdio_files::unlock();

    return (FILE *)(curr_stream);

} // _fopen()
        
#endif // defined(CYGPKG_LIBC_STDIO) && defined(CYGPKG_LIBC_STDIO_OPEN)

// EOF fopen.cxx
