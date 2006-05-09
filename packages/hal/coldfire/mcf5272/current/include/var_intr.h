#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//==========================================================================
//
//      var_intr.h
//
//      MCF5272 processor variant interrupt, exception and clock support
//
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
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
// Author(s):     Enrico Piria
// Contributors:  Wade Jensen
// Date:          2005-25-06
// Purpose:       Provide interrupt, exception and clock definitions specific
//                to the MCF5272 processor.
// Usage:         Included via "hal_intr.h". Do not use directly.
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

// Include any platform specific interrupt definitions.
#include <cyg/hal/plf_intr.h>

// Include for the device addresses (MCF5272_DEVS).
#include <cyg/hal/var_arch.h>

// Include for HAL I/O macros
#include <cyg/hal/hal_io.h>

// --------------------------------------------------------------------------
// Interrupt controller management

// This chip has a programmable interrupt vector base which is different
// from the vector base  register (VBR). All interrupts from the interrupt
// controller are offset from the programmable interrupt vector register
// (PIVR). However, the only legal value is 64.

#define HAL_PROG_INT_VEC_BASE 64

// Vector numbers defined by the interrupt controller.
// These are all relative to the interrupt vector base number.
#define CYGNUM_HAL_INTERRUPT_USR_SPURINT       (0 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT1           (1 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT2           (2 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT3           (3 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT4           (4 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_TMR0              (5 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_TMR1              (6 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_TMR2              (7 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_TMR3              (8 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_UART1             (9 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_UART2             (10 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_PLIP              (11 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_PLIA              (12 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB0              (13 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB1              (14 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB2              (15 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB3              (16 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB4              (17 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB5              (18 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB6              (19 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_USB7              (20 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_DMA               (21 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_ERX               (22 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_ETX               (23 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_ENTC              (24 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_QSPI              (25 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT5           (26 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_EXTINT6           (27 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_SWTO              (28 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_RES1              (29 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_RES2              (30 + HAL_PROG_INT_VEC_BASE)
#define CYGNUM_HAL_INTERRUPT_RES3              (31 + HAL_PROG_INT_VEC_BASE)

// -------------------------------------------------------------------------
// Interrupt and exception vector table definitions. We need to redifine 
// CYGNUM_HAL_ISR_MIN because the first usable vector is 65

#define CYGNUM_HAL_ISR_RANGE_DEFINED
#define CYGNUM_HAL_ISR_MIN                   CYGNUM_HAL_INTERRUPT_EXTINT1
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_RES3
#define CYGNUM_HAL_ISR_COUNT                 (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)

// -------------------------------------------------------------------------
// Spurious interrupt definition. The MCF5272 returns vector number 64,
// instead of 24, for spurious interrupts

#define CYGNUM_HAL_SPURIOUS_INTERRUPT CYGNUM_HAL_INTERRUPT_USR_SPURINT

// --------------------------------------------------------------------------
// Interrupt controller definitions.

// Interrupt priority tables
externC volatile cyg_uint8 cyg_hal_ILVL_table[CYGNUM_HAL_ISR_COUNT];
externC volatile cyg_uint8 cyg_hal_IMASK_table[CYGNUM_HAL_ISR_COUNT];

externC void hal_interrupt_set_level(int vector, int level);
externC void hal_interrupt_mask(int vector);
externC void hal_interrupt_unmask(int vector);

// Mask the interrupt associated with the given vector.
#define HAL_INTERRUPT_MASK( _vector_ ) \
    hal_interrupt_mask(_vector_)

// Unmask the interrupt associated with the given vector.
#define HAL_INTERRUPT_UNMASK( _vector_ ) \
    hal_interrupt_unmask(_vector_)

// Set the priority level of an interrupt.
#define HAL_INTERRUPT_SET_LEVEL( _vector_, _prilevel_ ) \
    hal_interrupt_set_level(_vector_, _prilevel_)

// Acknowledge the interrupt by writing a 1 to the corresponding
// interrupt pending bit. Write 0 to all other interrupt pending bits. Leave
// all priority levels unchanged. Disable all interrupts while we access the
// hardware registers.
#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                           \
CYG_MACRO_START                                                         \
    cyg_uint32 _vec_offset = (_vector_) - HAL_PROG_INT_VEC_BASE - 1;    \
    cyg_uint32 _icr = _vec_offset / 8;                                  \
    cyg_uint32 _icr_msk = 0x80000000 >> ((_vec_offset % 8) * 4);        \
    cyg_uint32 _icr_oldval;                                             \
    CYG_INTERRUPT_STATE _intr_state;                                    \
                                                                        \
    HAL_DISABLE_INTERRUPTS(_intr_state);                                \
    HAL_READ_UINT32(&MCF5272_DEVS->intc.icr[_icr], _icr_oldval);        \
    HAL_WRITE_UINT32(&MCF5272_DEVS->intc.icr[_icr],                     \
        _icr_oldval & (_icr_msk | 0x77777777));                         \
    HAL_RESTORE_INTERRUPTS(_intr_state);                                \
CYG_MACRO_END

// Set/clear the interrupt transition register bit. Disable all
// interrupts while we access the hardware registers.
#define HAL_INTERRUPT_CONFIGURE( _vector_, _leveltriggered_, _up_ )                     \
CYG_MACRO_START                                                                         \
    if (!(_leveltriggered_))                                                            \
    {                                                                                   \
        cyg_uint32 _vec_offset = (_vector_) - HAL_PROG_INT_VEC_BASE - 1;                \
        cyg_uint32 _itr_bit = 0x80000000 >> _vec_offset;                                \
        cyg_uint32 _pitr_oldval;                                                        \
        CYG_INTERRUPT_STATE _intr_state;                                                \
                                                                                        \
        HAL_DISABLE_INTERRUPTS(_intr_state);                                            \
        HAL_READ_UINT32(&MCF5272_DEVS->intc.pitr, _pitr_oldval);                        \
        if (_up_)                                                                       \
        {                                                                               \
            HAL_WRITE_UINT32(&MCF5272_DEVS->intc.pitr, _pitr_oldval | _itr_bit);        \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            HAL_WRITE_UINT32(&MCF5272_DEVS->intc.pitr, _pitr_oldval & (~_itr_bit));     \
        }                                                                               \
        HAL_RESTORE_INTERRUPTS(_intr_state);                                            \
    }                                                                                   \
CYG_MACRO_END

// --------------------------------------------------------------------------
// Clock control

// The MCF5272 has 4 timers, numbered 0...3. Define the timer number that we
// want to use for the OS clock.
#define CYGNUM_HAL_RTC_TIMER_NUM (3)

// The vector used by the real-time clock
#define CYGNUM_HAL_INTERRUPT_RTC (CYGNUM_HAL_INTERRUPT_TMR3)

// Initialize the timer to generate an interrupt every 10 ms. Use the
// system clock divided by 16 as the source. Using 10 as the prescaler
// gives a 2.4 us counter. When this counter reaches _period_, generate
// an interrupt.
#define HAL_CLOCK_INITIALIZE(_period_)                                          \
CYG_MACRO_START                                                                 \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].tmr,            \
                 0x0000);                                                       \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].trr,            \
                 (_period_));                                                   \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].tcn, 0);        \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].ter, 0x0003);   \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].tmr,            \
    (((10)-1) << MCF5272_TIMER_TMR_PS_BIT) |                                    \
    (0 << MCF5272_TIMER_TMR_CE_BIT) |                                           \
    (0 << MCF5272_TIMER_TMR_OM_BIT) |                                           \
    (1 << MCF5272_TIMER_TMR_ORI_BIT) |                                          \
    (0 << MCF5272_TIMER_TMR_FRR_BIT) |                                          \
    (2 << MCF5272_TIMER_TMR_CLK_BIT) |                                          \
    (1 << MCF5272_TIMER_TMR_RST_BIT));                                          \
CYG_MACRO_END

// We must clear the bit in the timer event register before we can get
// another interrupt.
#define HAL_CLOCK_RESET( _vector_, _period_ )                                   \
CYG_MACRO_START                                                                 \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].tcn, 0);        \
HAL_WRITE_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].ter, 0x0002);   \
CYG_MACRO_END

// Read the current counter from the timer
#define HAL_CLOCK_READ( _pvalue_ )                                              \
CYG_MACRO_START                                                                 \
HAL_READ_UINT16(&MCF5272_DEVS->timer[CYGNUM_HAL_RTC_TIMER_NUM].tcn,             \
                *(_pvalue_));                                                   \
CYG_MACRO_END

// Measure clock latency
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )                                       \
CYG_MACRO_START                                                             \
    register cyg_int32 result;                                              \
    HAL_CLOCK_READ( &result );                                              \
    *_pvalue_ = result - CYGNUM_HAL_RTC_PERIOD;                             \
CYG_MACRO_END
#endif

// ---------------------------------------------------------------------------
// End of var_intr.h
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
