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
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-30
// Description:   Contains testcode for C library malloc(), calloc() and
//                free() functions
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/system.h>    // Overall system configuration
#include <pkgconf/memalloc.h>  // config header
#ifdef CYGPKG_ISOINFRA
# include <pkgconf/isoinfra.h>
# include <stdlib.h>
#endif
#include <cyg/infra/testcase.h>

#if !defined(CYGPKG_ISOINFRA)
# define NA_MSG "Requires isoinfra package"
#elif !CYGINT_ISO_MAIN_STARTUP
# define NA_MSG "Requires main() to be called"
#elif !CYGINT_ISO_MALLOC
# define NA_MSG "Requires malloc"
#elif !CYGINT_ISO_MALLINFO
# define NA_MSG "Requires mallinfo"
#endif

#ifdef NA_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
    CYG_TEST_FINISH("Done");
}
#else

// FUNCTIONS

extern int
cyg_memalloc_maxalloc( void );

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

int
main( int argc, char *argv[] )
{
    char *str;
    int size;
    int poolmax;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "malloc() and free() functions");
    CYG_TEST_INFO("This checks allocation and freeing of large regions");

    poolmax = mallinfo().maxfree;
    
    if ( poolmax <= 0 ) {
        CYG_TEST_FAIL_FINISH( "Can't determine allocation size to use" );
    }

    size = poolmax/2;

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

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "malloc() and free() functions");

    return 0;
} // main()

#endif // ifndef NA_MSG

// EOF malloc3.c
