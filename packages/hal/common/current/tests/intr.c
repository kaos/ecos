/*=================================================================
//
//        intr.c
//
//        HAL Interrupt API test
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  nickg
// Date:          1998-10-08
//####DESCRIPTIONEND####
*/

#include <pkgconf/hal.h>

#include <cyg/infra/testcase.h>

#include <cyg/hal/hal_intr.h>

// Include HAL/Platform specifics
#include CYGBLD_HAL_PLATFORM_H

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>             // Need to look for the RTC config.
#endif

// Fallback defaults (in case HAL didn't define these)
#ifndef CYGNUM_HAL_RTC_NUMERATOR       
#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        9999
#endif

// -------------------------------------------------------------------------

#define ISR_DATA        0xAAAA1234

// -------------------------------------------------------------------------

volatile cyg_count32 ticks = 0;

// -------------------------------------------------------------------------

cyg_uint32 isr( cyg_uint32 vector, CYG_ADDRWORD data )
{
    CYG_TEST_CHECK( ISR_DATA == data , "Bad data passed to ISR");
    CYG_TEST_CHECK( CYGNUM_HAL_INTERRUPT_RTC == vector ,
                    "Bad vector passed to ISR");

    HAL_CLOCK_RESET( vector, CYGNUM_HAL_RTC_PERIOD );

    HAL_INTERRUPT_ACKNOWLEDGE( vector );
    
    ticks++;

    return 0;
}

// -------------------------------------------------------------------------

void intr_main( void )
{
    CYG_INTERRUPT_STATE oldints;

    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_RTC, isr, ISR_DATA, 0 );

    HAL_CLOCK_INITIALIZE( CYGNUM_HAL_RTC_PERIOD );

    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_RTC );

    HAL_ENABLE_INTERRUPTS();

    while( ticks < 10 )
    {
        
    }

    HAL_DISABLE_INTERRUPTS(oldints);

    CYG_TEST_PASS_FINISH("HAL interrupt test");

}


// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    CYG_TEST_INIT();

    // Attaching the ISR will not succeed if the kernel real-time
    // clock has been configured in.
#ifndef CYGVAR_KERNEL_COUNTERS_CLOCK

    intr_main();

#else

    CYG_TEST_NA("Cannot override kernel real-time clock.");

#endif
}


// -------------------------------------------------------------------------
/* EOF intr.c */
