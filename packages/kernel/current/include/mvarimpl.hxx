#ifndef CYGONCE_KERNEL_MVARIMPL_HXX
#define CYGONCE_KERNEL_MVARIMPL_HXX

//==========================================================================
//
//	mvarimpl.hxx
//
//	Memory pool with variable block class declarations
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	dsm
// Contributors:	dsm
// Date:	1998-05-21
// Purpose:	Define Mvarimpl class interface
// Description:	Inline class for constructing a variable block allocator
// Usage:	#include <cyg/kernel/mvarimpl.hxx>
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/thread.hxx>

class Cyg_Mempool_Variable_Implementation {
private:
    // these constructors are explicitly disallowed
    Cyg_Mempool_Variable_Implementation() {};
//    Cyg_Mempool_Variable_Implementation( Cyg_Mempool_Variable_Implementation &ref )
//    {};
    Cyg_Mempool_Variable_Implementation &
    operator=( Cyg_Mempool_Variable_Implementation &ref )
    { return ref; };

private:
    struct memdq {
        struct memdq *prev, *next;
        cyg_int32 size;
    };

    struct memdq head;
    cyg_uint8  *obase;
    cyg_int32  osize;
    cyg_int32  oalign;
    cyg_uint8  *bottom;
    cyg_uint8  *top;
    cyg_int32  alignment;
    cyg_int32  freemem;

    // round up size passed to alloc/free to a size that will be used
    // for allocation
    cyg_int32
    roundup(cyg_int32 size);

public:
    // THIS is the public API of memory pools generally that can have the
    // kernel oriented thread-safe package layer atop.

    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out.
    Cyg_Mempool_Variable_Implementation(
        cyg_uint8 *  /* base */,
        cyg_int32    /* size */,
        CYG_ADDRWORD /* alignment */);

    // Destructor
    ~Cyg_Mempool_Variable_Implementation();

    // get size bytes of memory
    inline cyg_uint8 *
    alloc( cyg_int32 /* size */ );
    
    // free size bytes of memory back to the pool
    inline cyg_bool
    free( cyg_uint8 * /* ptr */,
          cyg_int32   /* size */ );

    // returns -1 as not variable size
    inline cyg_int32
    get_blocksize( void ) { return -1; }

    // these two are obvious and generic
    inline cyg_int32
    get_totalmem( void ) { return top-bottom; }

    inline cyg_int32
    get_freemem( void ) { return freemem; }

    // get information about the construction parameters for external
    // freeing after the destruction of the holding object
    inline void
    get_arena( cyg_uint8 * &  /* base */,
               cyg_int32 &    /* size */,
               CYG_ADDRWORD & /* maxfree */ );

    // Return the size of the memory allocation (previously returned 
    // by alloc() or try_alloc() ) at ptr. Returns -1 if not found
    inline cyg_int32
    get_allocation_size( cyg_uint8 * /* ptr */ );

};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MVARIMPL_HXX
// EOF mvarimpl.hxx
