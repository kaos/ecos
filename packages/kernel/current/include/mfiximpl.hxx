#ifndef CYGONCE_KERNEL_MFIXIMPL_HXX
#define CYGONCE_KERNEL_MFIXIMPL_HXX

//==========================================================================
//
//	mfiximpl.hxx
//
//	Memory pool with fixed block class declarations
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
// Author(s): 	hmt
// Contributors:	hmt
// Date:	1998-03-23
// Purpose:	Define Mfiximpl class interface
// Description:	Inline class for constructing a fixed block allocator
// Usage:	#include <cyg/kernel/mfiximpl.hxx>
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/thread.hxx>

class Cyg_Mempool_Fixed_Implementation {
private:
    // these constructors are explicitly disallowed
    Cyg_Mempool_Fixed_Implementation() {};
//    Cyg_Mempool_Fixed_Implementation( Cyg_Mempool_Fixed_Implementation &ref )
//    {};
    Cyg_Mempool_Fixed_Implementation &
    operator=( Cyg_Mempool_Fixed_Implementation &ref )
    { return ref; };

private:
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
    inline cyg_uint8 *alloc( cyg_int32 size );
    
    // free the memory back to the pool; size ignored here
    inline cyg_bool free( cyg_uint8 *p, cyg_int32 size );

    // if applicable: return -1 if not fixed size
    inline cyg_int32 get_blocksize() { return blocksize; }

    // these two are obvious and generic
    inline cyg_int32 get_totalmem() { return blocksize * numblocks; }
    inline cyg_int32 get_freemem() { return blocksize * freeblocks; }

    // get information about the construction parameters for external
    // freeing after the destruction of the holding object
    inline void get_arena( cyg_uint8 * &base, cyg_int32 &size, CYG_ADDRWORD &alloc  )
    {
        base = (cyg_uint8 *)bitmap;
        size = top - base;
        alloc = blocksize;
    }

};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MFIXIMPL_HXX
// EOF mfiximpl.hxx
