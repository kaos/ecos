//=================================================================
//
//        qsort.c
//
//        Testcase for C library qsort()
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
// Contributors:    jlarmour
// Date:          1998/6/3
// Description:   Contains testcode for C library qsort() function
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


#ifdef CYGPKG_LIBC
static int
Compar( const void *int1, const void *int2 )
{
    if ( *(int*)int1 < *(int*)int2 )
        return -1;
    else if ( *(int*)int1 == *(int*)int2 )
        return 0;
    else
        return 1;
} // Compar()
#endif


int
main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    unsigned int ctr;
    int fail=0;
    int i_array_sorted[] = {1, 5, 8, 35, 84, 258, 1022, 5300, 7372, 9029};
    int i_array_unsorted[] = {258, 8, 7372, 1, 5, 9029, 1022, 35, 5300, 84};
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "qsort() function");

#ifdef CYGPKG_LIBC
    qsort(i_array_unsorted,
          sizeof(i_array_unsorted) / sizeof(i_array_unsorted[0]),
          sizeof(int), &Compar);

    for (ctr = 0;
         ctr < (sizeof(i_array_unsorted) / sizeof(i_array_unsorted[0]));
         ++ctr)
    {
        if (i_array_sorted[ctr] != i_array_unsorted[ctr])
            ++fail;
    } // for

    CYG_TEST_PASS_FAIL( fail == 0, "qsort() sorts correctly" );

#else // ifndef CYGPKG_LIBC
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "qsort() function");

} // main()

// EOF qsort.c
