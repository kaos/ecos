//==========================================================================
//
//        wallclock2.cxx
//
//        WallClock battery backed up test
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          2000-03-29
// Description:   Tests the WallClock device.
//                Prints wallclock setting (for human verification that time
//                matches the expected value). Then tries to set/restore
//                clock value, and verifies that the WallClock epoch matches
//                that of the libc.
//####DESCRIPTIONEND####
// -------------------------------------------------------------------------

#include <pkgconf/system.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#ifdef CYGPKG_LIBC_TIME
#include <pkgconf/libc_time.h>
#endif
#ifdef CYGPKG_LIBC_STARTUP
#include <pkgconf/libc_startup.h>
#endif

#if !defined(CYGSEM_LIBC_TIME_CLOCK_WORKING)
# define NA_MSG "Requires libc time functions"
#elif !defined(CYGPKG_LIBC_STARTUP)
# define NA_MSG "Requires libc startup package"
#elif !defined(CYGINT_LIBC_STARTUP_CONTEXT)
# define NA_MSG "Requires main to be invoked"
#endif

#ifndef NA_MSG
#include <time.h>
#include <string.h>                     // strcmp
#include <cyg/io/wallclock.hxx>         // The WallClock API

// -------------------------------------------------------------------------


externC int
main (void )
{
    time_t now, test, test2;

    CYG_TEST_INIT();

    // make this predictable - independent of the user option
    cyg_libc_time_setzoneoffsets(0, 3600);
    cyg_libc_time_setdst( CYG_LIBC_TIME_DSTOFF );

    // Print out current setting so a human can verify that clock state
    // was valid.
    now = (time_t) Cyg_WallClock::wallclock->get_current_time();
    diag_printf("WallClock is set to: %s", ctime(&now));

    // Check that set/get works
    Cyg_WallClock::wallclock->set_current_time( 0 );
    test = (time_t) Cyg_WallClock::wallclock->get_current_time();
    // 2000.03.01 00:00:00 UTC
    Cyg_WallClock::wallclock->set_current_time( 951868800 );
    test2 = (time_t) Cyg_WallClock::wallclock->get_current_time();
    // Should test 2100 and 2400 leap year calculations as well, but 
    // these would overflow with today's time_t.
    Cyg_WallClock::wallclock->set_current_time( now );
    CYG_TEST_PASS_FAIL(2 >= test,  // each operation can take one second
                       "Can set WallClock to epoch");
    CYG_TEST_PASS_FAIL(951868800+2 > test2 &&
                       951868800 <= test2,
                       "WallClock date conversion Y2K safe");

    // Test that the wallclock and libc use same epoch.
    test = (time_t) 0;
    CYG_TEST_PASS_FAIL(!strcmp(ctime(&test), "Thu Jan 01 00:00:00 1970\n"),
                       "WallClock epoch matches libc epoch");

    CYG_TEST_FINISH("Finished wallclock battery test");
}

#else 

externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH(NA_MSG);
}

#endif // NA_MSG

// -------------------------------------------------------------------------
// EOF wallclock2.cxx
