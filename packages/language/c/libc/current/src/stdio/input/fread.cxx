//========================================================================
//
//      fread.cxx
//
//      ANSI Stdio fread() function
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
// Date:          1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//=======================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include "clibincl/stdiosupp.hxx"   // Support functions for stdio
#include "clibincl/stream.hxx"      // Cyg_StdioStream

// EXPORTED SYMBOLS

externC size_t
fread( void *, size_t, size_t, FILE * ) CYGPRI_LIBC_WEAK_ALIAS("_fread");

// FUNCTIONS

externC size_t
_fread( void *ptr, size_t object_size, size_t num_objects, FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    cyg_ucount32 bytes_read;
    Cyg_ErrNo err;
    

    CYG_REPORT_FUNCNAMETYPE( "_fread", "read %d objects" );
    CYG_REPORT_FUNCARG4( "ptr=%08x, object_size=%d, num_objects=%d, "
                         "stream=%08x", ptr, object_size, num_objects,
                         stream );

    if ( (object_size==0) || (num_objects==0) ) {
        CYG_REPORT_RETVAL(0);
        return 0;
    } // if

    err = real_stream->read( (cyg_uint8 *)ptr, object_size*num_objects,
                             &bytes_read );

    if (!err && !bytes_read) { // if no err, but nothing to read, try again
        real_stream->refill_read_buffer();
        err = real_stream->read( (cyg_uint8 *)ptr, object_size*num_objects,
                                 &bytes_read );
    } // if

    if (err) {
        real_stream->set_error( err );
        errno = err;
    } // if
    
    // we return the number of _objects_ read. Simple division is
    // sufficient as this returns the quotient rather than rounding
    CYG_REPORT_RETVAL( bytes_read/object_size );
    return bytes_read/object_size;

} // _fread()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF fread.cxx
