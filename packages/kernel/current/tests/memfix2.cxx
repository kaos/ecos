//==========================================================================
//
//        memfix2.cxx
//
//        Fixed memory pool test 2
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-06-08
// Description:   test allocation and freeing in fixed memory pools
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>         // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>        // Cyg_Thread
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sema.hxx>

#include <cyg/kernel/memfixed.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>


#define NTHREADS 1
#include "testaux.hxx"

static const cyg_int32 memsize = 1024;

static cyg_uint8 mem[memsize];

#define NUM_PTRS 16                     // Should be even
#define BLOCKSIZE 12

static Cyg_Mempool_Fixed mempool(mem, memsize, BLOCKSIZE);

static cyg_uint8 *ptr[NUM_PTRS];

// We make a number of passes over a table of pointers which point to
// blocks of allocated memory.  The block is freed and a new block
// allocated.  The order of the processing of blocks is varied.
static void entry( CYG_ADDRWORD data )
{
    for(cyg_ucount32 passes = 0; passes < 10; passes++) {


        // The order which the table is processed varies according to
        // stepsize.
        cyg_ucount8 stepsize = (passes*2 + 1) % NUM_PTRS; // odd
        

        for(cyg_ucount8 c=0, i=0; c < NUM_PTRS; c++) {
            i = (i+stepsize) % NUM_PTRS;
            if(ptr[i]) {
                for(cyg_ucount32 j=BLOCKSIZE;j--;) {
                    CYG_TEST_CHECK(ptr[i][j]==i, "Memory corrupted");
                }
                CYG_TEST_CHECK(mempool.free(ptr[i]), "bad free");
            }
            ptr[i] = mempool.try_alloc();

            CYG_TEST_CHECK(NULL != ptr[i], "Memory pool not big enough");
            CYG_TEST_CHECK(mem<=ptr[i] && ptr[i]+BLOCKSIZE < mem+memsize,
                           "Allocated region not within pool");
            
            // Scribble over memory to check whether region overlaps
            // with other regions.  The contents of the memory are
            // checked on freeing.  This also tests that the memory
            // does not overlap with allocator memory structures.
            for(cyg_ucount32 j=BLOCKSIZE;j--;) {
                ptr[i][j]=i;
            }
        }
    }
    
    CYG_TEST_PASS_FINISH("Fixed memory pool 2 OK");
}


void memfix2_main( void )
{
    CYG_TEST_INIT();

    new_thread(entry, 0);

    for(cyg_ucount32 i = 0; i<NUM_PTRS; i++) {
        ptr[i]      = NULL;
    }

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    memfix2_main();
}
// EOF memfix1.cxx
