//===========================================================================
//
//      clock.cxx
//
//      ISO C date and time implementation for clock()
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: jlarmour
// Date:         1999-03-05
// Purpose:      Provides an implementation of the ISO C function clock()
//               from ISO C section 7.12.2.1
// Description:  This file uses the kernel real time clock to determine
//               the complete running time of the system - since we only
//               have one task, even though there are perhaps multiple threads
//               that is still the running time of the "program"
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>    // Configuration header

#ifdef CYGSEM_LIBC_TIME_CLOCK_WORKING
# include <pkgconf/kernel.h> // Kernel config header
#endif

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_ass.h>     // Assertion infrastructure
#include <cyg/infra/cyg_trac.h>    // Tracing infrastructure

#include <time.h>                  // Header for all time-related functions

#ifdef CYGSEM_LIBC_TIME_CLOCK_WORKING
# include <cyg/kernel/clock.hxx>   // Kernel clock definitions
# include <cyg/kernel/clock.inl>   // Kernel clock inline functions
#endif


// TRACE

# if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_TIME_CLOCK_TRACE_LEVEL)
static int clock_trace = CYGNUM_LIBC_TIME_CLOCK_TRACE_LEVEL;
#  define TL1 (0 < clock_trace)
# else
#  define TL1 (0)
# endif

// FUNCTIONS

externC clock_t
__clock( void )
{
    CYG_REPORT_FUNCNAMETYPE( "__clock", "returning clock tick %d" );
    CYG_REPORT_FUNCARGVOID();

#ifdef CYGSEM_LIBC_TIME_CLOCK_WORKING
    cyg_tick_count curr_clock;            // kernel clock value
    Cyg_Clock::cyg_resolution resolution; // kernel clock resolution
    clock_t clocks;

    CYG_TRACE0( TL1, "getting clock resolution" );
    
    // get the resolution
    resolution = Cyg_Clock::real_time_clock->get_resolution();

    CYG_TRACE2( TL1, "got resolution dividend %d divisor %d. Getting "
                "clock value", resolution.dividend, resolution.divisor );

    // get the value
    curr_clock = Cyg_Clock::real_time_clock->current_value();

    CYG_TRACE1( TL1, "got clock value %d", curr_clock );
    
    // scale the value so that clock()/CLOCKS_PER_SEC works - clock_t
    // should be big enough to cope
    clocks = ((clock_t)curr_clock * resolution.dividend) / 
      (resolution.divisor * (1000000000 / CLOCKS_PER_SEC));

    CYG_REPORT_RETVAL( clocks );
    return clocks;

#else // i.e. ifndef CYGSEM_LIBC_TIME_CLOCK_WORKING
    CYG_REPORT_RETVAL( -1 );
    return (clock_t) -1;
#endif

} // __clock()


// SYMBOL DEFINITIONS

externC clock_t
clock( void ) CYGBLD_ATTRIB_WEAK_ALIAS(__clock);

// EOF clock.cxx
