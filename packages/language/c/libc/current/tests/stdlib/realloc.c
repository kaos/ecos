//=================================================================
//
//        realloc.c
//
//        Testcase for C library realloc()
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
// Date:          1998/8/31
// Description:   Contains testcode for C library realloc() function
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

static const char alphabet[]="abcdefghijklmnopqrstuvwxyz{-}[]#';:@~!$^&*()"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

static int
compare_with_alphabet( char *buf, int size, int offset )
{
    int i, buf_offset;

    for (i=offset, buf_offset=0;
         buf_offset < size;
         buf_offset++,i++ ) {

        if ( i==sizeof(alphabet)-1 )
            i=0;

        if ( buf[buf_offset] != alphabet[i] ) {
            CYG_TEST_FAIL( "buffer has not retained correct data!");
            return 0; // fail
        } // if
    } // for

    return 1; // success
} // compare_with_alphabet()

static int
fill_with_alphabet( char *buf, int size, int offset )
{
    int i, buf_offset;

    for (i=offset, buf_offset=0;
         buf_offset < size;
         buf_offset++,i++ ) {

        if ( i==sizeof(alphabet)-1 )
            i=0;

        buf[buf_offset] = alphabet[i];

    } // for

    return compare_with_alphabet( buf, size, offset); // be sure
} // fill_with_alphabet()

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
                  "realloc() function");

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) && \
    defined(CYGSEM_KERNEL_MEMORY_COALESCE)

    str = (char *)realloc( NULL, size );
    CYG_TEST_PASS_FAIL( str != NULL, "realloc doing only allocation");
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size, 0 ),
                        "allocation usability");

    str = (char *)realloc( str, 0 );
    CYG_TEST_PASS_FAIL( str == NULL, "realloc doing implicit free" );

    str = (char *)realloc( NULL, size/2 );
    CYG_TEST_PASS_FAIL( str != NULL, "realloc doing allocation to half size");
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size/2, 5 ),
                        "half allocation usability");

    str = (char *)realloc( str, size );
    CYG_TEST_PASS_FAIL( str != NULL,
                        "reallocing allocation back to normal size");
    CYG_TEST_PASS_FAIL( compare_with_alphabet(str, size/2, 5),
                        "after realloc to normal size, old contents kept" );
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size, 3 ),
                        "reallocation normal size usability");

    str = (char *)realloc( str, size/4 );
    CYG_TEST_PASS_FAIL( str != NULL, "reallocing allocation to quarter size");
    CYG_TEST_PASS_FAIL( compare_with_alphabet(str, size/4, 3),
                        "after realloc to quarter size, old contents kept" );
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size/4, 1 ),
                        "reallocation quarter size usability");

    CYG_TEST_PASS_FAIL( realloc( str, size*100 ) == NULL,
                        "reallocing allocation that is too large" );
    CYG_TEST_PASS_FAIL( compare_with_alphabet( str, size/4, 1 ),
                        "Checking old contents maintained despite failure" );

    str = (char *)realloc( str, 0 );
    CYG_TEST_PASS_FAIL( str == NULL, "realloc doing implicit free again" );


#else
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC) &&
       //    defined(CYGSEM_KERNEL_MEMORY_COALESCE)


    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "realloc() function");
} // main()

// EOF realloc.c
