//=================================================================
//
//        rand4.c
//
//        Testcase for C library rand()
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
// Author(s):     rosalia
// Contributors:  jlarmour
// Date:          2000-04-30
// Description:   Contains testcode for C library rand() function. This tests
//                that random numbers do not have periodicity in the lower
//                bit
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/libc_stdlib.h>   // Configuration header

// INCLUDES

#include <stdlib.h>
#include <cyg/infra/testcase.h>

// CONSTANTS

#define TEST_LENGTH 100000        // how many samples to take

// DEFINES

#define TEST_VALID (!defined(CYGIMP_LIBC_RAND_SIMPLEST))

// FUNCTIONS

int
main(int argc, char *argv[])
{
#if TEST_VALID
    int i;
    int r, prev, prevprev;
    int how_many_periodics = 0;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "rand() function");

    CYG_TEST_INFO("This test tests the distribution of random numbers and");
    CYG_TEST_INFO("may take some time");

#if TEST_VALID
    r = rand() % 2;
    prev = r;
    r = rand() % 2;
    for (i = 0; i < TEST_LENGTH; ++i) {
      prevprev = prev;
      prev = r;
      r = rand() % 2;
      if (r == prevprev) {
        ++how_many_periodics;
      }
      if (how_many_periodics > (2*TEST_LENGTH)/3) {
        break;
      }
    }

    CYG_TEST_PASS_FAIL( (how_many_periodics <= (2*TEST_LENGTH)/3),
                        "periodicity of rand() in lowest bit");

#else
    
    // TODO: should be an _expected_ fail i.e. XFAIL
    CYG_TEST_NA("Chosen rand algorithm is known to fail this test");
    
#endif

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for "
                    "C library rand() function");
} // main()


// EOF rand4.c
