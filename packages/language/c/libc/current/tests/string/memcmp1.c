//=================================================================
//
//        memcmp1.c
//
//        Testcase for C library memcmp()
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
// Description:   Contains testcode for C library memcmp() function
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
static int my_strlen(const char *s)
{
    const char *ptr;

    ptr = s;
    for ( ptr=s ; *ptr != '\0' ; ptr++ )
        ;

    return (int)(ptr-s);
} // my_strlen()


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
    char y[300];
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "memcmp() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

#ifdef CYGPKG_LIBC

    // Check 1
    my_strcpy(x, "I have become, comfortably numb");
    my_strcpy(y, "I have become, comfortably numb");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, my_strlen(x)) == 0), "Simple compare");


    // Check 2
    my_strcpy(x, "");
    my_strcpy(y, "");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, 0) == 0), "Simple empty string compare");


    // Check 3
    my_strcpy(x, "..shall snuff it. And the Lord did grin");
    my_strcpy(y, "..shall snuff it. And the Lord did grio");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, my_strlen(x)) < 0),
                        "Memory less than #1" );


    // Check 4
    my_strcpy(x, "A reading from the Book of Armaments, Chapter 4, "
                  "Verses 16 to 20:");
    my_strcpy(y, "Bless this, O Lord, that with it thou mayst blow thine "
                 "enemies to tiny bits, in thy mercy.");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, my_strlen(x)) < 0),
                        "Memory less than #2");

    // Check 5
    my_strcpy(x, "Lobeth this thy holy hand grenade at thy foe");
    my_strcpy(y, "Lobeth this thy holy hand grenade at thy fod");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, my_strlen(x)) > 0),
                        "Memory greater than #1" );


    // Check 6
    my_strcpy(y, "Three shall be the number of the counting and the");
    my_strcpy(x, "number of the counting shall be three");
    CYG_TEST_PASS_FAIL( (memcmp(x, y, my_strlen(x)) > 0),
                        "Memory greater than #2" );

#else // ifndef CYGPKG_LIBC
    CYG_TEST_PASS("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "memcmp() function");
} // main()



// EOF memcmp1.c
