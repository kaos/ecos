//=================================================================
//
//        strtol.c
//
//        Testcase for C library strtol()
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
// Author(s):     ctarpy, jlarmour
// Contributors:    jlarmour
// Date:          1998/6/3
// Description:   Contains testcode for C library strtol() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// HOW TO START TESTS

#if defined(CYGPKG_LIBC)

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


#ifdef CYGPKG_LIBC
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
    char x[30];
    long z;
    char *endptr;

    my_strcpy(x, "20");
    CYG_TEST_PASS_FAIL( strtol(x, (char**)NULL, 10) == 20,
                        "Simple strtol(20 ,...)" );

    my_strcpy(x, "1972100");
    CYG_TEST_PASS_FAIL( strtol(x, (char**)NULL, 10) == 1972100,
                        "Simple strtol(1972100, ..." );

    my_strcpy(x, "0xFFEE");
    z = strtol(x, (char**)NULL, 16);
    CYG_TEST_PASS_FAIL(z == 65518, "Hex base strtol()");

    my_strcpy(x, "100111011");
    z = strtol(x, (char**)NULL, 2);
    CYG_TEST_PASS_FAIL(z == 315, "Binary base strtol()");

    my_strcpy(x, "10372");
    z = strtol(x, (char**)NULL, 8);
    CYG_TEST_PASS_FAIL(z == 4346, "Octal base strtol()" );

    my_strcpy(x, "317823");
    z = strtol(x, (char**)NULL, 8);
    CYG_TEST_PASS_FAIL(z == 207, "Partial string" );

    my_strcpy(x, " 53ab823");
    z = strtol(x, &endptr, 10);
    CYG_TEST_PASS_FAIL((z == 53) && (endptr==&x[3]), "Correct end pointer" );

    my_strcpy(x, "-479");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL(z == -479, "Negative string");

    my_strcpy(x, "+4796");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL(z == 4796, "Positive string");

    my_strcpy(x, "");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL(z == 0, "Empty string");

    my_strcpy(x, "");
    z = strtol(x, &endptr, 10);
    CYG_TEST_PASS_FAIL((z == 0) && (endptr==x),
                       "Empty string sets endptr correctly");

    my_strcpy(x, "    ");
    z = strtol(x, &endptr, 10);
    CYG_TEST_PASS_FAIL((z == 0) && (endptr==x),
                       "White space only string sets endptr correctly");

    my_strcpy(x, "0XFFEE");
    z = strtol(x, (char**)NULL, 0);
    CYG_TEST_PASS_FAIL(z == 65518, "Base 0 but hex");

    my_strcpy(x, "\t    0629");
    z = strtol(x, (char**)NULL, 0);
    CYG_TEST_PASS_FAIL(z == 50, "Base 0 but octal");

    my_strcpy(x, "42");
    z = strtol(x, (char**)NULL, 0);
    CYG_TEST_PASS_FAIL(z == 42, "Base 0 but decimal");

    my_strcpy(x, "hello");
    z = strtol(x, &endptr, 0);
    CYG_TEST_PASS_FAIL((z == 0) && (endptr==x),
                       "endptr set correctly on conversion failure");

    my_strcpy(x, "z2f");
    z = strtol(x, (char**)NULL, 36);
    CYG_TEST_PASS_FAIL(z == 45447, "Base==36");

    my_strcpy(x, "h547324");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL(z == 0, "No valid number string");

    my_strcpy(x, "545425876654547324");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL( (z == LONG_MAX) && (errno == ERANGE),
                        "Number out of range");

    my_strcpy(x, "-545425876654547324");
    z = strtol(x, (char**)NULL, 10);
    CYG_TEST_PASS_FAIL( (z == LONG_MIN) && (errno == ERANGE),
                        "Number out of range");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strtol() function");
} // test()

#endif // ifndef CYGPKG_LIBC

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strtol() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");
} // main()

// EOF strtol.c
