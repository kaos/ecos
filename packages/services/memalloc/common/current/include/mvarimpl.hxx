#ifndef CYGONCE_MEMALLOC_MVARIMPL_HXX
#define CYGONCE_MEMALLOC_MVARIMPL_HXX

//==========================================================================
//
//      mvarimpl.hxx
//
//      Memory pool with variable block class declarations
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
// Author(s):    dsm, jlarmour
// Contributors: 
// Date:         2000-06-12
// Purpose:      Define Mvarimpl class interface
// Description:  Inline class for constructing a variable block allocator
// Usage:        #include <cyg/memalloc/mvarimpl.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#include <cyg/infra/cyg_type.h>
#include <pkgconf/memalloc.h>
#include <cyg/memalloc/common.hxx>     // Common memory allocator infra

class Cyg_Mempool_Variable_Implementation {
protected:
    // these constructors are explicitly disallowed
    Cyg_Mempool_Variable_Implementation() {};
//    Cyg_Mempool_Variable_Implementation( Cyg_Mempool_Variable_Implementation &ref )
//    {};
    Cyg_Mempool_Variable_Implementation &
    operator=( Cyg_Mempool_Variable_Implementation &ref )
    { return ref; };

    struct memdq {
        struct memdq *prev, *next;
        cyg_int32 size;
    };

    struct memdq head;
    cyg_uint8  *obase;
    cyg_int32  osize;
    cyg_uint8  *bottom;
    cyg_uint8  *top;
    cyg_int32  alignment;
    cyg_int32  freemem;

    // round up size passed to alloc/free to a size that will be used
    // for allocation
    cyg_int32
    roundup(cyg_int32 size);

    struct memdq *
    addr2memdq( cyg_uint8 *addr );

    struct memdq *
    alloc2memdq( cyg_uint8 *addr );

    cyg_uint8 *
    memdq2alloc( struct memdq *dq );

    void
    insert_free_block( struct memdq *freedq );

public:
    // THIS is the public API of memory pools generally that can have the
    // kernel oriented thread-safe package layer atop.

    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out.
    Cyg_Mempool_Variable_Implementation(
        cyg_uint8 *  /* base */,
        cyg_int32    /* size */,
        CYG_ADDRWORD /* alignment */ = 8 );

    // Destructor
    ~Cyg_Mempool_Variable_Implementation();

    // get size bytes of memory
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
                  cyg_int32 *oldsize );

    // free size bytes of memory back to the pool
    // returns true on success
    cyg_bool
    free( cyg_uint8 * /* ptr */,
          cyg_int32   /* size */ );

    // Get memory pool status
    // flags is a bitmask of requested fields to fill in. The flags are
    // defined in common.hxx
    void
    get_status( cyg_mempool_status_flag_t /* flags */,
                Cyg_Mempool_Status & /* status */ );
    
};

#include <cyg/memalloc/mvarimpl.inl>

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_MEMALLOC_MVARIMPL_HXX
// EOF mvarimpl.hxx
