#ifndef CYGONCE_LIBC_CLIBINCL_CLIBDATA_HXX
#define CYGONCE_LIBC_CLIBINCL_CLIBDATA_HXX
//========================================================================
//
//      clibdata.hxx
//
//      C library internal data - held in one place to make thread support
//      easier
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
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include "clibincl/clibdata.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

#include <cyg/infra/cyg_type.h>      // Common project-wide type definitions
#include <errno.h>                   // For errno type

#ifdef CYGSEM_LIBC_THREAD_SAFETY
# include <cyg/kernel/thread.hxx>     // Kernel thread support
# include <cyg/kernel/thread.inl>     // and inline functions
#endif

#ifdef CYGPKG_LIBC_STDIO
# include "clibincl/stdiosupp.hxx"    // _fopen() declarations
# include "clibincl/stream.hxx"       // Cyg_StdioStream internal class
#endif

#ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
# include <cyg/kernel/mutex.hxx>
#endif


// TYPE DEFINITIONS

class Cyg_CLibInternalData
{
#ifdef CYGPKG_LIBC_STDIO    
    // allow fopen only to access the files
    friend FILE *_fopen( const char *filename, const char *mode );
    friend int _fclose( FILE *stream );
#endif    

private:

#ifdef CYGPKG_LIBC_STDIO
    // the streams are separated for compatibility with newlib

    static    // Make the following members static
    Cyg_StdioStream *stdin, *stdout, *stderr;  // default I/O streams


    // List of open files
    static    // Make the following member static (and initialises to NULL)
    Cyg_StdioStream *files[FOPEN_MAX - 3]; // subtract 3 for stdin, stdout,
                                           // and stderr
# ifdef CYGSEM_LIBC_STDIO_THREAD_SAFE_STREAMS
    // lock for the above
    static
    Cyg_Mutex files_lock;
# endif

#endif // ifdef CYGPKG_LIBC_STDIO


# ifdef CYGSEM_LIBC_PER_THREAD_ERRNO
    Cyg_ErrNo safe_errno;
# endif // ifdef CYGSEM_LIBC_PER_THREAD_ERRNO


#ifdef CYGFUN_LIBC_strtok
# ifndef CYGSEM_LIBC_PER_THREAD_STRTOK
    static    // Make the following member static
# endif
    // last saved position of strtok()
    char *strtok_last;
#endif // ifdef CYGFUN_LIBC_strtok


#ifdef CYGPKG_LIBC_RAND

    typedef cyg_ucount32 Cyg_rand_seed_t;

# ifndef CYGSEM_LIBC_PER_THREAD_RAND
    static    // Make the following member static
# endif
    // random seed to be used with rand()
    Cyg_rand_seed_t rand_seed;
#endif // ifdef CYGPKG_LIBC_RAND


public:

    // Normal constructor
    Cyg_CLibInternalData( void );

    // same as above but allows the constructor to be invoked explicitly
    // when needed, when it wouldn't otherwise be invoked
    void
    construct( void );

#ifdef CYGFUN_LIBC_strtok
    char **
    get_strtok_last_p( void );
#endif

#ifdef CYGPKG_LIBC_RAND
    Cyg_rand_seed_t *
    get_rand_seed_p( void );
#endif

#ifdef CYGPKG_LIBC_STDIO
    static Cyg_StdioStream *
    get_file_stream( cyg_ucount32 fd );

    static void
    set_file_stream( cyg_ucount32 fd, Cyg_StdioStream *stream );

#endif // ifdef CYGPKG_LIBC_STDIO

    Cyg_ErrNo *
    get_errno_p( void );

}; // class Cyg_CLibInternalData


// MACROS

// CYGPRI_LIBC_INTERNAL_DATA_PREAMBLE is used at the start of a function in
// case any "magic" at the start is required for this implementation to get
// at the data. Or in some cases, it may just be helpful for speed.
//
// CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE is used at the "upper"
// entry points to the C library, to ensure that the internal data is
// allocated. It also does what CYGPRI_LIBC_INTERNAL_DATA_PREAMBLE does.
// 
// CYGPRI_LIBC_INTERNAL_DATA is used to get at the C library internal data,
// available when the above two macros have been invoked.
//
// These macros pollute the namespace of the functions they are invoked in,
// but these should only be used internally by the C library, so that is
// fine.

#ifdef CYGSEM_LIBC_THREAD_SAFETY

#define CYGPRI_LIBC_INTERNAL_DATA_PREAMBLE                                \
    Cyg_CLibInternalData *cyg_tsd_xxx =                                   \
      ( (Cyg_CLibInternalData *)(Cyg_Thread::self()->                     \
                              get_data(CYGNUM_KERNEL_THREADS_DATA_LIBC)) )


#define CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE                    \
    Cyg_Thread *cyg_self_xxx = Cyg_Thread::self();                        \
    Cyg_CLibInternalData *cyg_tsd_xxx;                                    \
    if ( cyg_self_xxx->get_data(CYGNUM_KERNEL_THREADS_DATA_LIBC) == 0 )   \
    {                                                                     \
        cyg_tsd_xxx = (Cyg_CLibInternalData *)cyg_self_xxx->              \
                      increment_stack_limit(sizeof(Cyg_CLibInternalData));\
        cyg_self_xxx->set_data( CYGNUM_KERNEL_THREADS_DATA_LIBC,          \
                                (CYG_ADDRWORD) cyg_tsd_xxx );             \
        cyg_tsd_xxx->construct();                                         \
    }                                                                     \
    else                                                                  \
        cyg_tsd_xxx = ( (Cyg_CLibInternalData *)(Cyg_Thread::self()->     \
                              get_data(CYGNUM_KERNEL_THREADS_DATA_LIBC)) )


#define CYGPRI_LIBC_INTERNAL_DATA (*cyg_tsd_xxx)

#define CYGPRI_LIBC_INTERNAL_DATA_CHECK                                   \
    CYG_ASSERT( Cyg_Thread::self()->                                      \
                   get_data(CYGNUM_KERNEL_THREADS_DATA_LIBC) != 0,        \
                "C library internal data has not been initialised!" )

#else // ifdef CYGSEM_LIBC_THREAD_SAFETY

extern Cyg_CLibInternalData cyg_my_clib_internal_data;

#define CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE CYG_EMPTY_STATEMENT

#define CYGPRI_LIBC_INTERNAL_DATA_PREAMBLE CYG_EMPTY_STATEMENT

#define CYGPRI_LIBC_INTERNAL_DATA cyg_my_clib_internal_data

#define CYGPRI_LIBC_INTERNAL_DATA_CHECK CYG_EMPTY_STATEMENT

#endif // ifdef CYGSEM_LIBC_THREAD_SAFETY


// Inline functions for this class
#include "clibincl/clibdata.inl"


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CLIBINCL_CLIBDATA_HXX multiple inclusion protection

// EOF clibdata.hxx
