//========================================================================
//
//      fgets.cxx
//
//      ANSI Stdio fgets() function
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
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-20
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//=======================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

// FUNCTIONS

externC char *
fgets( char *s, int n, FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    cyg_ucount32 bytes_read;
    Cyg_ErrNo err;
    

    err = real_stream->read( (cyg_uint8 *)s, n-1, &bytes_read );

    if (!err && !bytes_read) { // if no err, but nothing to read, try again
        err = real_stream->refill_read_buffer();
        if ( !err )
            err = real_stream->read( (cyg_uint8 *)s, n-1, &bytes_read );

    } // if

    if (err) {
        real_stream->set_error( err );
        errno = err;
        return NULL;
    } // if
    
    if (bytes_read > 0)
        s[bytes_read]='\0'; // NULL terminate it
    
    return s;

} // fgets()

// EOF fgets.cxx
