//=================================================================
//
//        abs.c
//
//        Testcase for C library abs()
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy, jlarmour
// Contributors:  jlarmour
// Date:          1999-02-12
// Description:   Contains testcode for C library abs() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE
// COMPOUND_TESTCASE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>
#include <limits.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation

// FUNCTIONS

externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
    int x;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library abs() function");

    // Check 1
    x = 5;
    CYG_TEST_PASS_FAIL( abs(x) == 5, "abs(5)");

    // Check 2
    x = -5;
    CYG_TEST_PASS_FAIL( abs(x) == 5, "abs(-5)");
    
    // Check 3
    x = 12345;
    CYG_TEST_PASS_FAIL( abs(x) == 12345, "abs(12345)");

    // Check 4
    x = -23456;
    CYG_TEST_PASS_FAIL( abs(x) == 23456, "abs(-23456");

    // Check 5
    x = 0;
    CYG_TEST_PASS_FAIL( abs(x) == 0, "abs(0)");

    // Check 6
    x = INT_MAX;
    CYG_TEST_PASS_FAIL( abs(x) == INT_MAX, "abs(INT_MAX)");

    // Check 7
    x = -INT_MAX;
    CYG_TEST_PASS_FAIL( abs(x) == INT_MAX, "abs(-INT_MAX)");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library abs() function");

} // main()

// EOF abs.c
