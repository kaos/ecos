//===========================================================================
//
//      setvbuf.cxx
//
//      Implementation of C library buffering setup as per ANSI 7.9.5.6
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
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


// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <stdio.h>                  // header for setvbuf()
#include "clibincl/stream.hxx"      // C libray streams
#include "clibincl/stdiosupp.hxx"   // support for stdio


// EXPORTED SYMBOLS

externC int
setvbuf( FILE *, char *, int, size_t ) CYGPRI_LIBC_WEAK_ALIAS("_setvbuf");


// FUNCTIONS


externC int
_setvbuf( FILE *stream, char *buf, int mode, size_t size )
{
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    
#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    if ( !real_stream->lock_me() ) {
        errno = EBADF;
        return EBADF;
    } // if
#endif

    err = real_stream->io_buf.set_buffer( (cyg_ucount32) size, 
                                          (cyg_uint8 *) buf );
    if (!err) {
        switch (mode) {
        case _IONBF:
            CYG_ASSERT( (size == 0) && (buf == NULL),
                        "No buffering wanted but size/address specified!" );
            real_stream->flags.buffering = false;
            real_stream->flags.line_buffering = false;
            break;
        case _IOLBF:
            real_stream->flags.buffering = true;
            real_stream->flags.line_buffering = true;
            break;
        case _IOFBF:
            real_stream->flags.buffering = true;
            real_stream->flags.line_buffering = false;
            break;
        default:
            err = EINVAL;
            break;
        } // switch
    } // if

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    real_stream->unlock_me();
#endif
    
    if (err) {
        errno = err;
        return err;
    } // if
        
    return 0;

#else // ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

    errno = ENOSUPP;
    return ENOSUPP;

#endif // ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

} // _setvbuf()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF setvbuf.cxx
