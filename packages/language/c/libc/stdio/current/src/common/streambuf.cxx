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
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-04-20
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// Include buffered I/O?
#if defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)


// INCLUDES

#include <cyg/infra/cyg_type.h>   // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>    // Assertion support
#include <errno.h>                // Cyg_ErrNo
#include <stdlib.h>               // malloc() and free()
#include <cyg/libc/stdio/streambuf.hxx> // header for this file, just in case

// FUNCTIONS
    
Cyg_ErrNo
Cyg_StdioStreamBuffer::set_buffer( cyg_ucount32 size=BUFSIZ,
                                   cyg_uint8 *new_buffer=NULL )
{
    
#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    // user-supplied buffer?
    if (new_buffer != NULL) {
        CYG_CHECK_DATA_PTR(new_buffer, "new_buffer not valid");
#ifdef CYGSEM_LIBC_STDIO_SETVBUF_MALLOC
        // first check if we were responsible for the old buffer
        if (call_free) {
            free(buffer_bottom);
            call_free = false;
        }
#endif        
        buffer_bottom = new_buffer;
    }
#ifdef CYGSEM_LIBC_STDIO_SETVBUF_MALLOC    
    else if ( size != buffer_size ) { // as long as its different from
                                      // what we've got now
        cyg_uint8 *malloced_buf;

        malloced_buf = (cyg_uint8 * )malloc( size );
        if (malloced_buf == NULL)
            return ENOMEM;

        // should the old buffer be freed? This waits till after we know
        // whether the malloc succeeded
        if (call_free)
            free( buffer_bottom );
        
        buffer_bottom = malloced_buf;

        call_free=true;

    } // else if
#else
    // Here we have not been given a new buffer, but have been given
    // a new buffer size. If possible, we just shrink what we already
    // have.
    else if( size > buffer_size )
        return EINVAL;
#endif    
    
#else // ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    // In this config we can't use a different buffer, or set a
    // size greater than now. We can pretend to shrink it though
    
    // Note on the next line we compare it with the size of static_buffer
    // and not the current size, as that's what is the limiting factor
    if ( (new_buffer != NULL) || (size > sizeof(static_buffer)) )
        return EINVAL;
    
#endif // ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    
    buffer_top = current_buffer_position = &buffer_bottom[0];
    buffer_size = size;
    
    return ENOERR;
    
} // set_buffer()


#endif // if defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)


// EOF streambuf.cxx
