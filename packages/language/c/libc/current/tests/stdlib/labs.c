//=================================================================
//
//        labs.c
//
//        Testcase for C library labs()
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
// Contributors:    jlarmour
// Date:          1998/6/3
// Description:   Contains testcode for C library labs() function
//
//
//####DESCRIPTIONEND####

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE
// COMPOUND_TESTCASE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <stdlib.h>
#include <cyg/infra/testcase.h>
#include <sys/cstartup.h>          // C library initialisation

// FUNCTIONS

externC void
cyg_package_start( void )
{
#ifdef CYGPKG_LIBC
    cyg_iso_c_start();
#else
    (void)main(0, NULL);
#endif
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    int x;
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "labs() function");

#ifdef CYGPKG_LIBC
    // Check 1
    x = 5;
    CYG_TEST_PASS_FAIL( labs(x) == 5, "labs(5)");

    // Check 2
    x = -5;
    CYG_TEST_PASS_FAIL( labs(x) == 5, "labs(-5)");
    
    // Check 3
    x = 12345;
    CYG_TEST_PASS_FAIL( labs(x) == 12345, "labs(12345)");

    // Check 4
    x = -23456;
    CYG_TEST_PASS_FAIL( labs(x) == 23456, "labs(-23456");

    // Check 5
    x = 3456789;
    CYG_TEST_PASS_FAIL( labs(x) == 3456789, "labs(3456789");

    // Check 6
    x = -23456789;
    CYG_TEST_PASS_FAIL( labs(x) == 23456789, "labs(-23456789");

#else // ifndef CYGPKG_LIBC
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "labs() function");
} // main()

// EOF labs.c
