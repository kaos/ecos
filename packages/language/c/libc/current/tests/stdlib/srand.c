//=================================================================
//
//        srand.c
//
//        Testcase for C library srand()
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy, jlarmour
// Contributors:  jlarmour
// Date:          1999-01-20
// Description:   Contains testcode for C library srand() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// HOW TO START TESTS

#if defined(CYGPKG_LIBC)

# define START_TEST( test ) test(0)

#else

# define START_TEST( test ) CYG_EMPTY_STATEMENT

#endif // if defined(CYGPKG_LIBC)


// CONSTANTS

// Max cross correlation value of two sequences of
// different seed
#define X_CORREL 10

// size of arrays to use for testing
#define TEST_SIZE 1024


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
    int ctr;
    static int array_1[TEST_SIZE];
    static int array_2[TEST_SIZE];
    static int array_3[TEST_SIZE];
    int hits;
    int fail;


    srand(3);
    for (ctr=0; ctr<TEST_SIZE; ++ctr)
        array_1[ctr] = rand();

    srand(9);
    for (ctr=0; ctr<TEST_SIZE; ++ctr)
        array_2[ctr] = rand();

    srand(3);
    for (ctr=0; ctr<TEST_SIZE; ++ctr)
        array_3[ctr] = rand();

    // Make sure arrays 1 and 3 are the same
    fail = 0;
    for (ctr=0; ctr<TEST_SIZE; ++ctr) {
        if (array_1[ctr] != array_3[ctr])
            ++fail;
    } // for

    CYG_TEST_PASS_FAIL( fail == 0, "resetting the seed to the same value");

    // Check sequences of different seeds are different
    hits = 0;
    for (ctr=0; ctr<TEST_SIZE; ++ctr) {
        if (array_1[ctr] == array_2[ctr])
            ++hits;
    } // for

    CYG_TEST_PASS_FAIL(hits < X_CORREL,
                       "random sequence for different seeds is different");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for "
                    "C library srand() function");
} // test()

#endif // if defined(CYGPKG_LIBC)


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "srand() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");
} // main()



// EOF srand.c
