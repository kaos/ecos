//==========================================================================
//
//        thread0.cxx
//
//        Thread test 0
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
// Date:          1998-02-11
// Description:   Limited to checking constructors/destructors
// Omissions:
//     Thread constructors with 2 or 3 args are not supported at time
// of writing test.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/thread.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/thread.inl>

#include "testaux.hxx"

#define STACKSIZE 4096

static char stack[STACKSIZE];

static cyg_thread_entry entry;

static void entry( CYG_ADDRWORD data )
{
}

static int *p;

static bool flash( void )
{
#if 0 // no facility to allocate stack exists yet.
    Cyg_Thread t0( entry, 0x111 );

    CYG_ASSERTCLASS(&t0, "error");

    Cyg_Thread t1( entry, (CYG_ADDRWORD)&t0, STACKSIZE );

    CYG_ASSERTCLASS(&t1, "error");
#endif
    Cyg_Thread t2( entry, (CYG_ADDRWORD)p, STACKSIZE, (CYG_ADDRESS)stack );

    CYG_ASSERTCLASS(&t2, "error");

    return true;
}

void thread0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_TEST_PASS_FINISH("Thread 0 OK");
    
}

externC void
cyg_start( void )
{ 
    thread0_main();
}
// EOF thread0.cxx
