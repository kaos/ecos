//=================================================================
//
//        ldiv.c
//
//        Testcase for C library ldiv()
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy@cygnus.co.uk, jlarmour@cygnus.co.uk
// Contributors:    jlarmour@cygnus.co.uk
// Date:          1998/6/3
// Description:   Contains testcode for C library ldiv() function
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
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// FUNCTIONS

externC void
cyg_package_start( void )
{
#ifdef CYGPKG_LIBC
    cyg_iso_c_start();
#else
    (void)main(0, NULL);
#endif
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    long num, denom;
    ldiv_t result;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "ldiv() function");

#ifdef CYGPKG_LIBC
    num = 10232;
    denom = 43;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==237) && (result.rem==41),
                        "ldiv( 10232, 43 )");

    num = 4232;
    denom = 2000;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==2) && (result.rem==232),
                        "ldiv( 4232, 2000 )");


    num = 20;
    denom = 20;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==1) && (result.rem==0),
                   "ldiv( 20, 20 )");

    num = 90123456;
    denom = 12345678;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==7) && (result.rem==3703710),
                   "ldiv( 90123456, 12345678 )");

    num = 90123456;
    denom = 123;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==732711) && (result.rem==3),
                        "ldiv( 90123456, 123 )");

#else // ifndef CYGPKG_LIBC
    CYG_TEST_PASS("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "ldiv() function");

} // main()

// EOF ldiv.c
