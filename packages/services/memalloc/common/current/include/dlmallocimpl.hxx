#ifndef CYGONCE_MEMALLOC_DLMALLOCIMPL_HXX
#define CYGONCE_MEMALLOC_DLMALLOCIMPL_HXX

//==========================================================================
//
//      dlmallocimpl.hxx
//
//      Interface to the port of Doug Lea's malloc implementation
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-06-18
// Purpose:      Define standard interface to Doug Lea's malloc implementation
// Description:  Doug Lea's malloc has been ported to eCos. This file provides
//               the interface between the implementation and the standard
//               memory allocator interface required by eCos
// Usage:        #include <cyg/memalloc/dlmalloc.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// CONFIGURATION

#include <pkgconf/memalloc.h>

// INCLUDES

#include <stddef.h>                    // size_t, ptrdiff_t
#include <cyg/infra/cyg_type.h>        // types

#include <cyg/memalloc/common.hxx>     // Common memory allocator infra

// As a special case, override CYGIMP_MEMALLOC_ALLOCATOR_DLMALLOC_SAFE_MULTIPLE
// if the malloc config says so
#ifdef CYGIMP_MEMALLOC_MALLOC_DLMALLOC
// forward declaration to prevent header dependency problems
class Cyg_Mempool_dlmalloc;
# include <pkgconf/heaps.hxx>
# if (CYGMEM_HEAP_COUNT > 1) && \
     !defined(CYGIMP_MEMALLOC_ALLOCATOR_DLMALLOC_SAFE_MULTIPLE)
#  define CYGIMP_MEMALLOC_ALLOCATOR_DLMALLOC_SAFE_MULTIPLE 1
# endif
#endif

// CONSTANTS

// number of bins - but changing this alone will not change the number of
// bins!
#define CYGPRI_MEMALLOC_ALLOCATOR_DLMALLOC_NAV 128

// TYPE DEFINITIONS


class Cyg_Mempool_dlmalloc_Implementation
{
public:
    /* cyg_dlmalloc_size_t is the word-size used for internal bookkeeping
       of chunk sizes. On a 64-bit machine, you can reduce malloc
       overhead, especially for very small chunks, by defining
       cyg_dlmalloc_size_t to be a 32-bit type at the expense of not
       being able to handle requests greater than 2^31. This limitation is
       hardly ever a concern; you are encouraged to set this. However, the
       default version is the same as size_t. */

    typedef size_t Cyg_dlmalloc_size_t;
    
    typedef struct malloc_chunk
    {
        Cyg_dlmalloc_size_t prev_size; /* Size of previous chunk (if free). */
        Cyg_dlmalloc_size_t size;      /* Size in bytes, including overhead. */
        struct malloc_chunk* fd;   /* double links -- used only if free. */
        struct malloc_chunk* bk;
    };
    
protected:
    /* The first value returned from sbrk */
    cyg_uint8 *arenabase;

    /* The total memory in the pool */
    cyg_int32 arenasize;

#ifdef CYGIMP_MEMALLOC_ALLOCATOR_DLMALLOC_SAFE_MULTIPLE
    struct Cyg_Mempool_dlmalloc_Implementation::malloc_chunk *
    av_[ CYGPRI_MEMALLOC_ALLOCATOR_DLMALLOC_NAV * 2 + 2 ];
#endif

#ifdef CYGDBG_MEMALLOC_ALLOCATOR_DLMALLOC_DEBUG

    void
    do_check_chunk( struct malloc_chunk * );

    void
    do_check_free_chunk( struct malloc_chunk * );
    
    void
    do_check_inuse_chunk( struct malloc_chunk * );

    void
    do_check_malloced_chunk( struct malloc_chunk *, Cyg_dlmalloc_size_t );
#endif
    
public:
    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out, note that management structures are taken from the
    // same arena.
    Cyg_Mempool_dlmalloc_Implementation( cyg_uint8 *  /* base */,
                                         cyg_int32    /* size */,
                                         CYG_ADDRWORD /* argthru */ );

    // Destructor
    ~Cyg_Mempool_dlmalloc_Implementation() {}

    // get some memory, return NULL if none available
    cyg_uint8 *
    try_alloc( cyg_int32 /* size */ );
    
    // resize existing allocation, if oldsize is non-NULL, previous
    // allocation size is placed into it. If previous size not available,
    // it is set to 0. NB previous allocation size may have been rounded up.
    // Occasionally the allocation can be adjusted *backwards* as well as,
    // or instead of forwards, therefore the address of the resized
    // allocation is returned, or NULL if no resizing was possible.
    // Note that this differs from ::realloc() in that no attempt is
    // made to call malloc() if resizing is not possible - that is left
    // to higher layers. The data is copied from old to new though.
    // The effects of alloc_ptr==NULL or newsize==0 are undefined
    cyg_uint8 *
    resize_alloc( cyg_uint8 * /* alloc_ptr */, cyg_int32 /* newsize */,
                  cyg_int32 * /* oldsize */ );

    // free the memory back to the pool
    // returns true on success
    cyg_bool
    free( cyg_uint8 * /* ptr */, cyg_int32 /* size */ =0 );

    // Get memory pool status
    // flags is a bitmask of requested fields to fill in. The flags are
    // defined in common.hxx
    void
    get_status( cyg_mempool_status_flag_t /* flags */,
                Cyg_Mempool_Status & /* status */ );

};

#endif // ifndef CYGONCE_MEMALLOC_DLMALLOCIMPL_HXX
// EOF dlmallocimpl.hxx
