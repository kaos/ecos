#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//==========================================================================
//
//      hal_intr.h
//
//      ColdFire interrupt/exception support
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
// Contributors:
// Date:          2005-25-06
// Purpose:       Provide ColdFire-specific interrupt and exception
//                definitions.
// Usage:         #include <cyg/hal/hal_intr.h>
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>

#include <cyg/hal/var_intr.h>

#include <cyg/infra/cyg_ass.h>      // CYG_FAIL

// -------------------------------------------------------------------------
// ColdFire exception vectors. These correspond to VSRs and are the values
// to use for HAL_VSR_GET/SET.

#define CYGNUM_HAL_VECTOR_RESETSP           0
#define CYGNUM_HAL_VECTOR_RESETPC           1
#define CYGNUM_HAL_VECTOR_BUSERR            2
#define CYGNUM_HAL_VECTOR_ADDRERR           3
#define CYGNUM_HAL_VECTOR_ILLINST           4
#define CYGNUM_HAL_VECTOR_ZERODIV           5

// Exception vectors 6-7 are reserved

#define CYGNUM_HAL_VECTOR_PRIVVIOLATION     8
#define CYGNUM_HAL_VECTOR_TRACE             9
#define CYGNUM_HAL_VECTOR_L1010             10
#define CYGNUM_HAL_VECTOR_L1111             11
#define CYGNUM_HAL_VECTOR_DEBUG12           12
#define CYGNUM_HAL_VECTOR_DEBUG13           13
#define CYGNUM_HAL_VECTOR_FORMAT            14
#define CYGNUM_HAL_VECTOR_UNINITINT         15

// Exception vectors 16-23 are reserved

#define CYGNUM_HAL_VECTOR_SPURINT           24

#define CYGNUM_HAL_VECTOR_AUTOVEC1          25
#define CYGNUM_HAL_VECTOR_AUTOVEC2          26
#define CYGNUM_HAL_VECTOR_AUTOVEC3          27
#define CYGNUM_HAL_VECTOR_AUTOVEC4          28
#define CYGNUM_HAL_VECTOR_AUTOVEC5          29
#define CYGNUM_HAL_VECTOR_AUTOVEC6          30
#define CYGNUM_HAL_VECTOR_AUTOVEC7          31
#define CYGNUM_HAL_NUMAUTOVEC               7

#define CYGNUM_HAL_VECTOR_TRAPFIRST         32
#define CYGNUM_HAL_VECTOR_TRAPLAST          47
#define CYGNUM_HAL_NUMTRAPS                 16

#define CYGNUM_HAL_VECTOR_FP_BRANCH         48
#define CYGNUM_HAL_VECTOR_FP_INEXACT        49
#define CYGNUM_HAL_VECTOR_FP_ZERODIV        50
#define CYGNUM_HAL_VECTOR_FP_UNDERFLOW      51
#define CYGNUM_HAL_VECTOR_FP_OPERAND        52
#define CYGNUM_HAL_VECTOR_FP_OVERFLOW       53
#define CYGNUM_HAL_VECTOR_FP_NAN            54
#define CYGNUM_HAL_VECTOR_FP_DENORM         55

// Exception vectors 56-60 are reserved

#define CYGNUM_HAL_VECTOR_UNSUPINST         61

// Exception vectors 62-63 are reserved

#define CYGNUM_HAL_VECTOR_USERINTRFIRST     64
#define CYGNUM_HAL_VECTOR_USERINTRLAST      255
#define CYGNUM_HAL_NUMUSERINTR              192

// -------------------------------------------------------------------------
// Interrupt and exception vector table definitions.

#define CYGNUM_HAL_VSR_MIN                  0
#define CYGNUM_HAL_VSR_MAX                  255
#define CYGNUM_HAL_VSR_COUNT                (CYGNUM_HAL_VSR_MAX - CYGNUM_HAL_VSR_MIN + 1)

// To simplify things in interrupt handling code, we don't take into account
// autovectored, spurious and uninitialized interrupts.

#ifndef CYGNUM_HAL_ISR_RANGE_DEFINED
#define CYGNUM_HAL_ISR_MIN                   CYGNUM_HAL_VECTOR_USERINTRFIRST
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_VECTOR_USERINTRLAST
#define CYGNUM_HAL_ISR_COUNT                 (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)
#endif

#ifndef CYGNUM_HAL_EXCEPTION_RANGE_DEFINED
#define CYGNUM_HAL_EXCEPTION_MIN             CYGNUM_HAL_VECTOR_BUSERR
#define CYGNUM_HAL_EXCEPTION_MAX             CYGNUM_HAL_VECTOR_UNSUPINST
#define CYGNUM_HAL_EXCEPTION_COUNT           (CYGNUM_HAL_EXCEPTION_MAX -\
    CYGNUM_HAL_EXCEPTION_MIN + 1)
#endif

// -------------------------------------------------------------------------
// Equivalence between ColdFire exception names and target independent
// exception names.
// These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception().

#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION CYGNUM_HAL_VECTOR_ILLINST
#define CYGNUM_HAL_EXCEPTION_DIV_BY_ZERO         CYGNUM_HAL_VECTOR_ZERODIV
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS         CYGNUM_HAL_VECTOR_BUSERR
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS         CYGNUM_HAL_VECTOR_BUSERR

// -------------------------------------------------------------------------
// Spurious interrupt definition.

#ifndef CYGNUM_HAL_SPURIOUS_INTERRUPT
#define CYGNUM_HAL_SPURIOUS_INTERRUPT CYGNUM_HAL_VECTOR_SPURINT
#endif

// -------------------------------------------------------------------------
// Static data used by HAL.

// ISR tables
externC volatile CYG_ADDRESS  cyg_hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD cyg_hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS  cyg_hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
externC volatile CYG_ADDRESS  cyg_hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

// ROM VSR table
externC CYG_ADDRESS rom_vsr_table[CYGNUM_HAL_VSR_COUNT];

// -------------------------------------------------------------------------
// Interrupt stack definitions.

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

externC void hal_interrupt_stack_call_pending_DSRs(void);
#define HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() \
    hal_interrupt_stack_call_pending_DSRs()

#endif

// A separate stack always exist to allow the processor to initialize itself.
// It depends on CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK macro
// definition if this stack is used for interrupts too.

#define HAL_INTERRUPT_STACK_BASE cyg_interrupt_stack_base
#define HAL_INTERRUPT_STACK_TOP  cyg_interrupt_stack

externC char HAL_INTERRUPT_STACK_BASE[];
externC char HAL_INTERRUPT_STACK_TOP[];

// --------------------------------------------------------------------------
// Translate a vector number into an ISR table index.

#ifndef HAL_TRANSLATE_VECTOR
#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_- CYGNUM_HAL_ISR_MIN)
#endif

// -------------------------------------------------------------------------
// Interrupt state storage.

typedef cyg_uint16 CYG_INTERRUPT_STATE;

// --------------------------------------------------------------------------
// Interrupt and VSR attachment macros.

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

externC void hal_default_exception_handler(CYG_WORD vector,
                                           HAL_SavedRegisters *regs);

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)        \
CYG_MACRO_START                                         \
    cyg_uint32 _index_;                                 \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);         \
                                                        \
    if (cyg_hal_interrupt_handlers[_index_]             \
        == (CYG_ADDRESS) &hal_default_isr)              \
        (_state_) = 0;                                  \
    else                                                \
        (_state_) = 1;                                  \
CYG_MACRO_END

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )       \
CYG_MACRO_START                                                         \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);                          \
                                                                        \
    if (cyg_hal_interrupt_handlers[_index_]                             \
        == (CYG_ADDRESS) &hal_default_isr)                              \
    {                                                                   \
        cyg_hal_interrupt_handlers[_index_] = (CYG_ADDRESS)(_isr_);     \
        cyg_hal_interrupt_data[_index_] = (CYG_ADDRWORD)(_data_);       \
        cyg_hal_interrupt_objects[_index_] = (CYG_ADDRESS)(_object_);   \
    }                                                                   \
CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
CYG_MACRO_START                                                         \
    cyg_uint32 _index_;                                                 \
    HAL_INTERRUPT_MASK(_vector_);                                       \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);                          \
    if (cyg_hal_interrupt_handlers[_index_]                             \
        == (CYG_ADDRESS)(_isr_))                                        \
    {                                                                   \
        cyg_hal_interrupt_handlers[_index_] =                           \
            (CYG_ADDRESS)&hal_default_isr;                              \
        cyg_hal_interrupt_data[_index_] = 0;                            \
        cyg_hal_interrupt_objects[_index_] = 0;                         \
    }                                                                   \
CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *((CYG_ADDRESS *)(_pvsr_)) = cyg_hal_vsr_table[(_vector_)];


#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )                       \
CYG_MACRO_START                                                         \
    if( (_poldvsr_) != NULL )                                           \
        *(CYG_ADDRESS *)(_poldvsr_) = cyg_hal_vsr_table[(_vector_)];    \
    cyg_hal_vsr_table[(_vector_)] = (CYG_ADDRESS)(_vsr_);               \
CYG_MACRO_END


// This is an ugly name, but what it means is: grab the VSR back to eCos
// internal handling, or if you like, the default handler. But if
// cooperating with a ROM monitor, the default behaviour is to pass most
// exceptions to it. This macro undoes that so that eCos handles the
// exception. So use it with care.

#define HAL_VSR_SET_TO_ECOS_HANDLER( _vector_, _poldvsr_ )              \
    CYG_MACRO_START                                                     \
    if( (void*)_poldvsr_ != (void*)NULL )                               \
        *(CYG_ADDRESS *)_poldvsr_ = cyg_hal_vsr_table[_vector_];        \
    cyg_hal_vsr_table[_vector_] = rom_vsr_table[_vector_];              \
    CYG_MACRO_END

// -------------------------------------------------------------------------
// Interrupt control macros.

// The following interrupt control macros are the default for the ColdFire
// architecture. Some processor variants will override these definitions in
// their var_intr.h file.

#ifndef HAL_CF_SET_SR
#define HAL_CF_SET_SR(__newsr__)                                            \
    CYG_MACRO_START                                                         \
    asm volatile ("move.w   %0,%%sr\n"                                      \
                  :                                                         \
                  : "d" ((CYG_INTERRUPT_STATE)(__newsr__)));                \
    CYG_MACRO_END
#endif // HAL_CF_SET_SR

#ifndef HAL_ENABLE_INTERRUPTS
#define HAL_ENABLE_INTERRUPTS()                                             \
    CYG_MACRO_START                                                         \
    CYG_INTERRUPT_STATE _msk_;                                              \
    HAL_QUERY_INTERRUPTS(_msk_);                                            \
    HAL_CF_SET_SR((_msk_ & (CYG_INTERRUPT_STATE)0xf8ff));                   \
    CYG_MACRO_END
#endif // HAL_ENABLE_INTERRUPTS

#ifndef HAL_DISABLE_INTERRUPTS
#define HAL_DISABLE_INTERRUPTS(_old_)                                       \
    CYG_MACRO_START                                                         \
    HAL_QUERY_INTERRUPTS(_old_);                                            \
    HAL_CF_SET_SR((_old_ | (CYG_INTERRUPT_STATE)0x0700));                   \
    CYG_MACRO_END
#endif //HAL_DISABLE_INTERRUPTS

#ifndef HAL_RESTORE_INTERRUPTS
#define HAL_RESTORE_INTERRUPTS(_prev_)                                      \
    CYG_MACRO_START                                                         \
    CYG_INTERRUPT_STATE _msk_;                                              \
    HAL_QUERY_INTERRUPTS(_msk_);                                            \
    _msk_ &= (CYG_INTERRUPT_STATE)0xf8ff;                                   \
    _msk_ |= (((CYG_INTERRUPT_STATE)(_prev_))                               \
              & (CYG_INTERRUPT_STATE)0x0700);                               \
    asm volatile ("move.w   %0,%%sr\n"                                      \
                  :                                                         \
                  : "d" (_msk_));                                           \
    CYG_MACRO_END
#endif // HAL_RESTORE_INTERRUPTS

// Use the extra assignment to avoid warnings.
// The compiler should optimize it out.
#ifndef HAL_QUERY_INTERRUPTS
#define HAL_QUERY_INTERRUPTS(__oldmask__)                                   \
    CYG_MACRO_START                                                         \
    CYG_INTERRUPT_STATE _omsk_ = (CYG_INTERRUPT_STATE)(__oldmask__);        \
    asm volatile ("move.w   %%sr,%0\n"                                      \
                  : "=d" (_omsk_)                                           \
                  : );                                                      \
    (__oldmask__) = (__typeof__(__oldmask__))_omsk_;                        \
    CYG_MACRO_END
#endif // HAL_QUERY_INTERRUPTS

// ---------------------------------------------------------------------------
// End of hal_intr.h
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
