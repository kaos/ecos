//========================================================================
//
//      fflush.cxx
//
//      Implementation of C library file flush function as per ANSI 7.9.5.2
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
// Purpose:       Provides ISO C fflush() function
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
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <stdio.h>                  // header for fflush()
#include <cyg/libc/stdio/stream.hxx>// C library streams
#include <cyg/libc/stdio/stdiofiles.hxx>  // C library files
#include <cyg/libc/stdio/stdiosupp.hxx>   // support for stdio

// FUNCTIONS

// flush all but one stream
externC Cyg_ErrNo
cyg_libc_stdio_flush_all_but( Cyg_StdioStream *not_this_stream )
{
    cyg_bool files_flushed[FOPEN_MAX] = { false }; // sets all to 0
    cyg_bool loop_again;
    cyg_ucount32 i;
    Cyg_ErrNo err=ENOERR;
    Cyg_StdioStream *stream;

    do {
        loop_again = false;
        
        for (i=0; (i<FOPEN_MAX) && !err; i++) {
            if (files_flushed[i] == false) {
                
                stream = Cyg_libc_stdio_files::get_file_stream(i);
                
                if ((stream == NULL) || (stream == not_this_stream)) {
                    // if it isn't a valid stream, set its entry in the
                    // list of files flushed since we don't want to
                    // flush it
                    // Ditto if its the one we're meant to miss
                    
                    files_flushed[i] = true;
                } // if
                else {
                    // valid stream
#ifdef CYGSEM_LIBC_STDIO_WANT_BUFFERED_IO
                    // only buffers which we've written to need flushing
                    if ( !stream->flags.last_buffer_op_was_read)
#endif
		      {
                        if ( stream->trylock_me() ) {
                            err = stream->flush_output_unlocked();
                            stream->unlock_me();
                            files_flushed[i] = true;
                        } // if
                        else
                            loop_again = true;
                    }
                } // else
            } // if
        } // for
    } // do
    while(loop_again && !err);
    
    return err;
} // cyg_libc_stdio_flush_all_but()

externC int
fflush( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    Cyg_ErrNo err;

    if (stream == NULL) {  // tells us to flush ALL streams
        err = cyg_libc_stdio_flush_all_but(NULL);
    } // if
    else {
        err = real_stream->flush_output();
    } // else

    if (err) {
        errno = err;
        return EOF;
    } // if

    return 0;

} // fflush()
        
// EOF fflush.cxx
