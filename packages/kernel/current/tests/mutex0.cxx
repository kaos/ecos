//==========================================================================
//
//        mutex0.cxx
//
//        Mutex and condition variable test 0
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
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-24
// Description:   Limited to checking constructors/destructors
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/mutex.hxx>

#include <cyg/infra/testcase.h>

#include "testaux.hxx"

static Cyg_Mutex mutex0;

static Cyg_Condition_Variable cvar0( mutex0 );

static bool flash( void )
{
    Cyg_Mutex m0;

    CYG_ASSERTCLASSO(m0, "error");

    Cyg_Condition_Variable cv0( m0 );

    CYG_ASSERTCLASSO(cv0, "error");

    return true;
}

void mutex0_main( void )
{
    CYG_TEST_INIT();

    CHECK(flash());
    CHECK(flash());
    
    CYG_ASSERTCLASSO(mutex0, "error");
    CYG_ASSERTCLASSO(cvar0, "error");
    
    CYG_TEST_PASS_FINISH("Mutex 0 OK");

}

externC void
cyg_start( void )
{
    mutex0_main();
}   
// EOF mutex0.cxx
