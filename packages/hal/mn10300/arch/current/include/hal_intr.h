#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H
//==========================================================================
//
//      hal_intr.h
//
//      HAL Interrupt and clock support
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
// Author(s):    nickg
// Contributors: nickg, jlarmour
// Date:         1999-02-18
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
// Usage:
//               #include <cyg/hal/hal_intr.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>

#include <cyg/hal/var_intr.h>

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

// MN10300 interrupt control registers, mapped by linker script.
externC volatile cyg_uint16     mn10300_interrupt_control[];

//--------------------------------------------------------------------------
// Default ISR
// The #define is used to test whether this routine exists, and to allow
// us to call it.

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

#define HAL_DEFAULT_ISR hal_default_isr

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//--------------------------------------------------------------------------

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

// Routine to execute DSRs using separate interrupt stack
externC void hal_interrupt_stack_call_pending_DSRs(void);
#define HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() \
    hal_interrupt_stack_call_pending_DSRs()

// these are offered solely for stack usage testing
// if they are not defined, then there is no interrupt stack.
#define HAL_INTERRUPT_STACK_BASE cyg_interrupt_stack_base
#define HAL_INTERRUPT_STACK_TOP  cyg_interrupt_stack
// use them to declare these extern however you want:
//       extern char HAL_INTERRUPT_STACK_BASE[];
//       extern char HAL_INTERRUPT_STACK_TOP[];
// is recommended

#endif

//--------------------------------------------------------------------------
// Interrupt control macros

#define HAL_DISABLE_INTERRUPTS(_old_)   \
        asm volatile (                  \
            "mov        psw,%0;"        \
            "mov        0xF7FF,d0;"     \
            "and        %0,d0;"         \
            "mov        d0,psw;"        \
            "and        0x0800,%0;"     \
            : "=d"(_old_)               \
            :                           \
            : "d0"                      \
            );

#define HAL_ENABLE_INTERRUPTS()         \
        asm volatile (                  \
            "mov        psw,d0;"        \
            "or         0x0800,d0;"     \
            "mov        d0,psw;"        \
            :                           \
            :                           \
            : "d0"                      \
            );

#define HAL_RESTORE_INTERRUPTS(_old_)   \
        asm volatile (                  \
            "mov        psw,d1;"        \
            "and        0xF7FF,d1;"     \
            "or         %0,d1;"         \
            "mov        d1,psw;"        \
            :                           \
            : "d"(_old_)                \
            : "d1"                      \
            );

#define HAL_QUERY_INTERRUPTS(_old_)     \
        asm volatile (                  \
            "mov        psw,%0;"        \
            "and        0x0800,%0;"     \
            : "=d"(_old_)               \
            );

//--------------------------------------------------------------------------
// Translate a vector number into an ISR table index.
// If we have chained interrupts we have just a single ISR per priority
// level. On the MN103000 there are several interrupts per controller,
// so we have to decode to one of 100 vectors. On the MN103002 there is
// only one interrupt per controller, so we can have just one ISR per
// controller, except for the NMI vectors which occupy the first 3 slots.

#ifndef HAL_TRANSLATE_VECTOR

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_)                             \
{                                                                          \
    if( _vector_ <= CYGNUM_HAL_INTERRUPT_SYSTEM_ERROR )                    \
        (_index_) = (_vector_);                                            \
    else                                                                   \
    {                                                                      \
        /* ICRs are 16 bit regs at 32 bit spacing */                       \
        cyg_ucount16 _ix_ = ((_vector_)>>2)<<1;                            \
                                                                           \
        /* read the appropriate interrupt control register */              \
        cyg_uint16 _icr_ = mn10300_interrupt_control[_ix_];                \
                                                                           \
        /* extract interrupt priority level */                             \
        _index_ = CYGNUM_HAL_INTERRUPT_RESERVED_3 + ((_icr_ >> 12) & 0x7); \
    }                                                                      \
}

#else

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) _index_ = (_vector_)

#endif

#endif

//--------------------------------------------------------------------------
// Interrupt and VSR attachment macros

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)                                \
CYG_MACRO_START                                                                 \
    cyg_uint32 _index_;                                                         \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                                 \
                                                                                \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)HAL_DEFAULT_ISR )       \
        (_state_) = 0;                                                          \
    else                                                                        \
        (_state_) = 1;                                                          \
CYG_MACRO_END

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )               \
CYG_MACRO_START                                                                 \
    cyg_uint32 _index_;                                                         \
    HAL_TRANSLATE_VECTOR(_vector_,_index_);                                     \
                                                                                \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)HAL_DEFAULT_ISR )       \
    {                                                                           \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;                   \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD)_data_;                     \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;                 \
    }                                                                           \
CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
CYG_MACRO_START                                                         \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR(_vector_,_index_);                             \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )         \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)HAL_DEFAULT_ISR; \
        hal_interrupt_data[_index_] = 0;                                \
        hal_interrupt_objects[_index_] = 0;                             \
    }                                                                   \
CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *((CYG_ADDRESS *)_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )                       \
    if( _poldvsr_ != NULL )                                             \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];            \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;


//--------------------------------------------------------------------------
// Interrupt controller access
// Read interrupt control registers back after writing to them. This
// ensures that the written value is not sitting in the store buffers
// when interrupts are re-enabled.
#define HAL_INTERRUPT_MASK( _vector_ )                                  \
{                                                                       \
    /* ICRs are 16 bit regs at 32 bit spacing */                        \
    cyg_ucount16 _index_ = ((_vector_)>>2)<<1;                          \
                                                                        \
    /* read the appropriate interrupt control register */               \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];              \
                                                                        \
    /* clear interrupt enable bit for this vector */                    \
    _icr_ &= ~(0x0100<<((_vector_)&3));                                 \
                                                                        \
    /* restore the interrupt control register */                        \
    mn10300_interrupt_control[_index_] = _icr_;                         \
    _icr_ = mn10300_interrupt_control[_index_];                         \
}

#define HAL_INTERRUPT_UNMASK( _vector_ )                                \
{                                                                       \
    /* ICRs are 16 bit regs at 32 bit spacing */                        \
    cyg_ucount16 _index_ = (_vector_>>2)<<1;                            \
                                                                        \
    /* read the appropriate interrupt control register */               \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];              \
                                                                        \
    /* set interrupt enable bit for this vector */                      \
    _icr_ |= (0x0100<<(_vector_&3));                                    \
                                                                        \
    /* restore the interrupt control register */                        \
    mn10300_interrupt_control[_index_] = _icr_;                         \
    _icr_ = mn10300_interrupt_control[_index_];                         \
}

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                   \
{                                                               \
    /* ICRs are 16 bit regs at 32 bit spacing */                \
    cyg_ucount16 _index_ = ((_vector_)>>2)<<1;                  \
                                                                \
    /* read the appropriate interrupt control register */       \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];      \
                                                                \
    /* clear interrupt request bit for this vector */           \
    _icr_ &= ~(0x0010<<((_vector_)&3));                         \
                                                                \
    /* set interrupt detect bit for this vector */              \
    _icr_ |= (0x0001<<((_vector_)&3));                          \
                                                                \
    /* restore the interrupt control register */                \
    mn10300_interrupt_control[_index_] = _icr_;                 \
    _icr_ = mn10300_interrupt_control[_index_];                 \
}

#if !defined(HAL_INTERRUPT_CONFIGURE)

#error HAL_INTERRUPT_CONFIGURE not defined by variant

#endif

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            \
{                                                               \
    /* ICRs are 16 bit regs at 32 bit spacing */                \
    cyg_ucount16 _index_ = (_vector_>>2)<<1;                    \
                                                                \
    /* read the appropriate interrupt control register */       \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];      \
                                                                \
    /* set interrupt level for this group of vectors */         \
    _icr_ &= 0x0FFF;                                            \
    _icr_ |= (_level_)<<12;                                     \
                                                                \
    /* restore the interrupt control register */                \
    mn10300_interrupt_control[_index_] = _icr_;                 \
    _icr_ = mn10300_interrupt_control[_index_];                 \
}

//--------------------------------------------------------------------------
// Clock control.
// This is almost all handled in the var_intr.h.

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY(_pvalue_) HAL_CLOCK_READ(_pvalue_)
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// EOF hal_intr.h
