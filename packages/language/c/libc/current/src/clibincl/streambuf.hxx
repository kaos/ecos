#ifndef CYGONCE_LIBC_CLIBINCL_STREAMBUF_HXX
#define CYGONCE_LIBC_CLIBINCL_STREAMBUF_HXX
//===========================================================================
//
//      streambuf.hxx
//
//      Internal C library stdio stream buffer interface definitions
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: This file implements the buffer class used by Cyg_StdioStream
//              for file buffers. It is simple, and not thread-safe - that
//              is better done at a higher level for our purposes.
// Usage:       #include "clibincl/streambuf.hxx"
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
#include <errno.h>                // Cyg_ErrNo


// TYPE DEFINITIONS


class Cyg_StdioStreamBuffer
{
private:
    // Buffering data

#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    cyg_uint8 *buffer_bottom;
    cyg_bool call_free;  // should we free the old buffer if set_buffer is
                         // called?
#else 
    cyg_uint8 buffer_bottom[BUFSIZ];
#endif

    cyg_ucount32 buffer_size;

    cyg_uint8 *buffer_top;

    cyg_uint8 *current_buffer_position;

public:

    // CONSTRUCTORS
    
    // You can change the size, or even supply your own buffer, although
    // this only has an effect with dynamic buffers. Otherwise it is
    // silently ignored
    Cyg_StdioStreamBuffer( cyg_ucount32 size=BUFSIZ,
                           cyg_uint8 *new_buffer=NULL );


    // DESTRUCTORS

    ~Cyg_StdioStreamBuffer();

    // METHODS

    // Set up the buffer. As with the constructor, supplying a new_buffer
    // only has an effect with dynamic buffers, although EINVAL is returned
    // in that case. ENOMEM may also be returned
    Cyg_ErrNo
    set_buffer( cyg_ucount32 size=BUFSIZ, cyg_uint8 *new_buffer=NULL );

    // Find out how much buffer space is in use
    cyg_ucount32
    get_buffer_space_used( void );


    // What total size is the current buffer set to be. Can be -1 if the
    // the buffer is invalid
    cyg_count32
    get_buffer_size( void );

    
    // get buffer address to read from, and return the number of bytes
    // available to read
    cyg_ucount32
    get_buffer_addr_to_read( cyg_uint8 **buffer );

    // when finished reading from said space, tell the buffer how much was
    // actually read
    void
    set_bytes_read( cyg_ucount32 bytes );

    // return address of buffer that can be used to write into, and its
    // available capacity
    cyg_ucount32
    get_buffer_addr_to_write( cyg_uint8 **buffer );

    // when finished writing into said space, tell the buffer how much was
    // actually written
    void
    set_bytes_written( cyg_ucount32 bytes );

    // just empty the whole buffer contents
    void
    drain_buffer( void );
}; // class Cyg_StdioStreamBuffer

// INLINE FUNCTIONS

#include "clibincl/streambuf.inl"


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) &&
       // defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)

#endif CYGONCE_LIBC_CLIBINCL_STREAMBUF_HXX multiple inclusion protection

// EOF streambuf.hxx
