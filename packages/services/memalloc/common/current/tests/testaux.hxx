#ifndef CYGONCE_MEMALLOC_TESTS_TESTAUX_HXX
#define CYGONCE_MEMALLOC_TESTS_TESTAUX_HXX

//==========================================================================
//
//        testaux.hxx
//
//        Auxiliary test header file
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
// Author(s):     dsm
// Contributors:  dsm
// Date:          1998-03-09
// Description:
//     Defines some convenience functions to get us going.  In
//     particular this file reserves space for NTHREADS threads,
//     which can be created by calls to aux_new_thread()
//     It also defines a CHECK function.
//
//####DESCRIPTIONEND####


static inline void *operator new(size_t size, void *ptr) { return ptr; };


#include <pkgconf/hal.h>

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
externC void
cyg_hal_invoke_constructors();
#endif

#ifdef NTHREADS

#ifndef STACKSIZE
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL*2
#endif

static Cyg_Thread *thread[NTHREADS];

typedef CYG_WORD64 CYG_ALIGNMENT_TYPE;

static CYG_ALIGNMENT_TYPE thread_obj[NTHREADS] [
   (sizeof(Cyg_Thread)+sizeof(CYG_ALIGNMENT_TYPE)-1)
     / sizeof(CYG_ALIGNMENT_TYPE)                     ];

static CYG_ALIGNMENT_TYPE stack[NTHREADS] [
   (STACKSIZE+sizeof(CYG_ALIGNMENT_TYPE)-1)
     / sizeof(CYG_ALIGNMENT_TYPE)                     ];

static int nthreads = 0;

static Cyg_Thread *new_thread(cyg_thread_entry *entry, CYG_ADDRWORD data) {

    CYG_ASSERT(nthreads < NTHREADS, 
               "Attempt to create more than NTHREADS threads");

    thread[nthreads] = new( (void *)&thread_obj[nthreads] )
        Cyg_Thread(CYG_SCHED_DEFAULT_INFO,
                   entry, data, 
                   NULL,                // no name
                   (CYG_ADDRESS)stack[nthreads], STACKSIZE );

    thread[nthreads]->resume();

    return thread[nthreads++];
}
#endif // defined(NTHREADS)

#define CHECK(b) CYG_TEST_CHECK(b,#b)

#endif // ifndef CYGONCE_KERNEL_TESTS_TESTAUX_HXX

// End of testaux.hxx
