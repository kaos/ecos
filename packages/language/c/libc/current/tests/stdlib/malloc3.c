//=================================================================
//
//        malloc3.c
//
//        Testcase for C library malloc(), calloc() and free()
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
// Date:          1998/6/3
// Description:   Contains testcode for C library malloc(), calloc() and
//                free() functions
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// INCLUDES

#include <pkgconf/system.h> // Overall system configuration
#include <pkgconf/libc.h>   // config header for C library so we can know
                            // size of malloc pool
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h> // CYGSEM_KERNEL_MEMORY_COALESCE
#endif
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


#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) && \
    defined(CYGSEM_KERNEL_MEMORY_COALESCE)
static int
fill_with_data( char *buf, int size )
{
    int i;

    for (i=0; i < size; i++)
        buf[i] = 'f';

    for (i=0; i < size; i++)
        if (buf[i] != 'f') {
            CYG_TEST_FAIL( "data written to buffer does not compare "
                           "correctly! #1" );
            return 0;
        } // if


    for (i=0; i < size; i++)
        buf[i] = 'z';

    for (i=0; i < size; i++)
        if (buf[i] != 'z') {
            CYG_TEST_FAIL( "data written to buffer does not compare "
                           "correctly! #2" );
            return 0;
        } // if

    return 1;
} // fill_with_data()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) &&
       //    defined(CYGSEM_KERNEL_MEMORY_COALESCE)


int
main( int argc, char *argv[] )
{
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) && \
    defined(CYGSEM_KERNEL_MEMORY_COALESCE)

    char *str;
    int size = CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE/2;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "malloc() and free() functions");
    CYG_TEST_INFO("This checks allocation and freeing of large regions");

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) && \
    defined(CYGSEM_KERNEL_MEMORY_COALESCE)

    // Don't allocate all the memory at once - leave room for any structures
    // used to manage the memory
    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 1");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 1 usability");
    free( str );


    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 2");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 2 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 3");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 3 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 4");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 4 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 5");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 5 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 6");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 6 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 7");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 7 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 8");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 8 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 9");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ), "allocation 9 usability");
    free( str );

    str = (char *)malloc( size );
    CYG_TEST_PASS_FAIL( str != NULL, "allocation 10");
    CYG_TEST_PASS_FAIL( fill_with_data( str, size ),"allocation 10 usability");
    free( str );


#else
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) &&
       //    defined(CYGSEM_KERNEL_MEMORY_COALESCE)


    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "malloc() and free() functions");
} // main()

// EOF malloc3.c
