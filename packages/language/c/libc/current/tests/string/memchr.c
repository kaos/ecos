//=================================================================
//
//        memchr.c
//
//        Testcase for C library memchr()
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
// Description:   Contains testcode for C library memchr() function
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

#include <string.h>
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


// Functions to avoid having to use libc strings

#ifdef CYGPKG_LIBC
static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()
#endif


int main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    char x[] = "Your feeble skills are no match for the power of "
               "the dark side!";
    void *ret;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "memchr() function");

#ifdef CYGPKG_LIBC
    // Check 1
    ret = memchr(x, (int)'k', sizeof(x));
    CYG_TEST_PASS_FAIL( (my_strcmp(ret, x+13) == 0), "Simple test 1" );

    // Check 2
    ret = memchr(x, (int)'p', sizeof(x));
    CYG_TEST_PASS_FAIL( (my_strcmp(ret, x+40) == 0), "Simple test 2" );

    // Check 3
    ret = memchr(x, (int)'z', sizeof(x));
    CYG_TEST_PASS_FAIL( (ret == NULL), "Simple test 3" );

    // Check 4 (boundary condition)
    ret = memchr(x, (int)'\0', sizeof(x));
    CYG_TEST_PASS_FAIL( ret == (x+sizeof(x)-1), "Boundary test 1" );

    // Check 5 (boundary condition)
    ret = memchr(x, (int)'\0', sizeof(x)-1);
    CYG_TEST_PASS_FAIL( ret == NULL, "Boundary test 2" );

    // Check 6 (boundary condition)
    ret = memchr( (void *)"", (int)'\0', 0 );
    CYG_TEST_PASS_FAIL( ret == NULL, "Boundary test 3" );

#else // ifndef CYGPKG_LIBC
    CYG_TEST_PASS("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "memchr() function");
} // main()

// EOF memchr.c
