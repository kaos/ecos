//==========================================================================
//
//        tcdiag.cxx
//
//        Infrastructure diag test harness.
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
// Author(s):     dsm
// Contributors:  dsm, jlarmour
// Date:          1999-02-16
// Description:   Test harness implementation that uses the infrastructure
//                diag channel.  This is intended for manual testing.
// 
//####DESCRIPTIONEND####

#include <pkgconf/infra.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_TARGET_H           // get initialization for
#include CYGBLD_HAL_PLATFORM_H         //   cyg_test_is_simulator

#include <cyg/infra/cyg_type.h>        // base types
#include <cyg/hal/hal_arch.h>          // any architecture specific stuff
#include <cyg/infra/diag.h>            // HAL polled output
#include <cyg/infra/testcase.h>        // what we implement

#include <cyg/hal/hal_intr.h>          // exit macro, if defined

#ifdef CYGHWR_TARGET_SIMULATOR_NO_GDB_WORKING
int cyg_test_is_simulator = 1;         // set this anyway
#else
int cyg_test_is_simulator = 0;         // infrastructure changes as necessary
#endif


externC void
cyg_test_init(void)
{
    // currently nothing
}

externC void
cyg_test_output(Cyg_test_code status, const char *msg, int line,
                const char *file)
{
    char *st;

    switch (status) {
    case CYGNUM_TEST_FAIL:
        st = "FAIL:";
        break;
    case CYGNUM_TEST_PASS:
        st = "PASS:";
        break;
    case CYGNUM_TEST_EXIT:
        st = "EXIT:";
        break;
    case CYGNUM_TEST_INFO:
        st = "INFO:";
        break;
    case CYGNUM_TEST_GDBCMD:
        st = "GDB:";
        break;
    case CYGNUM_TEST_NA:
        st = "NOTAPPLICABLE:";
        break;
    default:
        st = "UNKNOWN STATUS:";
        break;
    }

    diag_write_string(st);
    diag_write_char('<');
    diag_write_string(msg);
    diag_write_char('>');
    if( CYGNUM_TEST_FAIL == status ) {
        diag_write_string(" Line: ");
        diag_write_dec(line);
        diag_write_string(", File: ");
        diag_write_string(file);
    }
    diag_write_char('\n');
}

// This is an appropriate function to set a breakpoint on
externC void
cyg_test_exit(void)
{
#ifdef CYGHWR_TEST_PROGRAM_EXIT
    CYGHWR_TEST_PROGRAM_EXIT();
#endif

    for(;;)
        ;
}
// EOF tcdiag.cxx
