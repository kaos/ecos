//=================================================================
//
//        atexit.c
//
//        Testcase for C library atexit()
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-03
// Description:   Contains testcode for C library atexit() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>                 // Main header for stdlib functions
#include <cyg/infra/testcase.h>     // Testcase API
#include <sys/cstartup.h>           // C library initialisation


// STATICS

static int stage;
static int failed;

// FUNCTIONS

externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()


static void
myfun1( void )
{
    if (!failed)
    {
        CYG_TEST_PASS_FAIL( stage==0, "first function called correctly" );
        stage=1;
    } // if
} // myfun1()

static void
myfun2( void )
{
    if (!failed)
    {
        CYG_TEST_PASS_FAIL( stage==1, "Second function called correctly" );
        stage=2;
    } // if
} // myfun2()

static void
myfun3( void )
{
    if (!failed)
    {
        CYG_TEST_PASS_FAIL( stage==2, "Second function called correctly" );
        stage=3;
    } // if

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library atexit() function");

} // myfun3()

int
main( int argc, char *argv[] )
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library atexit() function");

#if defined(CYGFUN_LIBC_ATEXIT)

    // we only have one test in us! We can only exit once :-)

    CYG_TEST_PASS_FAIL( atexit(&myfun3)==0, 
                        "Simple registration of first atexit() function" );

    CYG_TEST_PASS_FAIL( atexit(&myfun2)==0, 
                       "Simple registration of second atexit() function" );

    CYG_TEST_PASS_FAIL( atexit(&myfun1)==0, 
                        "Simple registration of third atexit() function" );

    return 0;
#else
    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library atexit() function");

#endif // if defined(CYGFUN_LIBC_ATEXIT)

} // main()

// EOF atexit.c
