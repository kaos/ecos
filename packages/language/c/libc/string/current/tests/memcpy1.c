//=================================================================
//
//        memcpy1.c
//
//        Testcase for C library memcpy()
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
// Description:   Contains testcode for C library memcpy() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <string.h>
#include <cyg/infra/testcase.h>

// FUNCTIONS

// Functions to avoid having to use libc strings

static int my_strlen(const char *s)
{
    const char *ptr;

    ptr = s;
    for ( ptr=s ; *ptr != '\0' ; ptr++ )
        ;

    return (int)(ptr-s);
} // my_strlen()


static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


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
    void *ret, *ptr1, *ptr2;
    char *c_ret;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "memcpy() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

    // Check 1
    ptr1 = x; 
    ptr2 = y;
    my_strcpy(x, "Great shot kid! That was one in a million!");
    ret = memcpy(ptr2, ptr1, my_strlen(x) + 1);
    CYG_TEST_PASS_FAIL( (my_strcmp(x, ptr2)==0), "Simple copy" );

    // Check return value
    CYG_TEST_PASS_FAIL( (my_strcmp(ret, ptr2)==0), "Simple copy return value");


    // Check 2
    ptr1 = x; 
    ptr2 = y;
    my_strcpy(x, "");
    my_strcpy(y, "xxxx"); // Bogus val to get overwritten
    ret = memcpy(ptr2, ptr1, 1);
    c_ret = ret;
    if ((*c_ret == '\0') && (y[0] == '\0') && (y[1] == 'x'))
        CYG_TEST_PASS("Simple copy with boundary check worked");
    else
        CYG_TEST_FAIL("Simple copy with boundary check failed");

    // Check 3
    ptr1 = x; 
    ptr2 = y;
    my_strcpy(x, "xxxx");
    my_strcpy(y, "yyyy");
    ret = memcpy(ptr1, ptr2, 0);
    c_ret = ret;
    if ((*c_ret =='x') && (x[0] == 'x'))
        CYG_TEST_PASS("Simple copy with size=0 worked");
    else
        CYG_TEST_FAIL("Simple copy with size=0 failed");

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "memcpy() function");
} // main()


// EOF memcpy1.c
