#ifndef CYGONCE_LIBC_STDIO_STREAMBUF_INL
#define CYGONCE_LIBC_STDIO_STREAMBUF_INL
//===========================================================================
//
//      streambuf.inl
//
//      Internal C library stdio stream buffer inline functions
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
// Date:         2000-04-19
// Purpose:     
// Description: 
// Usage:       Do not include this file directly, instead
//              #include <cyg/libc/stdio/streambuf.hxx>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// Include buffered I/O?
#if defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)


// INCLUDES

#include <cyg/infra/cyg_type.h>          // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>           // Assertion support
#include <errno.h>                       // Cyg_ErrNo
#include <stdio.h>                       // fpos_t and iobuf defines
#include <stdlib.h>                      // free()
#include <cyg/libc/stdio/streambuf.hxx>  // header for this file, just in case
#include <limits.h>                      // INT_MAX

// FUNCTIONS
    
inline
Cyg_StdioStreamBuffer::Cyg_StdioStreamBuffer( cyg_ucount32 size=BUFSIZ,
                                              cyg_uint8 *new_buffer=NULL ) :
#if defined(CYGSEM_LIBC_STDIO_SETVBUF_MALLOC)
    call_free(false),
    buffer_bottom( NULL ),
    buffer_size(0),
#else
    buffer_bottom( static_buffer ),
    buffer_size(sizeof(static_buffer)),    
#endif    
    buffer_top(NULL),
    current_buffer_position(NULL)
{
    // NB Many of the above members in the initialisation list may seem
    // unnecessary, but it is to ensure a defined state if e.g. the malloc
    // in set_buffer() should fail

    (void)set_buffer(size, new_buffer);
} // Cyg_StdioStreamBuffer constructor


inline
Cyg_StdioStreamBuffer::~Cyg_StdioStreamBuffer()
{
#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    if ((buffer_bottom != NULL) && call_free)
        free( buffer_bottom );
#endif        
} // Cyg_StdioStreamBuffer destructor


inline cyg_ucount32
Cyg_StdioStreamBuffer::get_buffer_space_used( void )
{
    return (buffer_top - current_buffer_position);
} // get_buffer_space_used()


inline cyg_count32
Cyg_StdioStreamBuffer::get_buffer_size( void )
{
#ifdef CYGSEM_LIBC_STDIO_DYNAMIC_SETVBUF
    if (buffer_bottom==NULL)
        return -1;
#endif
    return buffer_size;
} // get_buffer_size()        


inline cyg_ucount32
Cyg_StdioStreamBuffer::get_buffer_addr_to_read( cyg_uint8 **buffer )
{
    *buffer = current_buffer_position;
    
    return (buffer_top - current_buffer_position);
} // get_buffer_addr_to_read()

inline void
Cyg_StdioStreamBuffer::set_bytes_read( cyg_ucount32 bytes )
{
    cyg_uint8 *buffer_max = &buffer_bottom[ get_buffer_size() ];

    current_buffer_position += bytes;

    // INT_MAX is used by some callers to mean infinite.
    CYG_ASSERT( (current_buffer_position <= buffer_top)
                || (get_buffer_size() == INT_MAX),
                "read too many bytes from buffer" );

    if (current_buffer_position == buffer_max)
        current_buffer_position = buffer_top = &buffer_bottom[0];
    
} // set_bytes_read()


inline cyg_ucount32
Cyg_StdioStreamBuffer::get_buffer_addr_to_write( cyg_uint8 **buffer )
{
    cyg_uint8 *buffer_max = &buffer_bottom[ get_buffer_size() ];
    
    *buffer = buffer_top;
    
    return (buffer_max - buffer_top);
} // get_buffer_addr_to_write()


inline void
Cyg_StdioStreamBuffer::set_bytes_written( cyg_ucount32 bytes )
{
    buffer_top += bytes;

    // INT_MAX is used by some callers to mean infinite.
    CYG_ASSERT( (buffer_top <= &buffer_bottom[ get_buffer_size() ])
                || (get_buffer_size() == INT_MAX),
                "wrote too many bytes into buffer" );
} // set_bytes_written()


inline void
Cyg_StdioStreamBuffer::drain_buffer( void )
{
    buffer_top = current_buffer_position = &buffer_bottom[0];
} // drain_buffer()

#endif // if defined(CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)

#endif // CYGONCE_LIBC_STDIO_STREAMBUF_INL multiple inclusion protection

// EOF streambuf.inl
