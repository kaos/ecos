//=================================================================
//
//        getenv.c
//
//        Testcase for C library getenv()
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
// Date:          2000-04-30
// Description:   Contains testcode for C library getenv() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <stdlib.h>                 // Main header for stdlib functions
#include <cyg/infra/testcase.h>     // Testcase API

// GLOBALS

extern char **environ;              // Standard environment definition

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

    char *env1[] = { NULL };
    char *env2[] = { "WIBBLE=fred", NULL };
    char *env3[] = { "PATH=/usr/local/bin:/usr/bin",
                     "HOME=/home/fred",
                     "TEST=1234=5678",
                     "home=hatstand",
                     NULL };

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library getenv() function");


    // Check 1
    str = getenv("ThisIsAVeryUnlikelyName");
    CYG_TEST_PASS_FAIL( str==NULL, "Simple getenv() default environ" );

    // Check 2
    environ = (char **)&env1;
    str = getenv("wibble");
    CYG_TEST_PASS_FAIL( str==NULL, "Simple getenv() with empty environ" );
    
    // Check 3
    environ = (char **)&env2;
    str = getenv("WIBBLE");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "fred"),
                        "Simple getenv()" );

    // Check 4
    str = getenv("wibble");
    CYG_TEST_PASS_FAIL( str==NULL,
                        "Simple getenv() for something not in the "
                        "environment" );

    // Check 5
    environ = (char **)&env3;
    str = getenv("PATH");
    CYG_TEST_PASS_FAIL( (str!= NULL) &&
                        !my_strcmp(str,"/usr/local/bin:/usr/bin"),
                        "Multiple string environment" );

    // Check 6
    str = getenv("PATh");
    CYG_TEST_PASS_FAIL( str==NULL, "getenv() for something not in the "
                        "environment for multiple string environment" );

    // Check 7
    str = getenv("home");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "hatstand"),
                        "Case-sensitive environment names" );

    // Check 8
    str = getenv("TEST");
    CYG_TEST_PASS_FAIL( (str != NULL) && !my_strcmp(str, "1234=5678"),
                        "environment value containing '='" );

//    CYG_TEST_NA("Testing is not applicable to this configuration");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library getenv() function");

} // main()

// EOF getenv.c
