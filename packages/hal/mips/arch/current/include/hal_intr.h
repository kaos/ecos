#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//=============================================================================
//
//	hal_intr.h
//
//	HAL Interrupt and clock support
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
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-02-17
// Purpose:	Define Interrupt support
// Description:	The macros defined here provide the HAL APIs for handling
//              interrupts and the clock.
//              
// Usage:
//		#include <cyg/hal/hal_intr.h>
//		...
//		
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// MIPS exception vectors. 

// Exception vectors.
// These are the exception codes presented in the
// Cause register and correspond to VSRs.

#define CYG_VECTOR_INTERRUPT            0
#define CYG_VECTOR_TLB_MOD              1
#define CYG_VECTOR_TLB_LOAD_REFILL      2
#define CYG_VECTOR_TLB_STORE_REFILL     3
#define CYG_VECTOR_LOAD_ADDRESS         4
#define CYG_VECTOR_STORE_ADDRESS        5
#define CYG_VECTOR_IBE                  6
#define CYG_VECTOR_DBE                  7
#define CYG_VECTOR_SYSTEM_CALL          8
#define CYG_VECTOR_BREAKPOINT           9
#define CYG_VECTOR_RESERVED_INSTRUCTION 10
#define CYG_VECTOR_COPROCESSOR          11
#define CYG_VECTOR_OVERFLOW             12
#define CYG_VECTOR_RESERVED_13          13

#define CYG_VSR_MIN                     0
#define CYG_VSR_MAX                     13
#define CYG_VSR_COUNT                   14

#define CYG_EXCEPTION_MIN                1
#define CYG_EXCEPTION_MAX               13
#define CYG_EXCEPTION_COUNT             14

// Interrupt vectors.

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

// These are decoded via the IP bits of the cause
// register when an external interrupt is delivered.

#define CYG_VECTOR_INT_1                0
#define CYG_VECTOR_INT_2                1
#define CYG_VECTOR_INT_3                2
#define CYG_VECTOR_INT_4                3
#define CYG_VECTOR_INT_5                4
#define CYG_VECTOR_INT_6                5
#define CYG_VECTOR_INT_7                6
#define CYG_VECTOR_DMAC1_CH3            7
#define CYG_VECTOR_DMAC1_CH2            8
#define CYG_VECTOR_DMAC0_CH1            9
#define CYG_VECTOR_DMAC0_CH0            10
#define CYG_VECTOR_SIO_0                11
#define CYG_VECTOR_SIO_1                12
#define CYG_VECTOR_TMR_0                13
#define CYG_VECTOR_TMR_1                14
#define CYG_VECTOR_TMR_2                15
#define CYG_VECTOR_INT_0                16

#define CYG_ISR_MIN                     0
#define CYG_ISR_MAX                     16
#define CYG_ISR_COUNT                   17

// The vector used by the Real time clock
#define CYG_VECTOR_RTC                  CYG_VECTOR_TMR_0

#endif

//-----------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYG_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYG_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYG_ISR_COUNT];        

// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYG_VSR_MAX+1];

//-----------------------------------------------------------------------------
// Default ISR

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

//-----------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//-----------------------------------------------------------------------------
// Interrupt control macros
// Beware of the nops in this code. These fill load/store delay slots to prevent
// following code being run in the wrong state.

#define HAL_DISABLE_INTERRUPTS(_old_)           \
{                                               \
    asm volatile (                              \
        "mfc0   $8,$12; nop;"                   \
        "move   %0,$8;"                         \
        "and    $8,$8,0XFFFFFFFE;"              \
        "mtc0   $8,$12;"                        \
        "nop; nop; nop;"                        \
        "and    %0,%0,0X1;"                     \
        : "=r"(_old_)                           \
        :                                       \
        : "$8"                                  \
        );                                      \
}

#define HAL_ENABLE_INTERRUPTS()                 \
{                                               \
    asm volatile (                              \
        "mfc0   $8,$12; nop;"                   \
        "or     $8,$8,1;"                       \
        "mtc0   $8,$12;"                        \
        "nop; nop; nop;"                        \
        :                                       \
        :                                       \
        : "$8"                                  \
        );                                      \
}

#define HAL_RESTORE_INTERRUPTS(_old_)           \
{                                               \
    asm volatile (                              \
        "mfc0   $8,$12; nop;"                   \
        "and    %0,%0,0x1;"                     \
        "or     $8,$8,%0;"                      \
        "mtc0   $8,$12;"                        \
        "nop; nop; nop;"                        \
        :                                       \
        : "r"(_old_)                            \
        : "$8"                                  \
        );                                      \
}

#define HAL_QUERY_INTERRUPTS( _state_ )         \
{                                               \
    asm volatile (                              \
        "mfc0   %0,$12; nop;"                   \
        "and    %0,%0,0x1;"                     \
        : "=r"(_state_)                         \
        :                                       \
        : "$8"                                  \
        );                                      \
}

//-----------------------------------------------------------------------------
// Vector translation.
// For chained interrupts we only have a single vector though which all
// are passed. For unchained interrupts we have a vector per interrupt.

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = 0

#else

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)

#endif

//-----------------------------------------------------------------------------
// Interrupt and VSR attachment macros

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )               \
{                                                                               \
    cyg_uint32 _index_;                                                         \
    HAL_TRANSLATE_VECTOR( _vector_, _index_ );                                  \
                                                                                \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr )       \
    {                                                                           \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;                   \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD)_data_;                     \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;                 \
    }                                                                           \
}

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
{                                                                       \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR( _vector_, _index_ );                          \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )         \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)hal_default_isr; \
        hal_interrupt_data[_index_] = 0;                                \
        hal_interrupt_objects[_index_] = 0;                             \
    }                                                                   \
}

#define HAL_VSR_GET( _vector_, _pvsr_ )                 \
    *(_pvsr_) = (void (*)())hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )                               \
    if( _poldvsr_ != NULL ) *_poldvsr_ = (void (*)())hal_vsr_table[_vector_];   \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;

//-----------------------------------------------------------------------------
// Interrupt controller access

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

#define TX3904_ILR0     0xFFFFC010
#define TX3904_CConR    0xFFFFE000

// Array which stores the configured priority levels for the configured
// interrupts.
externC volatile CYG_BYTE hal_interrupt_level[CYG_ISR_COUNT];


#define HAL_INTERRUPT_MASK( _vector_ )          \
{                                               \
    HAL_IO_REGISTER _reg_ = TX3904_ILR0;        \
    CYG_WORD32 _ilr_;                           \
    _reg_ += (_vector_)&0xC;                    \
    HAL_READ_UINT32( _reg_, _ilr_ );            \
    _ilr_ &= ~(7 << (((_vector_)&0x3)<<3));     \
    HAL_WRITE_UINT32( _reg_, _ilr_ );           \
}

#define HAL_INTERRUPT_UNMASK( _vector_ )                                \
{                                                                       \
    HAL_IO_REGISTER _reg_ = TX3904_ILR0;                                \
    CYG_WORD32 _ilr_;                                                   \
    _reg_ += (_vector_)&0xC;                                            \
    HAL_READ_UINT32( _reg_, _ilr_ );                                    \
    _ilr_ |= hal_interrupt_level[_vector_] << (((_vector_)&0x3)<<3);    \
    HAL_WRITE_UINT32( _reg_, _ilr_ );                                   \
}

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                           \
{                                                                       \
    if( _vector_ <= CYG_VECTOR_INT_7 || _vector_ == CYG_VECTOR_INT_0 )  \
    {                                                                   \
        cyg_uint32 _val_ = _vector_ + 1;                                \
        if( _val_ == CYG_VECTOR_INT_0 + 1 ) _val_ = 0;                  \
        HAL_WRITE_UINT8( TX3904_CConR+1, _val_);                        \
    }                                                                   \
}

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )              \
{                                                                       \
    if( _vector_ <= CYG_VECTOR_INT_7 || _vector_ == CYG_VECTOR_INT_0 )  \
    {                                                                   \
        cyg_uint32 _val_ = 0;                                           \
        if( _up_ ) _val_ |= 1;                                          \
        if( !(_level_) ) _val_ |= 2;                                    \
        HAL_WRITE_UINT16( TX3904_CConR+2, _val_ );                      \
    }                                                                   \
}

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )    \
{                                                       \
    HAL_IO_REGISTER _reg_ = TX3904_ILR0;                \
    CYG_WORD32 _ilr_;                                   \
    _reg_ += (_vector_)&0xC;                            \
    HAL_READ_UINT32( _reg_, _ilr_ );                    \
    _ilr_ |= (_level_) << (((_vector_)&0x3)<<3);        \
    HAL_WRITE_UINT32( _reg_, _ilr_ );                   \
    hal_interrupt_level[_vector_] = _level_;            \
}

#else

#error Unspecified platform for MIPS

#endif

//-----------------------------------------------------------------------------
// Clock control

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

#define TX3904_TIMER_BASE 0xFFFFF000
#define TX3904_TIMER_CR   (TX3904_TIMER_BASE + 0x00)
#define TX3904_TIMER_SR   (TX3904_TIMER_BASE + 0x04)
#define TX3904_TIMER_CPR  (TX3904_TIMER_BASE + 0x08)
#define TX3904_TIMER_IMR  (TX3904_TIMER_BASE + 0x10)
#define TX3904_TIMER_DR   (TX3904_TIMER_BASE + 0x20)
#define TX3904_TIMER_RR   (TX3904_TIMER_BASE + 0xF0)

#define HAL_CLOCK_INITIALIZE( _period_ )                \
{                                                       \
    HAL_WRITE_UINT32( TX3904_TIMER_DR, 0x000000003 );   \
    HAL_WRITE_UINT32( TX3904_TIMER_CPR, _period_ );     \
    HAL_WRITE_UINT32( TX3904_TIMER_SR, 0x00000000 );    \
    HAL_WRITE_UINT32( TX3904_TIMER_IMR, 0x00008001 );   \
    HAL_WRITE_UINT32( TX3904_TIMER_CR, 0x000000C0 );    \
}

#define HAL_CLOCK_RESET( _vector_, _period_ )           \
{                                                       \
    HAL_WRITE_UINT32( TX3904_TIMER_SR, 0x00000000 );    \
}

#define HAL_CLOCK_READ( _pvalue_ )                      \
{                                                       \
    HAL_READ_UINT32( TX3904_TIMER_RR, *(_pvalue_) );    \
}

#else

#error Unspecified platform for MIPS

#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// End of hal_intr.h
