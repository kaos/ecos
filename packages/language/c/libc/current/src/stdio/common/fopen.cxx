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
//===========================================================================
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
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Include the C library? And do we want the stdio stuff? And fopen()?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) && \
    defined(CYGPKG_LIBC_STDIO_OPEN)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <cyg/devs/common/iorb.h>   // Cyg_IORB
#include <cyg/devs/common/table.h>  // Device table
#include <stdio.h>                  // header for fopen()
#include "clibincl/stdlibsupp.hxx"  // _malloc()
#include "clibincl/stringsupp.hxx"  // _strncmp() and _strcmp()
#include "clibincl/clibdata.hxx"    // C library private data
#include "clibincl/stream.hxx"      // C library streams
#include "clibincl/stdiosupp.hxx"   // support for stdio


// EXPORTED SYMBOLS

externC FILE *
fopen( const char *, const char * ) CYGPRI_LIBC_WEAK_ALIAS("_fopen");


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
    struct Cyg_Device_Table_t *dev;
    Cyg_StdioStream *curr_stream;
    int i;
    Cyg_ErrNo err;
    Cyg_StdioStream::OpenMode open_mode;
    cyg_bool binary, append;

    CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE;

    dev = Cyg_libc_stdio_find_filename( filename );

    // if not found
    if (!dev) {
        *CYGPRI_LIBC_INTERNAL_DATA.get_errno_p() = ENOENT;
        return NULL;
    } // if

    // process_mode returns true on error
    if (process_mode( mode, &open_mode, &binary, &append )) {
        *CYGPRI_LIBC_INTERNAL_DATA.get_errno_p() = EINVAL;
        return NULL;
    } // if

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    CYGPRI_LIBC_INTERNAL_DATA.files_lock.lock();
#endif // CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS

    // find an empty slot
    for (i=0; i < FOPEN_MAX; i++) {
        curr_stream = CYGPRI_LIBC_INTERNAL_DATA.get_file_stream(i);
        if (curr_stream == NULL)
            break;
    } // for

    if (i == FOPEN_MAX) { // didn't find an empty slot
        *CYGPRI_LIBC_INTERNAL_DATA.get_errno_p() = EMFILE;

        return NULL;
    } // if

    // allocate it some memory and construct it. Currently we only have
    // potentially interactive devices, so as per ANSI 7.9.3 it must not
    // be buffered
    curr_stream = new Cyg_StdioStream( dev, open_mode, append, binary,
                                       _IONBF );

    if (curr_stream == NULL) {
        *CYGPRI_LIBC_INTERNAL_DATA.get_errno_p() = ENOMEM;
        return NULL;
    } // if


    // it puts any error in its own error flag
    if (( err=curr_stream->get_error() )) {

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
        CYGPRI_LIBC_INTERNAL_DATA.files_lock.unlock();
#endif // CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
        
        _free( curr_stream );

        *CYGPRI_LIBC_INTERNAL_DATA.get_errno_p() = err;

        return NULL;

    } // if

    CYGPRI_LIBC_INTERNAL_DATA.set_file_stream(i, curr_stream);
        
#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    CYGPRI_LIBC_INTERNAL_DATA.files_lock.unlock();
#endif // CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS

    return (FILE *)(curr_stream);

} // _fopen()
        
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) && 
       //    defined(CYGPKG_LIBC_STDIO_OPEN)

// EOF fopen.cxx
