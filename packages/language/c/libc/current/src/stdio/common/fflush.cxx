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
#include <stdio.h>                  // header for fflush()
#include "clibincl/stream.hxx"      // C library streams
#include "clibincl/clibdata.hxx"    // C library private data
#include "clibincl/stdiosupp.hxx"   // support for stdio


// EXPORTED SYMBOLS

externC int
fflush( FILE * ) CYGPRI_LIBC_WEAK_ALIAS("_fflush");


// FUNCTIONS


externC int
_fflush( FILE *stream )
{
    Cyg_StdioStream *real_stream = (Cyg_StdioStream *)stream;
    int i;
    Cyg_ErrNo err=ENOERR;

    if (stream == NULL) {  // tells us to flush ALL streams
        cyg_bool files_flushed[FOPEN_MAX] = { false }; // sets all to 0
        cyg_bool loop_again;

        do {
            loop_again = false;

            for (i=0; (i<FOPEN_MAX) && !err; i++) {
                if (files_flushed[i] == false) {

                    real_stream = Cyg_CLibInternalData::get_file_stream(i);
                    
                    if (real_stream == NULL) {
                        // if it isn't a valid stream, set its entry in the
                        // list of files flushed since we don't want to
                        // flush it

                        files_flushed[i] = true;
                    } // if
                    else {
                        // valid stream
                        
                        if ( real_stream->trylock_me() ) {
                            err = real_stream->flush_output_unlocked();
                            real_stream->unlock_me();
                            files_flushed[i] = true;
                        } // if
                        else
                            loop_again = true;
                    } // else
                } // if
            } // for
        } // do
        while(loop_again && !err);

    } // if
    else {
        err = real_stream->flush_output();
    } // else

    if (err) {
        errno = err;
        return EOF;
    } // if

    return 0;

} // _fflush()
        
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF fflush.cxx
