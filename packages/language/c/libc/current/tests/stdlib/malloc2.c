//=================================================================
//
//        malloc2.c
//
//        Stress testcase for C library malloc(), calloc() and free()
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
// Author(s):     jlarmour@cygnus.co.uk
// Contributors:    jlarmour@cygnus.co.uk
// Date:          1998/6/3
// Description:   Contains testcode to stress-test C library malloc(),
//                calloc() and free() functions
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE
// CYG_TEST_HEARTBEAT=120

// INCLUDES

#include <pkgconf/libc.h> // config header for C library so we can know size
                          // of malloc pool
#include <stdlib.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// CONSTANTS

#define NUM_ITERATIONS 1000

// GLOBALS

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)
static int problem=0;
#endif

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


#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

static void *
safe_malloc( size_t size )
{
    void *ptr;

    ptr=malloc(size);

    if (ptr==NULL)
    {
        CYG_TEST_FAIL( "malloc returned NULL! "
                       "Perhaps the allocator doesn't coalesce?" );
        problem++;
    } // if

    return ptr;
} // safe_malloc()


static void *
safe_calloc( size_t size )
{
    void *ptr;
    int i;

    ptr=calloc(size,1);

    if (ptr==NULL)
    {
        CYG_TEST_FAIL( "calloc returned NULL! "
                       "Perhaps the allocator doesn't coalesce" );
        problem++;
    } // if
    else
    {
        for (i=0; i < size; i++)
        {
            if (((char *)ptr)[i] != 0)
            {
                CYG_TEST_FAIL("calloc didn't clear data completely");
                problem++;
                return ptr;
            } // if
        } // for
    } // else

    return ptr;
} // safe_calloc()


static void
fill_with_data( char *buf, int size )
{
    int i;

    for (i=0; i < size; i++)
        buf[i] = 'f';

    for (i=0; i < size; i++)
        if (buf[i] != 'f')
        {
            CYG_TEST_FAIL( "data written to buffer does not compare "
                           "correctly! #1" );
            problem++;
            return;
        } // if


    for (i=0; i < size; i++)
        buf[i] = 'z';

    for (i=0; i < size; i++)
        if (buf[i] != 'z')
        {
            CYG_TEST_FAIL( "data written to buffer does not compare "
                           "correctly! #2" );
            problem++;
            return;
        } // if

} // fill_with_data()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

int
main( int argc, char *argv[] )
{
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)
    char *str1, *str2, *str3;
    int j;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting stress tests from testcase " __FILE__ " for C "
                  "library malloc(), calloc() and free() functions");

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

    if ( CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE < 300 )
    {
        CYG_TEST_FAIL_FINISH("This testcase cannot safely be used with a "
                             "memory pool for malloc less than 300 bytes");
    } // if


    for ( j=1; j < NUM_ITERATIONS; j++)
    {
//        if ((j % 100) == 0)
//            CYG_TEST_STILL_ALIVE( j, "Multiple mallocs and frees continuing" );

        
        str1 = (char *)safe_malloc(50);
        fill_with_data( str1, 50 );
        str2 = (char *)safe_calloc(11);
        fill_with_data( str2, 11 );
        str3 = (char *)safe_malloc(32);
        fill_with_data( str3, 32 );

        free(str2);
        free(str1);

        str2 = (char *)safe_calloc(11);
        fill_with_data( str2, 11 );
        free(str2);

        str1 = (char *)safe_calloc(50);
        fill_with_data( str1, 50 );
        free(str3);

        str3 = (char *)safe_malloc(32);
        fill_with_data( str3, 32 );
        free(str1);

        str2 = (char *)safe_calloc(11);
        fill_with_data( str2, 11 );
        str1 = (char *)safe_malloc(50);
        fill_with_data( str1, 50 );

        free(str3);
        free(str1);
        free(str2);

        if (problem != 0)
            break;
    } // for

    // Did it completely successfully?
    if (j==NUM_ITERATIONS)
        CYG_TEST_PASS("Stress test completed successfully");

#else
    CYG_TEST_PASS("Testing is not applicable to this configuration");
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

    
    CYG_TEST_FINISH("Finished stress tests from testcase " __FILE__ " for C "
                    "library malloc(), calloc() and free() functions");
} // main()

// EOF malloc2.c
