//========================================================================
//
//      time.cxx
//
//      ISO C date and time implementation for time()
//
//========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-03-04
// Purpose:      Provide implementation of ISO C time() as defined in
//               ISO C section 7.12.2.4
// Description:  This file provides an implementation of time() using
//               the "wallclock" device
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_ass.h>     // Assertion infrastructure
#include <cyg/infra/cyg_trac.h>    // Tracing infrastructure
#include <time.h>                  // Main date and time definitions

#ifdef CYGSEM_LIBC_TIME_TIME_WORKING
# include <cyg/devs/wallclock.hxx> // Wallclock class definitions
#endif

// FUNCTIONS

externC time_t
__time( time_t *timer )
{
    CYG_REPORT_FUNCNAMETYPE("__time", "returning %d");
    if (timer) // its allowed to be NULL
        CYG_CHECK_DATA_PTR( timer, "timer is not a valid pointer!");
    CYG_REPORT_FUNCARG1("timer = %08x", timer);

#ifdef CYGSEM_LIBC_TIME_TIME_WORKING
    time_t ret;

    ret = (time_t) Cyg_WallClock::wallclock->get_current_time();
    
    if (timer)
        *timer = ret;

    CYG_REPORT_RETVAL(ret);

    return ret;

#else // i.e. ifndef CYGSEM_LIBC_TIME_TIME_WORKING

    if (timer)
        *timer = (time_t)-1;
    CYG_REPORT_RETVAL(-1);
    return (time_t)-1;

#endif

} // __time()


// SYMBOL DEFINITIONS

externC time_t
time( time_t *timer ) CYGBLD_ATTRIB_WEAK_ALIAS(__time);

// EOF time.cxx
