//===========================================================================
//
//      gets.cxx
//
//      ANSI Stdio gets() function
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

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include <limits.h>                 // UINT_MAX
#include "clibincl/stdiosupp.hxx"   // Support functions for stdio
#include "clibincl/stream.hxx"      // Cyg_StdioStream

// EXPORTED SYMBOLS

externC char *
gets( char * ) CYGPRI_LIBC_WEAK_ALIAS("_gets");

// FUNCTIONS

externC char *
_gets( char *s )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stdin;
    cyg_ucount32 bytes_read;
    Cyg_ErrNo err;
    

    err = real_stream->read( (cyg_uint8 *)s, UINT_MAX, &bytes_read );

    if (!err && !bytes_read)  // if no err, but nothing to read, try again
    {
        real_stream->refill_read_buffer();
        err = real_stream->read( (cyg_uint8 *)s, UINT_MAX, &bytes_read );

    } // if

    if (err)
    {
        real_stream->set_error( err );
        errno = err;
        return NULL;
    } // if
    
    if (bytes_read > 0)
    {
        // NULL terminate it
        if (s[bytes_read-1] == '\n')
            s[bytes_read-1] = '\0';
        else
            s[bytes_read]='\0'; 
    } // if

    return s;

} // _gets()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF gets.cxx
