//=================================================================
//
//        rand1.c
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

// This is the max allowable hits, where a hit is an element
// of a random array being the same as index+1 or index+2
#define TOLERANCE 2

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

static void
test( CYG_ADDRWORD data )
{
    int ctr;
    static int array[1024];
    int hits=0;

    for (ctr=0; ctr<1024; ctr++)
        array[ctr] = rand();

    for (ctr=0; ctr< 1021; ctr++)
        if ((array[ctr]==array[ctr+1]) ||
            (array[ctr]==array[ctr+2]) ||
            (array[ctr]==array[ctr+3]))
            hits++;
    
    CYG_TEST_PASS_FAIL(hits <= TOLERANCE, "Simple test for rand() repetition");

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

    START_TEST( test );

    CYG_TEST_PASS_FINISH("Testing is not applicable to this configuration");
} // main()

// EOF rand1.c
