//===========================================================================
//
//      gets.cxx
//
//      ISO C standard I/O gets() function
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors: 
// Date:          2001-03-15
// Purpose:       Implementation of ISO C standard I/O gets() function
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

// FUNCTIONS

// FIXME: should be reworked to read buffer at a time, and scan that
// for newlines, rather than reading byte at a time.

externC char *
gets( char *s )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stdin;
    Cyg_ErrNo err=ENOERR;
    cyg_uint8 c;
    cyg_uint8 *str=(cyg_uint8 *)s;
    int nch;

    CYG_CHECK_DATA_PTRC( s );
    CYG_CHECK_DATA_PTRC( real_stream );

    CYG_REPORT_FUNCTYPE( "returning string %08x");
    CYG_REPORT_FUNCARG1( "s=%08x", s );
    
    for (nch=1;; nch++) {
        err = real_stream->read_byte( &c );
        
        // if nothing to read, try again ONCE after refilling buffer
        if (EAGAIN == err) {
            err = real_stream->refill_read_buffer();
            if ( !err )
                err = real_stream->read_byte( &c );

            if (EAGAIN == err) {
                if (1 == nch) {   // indicates EOF at start
                    CYG_REPORT_RETVAL( NULL );
                    return NULL;
                } else
                    break; // EOF
            } // if
        } // if
        
        if ('\n' == c) // discard newlines
            break;
        *str++ = c;
    } // while
    
    *str = '\0'; // NULL terminate it
    
    if (err && EAGAIN != err) {
        real_stream->set_error( err );
        errno = err;
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if

    CYG_REPORT_RETVAL( s );
    return s;
} // gets()

// EOF gets.cxx
