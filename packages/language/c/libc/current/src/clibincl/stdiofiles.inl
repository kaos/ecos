#ifndef CYGONCE_LIBC_STDIOFILES_INL
#define CYGONCE_LIBC_STDIOFILES_INL
//========================================================================
//
//      stdiofiles.inl
//
//      ISO C library stdio central file inlines
//
//========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:     
// Description: 
// Usage:         Do not include this file directly. Instead use:
//                #include "clibincl/stdiosupp.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

#ifdef CYGPKG_LIBC_STDIO

// INCLUDES

#include <cyg/infra/cyg_type.h>    // cyg_bool
#include <cyg/infra/cyg_ass.h>     // Assert interface
#include "clibincl/stdiofiles.hxx" // header for this file
#include "clibincl/stream.hxx"     // Cyg_StdioStream

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// INLINE METHODS

inline Cyg_StdioStream *
Cyg_libc_stdio_files::get_file_stream( fd_t fd )
{
    CYG_PRECONDITION( (fd < FOPEN_MAX),
                  "Attempt to open larger file descriptor than FOPEN_MAX!" );

    return files[fd];

} // Cyg_libc_stdio_files::get_file_stream()
            
inline void
Cyg_libc_stdio_files::set_file_stream( fd_t fd, Cyg_StdioStream *stream )
{
    CYG_PRECONDITION( (fd < FOPEN_MAX),
                  "Attempt to set larger file descriptor than FOPEN_MAX!" );

    files[fd] = stream;

} // Cyg_libc_stdio_files::set_file_stream()


inline cyg_bool
Cyg_libc_stdio_files::lock(void)
{
# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    return files_lock.lock();
# else
    return true;
# endif
} // Cyg_libc_stdio_files::lock()

inline cyg_bool
Cyg_libc_stdio_files::trylock(void)
{
# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    return files_lock.trylock();
# else
    return true;
# endif
} // Cyg_libc_stdio_files::trylock()

inline void
Cyg_libc_stdio_files::unlock(void)
{
# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    files_lock.unlock();
# endif
} // Cyg_libc_stdio_files::unlock()


#endif // ifdef CYGPKG_LIBC_STDIO

#endif // CYGONCE_LIBC_STDIOFILES_INL multiple inclusion protection

// EOF stdiofiles.inl
