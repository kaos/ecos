//==========================================================================
//
//      memfixed.cxx
//
//      Memory pool with fixed block class declarations
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-03-23
// Purpose:     Define Memfixed class interface
// Description: Inline class for constructing a fixed block allocator
// Usage:       #include <cyg/kernel/memfixed.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/memfixed.hxx>

// implementation that we want to make concrete
#ifdef CYGIMP_MEM_USE_MEMPOOLT_PLAIN
#include <cyg/kernel/mempoolt.inl>     // kernel safe mempool template
#else
// tell it to optimize for the fixed block one-to-one case
#define CYGIMP_MEM_T_ONEFREE_TO_ONEALLOC
#include <cyg/kernel/mempolt2.inl>     // kernel safe mempool template
#endif

#include <cyg/kernel/mfiximpl.inl>     // implementation of a fixed mem pool

// -------------------------------------------------------------------------
// debugging/assert function

#ifdef CYGDBG_USE_ASSERTS
cyg_bool
Cyg_Mempool_Fixed::check_this(cyg_assert_class_zeal zeal) const
{
    CYG_REPORT_FUNCTION();
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    return mypool.check_this( zeal );
}
#endif

// -------------------------------------------------------------------------
// Constructor: gives the base and size of the arena in which memory is
// to be carved out, note that management structures are taken from the
// same arena.  Alloc_unit is the blocksize allocated.
Cyg_Mempool_Fixed::Cyg_Mempool_Fixed(
    cyg_uint8 *base,
    cyg_int32 size,
    CYG_ADDRWORD alloc_unit )
    : mypool( base, size, alloc_unit )
{
}

// Destructor
Cyg_Mempool_Fixed::~Cyg_Mempool_Fixed()
{
}

// -------------------------------------------------------------------------
// get some memory; wait if none available
cyg_uint8 *
Cyg_Mempool_Fixed::alloc()
{
    return mypool.alloc( 0 );
}
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
// get some memory with a timeout
cyg_uint8 *
Cyg_Mempool_Fixed::alloc(cyg_tick_count delay_timeout )
{
    return mypool.alloc( 0, delay_timeout );
}
#endif

// get some memory, return NULL if none available
cyg_uint8 *
Cyg_Mempool_Fixed::try_alloc()
{
    return mypool.try_alloc( 0 );
}
    
// free the memory back to the pool
cyg_bool 
Cyg_Mempool_Fixed::free( cyg_uint8 *p )
{
    return mypool.free( p, 0 );
}

// is anyone waiting for memory?
cyg_bool
Cyg_Mempool_Fixed::waiting()
{
    return mypool.waiting();
}

// read the fixed allocation size
cyg_int32 
Cyg_Mempool_Fixed::get_blocksize()
{
    return mypool.get_blocksize();
}

// these two are obvious and generic
cyg_int32 
Cyg_Mempool_Fixed::get_totalmem() {
    return mypool.get_totalmem();
}
cyg_int32 
Cyg_Mempool_Fixed::get_freemem() {
    return mypool.get_freemem();
}

// get information about the construction parameters for external
// freeing after the destruction of the holding object
void 
Cyg_Mempool_Fixed::get_arena(
    cyg_uint8 * &base, cyg_int32 &size, CYG_ADDRWORD &alloc  )
{
    mypool.get_arena( base, size, alloc );  
}

// Return the size of the memory allocation (previously returned 
// by alloc() or try_alloc() ) at ptr. Returns -1 if not found
cyg_int32
Cyg_Mempool_Fixed::get_allocation_size( cyg_uint8 *ptr )
{
    // can only be one size for the fixed block allocator
    return mypool.get_blocksize();
}
// -------------------------------------------------------------------------

// End of memfixed.cxx
