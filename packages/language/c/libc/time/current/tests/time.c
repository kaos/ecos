//=================================================================
//
//        time.c
//
//        Testcase for C library time()
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
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
// Description:   Contains testcode for C library time() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/libc_time.h>   // Configuration header

// INCLUDES

#include <time.h>
#include <cyg/infra/testcase.h>

// CONSTANTS

// This defines how many loops before we decide that
// time() doesn't work
#define MAX_TIMEOUT 50000000


// FUNCTIONS

int
main(int argc, char *argv[])
{
    time_t t1, t2;
    unsigned long ctr;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "time() function");

    t1 = time(&t2);

    CYG_TEST_PASS_FAIL(t1==t2, "time() return value == argument");
    
    if (t1 == (time_t)-1)  // unimplemented is just as valid
    {
#ifndef CYGSEM_LIBC_TIME_TIME_WORKING
        CYG_TEST_PASS_FINISH( "time() returns -1, meaning unimplemented");
#else
        CYG_TEST_FAIL("time() returned -1 unnecessarily");
#endif
    } // if

    // First wait for a clock tick 

    for (ctr = 0; ctr<MAX_TIMEOUT; ctr++) {
        if ((t2=time(NULL)) > t1)
            break; // Hit the next time pulse
    }
    CYG_TEST_PASS_FAIL( ctr< MAX_TIMEOUT, "time()'s state changes");
    
#ifdef CYGSEM_LIBC_TIME_SETTIME_WORKING
    CYG_TEST_PASS_FAIL(cyg_libc_time_settime(0)==0, "Set time to 0");
    
    t1 = time(NULL);
    
    // give it a small amount of tolerance
    CYG_TEST_PASS_FAIL(t1 < 3, "t1 remembered setting");
    
    CYG_TEST_PASS_FAIL(cyg_libc_time_settime(1000)==0, "Set time to 1000");
    
    // give it a small amount of tolerance
    CYG_TEST_PASS_FAIL(t1 < 1003, "t1 remembered setting");
    
#else // ! CYGSEM_LIBC_TIME_SETTIME_WORKING
    CYG_TEST_PASS_FAIL(cyg_libc_time_settime(0)!=0,
                       "Set time expected fail");
#endif // CYGSEM_LIBC_TIME_SETTIME_WORKING

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "time() function");
} // main()

// EOF time.c
