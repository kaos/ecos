//========================================================================
//
//      freopen.cxx
//
//      Implementation of C library freopen() function
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
// Author(s):     jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:          1998-02-13
// Purpose:     
// Description:   Implementation of C library freopen() function as per
//                ISO C standard chap. 7.9.5.4
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Include the C library? And do we want the stdio stuff? And we only want
// freopen if we have fopen()
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) && \
    defined(CYGPKG_LIBC_STDIO_OPEN)


// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common project-wide type definitions
#include <stddef.h>                 // NULL and size_t from compiler
#include <errno.h>                  // Error codes
#include <stdio.h>                  // header for freopen()
#include "clibincl/stream.hxx"      // C libray streams
#include "clibincl/clibdata.hxx"    // C library private data
#include "clibincl/stdiosupp.hxx"   // support for stdio


// EXPORTED SYMBOLS

externC FILE *
freopen( const char *, const char *, FILE * )
    CYGPRI_LIBC_WEAK_ALIAS("_freopen");


// FUNCTIONS


externC FILE *
_freopen( const char *, const char *, FILE * )
{
    return NULL;  // Returning NULL is valid!
} // _freopen()
        
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO) && 
       //    defined(CYGPKG_LIBC_STDIO_OPEN)

// EOF freopen.cxx
