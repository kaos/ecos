//===========================================================================
//
//      fputc.cxx
//
//      ISO Standard I/O character output functions
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
// Contributors: jlarmour
// Date:         1999-07-16
// Purpose:      Provide the fputc() function. Also provides the function
//               versions of putc() and putchar()
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Do we want the stdio stuff?
#ifdef CYGPKG_LIBC_STDIO

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>      // Standard eCos assertion support
#include <cyg/infra/cyg_trac.h>     // Standard eCos tracing support
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include "clibincl/stream.hxx"      // Cyg_StdioStream

// FUNCTIONS

externC int
__fputc( int c, FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;
    cyg_uint8 real_c = (cyg_uint8) c;
    
    CYG_REPORT_FUNCNAMETYPE("__fputc", "wrote char %d");
    CYG_REPORT_FUNCARG2( "c = %d, stream=%08x", c, stream );
    
    CYG_CHECK_DATA_PTR( stream, "stream is not a valid pointer" );

    err = real_stream->write_byte( real_c );

    if (err)
    {
        real_stream->set_error( err );
        errno = err;
        CYG_REPORT_RETVAL(EOF);
        return EOF;
    } // if
    
    CYG_REPORT_RETVAL((int)real_c);
    return (int)real_c;

} // __fputc()


externC int
__putchar( int c )
{
    return fputc( c, stdout );
} // __putchar()

// EXPORTED SYMBOLS

externC int
fputc( int, FILE * ) CYGBLD_ATTRIB_WEAK_ALIAS(__fputc);

// Also define putc() and putchar() even though they can be macros.
// Undefine the macros first though
#undef putc
#undef putchar

externC int
putc( int, FILE * ) CYGBLD_ATTRIB_WEAK_ALIAS(__fputc);

externC int
putchar( int ) CYGBLD_ATTRIB_WEAK_ALIAS(__putchar);

#endif // ifdef CYGPKG_LIBC_STDIO

// EOF fputc.cxx
