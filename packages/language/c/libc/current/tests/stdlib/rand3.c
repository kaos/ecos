//=================================================================
//
//        rand3.c
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
// Description:   Contains testcode for C library rand() function. This tests
//                that random numbers are distributed well between 0 and
//                RAND_MAX
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

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_RAND)

# define START_TEST( test ) test(0)

#else

# define START_TEST( test ) CYG_EMPTY_STATEMENT

#endif // if defined(CYGPKG_LIBC)


// CONSTANTS

#define NUM_BUCKETS 1024           // how many categories to define
#define TEST_LENGTH 100000         // how many samples to take
#define BUCKET_DIFF_TOLERANCE 4    // how much the buckets can vary at the end

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


#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_RAND)

static int
my_abs(int i)
{
    return (i < 0) ? -i : i;
} // my_abs()

static void
test( CYG_ADDRWORD data )
{
    static cyg_uint8 rand_bucket[NUM_BUCKETS]; // divide the space from
                                               // 0..RAND_MAX into
                                               // NUM_BUCKETS categories
    cyg_ucount32 count;                        // loop variable
    int r;                                     // temp for rand() variable
    cyg_ucount32 sum;                          // sum of bucket contents
    cyg_ucount32 average;                      // average of bucket contents

    // initialise all buckets to 0 - do it ourselves rather than rely on memset
    for ( count=0; count < NUM_BUCKETS; ++count )
    {
        rand_bucket[ count ] = 0;
    } // for

    for ( count=0; count < TEST_LENGTH; ++count )
    {
        r = rand();
        ++rand_bucket[ r % NUM_BUCKETS ];
    } // for

    for ( sum=0, count=0; count < NUM_BUCKETS; ++count )
    {
        sum += rand_bucket[ count ];
    } // for

    average = sum / NUM_BUCKETS;

    for ( count=0; count < NUM_BUCKETS; ++count )
    {
        if ( my_abs(rand_bucket[count] - average) > BUCKET_DIFF_TOLERANCE )
            break;
    } // for

    CYG_TEST_PASS_FAIL( (count >= NUM_BUCKETS), "even distribution of rand()");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "rand() function");
} // test()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_RAND)


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "rand() function");

    CYG_TEST_INFO("This test tests the distribution of random numbers and");
    CYG_TEST_INFO("may take some time");

    START_TEST( test );

    CYG_TEST_PASS_FINISH("Testing is not applicable to this configuration");
} // main()


// EOF rand3.c
