//=================================================================
//
//        strpbrk.c
//
//        Testcase for C library strpbrk()
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
// Description:   Contains testcode for C library strpbrk() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <string.h>
#include <cyg/infra/testcase.h>


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
    char *ret;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strpbrk() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

    // Check 1
    my_strcpy(x, "I will not have my fwends widiculed by the common soldiewy");
    my_strcpy(y, "zpm1"); // Finds the 'm'
    ret = strpbrk(x, y);
    CYG_TEST_PASS_FAIL( (ret == &x[16]), "Simple strpbrk()" );

    // Check 2
    my_strcpy(x, "Not bad for a little fur ball. You! Stay here.");
    my_strcpy(y, "zx");
    ret = strpbrk(x, y);
    CYG_TEST_PASS_FAIL( (ret == NULL), "Letters not found");

    // Check 3 (boundary condition)
    my_strcpy(x, "");
    my_strcpy(y, "zx");
    ret = strpbrk(x, y);
    CYG_TEST_PASS_FAIL( (ret == NULL), "String to search empty" );

    // Check 4 (boundary condition)
    my_strcpy(x, "zx");
    my_strcpy(y, "");
    ret = strpbrk(x, y);
    CYG_TEST_PASS_FAIL( (ret == NULL), "Empty search string" );

    // Check 5 (boundary condition)
    my_strcpy(x, "");
    my_strcpy(y, "");
    ret = strpbrk(x, y);
    CYG_TEST_PASS_FAIL( (ret == NULL), "Both strings empty" );

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strpbrk() function");

} // main()

// EOF strpbrk.c
