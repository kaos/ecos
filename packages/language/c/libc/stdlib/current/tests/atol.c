//=================================================================
//
//        atol.c
//
//        Testcase for C library atol()
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
// Description:   Contains testcode for C library atol() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <stdlib.h>
#include <cyg/infra/testcase.h>

// FUNCTIONS

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

    // Check 1
    my_strcpy(x, "20");
    CYG_TEST_PASS_FAIL( atol(x) == 20, "atol(20)");

    // Check 2
    my_strcpy(x, "1972");
    CYG_TEST_PASS_FAIL( atol(x) == 1972, "atol(1972)");

    // Check 3
    my_strcpy(x, "-98765");
    CYG_TEST_PASS_FAIL( atol(x) == -98765, "atol(-98765)");

    // Check 4
    my_strcpy(x, "  -98765xxx");
    CYG_TEST_PASS_FAIL( atol(x) == -98765, "atol(   -98765xxx)");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "atol() function");
} // test()

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "atol() function");

    test(0);

    CYG_TEST_NA("Testing is not applicable to this configuration");
} // main()

// EOF atol.c
