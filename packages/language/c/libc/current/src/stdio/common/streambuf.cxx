//===========================================================================
//
//      streambuf.cxx
//
//      C library stdio stream buffer functions
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

// Include the C library? And do we want the stdio stuff? And buffered I/O?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) && \
    defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)


// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>    // Assertion support
#include <errno.h>                // Cyg_ErrNo
#include "clibincl/stdlibsupp.hxx"// _malloc() and _free()
#include "clibincl/streambuf.hxx" // header for this file, just in case

// FUNCTIONS
    
Cyg_ErrNo
Cyg_StdioStreamBuffer::set_buffer( cyg_ucount32 size=BUFSIZE,
                                   cyg_uint8 *new_buffer=NULL )
{
    
#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    // user-supplied buffer?
    if (new_buffer != NULL) {
        CYG_CHECK_DATA_PTR(new_buffer, "new_buffer not valid");
        // first check if we were responsible for the old buffer
        if (call_free) {
            _free(buffer_bottom);
            call_free = false;
        }
        buffer_bottom = new_buffer;
    }
    else if ( size != get_buffer_size() ) { // as long as its different from
                                            // what we've got now
        cyg_uint8 *malloced_buf;

        malloced_buf = (cyg_uint8 * )_malloc( size );
        if (malloced_buf == NULL)
            return ENOMEM;

        // should the old buffer be freed? This waits till after we know
        // whether the malloc succeeded
        if (call_free)
            _free( buffer_bottom );
        
        buffer_bottom = malloced_buf;

        call_free=true;

    } // else if
    
    
#else // ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    // In this config we can't use a different buffer, or set a
    // size greater than now. We can pretend to shrink it though
    
    // Note on the next line we compare it with the size of buffer_bottom
    // and not the current size, as that's what is the limiting factor
    if ( (new_buffer != NULL) || (size > sizeof(buffer_bottom)) )
        return EINVAL;
    
#endif // ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    buffer_top = current_buffer_position = &buffer_bottom[0];
    buffer_size = size;
    
    return ENOERR;
    
} // set_buffer()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) &&
       // defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)


// EOF streambuf.cxx
