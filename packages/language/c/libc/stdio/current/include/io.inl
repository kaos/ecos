#ifndef CYGONCE_LIBC_STDIO_IO_INL
#define CYGONCE_LIBC_STDIO_IO_INL
//========================================================================
//
//      io.inl
//
//      Internal C library stdio io interface inlines
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  
// Date:          2000-06-30
// Purpose:     
// Description: 
// Usage:         #include <cyg/libc/stdio/io.hxx>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

//========================================================================
// INCLUDES

#include <cyg/libc/stdio/io.hxx>

//========================================================================
// FileIO versions of IO functions

#ifdef CYGPKG_LIBC_STDIO_FILEIO

inline Cyg_ErrNo cyg_stdio_open( const char *filename,
                                 const Cyg_StdioStream::OpenMode rw,
                                 const cyg_bool binary,
                                 const cyg_bool append,
                                 cyg_stdio_handle_t *dev)
{
    mode_t mode = 0;
    int fd;
    
    switch( rw )
    {
    case Cyg_StdioStream::CYG_STREAM_WRITE:
        mode = O_WRONLY|O_CREAT|O_TRUNC;
        break;
    case Cyg_StdioStream::CYG_STREAM_READ:
        mode = O_RDONLY;
        break;
    case Cyg_StdioStream::CYG_STREAM_READWRITE:
        mode = O_RDWR;
        break;
    }

    if( append )
    {
        mode |= O_APPEND;
        mode &= ~O_TRUNC;
    }

    fd = open( filename, mode );

    if( fd < 0 )
        return errno;

    *dev = fd;
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_close( cyg_stdio_handle_t dev )
{
    if( close( dev ) != ENOERR )
        return errno;
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_read( cyg_stdio_handle_t dev,
                                 void *buffer, cyg_uint32 *len )
{
    if( dev != CYG_STDIO_HANDLE_NULL )
    {
        ssize_t done = read( dev, buffer, *len );

        if( done < 0 )
        {
            *len = 0;
            return errno;
        }

        *len = done;
    }
    // If the device is NULL, just return EOF indication
    else *len = 0;
    
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_write( cyg_stdio_handle_t dev,
                                 const void *buffer, cyg_uint32 *len )
{
    if( dev != CYG_STDIO_HANDLE_NULL )
    {
        ssize_t done = write( dev, buffer, *len );

        if( done < 0 )
        {
            *len = 0;
            return errno;
        }

        *len = done;
    }
    // if the device is NULL, just absorb all writes.
    
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_lseek( cyg_stdio_handle_t dev,
                                  off_t *pos, int whence )
{
    off_t newpos = lseek( dev, *pos, whence );

    if( newpos < 0 )
        return errno;

    *pos = newpos;

    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_flush( cyg_stdio_handle_t dev )
{
    int err = fsync( dev );

    if( err < 0 )
        return errno;

    return ENOERR;
}

inline cyg_bool cyg_stdio_interactive( cyg_stdio_handle_t dev )
{
    struct stat buf;
    int err;

    err = fstat( dev, &buf );

    // If we get an error, assume interactive.
    if( err < 0 )
        return true;

    if( S_ISCHR(buf.st_mode) )
        return true;

    return false;
}


#endif // CYGPKG_LIBC_STDIO_FILEIO

//========================================================================
// Direct IO versions of IO functions

#ifndef CYGPKG_LIBC_STDIO_FILEIO

inline Cyg_ErrNo cyg_stdio_open( const char *filename,
                                 const Cyg_StdioStream::OpenMode rw,
                                 const cyg_bool binary,
                                 const cyg_bool append,
                                 cyg_stdio_handle_t *dev)
{
    return cyg_io_lookup( filename, dev );
}

inline Cyg_ErrNo cyg_stdio_close( cyg_stdio_handle_t dev )
{
    // Devices do not get closed
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_read( cyg_stdio_handle_t dev,
                                 void *buffer, cyg_uint32 *len )
{
    return cyg_io_read( dev, buffer, len );
}

inline Cyg_ErrNo cyg_stdio_write( cyg_stdio_handle_t dev,
                                 const void *buffer, cyg_uint32 *len )
{
    return cyg_io_write( dev, buffer, len );    
}

inline cyg_uint32 cyg_stdio_lseek( cyg_stdio_handle_t dev,
                                  cyg_uint32 *pos, int whence )
{
    // No seeking in raw devices, just return fake success
    return ENOERR;
}

inline Cyg_ErrNo cyg_stdio_flush( cyg_stdio_handle_t dev )
{
    return cyg_io_get_config(dev,
                             CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN,
                             NULL, NULL);
}

inline cyg_bool cyg_stdio_interactive( cyg_stdio_handle_t dev )
{
    return true;
}


#endif // !CYGPKG_LIBC_STDIO_FILEIO

//========================================================================
#endif // CYGONCE_LIBC_STDIO_IO_INL multiple inclusion protection
// EOF io.inl

