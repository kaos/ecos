//========================================================================
//
//      fwrite.cxx
//
//      ANSI Stdio fwrite() function
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
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdio.h>                  // header for this file
#include <errno.h>                  // error codes
#include <cyg/libc/stdio/stream.hxx>// Cyg_StdioStream

// FUNCTIONS

externC size_t
fwrite( const void *ptr, size_t object_size, size_t num_objects,
         FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    cyg_ucount32 written;
    Cyg_ErrNo err;
    
    CYG_REPORT_FUNCNAMETYPE( "fwrite", "wrote %d objects" );
    CYG_REPORT_FUNCARG4( "ptr=%08x, object_size=%d, num_objects=%d, "
                         "stream=%08x", ptr, object_size, num_objects,
                         stream );

    if ( (object_size==0) || (num_objects==0) ) {
        CYG_REPORT_RETVAL(0);
        return 0;
    } // if

    err = real_stream->write( (cyg_uint8 *)ptr, num_objects*object_size,
                              &written );

    if (err) {
        real_stream->set_error( err );
        errno = err;
    } // if
    
    // we return the number of _objects_ written. Simple division is
    // sufficient as this returns the quotient rather than rounding

    CYG_REPORT_RETVAL( written/object_size );
    return written/object_size;
} // fwrite()

// EOF fwrite.cxx
