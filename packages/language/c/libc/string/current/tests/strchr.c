//=================================================================
//
//        strchr.c
//
//        Testcase for C library strchr()
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
// Description:   Contains testcode for C library strchr() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <string.h>
#include <cyg/infra/testcase.h>


// FUNCTIONS

// Functions to avoid having to use libc strings

static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


int main( int argc, char *argv[] )
{
    char x[] = "Your feeble skills are no match for the power of "
               "the dark side!";
    char empty_str[] = "";
    char *ret;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strchr() function");

    // Check 1
    ret = strchr(x, (int)'k');
    CYG_TEST_PASS_FAIL( (my_strcmp(ret, x+13) == 0), "Simple test 1" );

    // Check 2
    ret = strchr(x, (int)'p');
    CYG_TEST_PASS_FAIL( (my_strcmp(ret, x+40) == 0), "Simple test 2" );

    // Check 3
    ret = strchr(x, (int)'z');
    CYG_TEST_PASS_FAIL( (ret == NULL), "Simple test 3" );

    // Check 4 (boundary condition)
    ret = strchr(x, (int)'\0');
    CYG_TEST_PASS_FAIL( ret == (x+sizeof(x)-1), "Boundary test 1" );

    // Check 5 (boundary condition)
    ret = strchr( empty_str, (int)'\0' );
    CYG_TEST_PASS_FAIL( ret == empty_str, "Boundary test 2" );

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strchr() function");
} // main()

// EOF strchr.c
