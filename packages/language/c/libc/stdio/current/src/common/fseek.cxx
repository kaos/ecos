//===========================================================================
//
//      fseek.cxx
//
//      Implementation of C library file positioning functions as per ANSI 7.9.9
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
// Author(s):     nickg
// Contributors:  
// Date:          2000-07-12
// Purpose:       Implements ISO C file positioning functions
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <stdio.h>                  // header for fseek() etc.
#include <cyg/libc/stdio/stdiofiles.hxx> // C library files
#include <cyg/libc/stdio/stream.hxx>     // C library streams

//========================================================================

// ISO C 7.9.9 File positioning functions

externC int
fgetpos( FILE * stream , fpos_t *pos  )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    int ret = 0;
    
    CYG_REPORT_FUNCNAME( "fgetpos" );

    err = real_stream->get_position( pos );
    
    if( err != ENOERR )
    {
        errno = err;
        ret = -1;
    }
    
    CYG_REPORT_RETVAL( ret );
    return ret;
}

externC int
fseek( FILE * stream , long int offset , int whence  )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    int ret = 0;
    
    CYG_REPORT_FUNCNAME( "fgetpos" );

    err = real_stream->set_position( (fpos_t)offset, whence );
    
    if( err != ENOERR )
    {
        errno = err;
        ret = -1;
    }
    
    CYG_REPORT_RETVAL( ret );
    return ret;
}

externC int
fsetpos( FILE * stream , const fpos_t * pos )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    int ret = 0;
    
    CYG_REPORT_FUNCNAME( "fgetpos" );

    err = real_stream->set_position( *pos, SEEK_SET );
    
    if( err != ENOERR )
    {
        errno = err;
        ret = -1;
    }
    
    CYG_REPORT_RETVAL( ret );
    return ret;
}

externC long int
ftell( FILE * stream  )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    long int ret = 0;
    fpos_t pos;
    
    CYG_REPORT_FUNCNAME( "ftell" );

    err = real_stream->get_position( &pos );
    
    if( err != ENOERR )
    {
        errno = err;
        ret = -1;
    }
    else ret = pos;
    
    CYG_REPORT_RETVAL( ret );
    return ret;
}

externC void
rewind( FILE * stream  )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;    
    (void)fseek( stream, 0L, SEEK_SET );

    real_stream->set_error( ENOERR );
}


// EOF fseek.cxx
