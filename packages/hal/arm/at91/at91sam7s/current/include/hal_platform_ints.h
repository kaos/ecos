#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock assignments for AT91SAM7
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, Oliver Munz, Andrew Lunn, John Eigelaar
// Date:         2001-07-12
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for the AT91SAM7 platform are
//               defined here.
//              
// Usage:        #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal_arm_at91sam7.h>

#define CYGNUM_HAL_INTERRUPT_FIQ		0

#define CYGNUM_HAL_INTERRUPT_SYS		1
#define CYGNUM_HAL_INTERRUPT_PIOA		2

#ifdef CYGHWR_HAL_ARM_AT91SAM7X
#define CYGNUM_HAL_INTERRUPT_PIOB		3
#define CYGNUM_HAL_INTERRUPT_SPI		4
#define CYGNUM_HAL_INTERRUPT_SPI1		5
#endif
#ifdef CYGHWR_HAL_ARM_AT91SAM7S
#define CYGNUM_HAL_INTERRUPT_ADC		4
#define CYGNUM_HAL_INTERRUPT_SPI		5
#endif

#define CYGNUM_HAL_INTERRUPT_USART0		6
#define CYGNUM_HAL_INTERRUPT_USART1		7
#define CYGNUM_HAL_INTERRUPT_SSC		8
#define CYGNUM_HAL_INTERRUPT_TWI		9
#define CYGNUM_HAL_INTERRUPT_PWMC		10
#define CYGNUM_HAL_INTERRUPT_UDP		11
#define CYGNUM_HAL_INTERRUPT_TC0		12
#define CYGNUM_HAL_INTERRUPT_TC1		13
#define CYGNUM_HAL_INTERRUPT_TC2		14

#ifdef CYGHWR_HAL_ARM_AT91SAM7X
#define CYGNUM_HAL_INTERRUPT_CAN		15
#define CYGNUM_HAL_INTERRUPT_EMAC		16
#define CYGNUM_HAL_INTERRUPT_ADC		17
#endif

#define CYGNUM_HAL_INTERRUPT_IRQ0		30
#define CYGNUM_HAL_INTERRUPT_IRQ1		31

// Interrupts which are multiplexed on to the System Interrupt
#define CYGNUM_HAL_INTERRUPT_PITC               32
#define CYGNUM_HAL_INTERRUPT_RTTC               33
#define CYGNUM_HAL_INTERRUPT_PMC                34
#define CYGNUM_HAL_INTERRUPT_MC                 35
#define CYGNUM_HAL_INTERRUPT_WDTC               36
#define CYGNUM_HAL_INTERRUPT_RSTC               37
#define CYGNUM_HAL_INTERRUPT_DBG                38

#define CYGNUM_HAL_ISR_MIN			 0
#define CYGNUM_HAL_ISR_MAX			38

#define CYGNUM_HAL_ISR_COUNT			(CYGNUM_HAL_ISR_MAX + 1)

// The vector used by the Real time clock
#ifdef CYGBLD_HAL_ARM_AT91_TIMER_TC
#define CYGNUM_HAL_INTERRUPT_RTC		CYGNUM_HAL_INTERRUPT_TC0
#endif
#ifdef CYGBLD_HAL_ARM_AT91_TIMER_PIT
#define CYGNUM_HAL_INTERRUPT_RTC		CYGNUM_HAL_INTERRUPT_PITC
#endif

//----------------------------------------------------------------------------
// Reset.
__externC void hal_at91_reset_cpu(void);
#define HAL_PLATFORM_RESET() hal_at91_reset_cpu()

#define HAL_PLATFORM_RESET_ENTRY 0x0000000

#endif // CYGONCE_HAL_PLATFORM_INTS_H
