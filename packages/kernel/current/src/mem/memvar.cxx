//==========================================================================
//
//	memvar.cxx
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
// Date:	1998-05-22
// Description:	
// Usage:	#include <cyg/kernel/memvar.hxx>
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

#include <cyg/kernel/memvar.hxx>

// implementation that we want to make concrete
#ifdef CYGIMP_MEM_USE_MEMPOOLT_PLAIN
#include <cyg/kernel/mempoolt.inl>     // kernel safe mempool template
#else
#include <cyg/kernel/mempolt2.inl>     // kernel safe mempool template
#endif

#include <cyg/kernel/mvarimpl.inl>     // mem pool implementation

// -------------------------------------------------------------------------
// debugging/assert function

#ifdef CYGDBG_USE_ASSERTS
cyg_bool
Cyg_Mempool_Variable::check_this(cyg_assert_class_zeal zeal)
{
    CYG_REPORT_FUNCTION();
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    return mypool.check_this( zeal );
}
#endif

// -------------------------------------------------------------------------
// Constructor: gives the base and size of the arena in which memory is
// to be carved out
Cyg_Mempool_Variable::Cyg_Mempool_Variable(
    cyg_uint8 *base,
    cyg_int32 size)
    : mypool( base, size, (CYG_ADDRWORD)4 )
{
}

// Destructor
Cyg_Mempool_Variable::~Cyg_Mempool_Variable()
{
}

// -------------------------------------------------------------------------
// get some memory; wait if none available
cyg_uint8 *
Cyg_Mempool_Variable::alloc(cyg_int32 size)
{
    return mypool.alloc( size );
}
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
// get some memory with a timeout
cyg_uint8 *
Cyg_Mempool_Variable::alloc(cyg_int32 size, cyg_tick_count delay_timeout )
{
    return mypool.alloc( size , delay_timeout );
}
#endif

// get some memory, return NULL if none available
cyg_uint8 *
Cyg_Mempool_Variable::try_alloc(cyg_int32 size)
{
    return mypool.try_alloc( size );
}
    
// free the memory back to the pool
cyg_bool 
Cyg_Mempool_Variable::free( cyg_uint8 *p, cyg_int32 size )
{
    return mypool.free( p, size );
}

// is anyone waiting for memory?
cyg_bool 
Cyg_Mempool_Variable::waiting()
{
    return mypool.waiting();
}

// read the fixed allocation size (will be -1 as variable)
cyg_int32 
Cyg_Mempool_Variable::get_blocksize()
{
    return mypool.get_blocksize();
}

// these two are obvious and generic
cyg_int32 
Cyg_Mempool_Variable::get_totalmem() {
    return mypool.get_totalmem();
}
cyg_int32 
Cyg_Mempool_Variable::get_freemem() {
    return mypool.get_freemem();
}

// get information about the construction parameters for external
// freeing after the destruction of the holding object
void 
Cyg_Mempool_Variable::get_arena(
    cyg_uint8 * &base, cyg_int32 &size, CYG_ADDRWORD &maxfree  )
{
    mypool.get_arena( base, size, maxfree );
}

// Return the size of the memory allocation (previously returned 
// by alloc() or try_alloc() ) at ptr. Returns -1 if not found
cyg_int32
Cyg_Mempool_Variable::get_allocation_size( cyg_uint8 *ptr )
{
    return mypool.get_allocation_size( ptr );
}

// -------------------------------------------------------------------------

// End of memvar.cxx
