#ifndef CYGONCE_LIBC_CLIBINCL_STREAM_HXX
#define CYGONCE_LIBC_CLIBINCL_STREAM_HXX
//========================================================================
//
//      stream.hxx
//
//      Internal C library stdio stream interface definitions
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1998-02-13
// Purpose:     
// Description: 
// Usage:         #include "clibincl/stream.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)


// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>     // Get assertion macros, as appropriate
#include <errno.h>                 // Cyg_ErrNo
#include <stdio.h>                 // fpos_t and IOBUF defines
#include "clibincl/streambuf.hxx"  // Stdio stream file buffers
#include <cyg/io/io.h>             // General I/O support

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
#include <pkgconf/kernel.h>
#include <cyg/kernel/mutex.hxx>    // Cyg_Mutex
#endif

// FUNCTION PROTOTYPES

// can't get this from stdiosupp.hxx due to potential header recursion
externC int
_setvbuf( FILE * /* stream */, char * /* buffer */, int /* mode */,
          size_t /* size */ );

// TYPE DEFINITIONS

class Cyg_StdioStream
{
    friend int _setvbuf( FILE *, char *, int, size_t );

private:

    // error status for this file
    Cyg_ErrNo error;


    cyg_io_handle_t my_device;

#ifdef CYGFUN_LIBC_STDIO_ungetc
    cyg_uint8 unread_char_buf;
#endif

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    Cyg_StdioStreamBuffer io_buf; // read/write buffer
#endif
    cyg_uint8 readbuf_char; // a one character emergency "buffer"
                            // only used when configured to not buffer
                            // (i.e. !CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO)
                            // or set at runtime to not buffer (i.e.
                            // buffering mode is _IONBF)

    // some flags indicating the state of the file. Some of it is internal
    // state, which should not be public. Use bitfields to save
    // space, which means using "unsigned int" rather than cyg_uintX
    struct {
        unsigned int at_eof                  : 1; // Have we reached eof?

        unsigned int opened_for_read         : 1; // opened_for_read and

        unsigned int opened_for_write        : 1; // opened_for_write can
                                                  // be set simultaneously

        unsigned int binary                  : 1; // opened for binary or
                                                  // text mode?
        
#ifdef CYGFUN_LIBC_STDIO_ungetc
        unsigned int unread_char_buf_in_use  : 1; // unget buf in use?
#endif

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
        unsigned int buffering               : 1; // Is this file buffered?

        unsigned int line_buffering          : 1; // If so, is it line
                                                  // buffered? If it is
                                                  // buffered, but NOT line
                                                  // buffered, it must be
                                                  // fully buffered

        unsigned int last_buffer_op_was_read : 1; // did we last read from
                                                  // the buffer. If not we
                                                  // must have written
#endif
        unsigned int readbuf_char_in_use     : 1; // is the above
                                                  // readbuf_char in use?
        
    } flags;

    // current position for reading/writing
    fpos_t position;

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    Cyg_Mutex stream_lock;  // used for locking this stream
#endif // ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS

#ifdef CYGDBG_USE_ASSERTS
    // a value to check that this class is hopefully valid
    cyg_ucount32 magic_validity_word;
#endif

public:
    // different modes when constructing (i.e. opening).
    typedef enum {
        CYG_STREAM_READ,
        CYG_STREAM_WRITE,
        CYG_STREAM_READWRITE
    } OpenMode;

    // CONSTRUCTORS

    // This constructs the stream - effectively opens the file. This is
    // used for static initialisation, and actually calls construct below
    //
    // dev is a valid Cyg_Device_Table_t, although it does not have to
    // be a member of the device table object itself
    //
    // open_mode is one of CYG_STREAM_READ, CYG_STREAM_WRITE or
    // CYG_STREAM_READWRITE
    //
    // append is true if the file position should be set at EOF on opening
    //
    // binary is true if this is a binary stream. Otherwise it is a text
    // stream and character conversions (especially newline) may occur
    //
    // buffer_mode is one of _IONBF, _IOLBF, _IOFBF (from <stdio.h>)
    // If buffer_mode is _IONBF, buffer_size should still be set to 0
    // and buffer_addr to NULL. If buffering is not configured, none
    // of buffer_mode, buffer_size and buffer_addr have any effect
    //
    // buffer_size is the size of buffer to use
    //
    // buffer_addr is the address of a user supplied buffer. By default
    // (when NULL) a system one is provided.
    //
    // The "return code" is set by assignment to the error member of this
    // stream - use the get_error() method to check

    Cyg_StdioStream( cyg_io_handle_t dev, OpenMode open_mode,
                     cyg_bool append, cyg_bool binary, int buffer_mode,
                     cyg_ucount32 buffer_size=BUFSIZ,
                     cyg_uint8 *buffer_addr=NULL );

    // DESTRUCTOR

    ~Cyg_StdioStream();


    // MEMBER FUNCTIONS


    // Refill read buffer from the stream - note this blocks until
    // something arrives on the stream
    Cyg_ErrNo
    refill_read_buffer( void );


    // Read not more than buffer_length bytes from the read buffer into the
    // user buffer.
    // The number of bytes put into the user buffer is written
    // into *bytes_read
    Cyg_ErrNo
    read( cyg_uint8 *user_buffer, cyg_ucount32 buffer_length,
          cyg_ucount32 *bytes_read );


    // Read a single byte from the stream. Returns EAGAIN if no character
    // available or EEOF if end of file (as well as setting the EOF state)
    Cyg_ErrNo
    read_byte( cyg_uint8 *c );

    // Read a single byte from the stream, but don't remove it. Returns
    // EAGAIN if no character available or EEOF if end of file (as well
    // as setting the EOF state)
    Cyg_ErrNo
    peek_byte( cyg_uint8 *c );


    // Return a byte into the stream - basically the same as ungetc()
    Cyg_ErrNo
    unread_byte( cyg_uint8 c );

    // the number of bytes available to read without needing to refill the
    // buffer
    cyg_ucount32
    bytes_available_to_read( void );


    Cyg_ErrNo
    write( const cyg_uint8 *buffer, cyg_ucount32 buffer_length,
           cyg_ucount32 *bytes_written );

    Cyg_ErrNo
    write_byte( cyg_uint8 c );


    Cyg_ErrNo
    flush_output( void );

    Cyg_ErrNo
    flush_output_unlocked( void );

    // prevent multiple access in thread safe mode

    // lock_me() returns false if it couldn't be locked, which could
    // happen if the file descriptor is bad

    cyg_bool
    lock_me( void );

    // trylock_me() returns false if it couldn't be locked, probably
    // because it is already locked
    cyg_bool
    trylock_me( void );

    void
    unlock_me( void );

    // get error status for this file 
    Cyg_ErrNo
    get_error( void );

    // set error status for this file.
    void
    set_error( Cyg_ErrNo errno_to_set );

public:
    // are we at EOF? true means we are, false means no
    cyg_bool
    get_eof_state( void );

private:

    // Set whether we are at EOF.
    void
    set_eof_state( cyg_bool eof_to_set );

public:
    // retrieve position
    Cyg_ErrNo
    get_position( fpos_t *pos );

    // set absolute position. whence is SEEK_SET, SEEK_CUR, or SEEK_END
    Cyg_ErrNo
    set_position( fpos_t pos, int whence );

    CYGDBG_DEFINE_CHECK_THIS
};

// INLINE FUNCTIONS

#include "clibincl/stream.inl"


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

#endif CYGONCE_LIBC_CLIBINCL_STREAM_HXX multiple inclusion protection

// EOF stream.hxx
