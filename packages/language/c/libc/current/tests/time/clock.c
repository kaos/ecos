//=================================================================
//
//        clock.c
//
//        Testcase for C library clock()
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
// Description:   Contains testcode for C library clock() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <time.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// HOW TO START TESTS

#if defined(CYGPKG_LIBC)

# define START_TEST( test ) test(0)

#else

# define START_TEST( test ) CYG_EMPTY_STATEMENT

#endif


// CONSTANTS

// This defines how many loops before we decide that
// the clock doesnt work
#define MAX_TIMEOUT 1000000

// Percentage error before we declare fail: range 0 - 100
#define TOLERANCE 20


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


#ifdef CYGPKG_LIBC

static int
my_abs(int i)
{
    return (i < 0) ? -i : i;
} // my_abs()


static void
test( CYG_ADDRWORD data )
{
    unsigned long ctr, ctr2, err;
    clock_t clock_init;
    clock_t clock_first, clock_second, clock_third;

    // This waits for a clock tick, to ensure that we are at the
    // start of a clock period. Then sit in a tight loop to get
    // the clock period. Repeat this, and make sure that it the
    // two timed periods are acceptably close.

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "clock() function");

    err = 101;
    clock_init = clock();
    
    if (clock_init == -1)  // unimplemented is just as valid
    {
        CYG_TEST_PASS_FINISH( "clock() returns -1, meaning unimplemented");
    } // if
    
    for (ctr = 0; ctr<MAX_TIMEOUT; ctr++)
    {
        if ((clock_first=clock()) > clock_init)
            break; // Hit the next clock pulse
    }
    if (ctr < MAX_TIMEOUT)
    {
        // We hit a clock pulse, rather than a timeout
        // Now we are at the beginning of a pulse, so time until the
        // next one in order to get a measure of the length.
        for (ctr = 0; ctr<MAX_TIMEOUT; ctr++)
        {
            if ((clock_second=clock()) > clock_first)
                break; // Hit the next clock pulse
        } // for
        if (ctr < MAX_TIMEOUT)
        {
            // We hit a clock pulse, rather than a timeout. ctr contains
            // the number of loops/tick. Measure this again, and see if
            // the two results are acceptably close.
            for (ctr2 = 0; ctr2<MAX_TIMEOUT; ctr2++)
            {
                if ((clock_third=clock()) > clock_second)
                    break; // Hit the next clock pulse
            } // for
            if (ctr < MAX_TIMEOUT)
            {
                err = (100 * my_abs(ctr-ctr2)) / ctr;
            } // if (ctr < MAX_TIMEOUT)
            else
            {
                CYG_TEST_FAIL_FINISH("No change in clock state!");
            } // else (ctr < MAX_TIMEOUT)
        } // if (ctr < MAX_TIMEOUT)
        else
        {
            CYG_TEST_FAIL_FINISH("No change in clock state!");
        } // else (ctr < MAX_TIMEOUT)

    } // if (ctr < MAX_TIMEOUT)
    else
    {
        CYG_TEST_FAIL_FINISH("No change in clock state!");
    } // else (ctr < MAX_TIMEOUT)

    CYG_TEST_PASS_FAIL(err < TOLERANCE, "clock() stability");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "clock() function");
} // test()
#endif

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "clock() function");

    START_TEST( test );

    CYG_TEST_PASS_FINISH("Testing is not applicable to this configuration");

} // main()

// EOF clock.c
