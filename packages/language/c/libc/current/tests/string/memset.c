//=================================================================
//
//        memset.c
//
//        Testcase for C library memset() function
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
// Author(s):     ctarpy, jlarmour
// Contributors:    jlarmour
// Date:          1998/6/3
// Description:   Contains testcode for C library memset() function
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


static char *my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()
#endif // ifdef CYGPKG_LIBC


int main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    char x[300];
    char *ret;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "memset() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

#ifdef CYGPKG_LIBC

    // Check 1
    my_strcpy(x, "      ");
    ret = memset(x, 'X', 5);
    if ((my_strcmp(x, "XXXXX ") == 0) && (ret == x))
        CYG_TEST_PASS("Simple memset");
    else
        CYG_TEST_FAIL("Simple memset");

    // Check 2
    my_strcpy(x, "XXXXX ");
    ret = memset(x, 'Y', 0);
    if ((my_strcmp(x, "XXXXX ") == 0) && (ret == x))
        CYG_TEST_PASS("Boundary case of 0 bytes");
    else
        CYG_TEST_FAIL("Boundary case of 0 bytes");

#else // ifndef CYGPKG_LIBC
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "memset() function");

} // main()

// EOF memset.c
