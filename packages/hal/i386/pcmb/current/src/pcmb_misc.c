//==========================================================================
//
//      pcmb_misc.c
//
//      HAL implementation miscellaneous functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jlarmour, pjo
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>
#include <pkgconf/hal_i386_pcmb.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_smp.h>

/*------------------------------------------------------------------------*/
// Array which stores the configured priority levels for the configured
// interrupts.

volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];

/*------------------------------------------------------------------------*/
// Static variables

/*------------------------------------------------------------------------*/
// Initializer

void hal_pcmb_init(void)
{
    // Disable NMI - this can be reenabled later, once a proper handler
    // is registered and ready to handle events
    HAL_WRITE_UINT8(0x70, 0x80);
}

/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
// Clock initialization and access

#ifdef CYGPKG_HAL_SMP_SUPPORT
static HAL_SPINLOCK_TYPE pc_clock_lock;
#else
#define HAL_SPINLOCK_SPIN( lock )
#define HAL_SPINLOCK_CLEAR( lock )
#endif

void hal_pc_clock_initialize(cyg_uint32 period)
{
    /* Select mode 2: rate generator.  Then we'll load LSB, and finally MSB. */
    HAL_WRITE_UINT8( PC_PIT_CONTROL, 0x34 );
    HAL_WRITE_UINT8( PC_PIT_CLOCK_0, period & 0xFF );
    HAL_WRITE_UINT8( PC_PIT_CLOCK_0, period >> 8 );

    HAL_SPINLOCK_CLEAR( pc_clock_lock );
}


void hal_pc_clock_read(cyg_uint32 * count)
{
    cyg_uint8 lo = 0,hi = 0;
    cyg_uint32 curr = 0;
    CYG_INTERRUPT_STATE interruptState ;

    /* Hold off on interrupts for a bit. */
    HAL_DISABLE_INTERRUPTS(interruptState) ;

    HAL_SPINLOCK_SPIN( pc_clock_lock );    
    
    /* Latch counter 0. */
    HAL_WRITE_UINT8(PC_PIT_CONTROL, 0x00);

    /* Now get the value. */
    HAL_READ_UINT8( PC_PIT_CLOCK_0, lo );
    HAL_READ_UINT8( PC_PIT_CLOCK_0, hi );

    curr = (hi<<8) | lo;

    HAL_SPINLOCK_CLEAR( pc_clock_lock );
    
    /* (Maybe) restore interrupts. */
    HAL_RESTORE_INTERRUPTS(interruptState) ;

    *count = CYGNUM_HAL_RTC_PERIOD - curr ;
}

/*------------------------------------------------------------------------*/

void hal_idle_thread_action(cyg_uint32 loop_count)
{
#if 1 //ndef CYGPKG_HAL_SMP_SUPPORT
    asm("hlt") ;

#else    

    CYG_WORD32 val;
    CYG_WORD32 cpu = 0;
#ifdef CYGPKG_HAL_SMP_SUPPORT    
    cpu = HAL_SMP_CPU_THIS();

    {
        __externC HAL_SPINLOCK_TYPE cyg_hal_ioapic_lock;

        HAL_SPINLOCK_SPIN( cyg_hal_ioapic_lock );
        HAL_IOAPIC_READ( HAL_IOAPIC_REG_REDTBL+4, val );
        PC_WRITE_SCREEN_32( PC_SCREEN_LINE(15)+10, val );

        HAL_IOAPIC_READ( HAL_IOAPIC_REG_REDTBL+5, val );
        PC_WRITE_SCREEN_32( PC_SCREEN_LINE(15), val );

        HAL_SPINLOCK_CLEAR( cyg_hal_ioapic_lock );
        
    }
#endif
    
    hal_pc_clock_read( &val);
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(15)+20, val );


#ifdef CYGPKG_HAL_SMP_SUPPORT    
    
    PC_WRITE_SCREEN_8( PC_SCREEN_LINE(16+cpu), cpu);

    HAL_APIC_READ( HAL_APIC_IRR, val );
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(16+cpu)+10, val );
    HAL_APIC_READ( HAL_APIC_IRR+1, val );
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(16+cpu)+20, val );
    HAL_APIC_READ( HAL_APIC_IRR+2, val );
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(16+cpu)+30, val );
#endif
    
    HAL_QUERY_INTERRUPTS( val );
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(16+cpu)+50, val );
    PC_WRITE_SCREEN_32( PC_SCREEN_LINE(16+cpu)+60, loop_count );
    
#endif
}

// ----------------------------------------------------------------------------
// Profiling support. mcount() is provided by the architectural HAL
// but the timer has to be provided by the platform. This is tricky on
// a PC because there is only a single clock which can generate
// interrupts, and that gets used for the system clock. To get around
// this we assume that the system clock has already been enabled,
// and we sneak in a new ISR routine which will chain to the default
// one. This is possible because of the way the interrupt subsystem
// is implemented in the x86 architectural HAL.

#ifdef CYGFUN_HAL_I386_PCMB_GPROF_SUPPORT
# include <cyg/profile/profile.h>

extern HAL_SavedRegisters *hal_saved_interrupt_state;
static int  (*hal_pcmb_profile_saved_isr)(CYG_ADDRWORD, CYG_ADDRWORD);

static int
hal_pcmb_profile_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    // Now chain to the saved ISR.
    __profile_hit(hal_saved_interrupt_state->pc);
    return (*hal_pcmb_profile_saved_isr)(vector, data);
}

int
hal_enable_profile_timer(int resolution)
{
    cyg_uint64  actual_resolution;

    hal_pcmb_profile_saved_isr  = (int (*)(CYG_ADDRWORD, CYG_ADDRWORD)) hal_interrupt_handlers[CYGNUM_HAL_INTERRUPT_TIMER - CYGNUM_HAL_ISR_MIN];
    hal_interrupt_handlers[CYGNUM_HAL_INTERRUPT_TIMER - CYGNUM_HAL_ISR_MIN]  = (CYG_ADDRESS) &hal_pcmb_profile_isr;
    
    // The only resolution that can be supported is that of the system
    // clock. RTC_PERIOD works in terms of a 1193180Hz clock, and we
    // need to return microseconds.
    actual_resolution = (1000000ll * CYGNUM_HAL_RTC_PERIOD) / 1193180;
    return actual_resolution;
}

#endif  // CYGFUN_HAL_I386_PCMB_GPROF_SUPPORT

/*------------------------------------------------------------------------*/
/* End of pcmb_misc.c                                                      */
