#ifndef CYGONCE_KERNEL_MEMPOLT2_HXX
#define CYGONCE_KERNEL_MEMPOLT2_HXX

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
// Date:        1998-02-10
// Purpose:     Define Mempolt2 class interface

// Description: The class defined here provides the APIs for thread-safe,
//              kernel-savvy memory managers; make a class with the
//              underlying allocator as the template parameter.
// Usage:       #include <cyg/kernel/mempolt2.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/thread.hxx>

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

    CYGDBG_DEFINE_CHECK_THIS
    
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
    
    // free the memory back to the pool
    cyg_bool free( cyg_uint8 *p, cyg_int32 size );

    // if applicable: return -1 if not fixed size
    cyg_int32 get_blocksize();

    // is anyone waiting for memory?
    cyg_bool waiting() { return ! queue.empty(); }

    // these two are obvious and generic
    cyg_int32 get_totalmem();
    cyg_int32 get_freemem();

    // get information about the construction parameters for external
    // freeing after the destruction of the holding object.
    void get_arena(
        cyg_uint8 * &base,
        cyg_int32 &size,
        CYG_ADDRWORD &arg_thru );

    // Return the size of the memory allocation (previously returned 
    // by alloc() or try_alloc() ) at ptr. Returns -1 if not found
    cyg_int32
    get_allocation_size( cyg_uint8 * /* ptr */ );
};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MEMPOLT2_HXX
// EOF mempolt2.hxx
