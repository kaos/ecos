#ifndef CYGONCE_LIBC_CLIBINCL_CLIBDATA_INL
#define CYGONCE_LIBC_CLIBINCL_CLIBDATA_INL
//===========================================================================
//
//      clibdata.inl
//
//      Inline functions for C library internal data
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       Do not include this file directly.
//              Instead #include "clibincl/clibdata.hxx"
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

#include <cyg/infra/cyg_type.h>       // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>        // Assert interface
#include "clibincl/clibdata.hxx"      // header for this file - just in case

#ifdef CYGPKG_LIBC_STDIO
# include "clibincl/stream.hxx"        // Cyg_StdioStream internal class
#endif

#ifndef CYGSEM_LIBC_PER_THREAD_ERRNO
# include <errno.h>                   // for errno variable
#endif


// INLINE METHODS

inline void
Cyg_CLibInternalData::construct( void )
{

#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO
    // errno is initialised to 0 at program startup - ANSI 7.1.4
    safe_errno = 0;
#endif // ifdef CYGSEM_LIBC_PER_THREAD_ERRNO

    
#ifdef CYGPKG_LIBC_RAND
    // Initialise seed for rand()
    rand_seed = CYGNUM_LIBC_RAND_SEED;
#endif
    
} // construct()

inline
Cyg_CLibInternalData::Cyg_CLibInternalData( void )
{
    construct();
} // Cyg_CLibInternalData() constructor


#ifdef CYGFUN_LIBC_strtok
inline char **
Cyg_CLibInternalData::get_strtok_last_p( void )
{
    return &strtok_last;
} // get_strtok_last_p()
#endif



#ifdef CYGPKG_LIBC_RAND
inline Cyg_CLibInternalData::Cyg_rand_seed_t *
Cyg_CLibInternalData::get_rand_seed_p( void )
{
    return &rand_seed;
} // get_rand_seed_p()
#endif


#ifdef CYGPKG_LIBC_STDIO

inline Cyg_StdioStream *
Cyg_CLibInternalData::get_file_stream( cyg_ucount32 fd )
{
    CYG_PRECONDITION( (fd < FOPEN_MAX),
                  "Attempt to open larger file descriptor than FOPEN_MAX!" );

    switch (fd)
    {
    case 0:
        return stdin;
    case 1:
        return stdout;
    case 2:
        return stderr;
    default:
        return files[fd - 3]; // shift 3 for stdin/stdout/stderr
    } // switch

} // get_file_stream()


inline void
Cyg_CLibInternalData::set_file_stream( cyg_ucount32 fd,
                                       Cyg_StdioStream *stream )
{
    CYG_PRECONDITION( (fd < FOPEN_MAX),
                  "Attempt to set larger file descriptor than FOPEN_MAX!" );

    switch (fd)
    {
    case 0:
        stdin = stream;
        break;
    case 1:
        stdout = stream;
        break;
    case 2:
        stderr = stream;
        break;
    default:
        files[fd - 3] = stream; // shift 3 for stdin/stdout/stderr
        break;
    } // switch
} // set_file_stream()

#endif // ifdef CYGPKG_LIBC_STDIO

inline Cyg_ErrNo *
Cyg_CLibInternalData::get_errno_p( void )
{
#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO
    return &safe_errno;
#else
    return &errno;
#endif
} // get_errno_p()

#endif // ifdef CYGPKG_LIBC     

#endif CYGONCE_LIBC_CLIBINCL_CLIBDATA_INL multiple inclusion protection

// EOF clibdata.inl
