//==========================================================================
//
//        bin_sem1.cxx
//
//        Binary semaphore test 1
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
// Date:          1998-02-24
// Description:   Tests basic binary semaphore functionality.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>       // Cyg_Thread
#include <cyg/kernel/thread.inl>

#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>

#define NTHREADS 2

#include "testaux.hxx"

static Cyg_Binary_Semaphore s0(true), s1(false), s2;

static volatile cyg_ucount8 q = 0;

static void entry0( CYG_ADDRWORD data )
{
    s0.wait();
    CHECK( 0 == q++ );
    s1.post();
    s0.wait();
    CHECK( 2 == q++ );
    CHECK( ! s0.posted() );
    CHECK( ! s0.trywait() );
    s0.post();
    CHECK( 3 == q++ );
    CHECK( s0.posted() );
    s1.post();
    CHECK( ! s2.posted() );
    s2.wait();
    CHECK( 5 == q++ );
    CYG_TEST_PASS_FINISH("Binary Semaphore 1 OK");
}

static void entry1( CYG_ADDRWORD data )
{
    CHECK( s1.posted() );
    s1.wait();
    CHECK( 1 == q++ );
    CHECK( ! s0.posted() );
    s0.post();
    s1.wait();
    CHECK( 4 == q++ );
    CHECK( s0.posted() );
    CHECK( s0.trywait() );
    CHECK( ! s0.posted() );
    s2.post();
    s0.wait();
    CYG_TEST_FAIL_FINISH("Not reached");
}

void bin_sem1_main( void )
{
    CYG_TEST_INIT();

    new_thread( entry0, 0);
    new_thread( entry1, 1);
    
    Cyg_Scheduler::start();
    
    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    bin_sem1_main();
}
// EOF bin_sem1.cxx
