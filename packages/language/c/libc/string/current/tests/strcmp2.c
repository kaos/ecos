//=================================================================
//
//        strcmp2.c
//
//        Testcase for C library strcmp()
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
// Contributors:  
// Date:          2000-04-30
// Description:   Contains testcode for C library strcmp() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <string.h>
#include <cyg/infra/testcase.h>

// CONSTANTS

#define NUM_ROBUSTNESS_RUNS 300

// FUNCTIONS

// Functions to avoid having to use libc strings

static char *my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()


int main( int argc, char *argv[] )
{
    char x[300];
    char y[300];
    char z[300];
    int ctr;
    int fail;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strcmp() function");
    CYG_TEST_INFO("This testcase tests robustness, and may take some time");

    fail = 0;
    for (ctr = 0; ctr < NUM_ROBUSTNESS_RUNS; ctr++)
    {
        my_strcpy(x, "Spare a talent for an old ex-leper");
        my_strcpy(y, "Spare a talent for an old ex-leper");
        my_strcpy(z, "I was hopping along, when suddenly he");

        if (strcmp(x, y) != 0)
        {
            fail = 1;
            break;
        }
        if (strcmp(x, z) == 0)
        {
            fail = 1;
            break;
        } // if
    } // for

    CYG_TEST_PASS_FAIL( (fail==0), "Robustness test");

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strcmp() function");
} // main()


// EOF strcmp2.c
