//=================================================================
//
//        stdiooutput.c
//
//        Testcase for miscellaneous C library output functions
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
// Author(s):       jlarmour
// Contributors:    jlarmour
// Date:            1998/9/4
// Description:     Contains testcode for C library stdio output
//                  functions
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdio.h>                 // All the stdio functions
#include <errno.h>                 // errno
#include <cyg/infra/testcase.h>    // Testcase API
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


static void
test( CYG_ADDRWORD data )
{
    int i;


    // Check 1
    CYG_TEST_INFO("Expect: Hello printf world 42!");
    printf("Hello printf %s %d!\n", "world", 42);

    // Check 2
    CYG_TEST_INFO("Expect: This is fprintf 0x07f!");
    fprintf(stdout, "This is fprintf 0x%03x!\n", 0x7f);

    // Check 3
    CYG_TEST_INFO("Expect: char tests");
    fputc( 'c', stderr );
    putc( 'h', stdout );
    putchar( 'a' );
    fputc( 'r', stdout );
    putchar( ' ' );
    fflush(stdout);
    putc( 't', stderr );
    fputc( 'e', stdout );
    putchar( 's' );
    fflush( NULL );
    putc( 't', stderr );
    fputc( 's', stderr );
    putchar( '\n' );

    // Check 4
    CYG_TEST_INFO("Expect: puts test");
    puts("puts test");

    // Check 5
    CYG_TEST_INFO("Expect: fputs test");
    fputs( "fputs test\n", stdout );

    // Check 6
    CYG_TEST_INFO("Expect: fwrite test");
    fwrite( "fwrite test\n", 6, 2, stderr );

    // Check 7
    CYG_TEST_INFO("Expect: wibble: No error");
    errno = ENOERR;
    perror( "wibble" );

    // Check 8
    CYG_TEST_INFO("Expect: Long string test!\nSitting on the side of the "
                  "highway waiting to catch speeding drivers, a State "
                  "Police Officer sees a car puttering along at 22 MPH. "
                  "He thinks to himself, \"This driver is just as "
                  "dangerous as a speeder!\" So he turns on his lights "
                  "and pulls the driver over. Approaching the car, "
                  "he notices that there are five old ladies-two in "
                  "the front seat and three in the back-wide eyed and "
                  "white as ghosts. The driver, obviously confused, says "
                  "to him, \"Officer, I don't understand, I was doing "
                  "exactly the speed limit! What seems to be the "
                  "problem?\" \"Ma'am,\" the officer replies, \"you "
                  "weren't speeding, but you should know that driving "
                  "slower than the speed limit can also be a danger to "
                  "other drivers.\" \"Slower than the speed limit? No "
                  "sir, I was doing the speed limit exactly...Twenty-two "
                  "miles an hour!\" the old woman says a bit proudly. "
                  "The State Police officer, trying to contain a "
                  "chuckle explains to her that \"22\" was the route "
                  "number, not the speed limit. A bit embarrassed, the "
                  "woman grinned and thanked the officer for pointing "
                  "out her error. \"But before I let you go, Ma'am, I "
                  "have to ask... Is everyone in this car OK? These "
                  "women seem awfully shaken and they haven't muttered a "
                  "single peep this whole time,\" the officer asks. \"Oh, "
                  "they'll be all right in a minute officer. We just got "
                  "off Route 119\".");

    printf("Long string test!\nSitting on the side of the "
           "highway waiting to catch speeding drivers, a State "
           "Police Officer sees a car puttering along at %d MPH. "
           "He thinks to himself, \"This driver is just as "
           "dangerous as a speeder!\" So he turns on his lights "
           "and pulls the driver over. Approaching the car, "
           "he notices that there are five old ladies-two in "
           "the front seat and three in the back-wide eyed and "
           "white as ghosts. The driver, obviously confused, says "
           "to him, \"Officer, I don't understand, I was doing "
           "exactly the speed limit! What seems to be the "
           "problem?\" \"Ma'am,\" the officer replies, \"you "
           "weren't speeding, but you should know that driving "
           "slower than the speed limit can also be a danger to "
           "other drivers.\" \"Slower than the speed limit? No "
           "sir, I was doing the speed limit exactly...Twenty-two "
           "miles an hour!\" the old woman says a bit proudly. "
           "The State Police officer, trying to contain a "
           "chuckle explains to her that \"%d\" was the route "
           "number, not the speed limit. A bit embarrassed, the "
           "%coman grinned and thanked the officer for pointing "
           "out her error. \"But before I let you go, Ma'am, I "
           "have to ask... Is everyone in this car OK? These "
           "women seem awfully shaken and they haven't muttered a "
           "single peep this whole time,\" the officer asks. \"Oh, "
           "they'll be all right in a minute officer. We just got "
           "off Route %d\".\n", 22, 22, 'w', 119);

    // Check 9
    CYG_TEST_INFO("Expect: Another puts test, just for fun");
    puts("Another puts test, just for fun");

    // Check 10
    CYG_TEST_INFO("Expect: more fputs testing");
    fputs( "more fputs testing\n", stderr );

    // Check 11
    CYG_TEST_INFO("Expect all the numbers from 1 to 100 in 5 columns");
    for (i=1; i<101; ++i) {

        printf("%5d ", i);
        if (i%5 == 0)
            fputc( '\n', stdout);
        
    } // for

    CYG_TEST_PASS("Stdio output tests completed");
    CYG_TEST_FINISH("Finished tests from testcase " __FILE__
                    " for C library stdio output functions");

} // test()

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library stdio output functions");
    CYG_TEST_INFO("The output of these tests needs to be manually "
                  "verified.");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF stdiooutput.c
