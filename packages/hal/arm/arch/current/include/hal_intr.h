#ifndef CYGONCE_HAL_INTR_H
#define CYGONCE_HAL_INTR_H

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
// Author(s):    nickg, gthomas
// Contributors: nickg, gthomas,
//               jlarmour
// Date:         1999-02-20
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:        #include <cyg/hal/hal_intr.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_platform_ints.h>

//--------------------------------------------------------------------------
// ARM exception vectors.

// These vectors correspond to VSRs. These values are the ones to use for
// HAL_VSR_GET/SET

#define CYGNUM_HAL_VECTOR_RESET                0
#define CYGNUM_HAL_VECTOR_UNDEF_INSTRUCTION    1
#define CYGNUM_HAL_VECTOR_SOFTWARE_INTERRUPT   2
#define CYGNUM_HAL_VECTOR_ABORT_PREFETCH       3
#define CYGNUM_HAL_VECTOR_ABORT_DATA           4
#define CYGNUM_HAL_VECTOR_reserved             5
#define CYGNUM_HAL_VECTOR_IRQ                  6
#define CYGNUM_HAL_VECTOR_FIQ                  7

#define CYGNUM_HAL_VSR_MIN                     0
#define CYGNUM_HAL_VSR_MAX                     7
#define CYGNUM_HAL_VSR_COUNT                   8

// Exception vectors. These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception()

#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION \
          CYGNUM_HAL_VECTOR_UNDEF_INSTRUCTION
#define CYGNUM_HAL_EXCEPTION_INTERRUPT \
          CYGNUM_HAL_VECTOR_SOFTWARE_INTERRUPT
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS    CYGNUM_HAL_VECTOR_ABORT_PREFETCH
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS    CYGNUM_HAL_VECTOR_ABORT_DATA
#define CYGNUM_HAL_EXCEPTION_FIQ            CYGNUM_HAL_VECTOR_FIQ

#define CYGNUM_HAL_EXCEPTION_MIN     CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION
#define CYGNUM_HAL_EXCEPTION_MAX     CYGNUM_HAL_EXCEPTION_FIQ
#define CYGNUM_HAL_EXCEPTION_COUNT   (CYGNUM_HAL_EXCEPTION_MAX - \
                                      CYGNUM_HAL_EXCEPTION_MIN + 1)

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
//externC volatile CYG_ADDRESS    hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

// actually, it is immediately after the hardware vectors
#define hal_vsr_table ((volatile CYG_ADDRESS *)(0x20))

// and followed by a DRAM size variable used during bootup.  This is in a
// fixed location so that it can be shared between an eCos Stub ROM and a
// RAM start eCos app.  Ie. so that RAM startup code knows DRAM size.
// If this variable is zero, RAM size is unknown/is the default.
#define hal_dram_size (*(CYG_WORD *)(0x40))

//--------------------------------------------------------------------------
// Default ISR

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//--------------------------------------------------------------------------
// Interrupt control macros

// Note: This disables both FIQ and IRQ interrupts!
#define HAL_DISABLE_INTERRUPTS(_old_)           \
    asm volatile (                              \
        "mrs %0,cpsr;"                          \
        "mrs r4,cpsr;"                          \
        "orr r4,r4,#0xC0;"                      \
        "msr cpsr,r4"                           \
        : "=r"(_old_)                           \
        :                                       \
        : "r4"                                  \
        );

#define HAL_ENABLE_INTERRUPTS()                 \
    asm volatile (                              \
        "mrs r3,cpsr;"                          \
        "bic r3,r3,#0xC0;"                      \
        "msr cpsr,r3"                           \
        :                                       \
        :                                       \
        : "r3"                                  \
        );

#define HAL_RESTORE_INTERRUPTS(_old_)           \
    asm volatile (                              \
        "mrs r3,cpsr;"                          \
        "and r4,%0,#0xC0;"                      \
        "bic r3,r3,#0xC0;"                      \
        "orr r3,r3,r4;"                         \
        "msr cpsr,r3"                           \
        :                                       \
        : "r"(_old_)                            \
        : "r3", "r4"                            \
        );

#define HAL_QUERY_INTERRUPTS(_old_)             \
    asm volatile (                              \
        "mrs r4,cpsr;"                          \
        "and r4,r4,#0xC0;"                      \
        "eor %0,r4,#0xC0;"                      \
        : "=r"(_old_)                           \
        :                                       \
        : "r4"                                  \
        );

//--------------------------------------------------------------------------
// Routine to execute DSRs using separate interrupt stack

#ifdef  CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
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
// Vector translation.

#ifndef HAL_TRANSLATE_VECTOR
#define HAL_TRANSLATE_VECTOR(_vector_,_index_) \
    (_index_) = (_vector_)
#endif

//--------------------------------------------------------------------------
// Interrupt and VSR attachment macros

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)                          \
    CYG_MACRO_START                                                       \
    cyg_uint32 _index_;                                                   \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                           \
                                                                          \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr ) \
        (_state_) = 0;                                                    \
    else                                                                  \
        (_state_) = 1;                                                    \
    CYG_MACRO_END

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )          \
    CYG_MACRO_START                                                        \
    if( hal_interrupt_handlers[_vector_] == (CYG_ADDRESS)hal_default_isr ) \
    {                                                                      \
        hal_interrupt_handlers[_vector_] = (CYG_ADDRESS)_isr_;             \
        hal_interrupt_data[_vector_] = (CYG_ADDRWORD) _data_;              \
        hal_interrupt_objects[_vector_] = (CYG_ADDRESS)_object_;           \
    }                                                                      \
    CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                            \
    CYG_MACRO_START                                                        \
    if( hal_interrupt_handlers[_vector_] == (CYG_ADDRESS)_isr_ )           \
    {                                                                      \
        hal_interrupt_handlers[_vector_] = (CYG_ADDRESS)hal_default_isr;   \
        hal_interrupt_data[_vector_] = 0;                                  \
        hal_interrupt_objects[_vector_] = 0;                               \
    }                                                                      \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                         \
    *(CYG_ADDRESS *)(_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )               \
    CYG_MACRO_START                                             \
    if( _poldvsr_ != NULL )                                     \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];    \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;               \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// Interrupt controller access

externC void hal_interrupt_mask(int);
externC void hal_interrupt_unmask(int);
externC void hal_interrupt_acknowledge(int);
externC void hal_interrupt_configure(int, int, int);
externC void hal_interrupt_set_level(int, int);

#define HAL_INTERRUPT_MASK( _vector_ )                     \
    hal_interrupt_mask( _vector_ ) 
#define HAL_INTERRUPT_UNMASK( _vector_ )                   \
    hal_interrupt_unmask( _vector_ )
#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )              \
    hal_interrupt_acknowledge( _vector_ )
#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ ) \
    hal_interrupt_configure( _vector_, _level_, _up_ )
#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )       \
    hal_interrupt_set_level( _vector_, _level_ )

//--------------------------------------------------------------------------
// Clock control

externC void hal_clock_initialize(cyg_uint32);
externC void hal_clock_read(cyg_uint32 *);
externC void hal_clock_reset(cyg_uint32, cyg_uint32);

#define HAL_CLOCK_INITIALIZE( _period_ )   hal_clock_initialize( _period_ )
#define HAL_CLOCK_RESET( _vec_, _period_ ) hal_clock_reset( _vec_, _period_ )
#define HAL_CLOCK_READ( _pvalue_ )         hal_clock_read( _pvalue_ )
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )      HAL_CLOCK_READ( (cyg_uint32 *)_pvalue_ )
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_INTR_H
// End of hal_intr.h
