//========================================================================
//
//      stream.cxx
//
//      Implementations of internal C library stdio stream functions
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour@cygnus.co.uk
// Contributors:  jlarmour@cygnus.co.uk
// Date:          1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include "clibincl/stringsupp.hxx"  // _memcpy()
#include "clibincl/stream.hxx"      // Header for this file
#include <cyg/devs/common/iorb.h>   // Cyg_IORB
#include <cyg/devs/common/table.h>  // Device table


// FUNCTIONS

Cyg_StdioStream::Cyg_StdioStream(Cyg_Device_Table_t *dev,
                                 OpenMode open_mode,
                                 cyg_bool append, cyg_bool binary,
                                 int buffer_mode, cyg_ucount32 buffer_size,
                                 cyg_uint8 *buffer_addr )
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    : io_buf( buffer_size, buffer_addr )
#endif
    
{
#ifdef CYGDBG_USE_ASSERTS
    magic_validity_word = 0xbadbad;
#endif

    my_device = dev;

    // Clear all flags
    _memset( &flags, 0, sizeof(flags) );

    switch (open_mode) {
    case CYG_STREAM_READ:
        flags.opened_for_read = true;
        break;
    case CYG_STREAM_WRITE:
        flags.opened_for_write = true;
        break;
    case CYG_STREAM_READWRITE:
        flags.opened_for_read = true;
        flags.opened_for_write = true;
        break;
    default:
        error=EINVAL;
        return;
    } // switch
        
    
    if (flags.opened_for_write) {
        if (!my_device->write_blocking) {
            error = EDEVNOSUPP;
            return;
        } // if

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
        flags.last_buffer_op_was_read = false;
#endif
    } // if


    if (flags.opened_for_read) {
        if (!my_device->read_blocking) {
            error = EDEVNOSUPP;
            return;
        } // if

        // NB also if opened for read AND write, then say last op was read
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
        flags.last_buffer_op_was_read = true;
#endif
    } // if

    flags.binary = binary ? 1 : 0;

    error = ENOERR;
    
    // in due course we would do an equivalent to fseek(...,0, SEEK_END);
    // when appending. for now, there's nothing, except set eof
    
    flags.at_eof = append ? 1 : 0;

    position = 0;

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

    switch (buffer_mode) {
    case _IONBF:
        CYG_ASSERT( (buffer_size == 0) && (buffer_addr == NULL),
                    "No buffering wanted but size/address specified!" );
        flags.buffering = flags.line_buffering = false;
        break;
    case _IOLBF:
        flags.buffering = true;
        flags.line_buffering = true;
        break;
    case _IOFBF:
        flags.buffering = true;
        flags.line_buffering = false;
        break;
    default:
        error = EINVAL;
        return;
    } // switch

    // one way of checking the buffer was set up correctly
    if (io_buf.get_buffer_size()==-1) {
        error = ENOMEM;
        return;
    }

#endif

    if (my_device->open) {
        error = (*my_device->open)( my_device->cookie, 
                                    binary ? CYG_DEVICE_OPEN_MODE_RAW
                                           : CYG_DEVICE_OPEN_MODE_TEXT );
        if (error != ENOERR)
            return; // keep error code the same
    } // if
    

#ifdef CYGDBG_USE_ASSERTS
    magic_validity_word = 0x7b4321ce;
#endif
    
} // Cyg_StdioStream constructor



Cyg_ErrNo
Cyg_StdioStream::refill_read_buffer( void )
{
    Cyg_ErrNo read_err;
    Cyg_IORB iorb;

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    if (!lock_me())
        return EBADF;  // assume file is now invalid

    // first just check that we _can_ read this device!
    if (!flags.opened_for_read) {
        unlock_me();
        return EINVAL;
    }
    
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    // If there is pending output to write, then this will check and
    // write it
    if (flags.buffering) {
        read_err = flush_output_unlocked();

        if (read_err != ENOERR) {
            unlock_me();
            return read_err;
        } // if
    } // if
#endif

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    // we're now reading
    flags.last_buffer_op_was_read = true;

    if (flags.buffering) {
        iorb.buffer_length = 
            io_buf.get_buffer_addr_to_write( (cyg_uint8**)&iorb.buffer );
    }
    else
#endif

    if (!flags.readbuf_char_in_use) {
        iorb.buffer_length = 1;
        iorb.buffer = &readbuf_char;
    }
    else
        iorb.buffer_length = 0;

    if (!iorb.buffer_length) { // no buffer space available
        unlock_me();
        return ENOERR;  // isn't an error, just needs user to read out data
    } // if

    read_err = (*my_device->read_blocking)( my_device->cookie, &iorb );


#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering)
        io_buf.set_bytes_written( iorb.xferred_length );
    else
#endif
        flags.readbuf_char_in_use = iorb.xferred_length ? 1 : 0;

    unlock_me();

    if (read_err == ENOERR) {
        if (iorb.xferred_length == 0)
            read_err = EAGAIN;
    } // if
    
    return read_err;
} // refill_read_buffer()


Cyg_ErrNo
Cyg_StdioStream::read( cyg_uint8 *user_buffer, cyg_ucount32 buffer_length,
                       cyg_ucount32 *bytes_read )
{
    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    *bytes_read = 0;

    if (!lock_me())
        return EBADF;  // assume file is now invalid

    if (!flags.opened_for_read) {
        unlock_me();
        return EINVAL;
    }

#ifdef CYGFUN_LIBC_STDIO_ungetc
    if (flags.unread_char_buf_in_use && buffer_length) {
        *user_buffer++ = unread_char_buf;
        ++*bytes_read;
        flags.unread_char_buf_in_use = false;
        --buffer_length;
    } // if

#endif // ifdef CYGFUN_LIBC_STDIO_ungetc

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering) {
        cyg_uint8 *buff_to_read_from;
        cyg_ucount32 bytes_available;
        Cyg_ErrNo err;
    
        // this should really only be called after refill_read_buffer, but
        // just in case
        err = flush_output_unlocked();

        if (err != ENOERR) {
            unlock_me();
            return err;
        }
        
        // we're now reading
        flags.last_buffer_op_was_read = true;
        
        bytes_available = io_buf.get_buffer_addr_to_read(
              (cyg_uint8 **)&buff_to_read_from );
        
        *bytes_read += (bytes_available < buffer_length) ? bytes_available
                                                         : buffer_length;
        if (*bytes_read) {
            memcpy( user_buffer, buff_to_read_from, *bytes_read );
            io_buf.set_bytes_read( *bytes_read );
        } // if

    } // if
    else
        
#endif

    if (flags.readbuf_char_in_use && buffer_length) {
        *user_buffer = readbuf_char;
        *bytes_read = 1;
        flags.readbuf_char_in_use = false;
    }

    unlock_me();

    return ENOERR;
} // read()


Cyg_ErrNo
Cyg_StdioStream::read_byte( cyg_uint8 *c )
{
    Cyg_ErrNo err=ENOERR;

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    if (!lock_me())
        return EBADF;  // assume file is now invalid

    if (!flags.opened_for_read) {
        unlock_me();
        return EINVAL;
    }

    // this should really only be called after refill_read_buffer, but just
    // in case
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering)
        err = flush_output_unlocked();

    if (err != ENOERR)
        return err;

    // we're now reading
    flags.last_buffer_op_was_read = true;
#endif

# ifdef CYGFUN_LIBC_STDIO_ungetc
    if (flags.unread_char_buf_in_use) {
        *c = unread_char_buf;
        flags.unread_char_buf_in_use = false;
        unlock_me();
        return ENOERR;
    } // if
# endif // ifdef CYGFUN_LIBC_STDIO_ungetc

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering) {
        cyg_uint8 *buff_to_read_from;
        cyg_ucount32 bytes_available;
    
        bytes_available=io_buf.get_buffer_addr_to_read(&buff_to_read_from);

        if (bytes_available) {
            *c = *buff_to_read_from;
            io_buf.set_bytes_read(1);
        }
        else
            err = EAGAIN;
    } // if
    else
    
#endif


    if (flags.readbuf_char_in_use) {
        *c = readbuf_char;
        flags.readbuf_char_in_use = false;
    }
    else
        err = EAGAIN;

    unlock_me();

    return err;
} // read_byte()


Cyg_ErrNo
Cyg_StdioStream::peek_byte( cyg_uint8 *c )
{
    Cyg_ErrNo err=ENOERR;

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    if (!lock_me())
        return EBADF;  // assume file is now invalid

    if (!flags.opened_for_read) {
        unlock_me();
        return EINVAL;
    }

    // this should really only be called after refill_read_buffer, but just
    // in case
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering)
        err = flush_output_unlocked();

    if (err != ENOERR)
        return err;

    // we're now reading
    flags.last_buffer_op_was_read = true;
#endif

# ifdef CYGFUN_LIBC_STDIO_ungetc
    if (flags.unread_char_buf_in_use) {
        *c = unread_char_buf;
        unlock_me();
        return ENOERR;
    } // if
# endif // ifdef CYGFUN_LIBC_STDIO_ungetc

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.buffering) {
        cyg_uint8 *buff_to_read_from;
        cyg_ucount32 bytes_available;
    
        bytes_available=io_buf.get_buffer_addr_to_read(&buff_to_read_from);

        if (bytes_available) {
            *c = *buff_to_read_from;
        }
        else
            err = EAGAIN;
    } // if
    else
    
#endif


    if (flags.readbuf_char_in_use) {
        *c = readbuf_char;
    }
    else
        err = EAGAIN;

    unlock_me();

    return err;
} // peek_byte()


Cyg_ErrNo
Cyg_StdioStream::flush_output_unlocked( void )
{
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    Cyg_IORB iorb;
    Cyg_ErrNo write_err=ENOERR;

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    if ( flags.last_buffer_op_was_read )
        return ENOERR;

    // first just check that we _can_ write to the device!
    if ( !flags.opened_for_write )
        return EINVAL;

    // shortcut if nothing to do
    if (io_buf.get_buffer_space_used() == 0)
        return ENOERR;
        
    iorb.buffer_length =
        io_buf.get_buffer_addr_to_read( (cyg_uint8 **)&iorb.buffer );
    
    CYG_ASSERT( iorb.buffer_length > 0, 
                "There should be data to read but there isn't!");

    write_err = (*my_device->write_blocking)( my_device->cookie, &iorb );

    // we've just read it all, so just wipe it out
    io_buf.drain_buffer();

    return write_err;

#else // ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    return ENOERR;

#endif // ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
} // flush_output_unlocked()



Cyg_ErrNo
Cyg_StdioStream::write( const cyg_uint8 *buffer,
                        cyg_ucount32 buffer_length,
                        cyg_ucount32 *bytes_written )
{
    Cyg_ErrNo write_err = ENOERR;

    CYG_ASSERTCLASS( this, "Stream object is not a valid stream!" );
    
    *bytes_written = 0;

    if (!lock_me())
        return EBADF;  // assume file is now invalid

    // first just check that we _can_ write to the device!
    if ( !flags.opened_for_write ) {
        unlock_me();
        return EINVAL;
    }

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    if (flags.last_buffer_op_was_read == true)
        io_buf.drain_buffer();  // nuke input bytes to prevent confusion

    flags.last_buffer_op_was_read = false;

    if (!flags.buffering) {
#endif
        Cyg_IORB iorb;
        iorb.buffer = (char *)buffer;
        iorb.buffer_length = buffer_length;

        write_err = (*my_device->write_blocking)( my_device->cookie,
                                                  &iorb );

        *bytes_written = iorb.xferred_length;

#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    } // if
    else {
        cyg_ucount32 bytes_available;
        cyg_ucount32 bytes_to_write;
        cyg_ucount32 newline_pos;
        cyg_uint8 *write_addr;
        cyg_bool must_flush = false;
        
        while ( buffer_length > 0 ) {
            bytes_available =
                io_buf.get_buffer_addr_to_write( &write_addr );
            
            // we need to flush if we've no room or the buffer has an up
            // and coming newline
            if ( !bytes_available || must_flush ) {
                write_err = flush_output_unlocked();
                
                // harmless even if there was an error
                bytes_available =
                    io_buf.get_buffer_addr_to_write( &write_addr );

                CYG_ASSERT( bytes_available > 0,
                            "Help! still no bytes available in "
                            "write buffer" );
            } // if
            
            if (write_err) {
                unlock_me();
                return write_err;
            } // if
            
            // choose the lower of the buffer available and the length
            // to write
            bytes_to_write=(bytes_available < buffer_length) 
                ? bytes_available
                : buffer_length;
        
            // if we're line buffered, we may want want to flush if there's
            // a newline character, so lets find out
        
            if (flags.line_buffering) {
                for (newline_pos=0;
                     newline_pos<bytes_to_write;
                     newline_pos++) {
                    if (buffer[newline_pos] == '\n') {
                        break;
                    } // if
                } // for
                // if we didn't reach the end
                if (newline_pos != bytes_to_write) {
                    // shrink bytes_to_write down to the bit we need to
                    // flush including the newline itself
                    bytes_to_write = newline_pos + 1;
                    must_flush = true;
                } // if
            } // if
            
            _memcpy( write_addr, buffer, bytes_to_write );
            
            *bytes_written += bytes_to_write;
            buffer += bytes_to_write;
            buffer_length -= bytes_to_write;
            io_buf.set_bytes_written( bytes_to_write );
            
        } // while
        
        if ( must_flush ) {
            write_err = flush_output_unlocked();
        } // if
    } // else
#endif // ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
    
    unlock_me();

    return write_err;
} // write()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF stream.cxx
