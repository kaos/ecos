//=================================================================
//
//        asctime.c
//
//        Testcase for C library asctime() function
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
// Contributors:  jlarmour
// Date:          1999-03-05
// Description:   Contains testcode for C library asctime() function
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
    struct tm tm1;
    char *ret;
    
    tm1.tm_sec = 4;
    tm1.tm_min = 23;
    tm1.tm_hour = 20;
    tm1.tm_mday = 21;
    tm1.tm_mon = 1;
    tm1.tm_year = 74;
    tm1.tm_wday = 4;
    tm1.tm_yday = 51;
    tm1.tm_isdst = 0;

    ret = asctime(&tm1);

    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Thu Feb 21 20:23:04 1974\n"),
                       "asctime test #1");
    tm1.tm_sec = 3;
    tm1.tm_min = 51;
    tm1.tm_hour = 5;
    tm1.tm_mday = 2;
    tm1.tm_mon = 11;
    tm1.tm_year = 68;
    tm1.tm_wday = 1;
    tm1.tm_yday = 336;
    tm1.tm_isdst = 0;

    ret = asctime(&tm1);

    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Mon Dec 02 05:51:03 1968\n"),
                       "asctime test #2");

    // make this predictable - independent of the user option
    cyg_libc_time_setzoneoffsets(0, 3600);

    tm1.tm_sec = 3;
    tm1.tm_min = 51;
    tm1.tm_hour = 5;
    tm1.tm_mday = 2;
    tm1.tm_mon = 6;
    tm1.tm_year = 68;
    tm1.tm_wday = 2;
    tm1.tm_yday = 183;
    tm1.tm_isdst = 1;

    ret = asctime(&tm1);

    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Tue Jul 02 05:51:03 1968\n"),
                       "asctime test #3");

    tm1.tm_sec = 0;
    tm1.tm_min = 0;
    tm1.tm_hour = 0;
    tm1.tm_mday = 1;
    tm1.tm_mon = 0;
    tm1.tm_year = 0;
    tm1.tm_wday = 1;
    tm1.tm_yday = 0;
    tm1.tm_isdst = 0;

    ret = asctime(&tm1);

    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Mon Jan 01 00:00:00 1900\n"),
                       "asctime test #4");

    tm1.tm_sec = 0;
    tm1.tm_min = 0;
    tm1.tm_hour = 0;
    tm1.tm_mday = 1;
    tm1.tm_mon = 0;
    tm1.tm_year = 100;
    tm1.tm_wday = 6;
    tm1.tm_yday = 0;
    tm1.tm_isdst = 0;

    ret = asctime(&tm1);

    CYG_TEST_PASS_FAIL(!my_strcmp(ret, "Sat Jan 01 00:00:00 2000\n"),
                       "asctime Y2K test #5");

#ifdef CYGFUN_LIBC_TIME_POSIX
    {
        char ret2[100];
        
        tm1.tm_sec = 3;
        tm1.tm_min = 51;
        tm1.tm_hour = 5;
        tm1.tm_mday = 2;
        tm1.tm_mon = 11;
        tm1.tm_year = 68;
        tm1.tm_wday = 1;
        tm1.tm_yday = 336;
        tm1.tm_isdst = 0;
        
        ret = asctime_r(&tm1, ret2);
        
        CYG_TEST_PASS_FAIL((ret==ret2) &&
                           !my_strcmp(ret, "Mon Dec 02 05:51:03 1968\n"),
                           "asctime_r test #1");

    }
#endif


    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "asctime() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "asctime() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF asctime.c
