#ifndef CYGONCE_HAL_INTR_H
#define CYGONCE_HAL_INTR_H

//=============================================================================
//
//      hal_intr.h
//
//      HAL Interrupt and clock support
//
//=============================================================================
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-02-17
// Purpose:     Define Interrupt support
// Description: The macros defined here provide the HAL APIs for handling
//              interrupts and the clock.
//              
// Usage:
//              #include <cyg/hal/hal_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// PowerPC exception vectors.

#define CYG_VECTOR_RESERVED_0           0
#define CYG_VECTOR_RESET                1
#define CYG_VECTOR_MACHINE_CHECK        2
#define CYG_VECTOR_DSI                  3
#define CYG_VECTOR_ISI                  4
#define CYG_VECTOR_INTERRUPT            5
#define CYG_VECTOR_ALIGNMENT            6
#define CYG_VECTOR_PROGRAM              7
#define CYG_VECTOR_FP_UNAVAILABLE       8
#define CYG_VECTOR_DECREMENTER          9
#define CYG_VECTOR_RESERVED_A           10
#define CYG_VECTOR_RESERVED_B           11
#define CYG_VECTOR_SYSTEM_CALL          12
#define CYG_VECTOR_TRACE                13
#define CYG_VECTOR_FP_ASSIST            14

#ifdef CYG_HAL_POWERPC_MP860
#define CYG_VECTOR_RESERVED_F           15
#define CYG_VECTOR_SW_EMUL              16
#define CYG_VECTOR_ITLB_MISS            17
#define CYG_VECTOR_DTLB_MISS            18
#define CYG_VECTOR_ITLB_ERROR           19
#define CYG_VECTOR_DTLB_ERROR           20
#define CYG_VECTOR_RESERVED_15          21
#define CYG_VECTOR_RESERVED_16          22
#define CYG_VECTOR_RESERVED_17          23
#define CYG_VECTOR_RESERVED_18          24
#define CYG_VECTOR_RESERVED_19          25
#define CYG_VECTOR_RESERVED_1A          26
#define CYG_VECTOR_RESERVED_1B          27
#define CYG_VECTOR_DATA_BP              28
#define CYG_VECTOR_INSTRUCTION_BP       29
#define CYG_VECTOR_PERIPHERAL_BP        30
#define CYG_VECTOR_NMI                  31
#endif



#define CYG_VSR_MIN                      0
#define CYG_VSR_MAX                     14
#define CYG_VSR_COUNT                   15

#define CYG_ISR_MIN                      0
#define CYG_ISR_MAX                     14
#define CYG_ISR_COUNT                   15

#ifdef CYG_HAL_POWERPC_MP860
#define CYG_EXCEPTION_MIN                0
#define CYG_EXCEPTION_MAX               31
#define CYG_EXCEPTION_COUNT             32
#else
#define CYG_EXCEPTION_MIN                0
#define CYG_EXCEPTION_MAX               14
#define CYG_EXCEPTION_COUNT             15
#endif


// The vector used by the Real time clock
#define CYG_VECTOR_RTC                  CYG_VECTOR_DECREMENTER         

//-----------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYG_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYG_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYG_ISR_COUNT];        

// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYG_VSR_COUNT];

//-----------------------------------------------------------------------------
// Default ISR

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

//-----------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//-----------------------------------------------------------------------------
// Interrupt control macros

#define HAL_DISABLE_INTERRUPTS(_old_)   \
    asm volatile (                      \
        "mfmsr  %0;"                    \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "not    5,4;"                   \
        "and    5,%0,5;"                \
        "mtmsr  5;"                     \
        "and    %0,%0,4"                \
        : "=r"(_old_)                   \
        :                               \
        : "r4", "r5"                    \
        );

#define HAL_ENABLE_INTERRUPTS()         \
    asm volatile (                      \
        "mfmsr  3;"                     \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "or     3,3,4;"                 \
        "mtmsr  3;"                     \
        :                               \
        :                               \
        : "r3", "r4"                    \
        );

#define HAL_RESTORE_INTERRUPTS(_old_)   \
    asm volatile (                      \
        "mfmsr  3;"                     \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "and    4,%0,4;"                \
        "or     4,3,4;"                 \
        "mtmsr  4;"                     \
        :                               \
        : "r"(_old_)                    \
        : "r3", "r4"                    \
        );

// FIXME: using andi with side-effect on cc would be better but causes
// compiler problem.
#define HAL_QUERY_INTERRUPTS(_old_)     \
    asm volatile (                      \
        "mfmsr  %0;"                    \
        "lis    4,0;"                   \
        "ori    4,4,0x8000;"            \
        "and    %0,%0,4;"               \
        : "=r"(_old_)                   \
        :                               \
        : "r4"                          \
        );

//-----------------------------------------------------------------------------
// Vector translation.
// Only one vector is available for external interrupts. No translation
// required.

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)


//-----------------------------------------------------------------------------
// Interrupt and VSR attachment macros

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )         \
    CYG_MACRO_START                                                       \
    cyg_uint32 _index_ = ((_vector_)>>3)&1;                               \
                                                                          \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr ) \
    {                                                                     \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;             \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD) _data_;              \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;           \
    }                                                                     \
    CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
    CYG_MACRO_START                                                     \
    cyg_uint32 _index_ = ((_vector_)>>3)&1;                             \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )         \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)hal_default_isr; \
        hal_interrupt_data[_index_] = 0;                                \
        hal_interrupt_objects[_index_] = 0;                             \
    }                                                                   \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *(CYG_ADDRESS *)(_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )               \
    CYG_MACRO_START                                             \
    if( _poldvsr_ != NULL )                                     \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];    \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;               \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// Interrupt controller access

// These are not defined since individual interrupts cannot be masked
// without the help of an external interrupt controller.

#define HAL_INTERRUPT_MASK( _vector_ )

#define HAL_INTERRUPT_UNMASK( _vector_ )

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )

//-----------------------------------------------------------------------------
// Clock control

#define HAL_CLOCK_INITIALIZE( _period_ )        \
    CYG_MACRO_START                             \
    cyg_uint32 period = _period_;               \
    asm volatile (                              \
        "mtdec %0;"                             \
        :                                       \
        : "r"(period)                           \
        );                                      \
    CYG_MACRO_END

#define HAL_CLOCK_RESET( _vector_, _period_ )   \
    CYG_MACRO_START                             \
    cyg_uint32 period = _period_;               \
    asm volatile (                              \
        "mtdec %0;"                             \
        :                                       \
        : "r"(period)                           \
        );                                      \
    CYG_MACRO_END

#define HAL_CLOCK_READ( _pvalue_ )                              \
    CYG_MACRO_START                                             \
    register cyg_uint32 result;                                 \
    asm volatile(                                               \
        "mfdec  %0;"                                            \
        : "=r"(result)                                          \
        );                                                      \
    *(_pvalue_) = CYGNUM_KERNEL_COUNTERS_RTC_PERIOD-result;     \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_INTR_H
// End of hal_intr.h
