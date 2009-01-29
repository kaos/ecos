#ifndef CYGONCE_HAL_PROC_INTR_H
#define CYGONCE_HAL_PROC_INTR_H

//==========================================================================
//
//      proc_intr.h
//
//      mcf5272 Processor variant interrupt and clock support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2004, 2005, 2006, 2008 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/plf_intr.h>

//---------------------------------------------------------------------------
// Interrupt controller management
//
// There are 32 interrupt sources, all vectored using VSR's 64 onwards.
// The ISR vector numbers are 0 to 31.

#define HAL_MCF5272_INT_VEC_BASE 64

// Vector numbers defined by the interrupt controller.
#define CYGNUM_HAL_VECTOR_USR_SPUR_INT  ( 0 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT1       ( 1 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT2       ( 2 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT3       ( 3 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT4       ( 4 + HAL_MCF5272_INT_VEC_BASE)
// The User's Manual uses TMR1 to TMR4 for the interrupt numbers, which
// is inconsistent with the chapter on the timers themselves. TMR0-TMR3
// are used here instead.
#define CYGNUM_HAL_VECTOR_TMR0          ( 5 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_TMR1          ( 6 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_TMR2          ( 7 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_TMR3          ( 8 + HAL_MCF5272_INT_VEC_BASE)
// The User's Manual uses UART1 and UART2 when describing the interrupt
// controller. UART0 and UART1 are used instead here, for consistency
// with other parts of the code and documentation.
#define CYGNUM_HAL_VECTOR_UART0         ( 9 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_UART1         (10 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_PLIP          (11 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_PLIA          (12 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB0          (13 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB1          (14 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB2          (15 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB3          (16 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB4          (17 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB5          (18 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB6          (19 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_USB7          (20 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_DMA           (21 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_ERX           (22 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_ETX           (23 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_ENTC          (24 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_QSPI          (25 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT5       (26 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_EXTINT6       (27 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_SWTO          (28 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_RES1          (29 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_RES2          (30 + HAL_MCF5272_INT_VEC_BASE)
#define CYGNUM_HAL_VECTOR_RES3          (31 + HAL_MCF5272_INT_VEC_BASE)

// ISR numbering starts with 0 corresponding to VSR 64, even though
// that interrupt won't actually be raised. This costs some memory and
// adds a bit of complexity to the mask/unmask macros, but saves an
// instruction during ISR decoding.
#define CYGNUM_HAL_ISR_MIN               0
#define CYGNUM_HAL_ISR_USR_SPUR_INT      0
#define CYGNUM_HAL_ISR_EXTINT1           1
#define CYGNUM_HAL_ISR_EXTINT2           2
#define CYGNUM_HAL_ISR_EXTINT3           3
#define CYGNUM_HAL_ISR_EXTINT4           4
#define CYGNUM_HAL_ISR_TMR0              5
#define CYGNUM_HAL_ISR_TMR1              6
#define CYGNUM_HAL_ISR_TMR2              7
#define CYGNUM_HAL_ISR_TMR3              8
#define CYGNUM_HAL_ISR_UART0             9
#define CYGNUM_HAL_ISR_UART1            10
#define CYGNUM_HAL_ISR_PLIP             11
#define CYGNUM_HAL_ISR_PLIA             12
#define CYGNUM_HAL_ISR_USB0             13
#define CYGNUM_HAL_ISR_USB1             14
#define CYGNUM_HAL_ISR_USB2             15
#define CYGNUM_HAL_ISR_USB3             16
#define CYGNUM_HAL_ISR_USB4             17
#define CYGNUM_HAL_ISR_USB5             18
#define CYGNUM_HAL_ISR_USB6             19
#define CYGNUM_HAL_ISR_USB7             20
#define CYGNUM_HAL_ISR_DMA              21
#define CYGNUM_HAL_ISR_ERX              22
#define CYGNUM_HAL_ISR_ETX              23
#define CYGNUM_HAL_ISR_ENTC             24
#define CYGNUM_HAL_ISR_QSPI             25
#define CYGNUM_HAL_ISR_EXTINT5          26
#define CYGNUM_HAL_ISR_EXTINT6          27
#define CYGNUM_HAL_ISR_SWTO             28
#define CYGNUM_HAL_ISR_RES1             29
#define CYGNUM_HAL_ISR_RES2             30
#define CYGNUM_HAL_ISR_RES3             31

#define CYGNUM_HAL_ISR_MAX              28
#define CYGNUM_HAL_ISR_COUNT            29

//---------------------------------------------------------------------------
// Interrupt controller macros.

// Declare a mirror copy of the interrupt control registers used to
// set interrupt priorities. In order to mask and unmask a specific
// interrupt, we must be able to set its priority to zero and then
// restore it to ist original priority. We use these locations to
// determine the level to restore the interrupt to in the unmask
// macro.

externC cyg_uint32 hal_mcf5272_icr_pri_mirror[4];

// Some of the operations should only be applied to external interrupts.
#define HAL_MCF5272_EXTERNAL_INTERRUPTS \
    ((0x01 << CYGNUM_HAL_ISR_EXTINT1) | (0x01 << CYGNUM_HAL_ISR_EXTINT2) |    \
     (0x01 << CYGNUM_HAL_ISR_EXTINT3) | (0x01 << CYGNUM_HAL_ISR_EXTINT4) |    \
     (0x01 << CYGNUM_HAL_ISR_EXTINT5) | (0x01 << CYGNUM_HAL_ISR_EXTINT6))
  
// Block an interrupt source. This involves setting the ISR vector's
// interrupt priority level to 0. The PI bit should be set to 1 for
// just the interrupt of interest, and the rest of the word written
// to the ICR register should be 0.

#define HAL_INTERRUPT_MASK( _vector_ )                                              \
    CYG_MACRO_START                                                                 \
    cyg_uint32 offset   = ((_vector_) - 1) / 8;                                     \
    cyg_uint32 shift    = (((_vector_) - 1) % 8) * 4;                               \
    cyg_uint32 mask     = 0x80000000;                                               \
    mask              >>= shift;                                                    \
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_ICR1 + (offset << 2), mask);    \
    CYG_MACRO_END

// Unblock an interrupt source. This involves restoring the ISR vector's
// interrupt priority level using the saved mirrors. The PI bit should be
// set to 1 for just the interrupt of interest, so the rest of the write
// will be ignored.

#define HAL_INTERRUPT_UNMASK( _vector_ )                                            \
    CYG_MACRO_START                                                                 \
    cyg_uint32 offset   = ((_vector_) - 1) / 8;                                     \
    cyg_uint32 shift    = (((_vector_) - 1) % 8) * 4;                               \
    cyg_uint32 mask     = 0x80000000;                                               \
    mask              >>= shift;                                                    \
    mask               |= hal_mcf5272_icr_pri_mirror[offset];                       \
    HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_ICR1 + (offset << 2), mask);    \
    CYG_MACRO_END

// Set the priority in the interrupt control register and the icr
// mirror.
#define HAL_INTERRUPT_SET_LEVEL( _vector_, _prilevel_)                                          \
     CYG_MACRO_START                                                                            \
     CYG_INTERRUPT_STATE    ints_enabled;                                                       \
     cyg_uint32     offset      = ((_vector_) - 1) / 8;                                         \
     cyg_uint32     shift       = (((_vector_) - 1) % 8) * 4;                                   \
     cyg_uint32     mask        = 0xF0000000;                                                   \
     cyg_uint32     new_level   = (_prilevel_) & 0x07;                                          \
     mask       >>= shift;                                                                      \
     new_level  <<= (28 - shift);                                                               \
     HAL_DISABLE_INTERRUPTS(ints_enabled);                                                      \
     hal_mcf5272_icr_pri_mirror[offset] &= ~mask;                                               \
     hal_mcf5272_icr_pri_mirror[offset] |= new_level;                                           \
     mask &= 0x88888888;                                                                        \
     HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_ICR1 + (offset << 2), mask | new_level);   \
     HAL_RESTORE_INTERRUPTS(ints_enabled);                                                      \
     CYG_MACRO_END

// Acknowledge is only relevant for external interrupts. It is necessary to
// write a 1 to the PI bit, which will unfortunately also reset this
// interrupt vector's priority level.
#define HAL_INTERRUPT_ACKNOWLEDGE(_vector_)                                                     \
    CYG_MACRO_START                                                                             \
    if (0 != (HAL_MCF5272_EXTERNAL_INTERRUPTS & (0x01 << (_vector_)))) {                        \
        cyg_uint32  offset  = ((_vector_) - 1) / 8;                                             \
        cyg_uint32  shift   = (((_vector_ - 1)) % 8) * 4;                                       \
        cyg_uint32  mask    = 0x80000000;                                                       \
        mask              >>= shift;                                                            \
        mask               |= hal_mcf5272_icr_pri_mirror[offset];                               \
        HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_ICR1 + (offset << 2), mask);            \
    }                                                                                           \
    CYG_MACRO_END

// Select between level and edge triggered. This is only relevant for the external
// interrupt sources. The interrupts remain level-triggered, but it is possible
// to choose between rising and falling interrupts.
//
// NOTE: the manual states 0 for low->high, 1 for high->low. The original code
// inverted this, claiming the manual is wrong. I have stuck with the original
// code's usage, but have not tested this.

#define HAL_INTERRUPT_CONFIGURE( _vector_, _leveltriggered_, _up_)                      \
    CYG_MACRO_START                                                                     \
    if (0 != (HAL_MCF5272_EXTERNAL_INTERRUPTS & (0x01 << (_vector_)))) {                \
        cyg_uint32 mask = 0x80000000 >> ((_vector_) - 1);                               \
        cyg_uint32 pitr;                                                                \
        HAL_READ_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PITR, pitr);                     \
        if (_up_) {                                                                     \
            HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PITR, pitr | mask);         \
        } else {                                                                        \
            HAL_WRITE_UINT32(HAL_MCFxxxx_MBAR + HAL_MCF5272_PITR, pitr & ~mask);        \
        }                                                                               \
    }                                                                                   \
    CYG_MACRO_END

// ----------------------------------------------------------------------------
// The clock. Timer 3 is used for the eCos system clock. Timers 0 and
// 1 offer additional functionality which is not required by eCos, so
// those are best left to application code. Timer 2 is used as the
// profiling timer, if desired. The prescaler is set to the clock's
// MHz rating provided by the platform HAL, so each tick is one
// microsecond.

#define CYGNUM_HAL_INTERRUPT_RTC (CYGNUM_HAL_ISR_TMR3)

#define HAL_CLOCK_INITIALIZE(_period_)  \
    CYG_MACRO_START                     \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TMR, 0);            \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TRR,                \
                     (_period_) - 1);                                                                   \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TCN, 0);            \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TER,                \
                     HAL_MCF5272_TIMER_TER_REF | HAL_MCF5272_TIMER_TER_CAP);                            \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TMR,                \
                     ((CYGHWR_HAL_SYSTEM_CLOCK_MHZ - 1) << HAL_MCF5272_TIMER_TMR_PS_SHIFT) |            \
                     HAL_MCF5272_TIMER_TMR_ORI | HAL_MCF5272_TIMER_TMR_FRR |                            \
                     HAL_MCF5272_TIMER_TMR_CLK_MASTER | HAL_MCF5272_TIMER_TMR_RST);                     \
    CYG_MACRO_END

// The timer itself runs in restart mode, but it is still necessary to
// reset the bits affecting the interrupt controller.
#define HAL_CLOCK_RESET(_vector_, _period_) \
    CYG_MACRO_START                         \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TER, HAL_MCF5272_TIMER_TER_REF);    \
    CYG_MACRO_END

#define HAL_CLOCK_READ(_pvalue_)                                                                \
    CYG_MACRO_START                                                                             \
    cyg_uint16 _tmp_;                                                                           \
    HAL_READ_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_TIMER3_BASE + HAL_MCF5272_TIMER_TCN, _tmp_); \
    *(_pvalue_) = _tmp_;                                                                        \
    CYG_MACRO_END

#define HAL_CLOCK_LATENCY(_pvalue_) HAL_CLOCK_READ(_pvalue_)

// ----------------------------------------------------------------------------
// HAL_DELAY_US(). Just use the default implementation from hal_intr.h
#ifndef _HAL_M68K_DELAY_US_LOOPS_
# define _HAL_M68K_DELAY_US_LOOPS_       20
# define _HAL_M68K_DELAY_US_UNCACHED_    2
#endif

// ----------------------------------------------------------------------------
// For reset, use the watchdog device.
#ifndef HAL_PLATFORM_RESET
# define HAL_PLATFORM_RESET()                                           \
    CYG_MACRO_START                                                     \
    int _ints_state_;                                                   \
    HAL_DISABLE_INTERRUPTS(_ints_state_);                               \
    HAL_WRITE_UINT16(HAL_MCFxxxx_MBAR + HAL_MCF5272_WRRR, 0x03);        \
    for ( ; ; ) ;                                                       \
    CYG_MACRO_END
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PROC_INTR_H
