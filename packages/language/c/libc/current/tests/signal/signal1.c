//========================================================================
//
//      signal1.c
//
//      ISO C signal handling test
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-18
// Purpose:       Test signal functionality
// Description:   This file contains a number of tests for ISO C signal
//                handling
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/testcase.h>    // Test infrastructure
#include <sys/cstartup.h>          // C library initialisation

#ifdef CYGPKG_LIBC_SIGNALS
#include <signal.h>                // Signal functions
#include <setjmp.h>                // setjmp(), longjmp()
#include <stdlib.h>                // abort()

// STATICS

static int state;
static jmp_buf jbuf;

// PROTOTYPES

// Private libc abort() function
externC void __abort(void);

#endif // CYGPKG_LIBC_SIGNALS

// FUNCTIONS

externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()

#ifdef CYGPKG_LIBC_SIGNALS

static void
myhandler1(int signal)
{
    CYG_TEST_INFO("myhandler1 called");
    ++state;
} // myhandler1()

static void
myhandler2(int signal)
{
    CYG_TEST_INFO("myhandler2 called");
    ++state;
    longjmp(jbuf, 1);
} // myhandler2()

#endif // ifdef CYGPKG_LIBC_SIGNALS

int
main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC_SIGNALS
    __sighandler_t handler1;
    int rc;

    // special callout to request GDB to alter its handling of signals
    CYG_TEST_GDBCMD("handle SIGTERM nostop");
    CYG_TEST_GDBCMD("handle SIGABRT nostop");

#endif // ifdef CYGPKG_LIBC_SIGNALS

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library signal functions");
#ifdef CYGPKG_LIBC_SIGNALS

    // Test 1

    CYG_TEST_INFO("Test 1");
    state = 1;
    handler1 = signal(SIGTERM, &myhandler1);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGTERM handler initialized to default");

    rc = raise(SIGTERM);

    CYG_TEST_PASS_FAIL(0==rc, "raise(SIGTERM) did not return error");

    CYG_TEST_PASS_FAIL(2==state, "SIGTERM handler returned correctly");

    // Test 2

    CYG_TEST_INFO("Test 2");

    state = 2;
    handler1 = signal(SIGTERM, &myhandler2);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGTERM handler reset to default after test 1");

    handler1 = signal(SIGTERM, &myhandler1);

    CYG_TEST_PASS_FAIL(handler1 == &myhandler2,
                       "SIGTERM handler was set correctly");

    rc = raise(SIGTERM);

    CYG_TEST_PASS_FAIL(0==rc, "raise(SIGTERM) did not return error");

    CYG_TEST_PASS_FAIL(3==state, "SIGTERM handler returned correctly");

    // Test 3

    CYG_TEST_INFO("Test 3");

    handler1 = signal(SIGTERM, &myhandler2);
    
    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGTERM handler reset to default after test 2");

    handler1 = signal(SIGTERM, SIG_DFL);

    CYG_TEST_PASS_FAIL(handler1 == &myhandler2,
                       "SIGTERM handler was set correctly");

    // Test 4

    CYG_TEST_INFO("Test 4");

    state = 4;
    handler1 = signal(SIGTERM, SIG_IGN);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGTERM handler was set correctly after test 3");
    rc = raise(SIGTERM);

    CYG_TEST_PASS_FAIL(0==rc, "raise(SIGTERM) did not return error");

    CYG_TEST_PASS_FAIL(4==state, "SIGTERM ignored");
    
    // Test 5

    CYG_TEST_INFO("Test 5");

    state = 5;
    handler1 = signal(SIGTERM, &myhandler2);

    // SIG_IGN doesn't reset back to SIG_DFL after a raise()
    CYG_TEST_PASS_FAIL(handler1 == SIG_IGN,
                       "SIGTERM handler was set correctly after test 4");
    
    if (0==setjmp(jbuf)) {
        raise(SIGTERM);
        CYG_TEST_FAIL("raise returned");
    }
    
    CYG_TEST_PASS_FAIL(6==state, "SIGTERM handler returned correctly");

    // Test 6

    CYG_TEST_INFO("Test 6");

    state = 6;
    handler1 = signal(SIGABRT, &myhandler2);
    
    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGABRT handler initialized to default");
    
    if (0==setjmp(jbuf)) {
        abort();
        CYG_TEST_FAIL("abort returned");
    }

    CYG_TEST_PASS_FAIL(7==state, "SIGABRT handler returned correctly");
    
#else
    CYG_TEST_NA("Testing not applicable to this configuration");
#endif

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library signal functions");

} // main()

// EOF signal1.c
