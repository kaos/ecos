//=================================================================
//
//        strspn.c
//
//        Testcase for C library strspn()
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
// Description:   Contains testcode for C library strspn() function
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
static char *my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()
#endif


int main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    char x[300];
    char y[300];
    int ret;
#endif


    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strspn() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

#ifdef CYGPKG_LIBC

    // Check 1
    my_strcpy(x, "ah blow my nose at you, so-called Arthur king!");
    my_strcpy(y, "wolbah ");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 8), "Simple strspn() #1" );


    // Check 2
    my_strcpy(x, "Not bad for a little fur ball. You! Stay here.");
    my_strcpy(y, "litea rofdbN");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 22), "Simple strspn() #2" );

    // Check 3
    my_strcpy(x, "Not bad for a little fur ball. You! Stay here.");
    my_strcpy(y, "litearofdbN");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 3), "Simple strspn() #3");

    // Check 4 (boundary condition)
    my_strcpy(x, "Not bad for a little fur ball. You! Stay here.");
    my_strcpy(y, "litearofdb");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 0), "First character not in string");

    
    // Check 5 (boundary condition)
    my_strcpy(x, "So, if she weighs the same as a duck, she's made of wood!");
    my_strcpy(y, "zx");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 0), "No characters in string" );

    // Check 6 (boundary condition)
    my_strcpy(x, "");
    my_strcpy(y, "qwerty");
    ret = strspn(x, y);
    CYG_TEST_PASS_FAIL( (ret == 0), "Empty search string" );


#else // ifndef CYGPKG_LIBC
    CYG_TEST_PASS("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strspn() function");
} // main()

// EOF strspn.c
