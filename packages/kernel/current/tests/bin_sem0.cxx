//==========================================================================
//
//        bin_sem0.cxx
//
//        Binary semaphore test 0
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
// Description:   Limited to checking constructors/destructors
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sema.hxx>

#include <cyg/infra/testcase.h>

#include "testaux.hxx"

static Cyg_Binary_Semaphore sema0, sema1(false), sema2(true);


static bool flash( void )
{
    Cyg_Binary_Semaphore s0;

    Cyg_Binary_Semaphore s1(true);

    Cyg_Binary_Semaphore s2(false);

    return true;
}

void bin_sem0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_TEST_PASS_FINISH("Binary Semaphore 0 OK");
}

externC void
cyg_start( void )
{
    bin_sem0_main();
}   
// EOF bin_sem0.cxx
