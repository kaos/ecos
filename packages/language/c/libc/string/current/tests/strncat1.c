//=================================================================
//
//        strncat1.c
//
//        Testcase for C library strncat()
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
// Description:   Contains testcode for C library strncat() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/system.h>
#include <pkgconf/libc_string.h>   // Configuration header

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
    char *ret;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "memmove() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

    // Check 1
    my_strcpy(x, "One ring to rule them all.");
    my_strcpy(y, "One ring to find them.");
    ret = strncat(x, y, my_strlen(y)+1);
    if ( my_strcmp(x, "One ring to rule them all.One ring to find them.")==0 ) 
        CYG_TEST_PASS("Simple concatenation");
    else 
        CYG_TEST_FAIL("Simple concatenation");
    // Check return val
    CYG_TEST_PASS_FAIL( ( ret == x ), "Simple concatenation return value" );


    // Check 2
    my_strcpy(x, "One ring to bring them all,");
    my_strcpy(y, "");
    ret = strncat(x, y, my_strlen(y)+1);
    if ( my_strcmp(x, "One ring to bring them all,")==0 ) 
        CYG_TEST_PASS("Concatenation of empty string");
    else 
        CYG_TEST_FAIL("Concatenation of empty string");
    // Check return val
    CYG_TEST_PASS_FAIL( ( ret == x ),
                        "Concatenation of empty string return value" );


    // Check 3
    my_strcpy(x, "and in the darkness bind them");
    my_strcpy(y, "");
    ret = strncat(y, x, my_strlen(x)+1 );
    if ( my_strcmp(x, "and in the darkness bind them")==0 ) 
        CYG_TEST_PASS("Concatenation to empty string");
    else 
        CYG_TEST_FAIL("Concatenation to empty string");
    // Check return val
    CYG_TEST_PASS_FAIL( ( ret == y ),
                        "Concatenation to empty string return value" );

    // Check 4
    my_strcpy(x, "in the land of ");
    my_strcpy(y, "Mordor, where the shadows lie");
    ret = strncat(x, y, 6 );
    if ( my_strcmp(x, "in the land of Mordor")==0 ) 
        CYG_TEST_PASS("Concatenation of partial string string");
    else 
        CYG_TEST_FAIL("Concatenation of partial string");
    // Check return val
    CYG_TEST_PASS_FAIL( ( ret == x ),
                        "Concatenation to empty string return value" );

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strncat() function");
} // main()


// EOF strncat1.c
