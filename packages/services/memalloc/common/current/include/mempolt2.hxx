#ifndef CYGONCE_MEMALLOC_MEMPOLT2_HXX
#define CYGONCE_MEMALLOC_MEMPOLT2_HXX

//==========================================================================
//
//      mempolt2.hxx
//
//      Mempolt2 (Memory pool template) class declarations
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
// Purpose:      Define Mempolt2 class interface
// Description:  The class defined here provides the APIs for thread-safe,
//               kernel-savvy memory managers; make a class with the
//               underlying allocator as the template parameter.
// Usage:        #include <cyg/memalloc/mempolt2.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// It is assumed that implementations using this file have already mandated
// that the kernel is present. So we just go ahead and use it

#include <pkgconf/memalloc.h>
#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/thread.hxx>
#include <cyg/memalloc/common.hxx>     // Common memory allocator infra

template <class T>
class Cyg_Mempolt2
{
private:
    T pool;                             // underlying memory manager
    Cyg_ThreadQueue queue;              // queue of waiting threads

    class Mempolt2WaitInfo {
    private:
        Mempolt2WaitInfo() {}
    public:
        cyg_int32 size;
        cyg_uint8 *addr;
        Mempolt2WaitInfo( cyg_int32 allocsize )
        { size = allocsize; addr = 0; }
    };

public:

    Cyg_Mempolt2(
        cyg_uint8 *base,
        cyg_int32 size,
        CYG_ADDRWORD arg_thru );        // Constructor
    ~Cyg_Mempolt2();                    // Destructor
        
    // get some memory; wait if none available; return NULL if failed
    // due to interrupt
    cyg_uint8 *alloc( cyg_int32 size );
    
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    // get some memory with a timeout; return NULL if failed
    // due to interrupt or timeout
    cyg_uint8 *alloc( cyg_int32 size, cyg_tick_count abs_timeout );
#endif

    // get some memory, return NULL if none available
    cyg_uint8 *try_alloc( cyg_int32 size );
    
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

    // free the memory back to the pool
    // returns true on success
    cyg_bool free( cyg_uint8 *p, cyg_int32 size );

    // Get memory pool status
    // flags is a bitmask of requested fields to fill in. The flags are
    // defined in common.hxx
    void get_status( cyg_mempool_status_flag_t flags,
                     Cyg_Mempool_Status &status );

    CYGDBG_DEFINE_CHECK_THIS
    
};

#include <cyg/memalloc/mempolt2.inl>

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_MEMALLOC_MEMPOLT2_HXX
// EOF mempolt2.hxx
