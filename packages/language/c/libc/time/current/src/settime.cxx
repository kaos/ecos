//========================================================================
//
//      settime.cxx
//
//      Date and time support function to set time
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
// Date:          1999-02-26
// Purpose:       Provide support function to set the time
// Description:   The wallclock device is used to store the time in UTC
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_time.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <time.h>                  // Main date and time definitions
#include <cyg/infra/cyg_ass.h>     // Assertion infrastructure
#include <cyg/infra/cyg_trac.h>    // Tracing infrastructure
#ifdef CYGSEM_LIBC_TIME_SETTIME_WORKING
# include <cyg/io/wallclock.hxx> // Wallclock class definitions
#endif

// FUNCTIONS

/////////////////////////////
// cyg_libc_time_settime() //
/////////////////////////////
//
// This function sets the current time for the system. The time is
// specified as a time_t in UTC. It returns non-zero on error.
//

externC cyg_bool
cyg_libc_time_settime( time_t utctime )
{
    CYG_REPORT_FUNCNAMETYPE("cyg_libc_time_settime", "returning status %d");
    CYG_REPORT_FUNCARG1DV(utctime);

#ifdef CYGSEM_LIBC_TIME_SETTIME_WORKING

    Cyg_WallClock::wallclock->set_current_time((cyg_uint32) utctime);
    
    CYG_REPORT_RETVAL(false);
    return false;
#else
    CYG_REPORT_RETVAL(true);
    return true;
#endif

} // cyg_libc_time_settime()

// EOF settime.cxx
