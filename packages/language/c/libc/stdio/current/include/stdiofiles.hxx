#ifndef CYGONCE_LIBC_STDIO_STDIOFILES_HXX
#define CYGONCE_LIBC_STDIO_STDIOFILES_HXX
//========================================================================
//
//      stdiofiles.hxx
//
//      ISO C library stdio central file access
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
// Date:          2000-04-19
// Purpose:     
// Description: 
// Usage:         #include <cyg/libc/stdio/stdiosupp.hxx>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>    // libc stdio configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>          // cyg_bool, cyg_ucount8, cyg_ucount16
#include <cyg/libc/stdio/stream.hxx>     // Cyg_StdioStream

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
# include <cyg/kernel/mutex.hxx>   // mutexes
#endif

// CLASSES

class Cyg_libc_stdio_files
{
    // List of open files - global for now
    static
    Cyg_StdioStream *files[FOPEN_MAX];

# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    // lock for the above
    static
    Cyg_Mutex files_lock;
# endif

public:

# if FOPEN_MAX < 256
    typedef cyg_ucount8 fd_t;
# else
    typedef cyg_ucount16 fd_t;
# endif

    static Cyg_StdioStream *
    get_file_stream( fd_t fd );

    static void
    set_file_stream( fd_t fd, Cyg_StdioStream *stream );


    // the following functions lock(), trylock() and unlock() do nothing
    // if we haven't got thread-safe streams
    static cyg_bool
    lock(void);

    static cyg_bool
    trylock(void);

    static void
    unlock(void);
    
}; // class Cyg_libc_stdio_files


// Inline functions for this class
#include <cyg/libc/stdio/stdiofiles.inl>

#endif // CYGONCE_LIBC_STDIO_STDIOFILES_HXX multiple inclusion protection

// EOF stdiofiles.hxx
