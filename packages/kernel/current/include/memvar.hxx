#ifndef CYGONCE_KERNEL_MEMVAR_HXX
#define CYGONCE_KERNEL_MEMVAR_HXX

//==========================================================================
//
//      memvar.hxx
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
// Author(s):   dsm
// Contributors:        dsm
// Date:        1998-05-21
// Purpose:     Define Memvar class interface
// Description: Inline class for constructing a variable block allocator
// Usage:       #include <cyg/kernel/memvar.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>         // kernel types
#include <cyg/infra/cyg_ass.h>         // assertion macros

#ifdef CYGIMP_MEM_USE_MEMPOOLT_PLAIN
#include <cyg/kernel/mempoolt.hxx>     // kernel safe mempool template
#else
#include <cyg/kernel/mempolt2.hxx>     // kernel safe mempool template
#endif

#include <cyg/kernel/mvarimpl.hxx>     // implementation of a variable mem pool

class Cyg_Mempool_Variable {
private:
#ifdef CYGIMP_MEM_USE_MEMPOOLT_PLAIN
    Cyg_Mempoolt<Cyg_Mempool_Variable_Implementation> mypool;
#else
    Cyg_Mempolt2<Cyg_Mempool_Variable_Implementation> mypool;
#endif

public:
    // this API makes concrete a class which implements a thread-safe
    // kernel-savvy memory pool which manages variable size blocks.

    CYGDBG_DEFINE_CHECK_THIS
    
    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out, note that management structures are taken from the
    // same arena.  Alignment is the alignment of allocated blocks.
    Cyg_Mempool_Variable(
        cyg_uint8 * /* base */,
        cyg_int32   /* size */);

    // Destructor
    ~Cyg_Mempool_Variable();

    // get some memory; wait if none available
    cyg_uint8 *
    alloc(cyg_int32 /* size */);
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    // get some memory with a timeout
    cyg_uint8 *
    alloc(cyg_int32 /* size */, cyg_tick_count /* delay_timeout */ );
#endif

    // get some memory, return NULL if none available
    cyg_uint8 *
    try_alloc(cyg_int32 /* size */ );
    
    // free the memory back to the pool
    cyg_bool
    free( cyg_uint8 * /* ptr */, cyg_int32 /* size */ );

    // is anyone waiting for memory?
    cyg_bool
    waiting( void );

    // read the fixed allocation size (-1 indicates variable)
    cyg_int32
    get_blocksize( void );

    // these two are obvious and generic
    cyg_int32
    get_totalmem( void );

    cyg_int32
    get_freemem( void );

    // get information about the construction parameters for external
    // freeing after the destruction of the holding object
    // also finds largest free block
    void
    get_arena( cyg_uint8 * &  /* base */, cyg_int32 & /* size */,
               CYG_ADDRWORD & /* maxfree */ );

    // Return the size of the memory allocation (previously returned 
    // by alloc() or try_alloc() ) at ptr. Returns -1 if not found
    cyg_int32
    get_allocation_size( cyg_uint8 * /* ptr */ );
};

#endif // ifndef CYGONCE_KERNEL_MEMVAR_HXX
// EOF memvar.hxx
