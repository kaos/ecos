//=================================================================
//
//        ldiv.c
//
//        Testcase for C library ldiv()
//
//=================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Contributors:  jlarmour
// Date:          1999-02-03
// Description:   Contains testcode for C library ldiv() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE
// COMPOUND_TESTCASE

// INCLUDES

#include <stdlib.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation


// FUNCTIONS

externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
    long num, denom;
    ldiv_t result;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "ldiv() function");

    num = 10232;
    denom = 43;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==237) && (result.rem==41),
                        "ldiv( 10232, 43 )");

    num = 4232;
    denom = 2000;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==2) && (result.rem==232),
                        "ldiv( 4232, 2000 )");

    num = 20;
    denom = 20;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==1) && (result.rem==0),
                   "ldiv( 20, 20 )");

    num = -5;
    denom = 4;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==-1) && (result.rem==-1),
                        "ldiv( -5, 4 )");

    num = 5;
    denom = -4;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==-1) && (result.rem==1),
                        "ldiv( 5, -4 )");

    num = -5;
    denom = -3;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==1) && (result.rem==-2),
                        "ldiv( -5, -3 )");

    num = -7;
    denom = -7;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==1) && (result.rem==0),
                        "ldiv( -7, -7 )");

    num = 90123456;
    denom = 12345678;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==7) && (result.rem==3703710),
                   "ldiv( 90123456, 12345678 )");

    num = 90123456;
    denom = 123;
    result = ldiv(num, denom);
    CYG_TEST_PASS_FAIL( (result.quot==732711) && (result.rem==3),
                        "ldiv( 90123456, 123 )");

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "ldiv() function");

} // main()

// EOF ldiv.c
