#ifndef CYGONCE_DEVS_COMMON_TESTAUX_HXX
#define CYGONCE_DEVS_COMMON_TESTAUX_HXX

//==========================================================================
//
//        testaux.hxx
//
//        Auxiliary test header file
//	  This file is temporary in the devs directory.
//	  --proven 19980514
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-03-09
// Description:
//     Defines some convenience functions to get us going.  In
//     particular this file reserves space for NTHREADS threads,
//     which can be created by calls to aux_new_thread()
//     It also defines a CHECK function.
//
//####DESCRIPTIONEND####

#ifdef NTHREADS

#define STACKSIZE 4096

static Cyg_Thread *thread[NTHREADS];

static char thread_obj[NTHREADS][sizeof(Cyg_Thread)];
static char stack[NTHREADS][STACKSIZE];

static inline void *operator new(size_t size, void *ptr) { return ptr; };

static int nthreads = 0;

static Cyg_Thread *new_thread(cyg_thread_entry *entry, CYG_ADDRWORD data) {

    CYG_ASSERT(nthreads < NTHREADS, 
               "Attempt to create more than NTHREADS threads");

    thread[nthreads] = new( (void *)&thread_obj[nthreads] )
        Cyg_Thread(entry, data, STACKSIZE, (CYG_ADDRESS)stack[nthreads] );

    thread[nthreads]->resume();

    return thread[nthreads++];
}
#endif // defined(NTHREADS)

#define CHECK(b) CYG_TEST_CHECK(b,#b)

#endif // CYGONCE_DEVS_COMMON_TESTAUX_HXX
// End of testaux.hxx
