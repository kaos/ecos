#ifndef CYGONCE_MEMALLOC_MEMJOIN_HXX
#define CYGONCE_MEMALLOC_MEMJOIN_HXX

//==========================================================================
//
//      memjoin.hxx
//
//      Pseudo memory pool used to join together other memory pools
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
// Date:         2000-06-12
// Purpose:      Define joined up memory pool class interface
// Description:  Inline class for constructing a pseudo allocator that contains
//               multiple other allocators. It caters solely to the requirements
//               of the malloc implementation.
// Usage:        #include <cyg/memalloc/memjoin.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// CONFIGURATION

#include <pkgconf/memalloc.h>

// INCLUDES

#include <cyg/infra/cyg_type.h>        // types
//#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/memalloc/common.hxx>     // Common memory allocator infra


// TYPE DEFINITIONS

template <class T>
class Cyg_Mempool_Joined
{
protected:
    struct pooldesc {
        const cyg_uint8 *startaddr;
        const cyg_uint8 *endaddr;
        T *pool;
    };
    struct pooldesc *pools;
    cyg_uint8 poolcount;

    T *
    find_pool_for_ptr( const cyg_uint8 * /* ptr */ );

public:
    // Constructor
    Cyg_Mempool_Joined( cyg_uint8 /* num_heaps */, T * /* heaps */[] );

    // Destructor
    ~Cyg_Mempool_Joined();

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
    resize_alloc( cyg_uint8 *alloc_ptr, cyg_int32 newsize,
                  cyg_int32 *oldsize=NULL );

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

#include <cyg/memalloc/memjoin.inl>

#endif // ifndef CYGONCE_MEMALLOC_MEMJOIN_HXX
// EOF memjoin.hxx
