//=================================================================
//
//        malloc1.c
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
// Contributors:    jlarmour
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

#include <pkgconf/libc.h> // config header for C library so we can know size
                          // of malloc pool
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


int
main( int argc, char *argv[] )
{
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)
    int *i;
    char *str, *str2, *str3;
    int j;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "malloc(), calloc() and free() functions");

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

    // Test 1
    i = (int *) malloc( sizeof(int) );

    // check if it should fit into pool
    if (sizeof(int) > CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE)
    {
        // didn't fit into pool, so should be NULL
        CYG_TEST_PASS_FAIL( i == NULL,
                            "1 int malloc with no space left works" );
    }
    else
    {
        // since it should fit into pool, we can fiddle with i
        *i=-12345;
        CYG_TEST_PASS_FAIL( i && (*i==-12345),
                            "1 int malloc with space left works" );
        free(i);
    } // else

    // Test 2
    str=(char *) malloc( 4096 );

    if ( 4096 > CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE)
    {
        // didn't fit into pool, so should be NULL
        CYG_TEST_PASS_FAIL( str == NULL,"4K string with no space left works" );
    }
    else
    {
        // since it should fit into pool, we can fiddle with it.
        for (j=0; j<1024; j++)
        {
            str[j*4] = 'f';
            str[(j*4)+1] = 'r';
            str[(j*4)+2] = 'e';
            str[(j*4)+3] = 'd';
        } // for

        for (j=0; j<1024; j++)
        {
            if ( ((str[j*4] != 'f') ||
                  (str[(j*4)+1] != 'r') ||
                  (str[(j*4)+2] != 'e') ||
                  (str[(j*4)+3] != 'd')) )
                break;
        } // for

        // did j reach the top?
        CYG_TEST_PASS_FAIL( j==1024, "4K string with space left works" );

        free(str);
    } // else       
        

    // Test 3
    str=(char *) calloc( 2, 1024 );

    if ( 2048 > CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE)
    {
        // didn't fit into pool, so should be NULL
        CYG_TEST_PASS_FAIL( str == NULL,
                            "calloc 2K string with no space left works" );
    }
    else
    {
        // check its zeroed
        for ( j=0; j<2048; j++ )
        {
            if (str[j] != 0)
                break;
        } // for

        CYG_TEST_PASS_FAIL( j==2048, "calloc 2K string is cleared" );

        // since it should fit into pool, we can fiddle with it.
        for (j=0; j<512; j++)
        {
            str[j*4] = 'j';
            str[(j*4)+1] = 'i';
            str[(j*4)+2] = 'f';
            str[(j*4)+3] = 'l';
        } // for

        for (j=0; j<512; j++)
        {
            if ( ((str[j*4] != 'j') ||
                  (str[(j*4)+1] != 'i') ||
                  (str[(j*4)+2] != 'f') ||
                  (str[(j*4)+3] != 'l')) )
                break;
        } // for

        // did j reach the top?
        CYG_TEST_PASS_FAIL( j==512, 
                            "calloc 2K string - with space left works" );

        free(str);
    } // else       


    // Test 4
    str=(char *)malloc( CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE+1 );

    CYG_TEST_PASS_FAIL( str==NULL, "malloc too much data returns NULL" );

    // Test 5
    str=(char *)calloc( 1, CYGNUM_LIBC_MALLOC_MEMPOOL_SIZE+1 );
    CYG_TEST_PASS_FAIL( str==NULL, "calloc too much data returns NULL" );

    // Test 6
    str=(char *)malloc(0);
    str=(char *)calloc(0, 1);
    str=(char *)calloc(1, 0);
    str=(char *)calloc(0, 0);
    // simply shouldn't barf by this point

    CYG_TEST_PASS_FAIL( 1, "malloc and calloc of 0 bytes doesn't crash" );

    // Test 7
    str = (char *)malloc(10);
    i = (int *)malloc(sizeof(int));
    str2 = (char *)malloc(10);

    str3=(char *)i;

    CYG_TEST_PASS_FAIL( ((str3 <= str-sizeof(int))  || (str >= &str[10])) &&
                        ((str3 <= str2-sizeof(int)) || (str3 >= &str2[10])) &&
                        ((str+10 <= str2) || (str2+10 <= str)),
                        "Objects don't overlap" );

    // Test 8

    free(i);
    i=(int *)malloc(sizeof(int)*2);
    str3=(char *)i;

    CYG_TEST_PASS_FAIL( ((str3 <= str-sizeof(int))  || (str3 >= &str[10])) &&
                        ((str3 <= str2-sizeof(int)) || (str3 >= &str2[10])) &&
                        ((&str[10] <= str2) || (&str2[10] <= str)),
                        "Objects don't overlap when middle is freed" );
    
    free(i);
    free(str);
    free(str2);

#else
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_MALLOC)

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "malloc(), calloc() and free() functions");
} // main()

// EOF malloc1.c
