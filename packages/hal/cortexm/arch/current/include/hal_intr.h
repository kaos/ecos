#ifndef CYGONCE_HAL_INTR_H
#define CYGONCE_HAL_INTR_H
/*==========================================================================
//
//      hal_intr.h
//
//      Cortex-M interrupt and clock abstractions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.
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
// Author(s):    nickg
// Date:         2008-07-30
// Description:  Define interrupt and clock abstractions
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_io.h>

#include <cyg/hal/var_intr.h>           // variant extensions

//==========================================================================
// Exception vectors
//
// These are the common vectors defined by all Cortex-M CPUs.  The
// exact number of vectors is variant specific, so the limits will be
// defined in var_intr.h.


#define CYGNUM_HAL_VECTOR_STACK          0      // Reset stack pointer
#define CYGNUM_HAL_VECTOR_RESET          1      // Reset entry point
#define CYGNUM_HAL_VECTOR_NMI            2      // Non-Maskable Interrupt
#define CYGNUM_HAL_VECTOR_HARD_FAULT     3      // Hard fault
#define CYGNUM_HAL_VECTOR_MEMORY_MAN     4      // Memory management (M3)
#define CYGNUM_HAL_VECTOR_BUS_FAULT      5      // Bus Fault
#define CYGNUM_HAL_VECTOR_USAGE_FAULT    6      // Usage Fault
#define CYGNUM_HAL_VECTOR_RESERVED_07    7      
#define CYGNUM_HAL_VECTOR_RESERVED_08    8
#define CYGNUM_HAL_VECTOR_RESERVED_09    9
#define CYGNUM_HAL_VECTOR_RESERVED_10   10
#define CYGNUM_HAL_VECTOR_SERVICE       11      // System service call
#define CYGNUM_HAL_VECTOR_DEBUG         12      // Debug monitor (M3)
#define CYGNUM_HAL_VECTOR_RESERVED_13   13
#define CYGNUM_HAL_VECTOR_PENDSV        14      // Pendable svc request
#define CYGNUM_HAL_VECTOR_SYS_TICK      15      // System timer tick
#define CYGNUM_HAL_VECTOR_EXTERNAL      16      // Base of external interrupts


//==========================================================================
// Interrupt vectors
//
// The system tick interrupt is mapped to vector 0 and all external
// interrupts are mapped from vector 1 up.

#define CYGNUM_HAL_INTERRUPT_SYS_TICK    0
#define CYGNUM_HAL_INTERRUPT_EXTERNAL    1


// Variant or platform allowed to override these definitions to use
// a different RTC
#ifndef CYGNUM_HAL_INTERRUPT_RTC
#define CYGNUM_HAL_INTERRUPT_RTC        CYGNUM_HAL_INTERRUPT_SYS_TICK
#endif

//==========================================================================
// Include variant definitions here.

#include <cyg/hal/var_intr.h>

//==========================================================================
// Exception vectors.
//
// These are the values used when passed out to an external exception
// handler using cyg_hal_deliver_exception()

#define CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_ACCESS        CYGNUM_HAL_VECTOR_MEMORY_MAN
#define CYGNUM_HAL_EXCEPTION_CODE_TLBMISS_ACCESS        CYGNUM_HAL_VECTOR_MEMORY_MAN
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS                CYGNUM_HAL_VECTOR_BUS_FAULT
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS                CYGNUM_HAL_VECTOR_BUS_FAULT
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION        CYGNUM_HAL_VECTOR_USAGE_FAULT
#define CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS      CYGNUM_HAL_VECTOR_USAGE_FAULT
#define CYGNUM_HAL_EXCEPTION_INTERRUPT                  CYGNUM_HAL_VECTOR_SERVICE


#define CYGNUM_HAL_EXCEPTION_MIN     CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS
#define CYGNUM_HAL_EXCEPTION_MAX     CYGNUM_HAL_EXCEPTION_INTERRUPT
#define CYGNUM_HAL_EXCEPTION_COUNT   (CYGNUM_HAL_EXCEPTION_MAX - \
                                      CYGNUM_HAL_EXCEPTION_MIN + 1)


//==========================================================================
// VSR handling

__externC volatile CYG_ADDRESS   hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

#ifndef HAL_VSR_GET
#define HAL_VSR_GET( __vector, __pvsr )                         \
    *(CYG_ADDRESS *)(__pvsr) = hal_vsr_table[__vector];
#endif

#ifndef HAL_VSR_SET
#define HAL_VSR_SET( __vector, __vsr, __poldvsr )               \
CYG_MACRO_START                                                 \
    if( __poldvsr != NULL )                                     \
        *(CYG_ADDRESS *)__poldvsr = hal_vsr_table[__vector];    \
    hal_vsr_table[__vector] = (CYG_ADDRESS)__vsr;               \
CYG_MACRO_END
#endif

#ifndef HAL_VSR_SET_TO_ECOS_HANDLER
__externC void hal_default_interrupt_vsr( void );
__externC void hal_default_exception_vsr( void );
# define HAL_VSR_SET_TO_ECOS_HANDLER( __vector, __poldvsr )             \
CYG_MACRO_START                                                         \
    cyg_uint32  __vector2    = (cyg_uint32) (__vector);                 \
    CYG_ADDRESS* __poldvsr2  = (CYG_ADDRESS*)(__poldvsr);               \
    if( __vector2 < CYGNUM_HAL_VECTOR_SYS_TICK )                        \
        HAL_VSR_SET(__vector2, &hal_default_exception_vsr, __poldvsr2); \
    else                                                                \
        HAL_VSR_SET(__vector2, &hal_default_interrupt_vsr, __poldvsr2); \
CYG_MACRO_END
#endif

// Default definition of HAL_TRANSLATE_VECTOR(), a no-op
#ifndef HAL_TRANSLATE_VECTOR
# define HAL_TRANSLATE_VECTOR(__vector, __index) ((__index) = (__vector))
#endif

//==========================================================================
// ISR handling
//
// Interrupt handler/data/object tables plus functions and macros to
// manipulate them.
 
__externC volatile CYG_ADDRESS   hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
__externC volatile CYG_ADDRWORD  hal_interrupt_data    [CYGNUM_HAL_ISR_COUNT];
__externC volatile CYG_ADDRESS   hal_interrupt_objects [CYGNUM_HAL_ISR_COUNT];

//--------------------------------------------------------------------------
// Interrupt delivery
//
// This function is used by the HAL to deliver an interrupt, and post
// a DSR if required. It may also be used to deliver secondary
// interrupts from springboard ISRs.

__externC void hal_deliver_interrupt( cyg_uint32 vector );

//--------------------------------------------------------------------------
// Default ISR The #define is used to test whether this routine
// exists, and to allow code outside the HAL to call it.

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);
#define HAL_DEFAULT_ISR hal_default_isr

//--------------------------------------------------------------------------

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)                        \
{                                                                       \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                         \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr ) \
        (_state_) = 0;                                                  \
    else                                                                \
        (_state_) = 1;                                                  \
}

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )       \
{                                                                       \
    if( hal_interrupt_handlers[_vector_] == (CYG_ADDRESS)hal_default_isr ) \
    {                                                                   \
        hal_interrupt_handlers[_vector_] = (CYG_ADDRESS)_isr_;          \
        hal_interrupt_data[_vector_] = (CYG_ADDRWORD) _data_;           \
        hal_interrupt_objects[_vector_] = (CYG_ADDRESS)_object_;        \
    }                                                                   \
}

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
{                                                                       \
    if( hal_interrupt_handlers[_vector_] == (CYG_ADDRESS)_isr_ )        \
    {                                                                   \
        hal_interrupt_handlers[_vector_] = (CYG_ADDRESS)hal_default_isr; \
        hal_interrupt_data[_vector_] = 0;                               \
        hal_interrupt_objects[_vector_] = 0;                            \
    }                                                                   \
}

//--------------------------------------------------------------------------
// CPU interrupt control.
//
// We use the BASEPRI register to control delivery of interrupts. The
// register is set to the second highest implemented priority for this
// Cortex-M implementation to mask interrupts. It is set to zero to
// enable interrupts, which will disable the BASEPRI mechanism.

#ifndef __ASSEMBLER__
typedef cyg_uint32  CYG_INTERRUPT_STATE;
#endif

#ifndef HAL_DISABLE_INTERRUPTS
# define HAL_DISABLE_INTERRUPTS(__old)          \
    __asm__ volatile (                          \
        "mrs    %0, basepri             \n"     \
        "mov    r1,%1                   \n"     \
        "msr    basepri,r1              \n"     \
        : "=r" (__old)                          \
        :  "r" (CYGNUM_HAL_CORTEXM_PRIORITY_MAX)\
        : "r1"                                  \
        );
#endif

#ifndef HAL_RESTORE_INTERRUPTS
# define HAL_RESTORE_INTERRUPTS(__old)          \
    __asm__ volatile (                          \
        "msr    basepri, %0             \n"     \
        :                                       \
        : "r" (__old)                           \
        );
#endif

#ifndef HAL_ENABLE_INTERRUPTS
# define HAL_ENABLE_INTERRUPTS()                \
    __asm__ volatile (                          \
        "mov    r1,#0                   \n"     \
        "msr    basepri,r1              \n"     \
        :                                       \
        :                                       \
        : "r1"                                  \
        );
#endif

#ifndef HAL_QUERY_INTERRUPTS
#define HAL_QUERY_INTERRUPTS(__state)           \
    __asm__ volatile (                          \
        "mrs    %0, basepri             \n"     \
        : "=r" (__state)                        \
        );
#endif

//--------------------------------------------------------------------------
// Interrupt masking and unmasking
//
// This is mostly done via the architecture defined NVIC. The
// HAL_VAR_*() macros allow the variant HAL to provide extended
// support for additional interrupt sources supported by supplementary
// interrupt controllers.

__externC void hal_interrupt_mask( cyg_uint32 vector );
__externC void hal_interrupt_unmask( cyg_uint32 vector );
__externC void hal_interrupt_set_level( cyg_uint32 vector, cyg_uint32 level );
__externC void hal_interrupt_acknowledge( cyg_uint32 vector );
__externC void hal_interrupt_configure( cyg_uint32 vector, cyg_uint32 level, cyg_uint32 up );


#define HAL_INTERRUPT_MASK( __vector ) hal_interrupt_mask( __vector )
#define HAL_INTERRUPT_UNMASK( __vector ) hal_interrupt_unmask( __vector )
#define HAL_INTERRUPT_SET_LEVEL( __vector, __level ) hal_interrupt_set_level( __vector, __level )
#define HAL_INTERRUPT_ACKNOWLEDGE( __vector ) hal_interrupt_acknowledge( __vector )
#define HAL_INTERRUPT_CONFIGURE( __vector, __level, __up ) hal_interrupt_configure( __vector, __level, __up )

//--------------------------------------------------------------------------
// Routine to execute DSRs using separate interrupt stack

__externC void hal_call_dsrs_vsr(void);
#define HAL_INTERRUPT_STACK_CALL_PENDING_DSRS()         \
{                                                       \
    __asm__ volatile (                                  \
        "ldr     r3,=hal_call_dsrs_vsr          \n"     \
        "swi 0                                  \n"     \
        :                                               \
        :                                               \
        : "r3"                                          \
        );                                              \
}

//--------------------------------------------------------------------------

#if 0
// these are offered solely for stack usage testing
// if they are not defined, then there is no interrupt stack.
#define HAL_INTERRUPT_STACK_BASE cyg_interrupt_stack_base
#define HAL_INTERRUPT_STACK_TOP  cyg_interrupt_stack
// use them to declare these extern however you want:
//       extern char HAL_INTERRUPT_STACK_BASE[];
//       extern char HAL_INTERRUPT_STACK_TOP[];
// is recommended
#endif

//==========================================================================
// Clock control
//
// This uses the CPU SysTick timer. Variant or platform allowed to override
// these definitions

#ifndef CYGHWR_HAL_CLOCK_DEFINED

__externC cyg_uint32 hal_cortexm_systick_clock;

// Select the clock source of the system tick timer
#ifdef CYGHWR_HAL_CORTEXM_SYSTICK_CLK_SOURCE_EXTERNAL
 #define CYGARC_REG_SYSTICK_CSR_CLK_SRC CYGARC_REG_SYSTICK_CSR_CLK_EXT
#elif defined(CYGHWR_HAL_CORTEXM_SYSTICK_CLK_SOURCE_INTERNAL)
 #define CYGARC_REG_SYSTICK_CSR_CLK_SRC CYGARC_REG_SYSTICK_CSR_CLK_INT
#endif

#define HAL_CLOCK_INITIALIZE( __period )                                \
{                                                                       \
    cyg_uint32 __p = __period;                                          \
    __p = hal_cortexm_systick_clock / ( 1000000 / __p ) - 1;            \
    HAL_WRITE_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_RELOAD, \
                     __p );                                             \
    HAL_WRITE_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR,    \
                     CYGARC_REG_SYSTICK_CSR_ENABLE      |               \
                     CYGARC_REG_SYSTICK_CSR_CLK_SRC     );              \
}

#define HAL_CLOCK_RESET( __vector, __period )                           \
{                                                                       \
    cyg_uint32 __csr;                                                   \
    HAL_READ_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_CSR, __csr ); \
}

#define HAL_CLOCK_READ( __pvalue )                                      \
{                                                                       \
    cyg_uint32 __period, __value;                                       \
    HAL_READ_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_RELOAD, __period ); \
    HAL_READ_UINT32(CYGARC_REG_SYSTICK_BASE+CYGARC_REG_SYSTICK_VALUE, __value ); \
    __value = ( __period + 1 ) - __value;                               \
    __value /= (hal_cortexm_systick_clock / 1000000 );                  \
    *(__pvalue) = __value;                                              \
}

#define HAL_CLOCK_LATENCY( __pvalue ) HAL_CLOCK_READ( __pvalue )

#endif // CYGHWR_HAL_CLOCK_DEFINED

//==========================================================================
// HAL_DELAY_US().
//

__externC void hal_delay_us( cyg_int32 us );
#define HAL_DELAY_US( __us ) hal_delay_us( __us )

//==========================================================================
// Reset.
//
// This uses the SYSRESETREQ bit in the Cortex-M3 NVIC.

#define HAL_PLATFORM_RESET()                                            \
{                                                                       \
    HAL_WRITE_UINT32(CYGARC_REG_NVIC_BASE+CYGARC_REG_NVIC_AIRCR,        \
                     CYGARC_REG_NVIC_AIRCR_KEY|                         \
                     CYGARC_REG_NVIC_AIRCR_SYSRESETREQ );               \
    for(;;);                                                            \
}

__externC void hal_reset_vsr( void );
#define HAL_PLATFORM_RESET_ENTRY &hal_reset_vsr

//==========================================================================
#endif //CYGONCE_HAL_INTR_H

