#ifndef CYGONCE_MEMALLOC_MFIXIMPL_HXX
#define CYGONCE_MEMALLOC_MFIXIMPL_HXX

//==========================================================================
//
//      mfiximpl.hxx
//
//      Memory pool with fixed block class declarations
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
// Author(s):    hmt
// Contributors: jlarmour
// Date:         2000-06-12
// Purpose:      Define Mfiximpl class interface
// Description:  Inline class for constructing a fixed block allocator
// Usage:        #include <cyg/memalloc/mfiximpl.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/memalloc/common.hxx>     // Common memory allocator infra

class Cyg_Mempool_Fixed_Implementation {
protected:
    // these constructors are explicitly disallowed
    Cyg_Mempool_Fixed_Implementation() {};
//    Cyg_Mempool_Fixed_Implementation( Cyg_Mempool_Fixed_Implementation &ref )
//    {};
    Cyg_Mempool_Fixed_Implementation &
    operator=( Cyg_Mempool_Fixed_Implementation &ref )
    { return ref; };

    cyg_uint32 *bitmap;
    cyg_int32 maptop;
    cyg_uint8  *mempool;
    cyg_int32 numblocks;
    cyg_int32 freeblocks;
    cyg_int32 blocksize;
    cyg_int32 firstfree;
    cyg_uint8  *top;

public:
    // THIS is the public API of memory pools generally that can have the
    // kernel oriented thread-safe package layer atop.
    //
    // The kernel package is a template whose type parameter is one of
    // these.  That is the reason there are superfluous parameters here and
    // more genereralization than might be expected in a fixed block
    // allocator.

    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out, note that management structures are taken from the
    // same arena.  The alloc_unit may be any other param in general; it
    // comes through from the outer constructor unchanged.
    Cyg_Mempool_Fixed_Implementation(
        cyg_uint8 *base,
        cyg_int32 size,
        CYG_ADDRWORD alloc_unit );

    // Destructor
    ~Cyg_Mempool_Fixed_Implementation();

    // get some memory; size is ignored in a fixed block allocator
    cyg_uint8 *try_alloc( cyg_int32 size );
    
    // supposedly resize existing allocation. This is defined in the
    // fixed block allocator purely for API consistency. It will return
    // an error (false) for all values, except for the blocksize
    // returns true on success
    cyg_uint8 *
    resize_alloc( cyg_uint8 *alloc_ptr, cyg_int32 newsize,
                  cyg_int32 *oldsize=NULL );

    // free the memory back to the pool; size ignored here
    cyg_bool free( cyg_uint8 *p, cyg_int32 size );

    // Get memory pool status
    // flags is a bitmask of requested fields to fill in. The flags are
    // defined in common.hxx
    void get_status( cyg_mempool_status_flag_t /* flags */,
                     Cyg_Mempool_Status & /* status */ );

};

#include <cyg/memalloc/mfiximpl.inl>

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_MEMALLOC_MFIXIMPL_HXX
// EOF mfiximpl.hxx
