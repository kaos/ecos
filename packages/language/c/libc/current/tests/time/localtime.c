//=================================================================
//
//        localtime.c
//
//        Testcase for C library localtime() function
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
// Description:   Contains testcode for C library localtime() function
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


static int
cmp_structtm(struct tm *tm1, struct tm *tm2)
{
    if ((tm1->tm_sec   != tm2->tm_sec)   ||
        (tm1->tm_min   != tm2->tm_min)   ||
        (tm1->tm_hour  != tm2->tm_hour)  ||
        (tm1->tm_mday  != tm2->tm_mday)  ||
        (tm1->tm_mon   != tm2->tm_mon)   ||
        (tm1->tm_year  != tm2->tm_year)  ||
        (tm1->tm_wday  != tm2->tm_wday)  ||
        (tm1->tm_yday  != tm2->tm_yday)  ||
        (tm1->tm_isdst != tm2->tm_isdst))
        return 1;
    else
        return 0;
}

static void
test( CYG_ADDRWORD data )
{
    struct tm tm1, *tm2;
    time_t t;
    
    // make this predictable - independent of the user option
    cyg_libc_time_setzoneoffsets(0, 3600);
    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTOFF );

    tm1.tm_sec = 4;
    tm1.tm_min = 23;
    tm1.tm_hour = 20;
    tm1.tm_mday = 21;
    tm1.tm_mon = 1;
    tm1.tm_year = 74;
    tm1.tm_wday = 4;
    tm1.tm_yday = 51;
    tm1.tm_isdst = 0;
    
    t = (time_t)130710184;

    tm2 = localtime(&t);
    CYG_TEST_PASS_FAIL(!cmp_structtm(&tm1, tm2), "localtime test #1");


    tm1.tm_sec = 1;
    tm1.tm_min = 21;
    tm1.tm_hour = 4;
    tm1.tm_mday = 5;
    tm1.tm_mon = 0;
    tm1.tm_wday = 2;
    tm1.tm_yday = 4;
    tm1.tm_year = 99;
    tm1.tm_isdst = 0;

    t = (time_t)915510061;
    
    tm2 = localtime(&t);
    CYG_TEST_PASS_FAIL(!cmp_structtm(&tm1, tm2), "localtime test #2");

    tm1.tm_sec = 54;
    tm1.tm_min = 24;
    tm1.tm_hour = 1;
    tm1.tm_mday = 1;
    tm1.tm_mon = 0;
    tm1.tm_year = 100;
    tm1.tm_wday = 6;
    tm1.tm_yday = 0;
    tm1.tm_isdst = 0;

    t = (time_t)946689894;

    tm2 = localtime(&t);
    CYG_TEST_PASS_FAIL(!cmp_structtm(&tm1, tm2), "localtime Y2K test #3");

    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTON );

    tm1.tm_sec = 54;
    tm1.tm_min = 24;
    tm1.tm_hour = 0;
    tm1.tm_mday = 1;
    tm1.tm_mon = 5;
    tm1.tm_wday = 2;
    tm1.tm_yday = 150;
    tm1.tm_year = 66;
    tm1.tm_isdst = 1;

    t = (time_t)-113186106;

    tm2 = localtime(&t);
    CYG_TEST_PASS_FAIL(!cmp_structtm(&tm1, tm2), "localtime test #4");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "localtime() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "localtime() function");

    START_TEST( test );

    CYG_TEST_NA("Testing is not applicable to this configuration");

} // main()

// EOF localtime.c
