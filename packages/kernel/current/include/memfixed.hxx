#ifndef CYGONCE_KERNEL_MEMFIXED_HXX
#define CYGONCE_KERNEL_MEMFIXED_HXX

//==========================================================================
//
//      memfixed.hxx
//
//      Memory pool with fixed block class declarations
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Purpose:     Define Memfixed class interface
// Description: Inline class for constructing a fixed block allocator
// Usage:       #include <cyg/kernel/memfixed.hxx>
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

#include <cyg/kernel/mfiximpl.hxx>     // implementation of a fixed mem pool

class Cyg_Mempool_Fixed {
private:
#ifdef CYGIMP_MEM_USE_MEMPOOLT_PLAIN
    Cyg_Mempoolt<Cyg_Mempool_Fixed_Implementation> mypool;
#else
    Cyg_Mempolt2<Cyg_Mempool_Fixed_Implementation> mypool;
#endif

public:
    // this API makes concrete a class which implements a thread-safe
    // kernel-savvy memory pool which manages fixed size blocks.

    CYGDBG_DEFINE_CHECK_THIS
    
    // Constructor: gives the base and size of the arena in which memory is
    // to be carved out, note that management structures are taken from the
    // same arena.  Alloc_unit is the blocksize allocated.
    Cyg_Mempool_Fixed(
        cyg_uint8 *base,
        cyg_int32 size,
        CYG_ADDRWORD alloc_unit );

    // Destructor
    ~Cyg_Mempool_Fixed();

    // get some memory; wait if none available
    cyg_uint8 *alloc();
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    // get some memory with a timeout
    cyg_uint8 *alloc(cyg_tick_count delay_timeout );
#endif

    // get some memory, return NULL if none available
    cyg_uint8 *try_alloc();
    
    // free the memory back to the pool
    cyg_bool free( cyg_uint8 *p);

    // is anyone waiting for memory?
    cyg_bool waiting();

    // read the fixed allocation size
    cyg_int32 get_blocksize();

    // these two are obvious and generic
    cyg_int32 get_totalmem();
    cyg_int32 get_freemem();

    // get information about the construction parameters for external
    // freeing after the destruction of the holding object
    void
    get_arena( cyg_uint8 * &base, cyg_int32 &size, CYG_ADDRWORD &alloc  );

    // Return the size of the memory allocation (previously returned 
    // by alloc() or try_alloc() ) at ptr. Returns -1 if not found
    cyg_int32
    get_allocation_size( cyg_uint8 * /* ptr */ );
};

#endif // ifndef CYGONCE_KERNEL_MEMFIXED_HXX
// EOF memfixed.hxx
