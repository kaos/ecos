//=================================================================
//
//        setlocale.c
//
//        Testcase for C library setlocale() function
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
// Contributors:
// Date:          2000-04-18
// Description:   Contains testcode for C library setlocale() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <locale.h>                // header for functions to test
#include <cyg/infra/testcase.h>    // Testcase API


// FUNCTIONS

static int
my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()

int
main( int argc, char *argv[] )
{
    char *str;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library setlocale() function");

    // Check 1
    str = setlocale(LC_CTYPE, NULL);
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Initial locale for LC_CTYPE is \"C\"" );
    
    // Check 2
    str = setlocale(LC_ALL, NULL);
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "#C#C#C#C#C#" ),
                        "Initial locale for LC_ALL is correct" );

    // Check 3
    str = setlocale(LC_COLLATE, "C");
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Set locale for LC_COLLATE to \"C\"" );

    // Check 4
    str = setlocale(LC_MONETARY, "");
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Set default locale for LC_MONETARY" );
    str = setlocale(LC_MONETARY, NULL);
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Stored default locale for LC_MONETARY" );

    // Check 5
    str = setlocale(LC_ALL, "C");
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Set locale for LC_COLLATE to \"C\"" );

    // Check 6
    str = setlocale(LC_ALL, "");
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "C" ),
                        "Set locale for LC_COLLATE to default" );

    // Check 7
    str = setlocale(LC_ALL, NULL);
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "#C#C#C#C#C#" ),
                        "Get locale for LC_ALL" );
    str = setlocale(LC_ALL, str);
    CYG_TEST_PASS_FAIL( !my_strcmp(str, "#C#C#C#C#C#" ),
                        "Set locale for LC_ALL from retrieved locale" );
    

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library setlocale() function");
} // main()

// EOF setlocale.c
