//==========================================================================
//
//      sa11x0_misc.c
//
//      HAL misc board support code for StrongARM SA11x0
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
// Author(s):    gthomas
// Contributors: hmt
//               Travis C. Furrer <furrer@mit.edu>
// Date:         2000-05-08
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_sa11x0.h>         // Hardware definitions

#include <cyg/infra/diag.h>             // diag_printf

// Most initialization has already been done before we get here.
// All we do here is set up the interrupt environment.
// FIXME: some of the stuff in hal_platform_setup could be moved here.

externC void plf_hardware_init(void);

void hal_hardware_init(void)
{
    // Mask all interrupts
    *SA11X0_ICMR = 0;
     
    // Make all interrupts do IRQ and not FIQ
    // FIXME: Change this if you use FIQs.
    *SA11X0_ICLR = 0;
     
    // Prevent masked interrupts from bringing us out of idle mode
    *SA11X0_ICCR = 1;

    // Disable all GPIO interrupt sources
    *SA11X0_GPIO_RISING_EDGE_DETECT = 0;
    *SA11X0_GPIO_FALLING_EDGE_DETECT = 0;
    *SA11X0_GPIO_EDGE_DETECT_STATUS = 0x0FFFFFFF;

    // Perform any platform specific initializations
    plf_hardware_init();

    // Set up eCos/ROM interfaces
    hal_if_init();

    // Enable caches
    HAL_DCACHE_ENABLE();
    HAL_ICACHE_ENABLE();
}


// -------------------------------------------------------------------------
static cyg_uint32  clock_period;

void hal_clock_initialize(cyg_uint32 period)
{
    // Load match value
    *SA11X0_OSMR0 = period;
    clock_period = period;

    // Start the counter
    *SA11X0_OSCR = 0;

    // Clear any pending interrupt
    *SA11X0_OSSR = SA11X0_OSSR_TIMER0;

    // Enable timer 0 interrupt    
    *SA11X0_OIER |= SA11X0_OIER_TIMER0;

    // Unmask timer 0 interrupt
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_TIMER0 );

    // That's all.
}

// This routine is called during a clock interrupt.

// Define this if you want to ensure that the clock is perfect (i.e. does
// not drift).  One reason to leave it turned off is that it costs some
// 1.5us per system clock interrupt for this maintenance.
#undef COMPENSATE_FOR_CLOCK_DRIFT

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
#ifdef COMPENSATE_FOR_CLOCK_DRIFT
    cyg_int32 ctr = (cyg_int32)*SA11X0_OSCR;    // Current value of the counter
    ctr -= period;                              // Reset counter
    if ((ctr < 0) || (ctr >= period)) ctr = 0;  // Adjust for missed interrupts
    *SA11X0_OSCR = ctr;
    clock_period = period;
#else
    *SA11X0_OSCR = 0;
#endif
    *SA11X0_OSMR0 = period;                     // Load new match value
    *SA11X0_OSSR = SA11X0_OSSR_TIMER0;          // Clear any pending interrupt
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)

// Note: The "contract" for this function is that the value is the number
// of hardware clocks that have happened since the last interrupt (i.e.
// when it was reset).  This value is used to measure interrupt latencies.
// However, since the hardware counter does not automatically reset, as
// many do, the value may be larger than the system "tick" interval.
// If this turns out to be the case, the result will be adjusted to be
// within the interval [0..N-1], where N is the number of hardware clocks/tick.

void hal_clock_read(cyg_uint32 *pvalue)
{
    cyg_uint32 ctr = *SA11X0_OSCR;
    if (ctr > clock_period) ctr -= clock_period;
    *pvalue = ctr;
}

// -------------------------------------------------------------------------

// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.
int hal_IRQ_handler(void)
{
    cyg_uint32 sources, index;

#if 0 // test FIQ and print alert if active - really for debugging
    sources = *SA11X0_ICFP;
    if ( 0 != sources )
        diag_printf( "FIQ source active!!! - fiqstatus %08x irqstatus %08x\n",
                     sources, *SA11X0_ICIP );
    else
#endif // Scan FIQ sources also

    sources = *SA11X0_ICIP;

// FIXME
// if we come to support FIQ properly...
//    if ( 0 == sources )
//        sources = *SA11X0_ICFP;

    // Nothing wrong with scanning them in any order we choose...
    // So here we try to make the serial devices steal fewer cycles.
    // So, knowing this is an ARM:
    if ( sources & 0xff0000 )
        index = 16;
    else if ( sources & 0xff00 )
        index = 8;
    else if ( sources & 0xff )
        index = 0;
    else // if ( sources & 0xff000000 )
        index = 24;

    do {
        if ( (1 << index) & sources ) {
            if (index == CYGNUM_HAL_INTERRUPT_GPIO) {
                // Special case of GPIO cascade.  Search for lowest set bit
                sources = *SA11X0_GPIO_EDGE_DETECT_STATUS & 0x0FFFF800;
                index = 11;
                do {
                    if (sources & (1 << index)) {
                        index += 32;
                        break;
                    }
                    index++;
                } while (index < 28);
            }
            return index;
        }
      index++;
    } while ( index & 7 );
    
    return CYGNUM_HAL_INTERRUPT_NONE; // This shouldn't happen!
}

//
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    // Non-GPIO interrupt sources can be masked separately.
    // Note: masking any non-unique GPIO signal (31..11) results in
    // all GPIO signals (31..11) being masked as only the "lump"
    // source will be changed.
    if (vector >= CYGNUM_HAL_INTERRUPT_GPIO11) {
        vector = CYGNUM_HAL_INTERRUPT_GPIO;
    }
    *SA11X0_ICMR &= ~(1 << vector);
}

void hal_interrupt_unmask(int vector)
{
    if (vector >= CYGNUM_HAL_INTERRUPT_GPIO11) {
        vector = CYGNUM_HAL_INTERRUPT_GPIO;
    }
    *SA11X0_ICMR |= (1 << vector);
}

void hal_interrupt_acknowledge(int vector)
{
    // GPIO interrupts are driven by an edge detection mechanism.  This
    // is latching so these interrupts must be acknowledged directly.
    // All other interrupts simply go away when the interrupting unit
    // has been serviced by the ISR.
    if ((vector < CYGNUM_HAL_INTERRUPT_GPIO) || 
        (vector >= CYGNUM_HAL_INTERRUPT_GPIO11)) {
        *SA11X0_GPIO_EDGE_DETECT_STATUS  = (1 << (vector & 0x1F));
    } else {
        // Not a GPIO interrupt
        return;
    }
}

void hal_interrupt_configure(int vector, int level, int up)
{
    // This function can be used to configure the GPIO interrupts.  All
    // of these pins can potentially generate an interrupt, but only
    // 0..10 are unique.  Thus the discontinuity in the numbers.
    // Also, if 'level' is true, then both edges are enabled if 'up' is
    // true, otherwise they will be disabled.
    // Non GPIO sources are ignored.
    if ((vector < CYGNUM_HAL_INTERRUPT_GPIO) || 
        (vector >= CYGNUM_HAL_INTERRUPT_GPIO11)) {
        if (level) {
            if (up) {
                // Enable both edges
                *SA11X0_GPIO_RISING_EDGE_DETECT |= (1 << (vector & 0x1F));
                *SA11X0_GPIO_FALLING_EDGE_DETECT |= (1 << (vector & 0x1F));
            } else {
                // Disable both edges
                *SA11X0_GPIO_RISING_EDGE_DETECT &= ~(1 << (vector & 0x1F));
                *SA11X0_GPIO_FALLING_EDGE_DETECT &= ~(1 << (vector & 0x1F));
            }
        } else {
            // Only interested in one edge
            if (up) {
                // Set rising edge detect and clear falling edge detect.
                *SA11X0_GPIO_RISING_EDGE_DETECT |= (1 << (vector & 0x1F));
                *SA11X0_GPIO_FALLING_EDGE_DETECT &= ~(1 << (vector & 0x1F));
            } else {
                // Set falling edge detect and clear rising edge detect.
                *SA11X0_GPIO_FALLING_EDGE_DETECT |= (1 << (vector & 0x1F));
                *SA11X0_GPIO_RISING_EDGE_DETECT &= ~(1 << (vector & 0x1F));
            }
        }
    }
}

void hal_interrupt_set_level(int vector, int level)
{
    // Interrupt priorities are not configurable on the SA11X0.
}

/*------------------------------------------------------------------------*/
// EOF sa11x0_misc.c
