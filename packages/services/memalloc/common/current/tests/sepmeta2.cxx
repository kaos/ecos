//==========================================================================
//
//        sepmeta2.cxx
//
//        Variable memory pool with separate metadata test 2
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                      
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm, jlarmour
// Contributors:  
// Date:          2001-06-28
// Description:   test allocation and freeing in variable memory pools
//####DESCRIPTIONEND####

#include <pkgconf/memalloc.h>
#include <pkgconf/system.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>         // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>        // Cyg_Thread
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sema.hxx>

#include <cyg/kernel/sched.inl>

#define NTHREADS 1
#include "testaux.hxx"

#endif

#include <cyg/memalloc/sepmeta.hxx>

#include <cyg/infra/testcase.h>

static const cyg_int32 memsize = 10240;
static const cyg_int32 metadatasize = 2048;

static cyg_uint8 mem[memsize];
static cyg_uint8 metadata[metadatasize];

static Cyg_Mempool_Sepmeta mempool(mem, memsize, 8,
                                   metadata, metadatasize);

#define NUM_PTRS 16                     // Should be even

static cyg_uint8 *ptr[NUM_PTRS];
static cyg_int32 size[NUM_PTRS];

// We make a number of passes over a table of pointers which point to
// blocks of allocated memory.  The block is freed and a new block
// allocated.  The size and the order of the processing of blocks
// is varied.
static void entry( CYG_ADDRWORD data )
{
    cyg_uint32 s = 1;

    // The number of passes that can be successfully performed
    // depends on the fragmentation performance of the memory
    // allocator.
    for(cyg_ucount32 passes = 0; passes < 10; passes++) {


        // The order which the table is processed varies according to
        // stepsize.
        cyg_ucount8 stepsize = (passes*2 + 1) % NUM_PTRS; // odd
        

        for(cyg_ucount8 c=0, i=0; c < NUM_PTRS; c++) {
            i = (i+stepsize) % NUM_PTRS;
            if(ptr[i]) {
                for(cyg_ucount32 j=size[i];j--;) {
                    CYG_TEST_CHECK(ptr[i][j]==i, "Memory corrupted");
                }
                CYG_TEST_CHECK(mempool.free(ptr[i], size[i]),
                               "bad free");
            }
            s = (s*2 + 17) % 100;  // size always odds therefore non-0
            ptr[i] = mempool.try_alloc(s);
            size[i] = s;

            CYG_TEST_CHECK(NULL != ptr[i], "Memory pool not big enough");
            CYG_TEST_CHECK(mem<=ptr[i] && ptr[i]+s < mem+memsize,
                           "Allocated region not within pool");
            
            // Scribble over memory to check whether region overlaps
            // with other regions.  The contents of the memory are
            // checked on freeing.  This also tests that the memory
            // does not overlap with allocator memory structures.
            for(cyg_ucount32 j=size[i];j--;) {
                ptr[i][j]=i;
            }
        }
    }
    
    CYG_TEST_PASS_FINISH("Sepmeta memory pool 2 OK");
}


void sepmeta2_main( void )
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Starting Seperate metadata memory pool 2 test");

    for(cyg_ucount32 i = 0; i<NUM_PTRS; i++) {
        ptr[i]      = NULL;
    }

#ifdef CYGPKG_KERNEL
    new_thread(entry, 0);
    Cyg_Scheduler::start();
#else
    entry(0);
#endif

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    sepmeta2_main();
}
// EOF sepmeta2.cxx
