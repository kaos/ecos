//=================================================================
//
//        rand2.c
//
//        Testcase for C library rand()
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
// Date:          1999-01-20
// Description:   Contains testcode for C library rand() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <pkgconf/libc.h>          // CYGNUM_LIBC_RAND_SEED
#include <stdlib.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// HOW TO START TESTS

#if defined(CYGPKG_LIBC)

# define START_TEST( test ) test(0)

#else

# define START_TEST( test ) CYG_EMPTY_STATEMENT

#endif // if defined(CYGPKG_LIBC)


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


#if defined(CYGPKG_LIBC)

static void
test( CYG_ADDRWORD data )
{
    static int rand_data1[100];
    static int rand_data2[100];
    int count;
    int fail=0;

    // Test that rand starts with the seed as CYGNUM_LIBC_RAND_SEED
    
    for ( count=0; count < 100; ++count ) {
        rand_data1[count] = rand();
    } // for

    // set seed to CYGNUM_LIBC_RAND_SEED
    srand(CYGNUM_LIBC_RAND_SEED);

    for ( count=0; count < 100; ++count ) {
        rand_data2[count] = rand();
    } // for

    for ( count=0; count < 100; ++count ) {
        if (rand_data1[count] != rand_data2[count])
            ++fail;
    } // for

    CYG_TEST_PASS_FAIL(fail==0, "rand() has seed initialised to "
                       "CYGNUM_LIBC_RAND_SEED");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library rand() function");
} // test()

#endif // if defined(CYGPKG_LIBC)


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library rand() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");
} // main()


// EOF rand2.c
