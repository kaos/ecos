//=================================================================
//
//        ctime.c
//
//        Testcase for C library ctime() function
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
// Contributors:  jlarmour
// Date:          1999-03-05
// Description:   Contains testcode for C library ctime() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <time.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation

// HOW TO START TESTS

# define START_TEST( test ) test(0)

// FUNCTIONS


externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()


static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


static void
test( CYG_ADDRWORD data )
{
    time_t t;
    char *ret;
    
    // make this predictable - independent of the user option
    cyg_libc_time_setzoneoffsets(0, 3600);
    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTOFF );

    t = (time_t)130710184;

    ret = ctime(&t);
    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Thu Feb 21 20:23:04 1974\n"),
                       "ctime test #1");

    t = (time_t)946689894;

    ret = ctime(&t);
    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Sat Jan 01 01:24:54 2000\n"),
                       "ctime Y2K test #2");

    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTON );

    t = (time_t)-113186106;

    ret = ctime(&t);
    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Tue Jun 01 00:24:54 1966\n"),
                       "ctime test #3");

#ifdef CYGFUN_LIBC_TIME_POSIX
    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTOFF );

    t = (time_t)915510061;
    
    {
        char ret2[26];

        ret = ctime_r(&t, ret2);
        CYG_TEST_PASS_FAIL(!my_strcmp(ret2, "Tue Jan 05 04:21:01 1999\n"),
                           "ctime_r test #1");
    }
#endif

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "ctime() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "ctime() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF ctime.c
