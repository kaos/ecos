//=================================================================
//
//        sprintf2.c
//
//        Testcase for C library sprintf()
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
// Author(s):       ctarpy, jlarmour
// Contributors:    jlarmour
// Date:            1998/6/3
// Description:     Contains testcode for C library sprintf() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdio.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation

// HOW TO START TESTS

#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

# define START_TEST( test ) test(0)

#else

# define START_TEST( test ) CYG_EMPTY_STATEMENT

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


#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// Functions to avoid having to use libc strings

static int
my_strlen(const char *s)
{
    const char *ptr;

    ptr = s;
    for ( ptr=s ; *ptr != '\0' ; ptr++ )
        ;

    return (int)(ptr-s);
} // my_strlen()


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


static char *
my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()



static void
test( CYG_ADDRWORD data )
{
    static char x[200];
    static char y[200];
    static char z[200];
    int ret;
    int tmp;
    int *ptr;

    // Check 1
    my_strcpy(x, "I'm afraid the shield generator");
    ptr = &tmp;
    ret = sprintf(y, "%s%n will be quite operational - %5d%%%c%05X", x,
                  ptr, 13, '5', 0x89ab);
    my_strcpy( z, "I'm afraid the shield generator will be "
                  "quite operational -    13%5089AB" );
    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "%s%n%d%%%c%0X test");

    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "%s%n%d%%%c%0X test return code" );

    CYG_TEST_PASS_FAIL(tmp==31, "%n test");

    // Check 2
    ret = sprintf(y, "|%5d|%10s|%03d|%c|%o|", 2, "times", 6, '=', 10 );
    my_strcpy(z, "|    2|     times|006|=|12|");

    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "|%5d|%10s|%03d|%c|%o| test");

    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "|%5d|%10s|%03d|%c|%o| test return code" );

#ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

    CYG_TEST_INFO("Starting floating point specific tests");

    // Check 3
    ret = sprintf(y, "|%5f|%10s|%03d|%c|%+-5.2G|%010.3G|",
                  2.0, "times", 6, '=', 12.0, -2.3451e-6 );
    my_strcpy(z, "|2.000000|     times|006|=|+12  |-02.35E-06|");

    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0,
                       "|%5f|%10s|%03d|%c|%+-5.2G|%010.3G| test");

    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "|%5f|%10s|%03d|%c|%+-5.2G|%010.3G| test "
                       "return code" );

#endif // ifdef CYGSEM_LIBC_STDIO_PRINTF_FLOATING_POINT

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__
                    " for C library sprintf() function");

} // test()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library sprintf() function");
    CYG_TEST_INFO("These test combinations of sprintf() features");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF sprintf2.c
