//=================================================================
//
//        strtok.c
//
//        Testcase for C library strtok()
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
// Date:          1999-01-20
// Description:   Contains testcode for C library strtok() function
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


static int
my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ ) {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


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
    char x[300];

    // Check 1
    my_strcpy(x, "tok1 tok2 3kot");
    if (( my_strcmp("tok1", strtok(x, " ")) == 0 ) &&
        ( my_strcmp("tok2", strtok(NULL, " ")) == 0 ) &&
        ( my_strcmp("3kot", strtok(NULL, " ")) == 0 ))

        CYG_TEST_PASS("Simple strtok() #1");
    else
        CYG_TEST_FAIL("Simple strtok() #1");

    // Check 2
    my_strcpy(x, "Hitteth@ye *not, the vicar");
    if (( my_strcmp("Hitteth", strtok(x, "@,*")) == 0 ) &&
        ( my_strcmp("ye ", strtok(NULL, ",@*")) == 0 ) &&
        ( my_strcmp("not", strtok(NULL, "*@,")) == 0 ) &&
        ( my_strcmp(" the vicar", strtok(NULL, "*@,")) == 0 ) )

        CYG_TEST_PASS("Simple strtok() #2");
    else
        CYG_TEST_FAIL("Simple strtok() #2");

    // Check 3
    my_strcpy(x, "on his bonce, with thine football");
    if ( my_strcmp(strtok(x, "@*"), x) == 0 )
        CYG_TEST_PASS("strtok() with token delimiters not found");
    else
        CYG_TEST_FAIL("strtok() with token delimiters not found");

    // Check 4
    my_strcpy(x, "@@@,,,...@,.,.@");
    if (strtok(x, "@,.") == NULL)
        CYG_TEST_PASS("All characters of string are token delimiters");
    else
        CYG_TEST_FAIL("All characters of string are token delimiters");

    // Check 5
    my_strcpy(x, "");
    if (strtok(x, "@,.") == NULL)
        CYG_TEST_PASS("String to tokenize empty");
    else
        CYG_TEST_FAIL("String to tokenize empty");

    // Check 6
    my_strcpy(x, "fdafda");
    if (strtok(x, "") == x)
        CYG_TEST_PASS("String to tokenize empty");
    else
        CYG_TEST_FAIL("String to tokenize empty");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "strtok() function");

} // test()

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "strtok() function");
    CYG_TEST_INFO("This testcase provides simple basic tests");

    test(0);

    CYG_TEST_NA("Testing is not applicable to this configuration");
} // main()


// EOF strtok.c
