//=================================================================
//
//        getenv.c
//
//        Testcase for C library getenv()
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
// Date:          1998/8/31
// Description:   Contains testcode for C library getenv() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>                 // Main header for stdlib functions
#include <cyg/infra/testcase.h>     // Testcase API
#include <sys/cstartup.h>           // C library initialisation

// GLOBALS

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)
extern char **environ;              // Standard environment definition
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

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)
static int
my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()
#endif

int
main( int argc, char *argv[] )
{
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)
    char *str;

    char *env1[] = { NULL };
    char *env2[] = { "WIBBLE=fred", NULL };
    char *env3[] = { "PATH=/usr/local/bin:/usr/bin",
                     "HOME=/home/fred",
                     "TEST=1234=5678",
                     "home=hatstand",
                     NULL };
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library getenv() function");

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)

    // Check 1
    str = getenv("ThisIsAVeryUnlikelyName");
    CYG_TEST_PASS_FAIL( str==NULL, "Simple getenv() default environ" );

    // Check 2
    environ = (char **)&env1;
    str = getenv("wibble");
    CYG_TEST_PASS_FAIL( str==NULL, "Simple getenv() with empty environ" );
    
    // Check 3
    environ = (char **)&env2;
    str = getenv("WIBBLE");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "fred"),
                        "Simple getenv()" );

    // Check 4
    str = getenv("wibble");
    CYG_TEST_PASS_FAIL( str==NULL,
                        "Simple getenv() for something not in the "
                        "environment" );

    // Check 5
    environ = (char **)&env3;
    str = getenv("PATH");
    CYG_TEST_PASS_FAIL( (str!= NULL) &&
                        !my_strcmp(str,"/usr/local/bin:/usr/bin"),
                        "Multiple string environment" );

    // Check 6
    str = getenv("PATh");
    CYG_TEST_PASS_FAIL( str==NULL, "getenv() for something not in the "
                        "environment for multiple string environment" );

    // Check 7
    str = getenv("home");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "hatstand"),
                        "Case-sensitive environment names" );

    // Check 8
    str = getenv("TEST");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "1234=5678"),
                        "environment value containing '='" );

    
#else
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_ENVIRONMENT)

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library getenv() function");

} // main()

// EOF getenv.c
