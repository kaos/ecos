#ifndef CYGONCE_KERNEL_MVARIMPL_INL
#define CYGONCE_KERNEL_MVARIMPL_INL

//==========================================================================
//
//      mvarimpl.inl
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
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-03-23
// Purpose:     Define Mvarimpl class interface
// Description: Inline class for constructing a variable block allocator
// Usage:       #include <cyg/kernel/mvarimpl.hxx>
//              #include <cyg/kernel/mvarimpl.inl>      
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Simple non-coalescing allocator

// The free list is stored on a doubly linked list, each member of
// which is stored in the body of the free memory.  The head of the
// list has the same structure but its size field is zero.  This
// resides in the memory pool structure.  Always having at least one
// item on the list simplifies the alloc and free code.

// 
inline cyg_int32
Cyg_Mempool_Variable_Implementation::roundup( cyg_int32 size )
{

    size += sizeof(struct memdq);
    size = (size + alignment - 1) & -alignment;
    return size;
}

Cyg_Mempool_Variable_Implementation::Cyg_Mempool_Variable_Implementation(
        cyg_uint8 *base,
        cyg_int32 size,
        CYG_ADDRWORD align )
{
    CYG_ASSERT( align > 0, "Bad alignment" );
    CYG_ASSERT( size > 0, "Bad size" );
    CYG_ASSERT(0!=align ,"align is zero");
    CYG_ASSERT(0==(align & align-1),"align not a power of 2");

    obase=base;
    osize=size;
    oalign=align;

    alignment = align;
    while(alignment < (cyg_int32)sizeof(struct memdq))
        alignment += alignment;
    CYG_ASSERT(0==(alignment & alignment-1),"alignment not a power of 2");

    bottom = (cyg_uint8 *)roundup((cyg_int32)base);
    top = (cyg_uint8 *)((cyg_int32)(base+size) & -alignment);
    
    CYG_ASSERT( top > bottom , "heap too small" );
    CYG_ASSERT( ((cyg_int32)bottom & alignment-1)==0, "bottom badly aligned" );
    CYG_ASSERT( ((cyg_int32)top & alignment-1)==0, "top badly aligned" );

    struct memdq *hdq = &head, *dq = (struct memdq *)bottom;
    
    hdq->prev = hdq->next = dq;
    hdq->size = 0;
    dq->prev = dq->next = hdq;

    freemem = dq->size = top-bottom;
}

Cyg_Mempool_Variable_Implementation::~Cyg_Mempool_Variable_Implementation()
{
}

// allocation is simple
// First we look down the free list for a large enough block
// If we find a block the right size, we unlink the block from
//    the free list and return a pointer to it.
// If we find a larger block, we chop a piece off the end
//    and return that
// Otherwise we will eventually get back to the head of the list
//    and return NULL
inline cyg_uint8 *
Cyg_Mempool_Variable_Implementation::alloc( cyg_int32 size )
{
    struct memdq *dq = &head;
    cyg_uint8 *alloced;

    size = roundup(size);

    do {
        CYG_ASSERT( dq->next->prev==dq, "Bad link in dq");
        dq = dq->next;
        if(0 == dq->size) {
            CYG_ASSERT(dq == &head, "bad free block");
            return NULL;
        }
    } while(dq->size < size);

    if( size == dq->size ) {
        // exact fit -- unlink from free list
        dq->prev->next = dq->next;
        dq->next->prev = dq->prev;
        alloced = (cyg_uint8 *)dq;
    } else {

        CYG_ASSERT( dq->size > size, "block found is too small");

        // allocate portion of memory from end of block
        
        dq->size -=size;

        // The portion left over has to be large enough to store a
        // struct memdq.  This is guaranteed because the alignment is
        // larger than the size of this structure.

        CYG_ASSERT( (cyg_int32)sizeof(struct memdq)<=dq->size ,
                "not enough space for list item" );

        alloced = (cyg_uint8 *)dq + dq->size;
    }

    CYG_ASSERT( bottom<=alloced && alloced<=top, "alloced outside pool" );

    // Set size on allocated block

    dq = (struct memdq *)alloced;
    dq->size = size;
    dq->next = dq->prev = (struct memdq *)0xd530d53; // magic number

    freemem -=size;
    return alloced + sizeof(struct memdq);
}

// As no coalescing is done, free is simply a matter of using the
// freed memory as an element of the free list linking it in at the
// start.
    
inline cyg_bool
Cyg_Mempool_Variable_Implementation::free( cyg_uint8 *p, cyg_int32 size )
{
    if(!(bottom<=p&&p<=top))
        return false;
    
    struct memdq *hdq=&head,*dq=(struct memdq *)(p-sizeof(struct memdq));

    // check magic number in block to be freed
    if(dq->next != dq->prev || dq->next != (struct memdq *)0xd530d53)
        return false;

    if(0==size) {
        size = dq->size;
    } else {
        size = roundup(size);
    }

    if(dq->size != size)
        return false;

    CYG_ASSERT( (cyg_int32)sizeof(struct memdq)<=size ,
                "not enough space for list item" );

#ifdef CYGSEM_KERNEL_MEMORY_COALESCE
// For simple coalescing have the free list be sorted by memory base address
    struct memdq *idq;
    
    for (idq = hdq->next; idq != hdq; idq = idq->next) {
        if (idq->next > dq)
            break;
    }
    dq->size = size;
    if (idq != hdq) {
        dq->prev = idq;
        dq->next = idq->next;
        idq->next = dq;
        dq->next->prev = dq;
    } else {
        dq->next = idq;
        dq->prev = idq->prev;
        idq->prev = dq;
        dq->prev->next = dq;
    }
    // Now do coalescing
    if ((char *)dq + dq->size == (char *)dq->next) {
        dq->size += dq->next->size;
        dq->next = dq->next->next;
        dq->next->prev = dq;
    }
    if ((char *)dq->prev + dq->prev->size == (char *)dq) {
        dq->prev->size += dq->size;
        dq->prev->next = dq->next;
        dq->next->prev = dq->prev;
        dq = dq->prev;
    }   
#else
    dq->prev = hdq;
    dq->next = hdq->next;
    dq->size = size;
    hdq->next = dq;
    dq->next->prev=dq;
#endif

    freemem +=size;
    return true;
}    


inline void
Cyg_Mempool_Variable_Implementation::get_arena(
    cyg_uint8 * &base,
    cyg_int32 &size,
    CYG_ADDRWORD &maxfree)
{
    struct memdq *dq = &head;
    cyg_int32 mf = 0;

    do {
        CYG_ASSERT( dq->next->prev==dq, "Bad link in dq");
        dq = dq->next;
        if(0 == dq->size) {
            CYG_ASSERT(dq == &head, "bad free block");
            break;
        }
        if(dq->size > mf)
            mf = dq->size;
    } while(1);

    base = obase;
    size = osize;
    maxfree = mf;
}


inline cyg_int32
Cyg_Mempool_Variable_Implementation::get_allocation_size( cyg_uint8 *ptr )
{
    CYG_CHECK_DATA_PTR(ptr, "Requested allocation size of bad data pointer!");

    if(!(bottom<=ptr && ptr<=top))
        return -1;
    
    struct memdq *dq=(struct memdq *)(ptr-sizeof(struct memdq));

    // check magic number in block to be freed
    if(dq->next != dq->prev || dq->next != (struct memdq *)0xd530d53)
        return -1;

    return dq->size;
    
} // get_allocation_size()

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MVARIMPL_INL
// EOF mvarimpl.inl
