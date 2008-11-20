#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//==========================================================================
//
//      hal_intr.h
//
//      m68k Interrupt and clock support
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2006,2008 Free Software Foundation, Inc.
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
//####DESCRIPTIONBEGIN####
//
// Author(s): 	bartv
// Date:	2003-06-04
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_arch.h>

// Allow variants to override some of these settings.
#include <cyg/hal/var_intr.h>

//--------------------------------------------------------------------------
// m68k exception vectors. These correspond to VSRs and are the values
// to use for HAL_VSR_GET/SET

extern CYG_ADDRESS hal_m68k_vsr_table[];

#ifndef HAL_VSR_GET
# define HAL_VSR_GET( _vector_, _pvsr_ )                                \
     CYG_MACRO_START                                                    \
    *((CYG_ADDRESS *)(_pvsr_)) = hal_m68k_vsr_table[(_vector_)];        \
    CYG_MACRO_END
#endif

#if !defined(HAL_VSR_SET) && !defined(_HAL_M68K_NO_VSR_SET_)
# define HAL_VSR_SET( _vector_arg_, _vsr_, _poldvsr_arg_ )              \
    CYG_MACRO_START                                                     \
    cyg_uint32  _vector_    = (cyg_uint32) (_vector_arg_);              \
    CYG_ADDRESS* _poldvsr_  = (CYG_ADDRESS*)(_poldvsr_arg_);            \
    if(_poldvsr_ != (CYG_ADDRESS*)0 )                                   \
        *_poldvsr_ = hal_m68k_vsr_table[(_vector_)];                    \
    hal_m68k_vsr_table[(_vector_)] = (CYG_ADDRESS)(_vsr_);              \
    CYG_MACRO_END
#endif

#define CYGNUM_HAL_VECTOR_SSP               0
#define CYGNUM_HAL_VECTOR_RESET             1
#define CYGNUM_HAL_VECTOR_BUSERR            2
#define CYGNUM_HAL_VECTOR_ADDERR            3
#define CYGNUM_HAL_VECTOR_ILLINST           4
#define CYGNUM_HAL_VECTOR_ZERODIV           5
#define CYGNUM_HAL_VECTOR_CHKINST           6
#define CYGNUM_HAL_VECTOR_TRAPVINST         7
#define CYGNUM_HAL_VECTOR_PRIVVIOLATION     8
#define CYGNUM_HAL_VECTOR_TRACE             9
#define CYGNUM_HAL_VECTOR_L1010             10
// 12-14 are reserved
#define CYGNUM_HAL_VECTOR_L1111             11
#define CYGNUM_HAL_VECTOR_UNINITINT         15
// 16-23 are reserved
#define CYGNUM_HAL_VECTOR_SPURINT           24
#define CYGNUM_HAL_VECTOR_AUTOVEC1          25
#define CYGNUM_HAL_VECTOR_AUTOVEC2          26
#define CYGNUM_HAL_VECTOR_AUTOVEC3          27
#define CYGNUM_HAL_VECTOR_AUTOVEC4          28
#define CYGNUM_HAL_VECTOR_AUTOVEC5          29
#define CYGNUM_HAL_VECTOR_AUTOVEC6          30
#define CYGNUM_HAL_VECTOR_AUTOVEC7          31
#define CYGNUM_HAL_VECTOR_NMI               CYGNUM_HAL_VECTOR_AUTOVEC7
#define CYGNUM_HAL_VECTOR_TRAP0             32
#define CYGNUM_HAL_VECTOR_TRAPFIRST         32
#define CYGNUM_HAL_VECTOR_TRAP1             33
#define CYGNUM_HAL_VECTOR_TRAP2             34
#define CYGNUM_HAL_VECTOR_TRAP3             35
#define CYGNUM_HAL_VECTOR_TRAP4             36
#define CYGNUM_HAL_VECTOR_TRAP5             37
#define CYGNUM_HAL_VECTOR_TRAP6             38
#define CYGNUM_HAL_VECTOR_TRAP7             39
#define CYGNUM_HAL_VECTOR_TRAP8             40
#define CYGNUM_HAL_VECTOR_TRAP9             41
#define CYGNUM_HAL_VECTOR_TRAP10            42
#define CYGNUM_HAL_VECTOR_TRAP11            43
#define CYGNUM_HAL_VECTOR_TRAP12            44
#define CYGNUM_HAL_VECTOR_TRAP13            45
#define CYGNUM_HAL_VECTOR_TRAP14            46
/* TRAP15 is used by gdb stubs   */
#define CYGNUM_HAL_VECTOR_TRAP15            47
#define CYGNUM_HAL_VECTOR_TRAPLAST          CYGNUM_HAL_VECTOR_TRAP15
#define CYGNUM_HAL_VECTOR_NUMTRAPS          16
// 48-63 are reserved
#define CYGNUM_HAL_VECTOR_INTRFIRST         64
#define CYGNUM_HAL_VECTOR_NUMINTRS          192
#define CYGNUM_HAL_VECTOR_INTRLAST          (CYGNUM_HAL_VECTOR_INTRFIRST+CYGNUM_HAL_VECTOR_NUMINTRS-1)

#define CYGNUM_HAL_VSR_MIN                  CYGNUM_HAL_VECTOR_SSP
#define CYGNUM_HAL_VSR_MAX                  CYGNUM_HAL_VECTOR_INTRLAST
#define CYGNUM_HAL_VSR_COUNT                (CYGNUM_HAL_VSR_MAX+1)

// ----------------------------------------------------------------------------
// Also give details of the defined exceptions. Only some of the exceptions
// are named explicitly here.
// NOTE: FPU exceptions should be added

#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS            CYGNUM_HAL_VECTOR_BUSERR
#define CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS  CYGNUM_HAL_VECTOR_ADDERR
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION    CYGNUM_HAL_VECTOR_ILLINST
#define CYGNUM_HAL_EXCEPTION_DIV_BY_ZERO            CYGNUM_HAL_VECTOR_ZERODIV
#define CYGNUM_HAL_EXCEPTION_SYSTEM_ERROR           CYGNUM_HAL_VECTOR_CHKINST
#define CYGNUM_HAL_EXCEPTION_TRACE                  CYGNUM_HAL_VECTOR_TRACE
#define CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP         CYGNUM_HAL_VECTOR_TRAP15

#ifndef CYGNUM_HAL_EXCEPTION_COUNT
# define CYGNUM_HAL_EXCEPTION_MIN                   CYGNUM_HAL_VECTOR_BUSERR
# define CYGNUM_HAL_EXCEPTION_MAX                   CYGNUM_HAL_VECTOR_TRAPLAST
# define CYGNUM_HAL_EXCEPTION_COUNT                 ((CYGNUM_HAL_EXCEPTION_MAX-CYGNUM_HAL_EXCEPTION_MIN)+1)
#endif

//--------------------------------------------------------------------------
// ISR vectors.

#ifndef CYGNUM_HAL_ISR_COUNT
# error The variant or platform HAL should provide details of ISR vectors.
#endif

// Default definition of HAL_TRANSLATE_VECTOR()
#ifndef HAL_TRANSLATE_VECTOR
# define HAL_TRANSLATE_VECTOR(_vector_, _index_)    \
    ((_index_) = ((_vector_) - CYGNUM_HAL_ISR_MIN))
#endif

externC volatile CYG_ADDRESS    cyg_hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   cyg_hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    cyg_hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];
externC cyg_uint32              hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);
// For consistency with other architectures.
#define hal_interrupt_handlers  cyg_hal_interrupt_handlers

#ifndef HAL_INTERRUPT_IN_USE
# define HAL_INTERRUPT_IN_USE( _vector_, _state_)                               \
    CYG_MACRO_START                                                             \
    if (cyg_hal_interrupt_handlers[(_vector_)] ==(CYG_ADDRESS)&hal_default_isr) \
        (_state_) = 0;                                                          \
    else                                                                        \
        (_state_) = 1;                                                          \
    CYG_MACRO_END
#endif

#ifndef HAL_INTERRUPT_ATTACH
# define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )              \
    CYG_MACRO_START                                                             \
    cyg_hal_interrupt_handlers[(_vector_)]  = (CYG_ADDRESS)(_isr_);             \
    cyg_hal_interrupt_data[(_vector_)]      = (CYG_ADDRWORD)(_data_);           \
    cyg_hal_interrupt_objects[(_vector_)]   = (CYG_ADDRESS)(_object_);          \
    CYG_MACRO_END
#endif

#ifndef HAL_INTERRUPT_DETACH
# define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                                \
    CYG_MACRO_START                                                             \
    if (cyg_hal_interrupt_handlers[(_vector_)] == (CYG_ADDRESS)(_isr_)) {       \
        cyg_hal_interrupt_handlers[(_vector_)] = (CYG_ADDRESS)&hal_default_isr; \
        cyg_hal_interrupt_data[(_vector_)] = 0;                                 \
        cyg_hal_interrupt_objects[(_vector_)] = 0;                              \
    }                                                                           \
    CYG_MACRO_END
#endif

//--------------------------------------------------------------------------
// Interrupt control macros. These just operate on the entire status
// register.

typedef cyg_uint16 CYG_INTERRUPT_STATE;

#ifndef HAL_ENABLE_INTERRUPTS
# define HAL_ENABLE_INTERRUPTS()                                                    \
    CYG_MACRO_START                                                                 \
    asm volatile ("move.w    %0,%%sr\n"                                             \
                  :                                                                 \
                  : "J" (0x2000 | (CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL << 8))    \
                  : "cc");                                                          \
    CYG_MACRO_END
#endif // HAL_ENABLE_INTERRUPTS

#ifndef HAL_DISABLE_INTERRUPTS
# define HAL_DISABLE_INTERRUPTS(_old_)                                      \
    CYG_MACRO_START                                                         \
    asm volatile ("move.w   %%sr,%0\n"                                      \
                  "move.w   #0x2700,%%sr\n" : "=d" (_old_) : : "cc");       \
     CYG_MACRO_END
#endif // HAL_DISABLE_INTERRUPTS

#ifndef HAL_RESTORE_INTERRUPTS
# define HAL_RESTORE_INTERRUPTS(_prev_)                                     \
    CYG_MACRO_START                                                         \
    asm volatile ("move.w %0,%%sr\n" : : "d" (_prev_) : "cc");              \
    CYG_MACRO_END
#endif

#ifndef HAL_QUERY_INTERRUPTS
# define HAL_QUERY_INTERRUPTS(_old_)                                        \
    CYG_MACRO_START                                                         \
    asm volatile ("move.w   %%sr,%0\n" : "=d" (_old_) : );                  \
    CYG_MACRO_END
#endif

// ----------------------------------------------------------------------------
// DSR's should always run on the interrupt stack if available.
// Even if no interrupt stack is in use we still want to go
// via call_pending_DSRs() so that it can manipulate the IPL
// level.
externC void hal_interrupt_stack_call_pending_DSRs(void);
# define HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() hal_interrupt_stack_call_pending_DSRs()

// ----------------------------------------------------------------------------
// HAL_DELAY_US(). This implementation should work fine on all M68K
// processors. The variant/processor/platform HAL may already have
// supplied an implementation if for some reason this version is
// inappropriate.
//
// Lower-level HALs should define a count _HAL_M68K_DELAY_US_LOOPS_,
// and optionally _HAL_M68K_DELAY_US_LOOPS_UNCACHED_. These give the
// number of iterations needed for a single microsecond delay. In
// theory it can be worked out from the cpu and board specs, but in
// practice it is easier to do a bit of experimenting and fine-tune
// the numbers. Good initial values are 20 and 1 respectively. Note
// that the symbols need not be compile-time constants if e.g. the
// processor frequency may change at run-time for power management,
// but it is assumed that the values will not change as a side
// effect of interrupts or anything like that.
//
// An inline assembler version is used for two reasons. First it
// prevents the compiler from optimizing the code. Second it avoids
// problems when different versions of the compiler generate slightly
// better or worse code.
#ifndef HAL_DELAY_US
# ifndef _HAL_M68K_DELAY_US_LOOPS_
#  error Variant, processor or platform HAL should supply loop count parameters
# else
#  ifdef _HAL_M68K_DELAY_US_LOOPS_UNCACHED_
// A cache-aware version of the macro
#   include <cyg/hal/hal_cache.h>
#   define HAL_DELAY_US(_delay_)                                        \
    CYG_MACRO_START                                                     \
    int _icache_enabled_;                                               \
    int _loops_;                                                        \
    HAL_ICACHE_IS_ENABLED(_icache_enabled_);                            \
    _loops_ = (_icache_enabled_) ? _HAL_M68K_DELAY_US_LOOPS_ :          \
                                  _HAL_M68K_DELAY_US_LOOPS_UNCACHED_;   \
    asm volatile ( "move.l   %0, %%d0\n"                                \
                   "1:\n"                                               \
                   "subq.l  #1, %%d0\n"                                 \
                   "beq     3f\n"                                       \
                   "move.l  %1, %%d1\n"                                 \
                   "2:\n"                                               \
                   "subq.l  #1,%%d1\n"                                  \
                   "bne     2b\n"                                       \
                   "bra     1b\n"                                       \
                   "3:\n"                                               \
                   :                                                    \
                   : "d" ((_delay_) + 1), "d" ((_loops_))               \
                   : "cc", "d0", "d1");                                 \
    CYG_MACRO_END
#  else
#   define HAL_DELAY_US(_delay_)                                        \
    CYG_MACRO_START                                                     \
    int _loops_ = _HAL_M68K_DELAY_US_LOOPS_;                            \
    asm volatile ( "move.l   %0, %%d0\n"                                \
                   "1:\n"                                               \
                   "subq.l  #1, %%d0\n"                                 \
                   "beq     3f\n"                                       \
                   "move.l  %1, %%d1\n"                                 \
                   "2:\n"                                               \
                   "subq.l  #1,%%d1\n"                                  \
                   "bne     2b\n"                                       \
                   "bra     1b\n"                                       \
                   "3:\n"                                               \
                   :                                                    \
                   : "d" ((_delay_) + 1), "d" ((_loops_))               \
                   : "cc", "d0", "d1");                                 \
    CYG_MACRO_END
#  endif
# endif
#endif

// ----------------------------------------------------------------------------
// Reset support. This is done by disabling all interrupts and indirecting
// through the exception vector. Where possible variant or processor HALs
// should instead activate an on-chip reset controller.

#ifndef HAL_PLATFORM_RESET
# define HAL_PLATFORM_RESET()                                               \
    CYG_MACRO_START                                                         \
    asm volatile ("move.w   #0x2700,%%sr\n"                                 \
                  "move.l   %0,%%a0\n"                                      \
                  "move.l   0(%%a0),%%a7\n"                                 \
                  "move.l   4(%%a0),%%a1\n"                                 \
                  "jmp      (%%a1)\n"                                       \
                  : : "a" (hal_m68k_vsr_table) : "a0", "a1", "memory");     \
    CYG_MACRO_END
#endif
#ifndef HAL_PLATFORM_RESET_ENTRY
# define HAL_PLATFORM_RESET_ENTRY    (*(cyg_uint32*)0x04)
#endif

// ----------------------------------------------------------------------------
// Functions/symbols exported to the variant and platform HAL's
externC void    hal_m68k_exception_reset(void);
externC void    hal_m68k_rte(void);
externC void    hal_m68k_exception_vsr(void);
externC void    hal_m68k_interrupt_vsr(void);

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
