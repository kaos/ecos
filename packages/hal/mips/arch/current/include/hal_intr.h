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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jskov,
//               gthomas, jlarmour
// Date:         1999-02-16
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//              #include <cyg/hal/hal_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>

//--------------------------------------------------------------------------
// MIPS vectors. 

// These are the exception codes presented in the Cause register and
// correspond to VSRs. These values are the ones to use for HAL_VSR_GET/SET

// External interrupt
#define CYGNUM_HAL_VECTOR_INTERRUPT            0
// TLB modification exception
#define CYGNUM_HAL_VECTOR_TLB_MOD              1
// TLB miss (Load or IFetch)
#define CYGNUM_HAL_VECTOR_TLB_LOAD_REFILL      2
// TLB miss (Store)
#define CYGNUM_HAL_VECTOR_TLB_STORE_REFILL     3
// Address error (Load or Ifetch)
#define CYGNUM_HAL_VECTOR_LOAD_ADDRESS         4
// Address error (store)
#define CYGNUM_HAL_VECTOR_STORE_ADDRESS        5
// Bus error (Ifetch)
#define CYGNUM_HAL_VECTOR_IBE                  6
// Bus error (data load or store)
#define CYGNUM_HAL_VECTOR_DBE                  7
// System call
#define CYGNUM_HAL_VECTOR_SYSTEM_CALL          8
// Break point
#define CYGNUM_HAL_VECTOR_BREAKPOINT           9
// Reserved instruction
#define CYGNUM_HAL_VECTOR_RESERVED_INSTRUCTION 10
// Coprocessor unusable
#define CYGNUM_HAL_VECTOR_COPROCESSOR          11
// Arithmetic overflow
#define CYGNUM_HAL_VECTOR_OVERFLOW             12
// Reserved
#define CYGNUM_HAL_VECTOR_RESERVED_13          13
// Floating point exception - not applicable yet
// #define CYGNUM_HAL_VECTOR_FPE                  15

#define CYGNUM_HAL_VSR_MIN                     0
#define CYGNUM_HAL_VSR_MAX                     13
#define CYGNUM_HAL_VSR_COUNT                   14

// Exception vectors. These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception()

#define CYGNUM_HAL_EXCEPTION_DATA_TLBERROR_ACCESS CYGNUM_HAL_VECTOR_TLB_MOD
#define CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_ACCESS \
          CYGNUM_HAL_VECTOR_TLB_LOAD_REFILL
#define CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_WRITE \
          CYGNUM_HAL_VECTOR_TLB_STORE_REFILL
#define CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS \
          CYGNUM_HAL_VECTOR_LOAD_ADDRESS
#define CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_WRITE \
          CYGNUM_HAL_VECTOR_STORE_ADDRESS
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS    CYGNUM_HAL_VECTOR_IBE
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS    CYGNUM_HAL_VECTOR_DBE
#define CYGNUM_HAL_EXCEPTION_SYSTEM_CALL    CYGNUM_HAL_VECTOR_SYSTEM_CALL
#define CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP CYGNUM_HAL_VECTOR_BREAKPOINT
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION \
          CYGNUM_HAL_VECTOR_RESERVED_INSTRUCTION
#define CYGNUM_HAL_EXCEPTION_COPROCESSOR    CYGNUM_HAL_VECTOR_COPROCESSOR
#define CYGNUM_HAL_EXCEPTION_OVERFLOW       CYGNUM_HAL_VECTOR_OVERFLOW

// Min/Max exception numbers and how many there are
#define CYGNUM_HAL_EXCEPTION_MIN                1
#define CYGNUM_HAL_EXCEPTION_MAX               13
#define CYGNUM_HAL_EXCEPTION_COUNT             13

// Interrupt vectors.

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

// These are decoded via the IP bits of the cause
// register when an external interrupt is delivered.

#define CYGNUM_HAL_INTERRUPT_1                0
#define CYGNUM_HAL_INTERRUPT_2                1
#define CYGNUM_HAL_INTERRUPT_3                2
#define CYGNUM_HAL_INTERRUPT_4                3
#define CYGNUM_HAL_INTERRUPT_5                4
#define CYGNUM_HAL_INTERRUPT_6                5
#define CYGNUM_HAL_INTERRUPT_7                6
#define CYGNUM_HAL_INTERRUPT_DMAC1_CH3        7
#define CYGNUM_HAL_INTERRUPT_DMAC1_CH2        8
#define CYGNUM_HAL_INTERRUPT_DMAC0_CH1        9
#define CYGNUM_HAL_INTERRUPT_DMAC0_CH0        10
#define CYGNUM_HAL_INTERRUPT_SIO_0            11
#define CYGNUM_HAL_INTERRUPT_SIO_1            12
#define CYGNUM_HAL_INTERRUPT_TMR_0            13
#define CYGNUM_HAL_INTERRUPT_TMR_1            14
#define CYGNUM_HAL_INTERRUPT_TMR_2            15
#define CYGNUM_HAL_INTERRUPT_0                16

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     16
#define CYGNUM_HAL_ISR_COUNT                   17

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC            CYGNUM_HAL_INTERRUPT_TMR_0

#endif

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYGNUM_HAL_VSR_MAX+1];

//--------------------------------------------------------------------------
// Default ISR

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//--------------------------------------------------------------------------
// Interrupt control macros
// Beware of the nops in this code. These fill load/store delay slots to
// prevent following code being run in the wrong state.

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

//--------------------------------------------------------------------------
// Vector translation.
// For chained interrupts we only have a single vector though which all
// are passed. For unchained interrupts we have a vector per interrupt.

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = 0

#else

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)

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

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )           \
{                                                                           \
    cyg_uint32 _index_;                                                     \
    HAL_TRANSLATE_VECTOR( _vector_, _index_ );                              \
                                                                            \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr )   \
    {                                                                       \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;               \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD)_data_;                 \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;             \
    }                                                                       \
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
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ ) CYG_MACRO_START         \
    if( _poldvsr_ != NULL)                                                \
        *(CYG_ADDRESS *)_poldvsr_ = (CYG_ADDRESS)hal_vsr_table[_vector_]; \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;                         \
CYG_MACRO_END

// This is an ugly name, but what it means is: grab the VSR back to eCos
// internal handling, or if you like, the default handler.  But if
// cooperating with GDB and CygMon, the default behaviour is to pass most
// exceptions to CygMon.  This macro undoes that so that eCos handles the
// exception.  So use it with care.

externC void __default_exception_vsr(void);
externC void __default_interrupt_vsr(void);

#define HAL_VSR_SET_TO_ECOS_HANDLER( _vector_, _poldvsr_ ) CYG_MACRO_START  \
    HAL_VSR_SET( _vector_, _vector_ == CYGNUM_HAL_VECTOR_INTERRUPT          \
                              ? (CYG_ADDRESS)__default_interrupt_vsr        \
                              : (CYG_ADDRESS)__default_exception_vsr,       \
                 _poldvsr_ );                                               \
CYG_MACRO_END

//--------------------------------------------------------------------------
// Interrupt controller access

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

#define CYG_HAL_MIPS_TX3904_ILR0     0xFFFFC010
#define CYG_HAL_MIPS_TX3904_CConR    0xFFFFE000

// Array which stores the configured priority levels for the configured
// interrupts.
externC volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];


#define HAL_INTERRUPT_MASK( _vector_ )                       \
{                                                            \
    HAL_IO_REGISTER _reg_ = CYG_HAL_MIPS_TX3904_ILR0;        \
    CYG_WORD32 _ilr_;                                        \
    _reg_ += (_vector_)&0xC;                                 \
    HAL_READ_UINT32( _reg_, _ilr_ );                         \
    _ilr_ &= ~(7 << (((_vector_)&0x3)<<3));                  \
    HAL_WRITE_UINT32( _reg_, _ilr_ );                        \
}

#define HAL_INTERRUPT_UNMASK( _vector_ )                                \
{                                                                       \
    HAL_IO_REGISTER _reg_ = CYG_HAL_MIPS_TX3904_ILR0;                   \
    CYG_WORD32 _ilr_;                                                   \
    _reg_ += (_vector_)&0xC;                                            \
    HAL_READ_UINT32( _reg_, _ilr_ );                                    \
    _ilr_ |= hal_interrupt_level[_vector_] << (((_vector_)&0x3)<<3);    \
    HAL_WRITE_UINT32( _reg_, _ilr_ );                                   \
}

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                           \
{                                                                       \
    if( _vector_ <= CYGNUM_HAL_INTERRUPT_7 ||                           \
        _vector_ == CYGNUM_HAL_INTERRUPT_0 )                            \
    {                                                                   \
        cyg_uint32 _val_ = _vector_ + 1;                                \
        if( _val_ == CYGNUM_HAL_INTERRUPT_0 + 1 ) _val_ = 0;            \
        HAL_WRITE_UINT8( CYG_HAL_MIPS_TX3904_CConR+1, _val_);           \
    }                                                                   \
}

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )              \
{                                                                       \
    if( _vector_ <= CYGNUM_HAL_INTERRUPT_7 ||                           \
        _vector_ == CYGNUM_HAL_INTERRUPT_0 )                            \
    {                                                                   \
        cyg_uint32 _val_ = 0;                                           \
        if( _up_ ) _val_ |= 1;                                          \
        if( !(_level_) ) _val_ |= 2;                                    \
        HAL_WRITE_UINT16( CYG_HAL_MIPS_TX3904_CConR+2, _val_ );         \
    }                                                                   \
}

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )    \
{                                                       \
    HAL_IO_REGISTER _reg_ = CYG_HAL_MIPS_TX3904_ILR0;   \
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

//--------------------------------------------------------------------------
// Clock control

#if defined(CYG_HAL_MIPS_TX3904) || defined(CYG_HAL_MIPS_SIM)

#define CYG_HAL_MIPS_TX3904_TIMER_BASE 0xFFFFF000
#define CYG_HAL_MIPS_TX3904_TIMER_CR  (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0x00)
#define CYG_HAL_MIPS_TX3904_TIMER_SR  (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0x04)
#define CYG_HAL_MIPS_TX3904_TIMER_CPR (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0x08)
#define CYG_HAL_MIPS_TX3904_TIMER_IMR (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0x10)
#define CYG_HAL_MIPS_TX3904_TIMER_DR  (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0x20)
#define CYG_HAL_MIPS_TX3904_TIMER_RR  (CYG_HAL_MIPS_TX3904_TIMER_BASE + 0xF0)

#define HAL_CLOCK_INITIALIZE( _period_ )                             \
{                                                                    \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_DR, 0x00000003 );    \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_CPR, _period_ );     \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_SR, 0x00000000 );    \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_IMR, 0x00008001 );   \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_CR, 0x000000C0 );    \
}

#define HAL_CLOCK_RESET( _vector_, _period_ )                        \
{                                                                    \
    HAL_WRITE_UINT32( CYG_HAL_MIPS_TX3904_TIMER_SR, 0x00000000 );    \
}

#if defined(CYGHWR_HAL_MIPS_TX3904_TRR_REQUIRES_SYNC) && \
    !defined(CYG_HAL_MIPS_SIM)

// We need to sync and check the coprocessor 0 condition - this indicates
// whether data is present in the write buffer. We need to wait until
// that data to be written is flushed out. This works around a tx39 bug.
// gcc will insert a NOP after the asm insns.

# define HAL_CLOCK_READ( _pvalue_ )                                  \
CYG_MACRO_START                                                      \
    asm volatile (                                                   \
        "sync; nop; 1: ; bc0f 1b"                                    \
        :                                                            \
        :                                                            \
        : "$0"                                                       \
        );                                                           \
    HAL_READ_UINT32( CYG_HAL_MIPS_TX3904_TIMER_RR, *(_pvalue_) );    \
CYG_MACRO_END

#else

# define HAL_CLOCK_READ( _pvalue_ )                                  \
CYG_MACRO_START                                                      \
    HAL_READ_UINT32( CYG_HAL_MIPS_TX3904_TIMER_RR, *(_pvalue_) );    \
CYG_MACRO_END

#endif

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ ) HAL_CLOCK_READ(_pvalue_)
#endif

#else

#error Unspecified platform for MIPS

#endif

//--------------------------------------------------------------------------
// Timeout exception support. This is only enabled on the real TX39 hardware.

#if defined(CYG_HAL_MIPS_TX3904)

#define HAL_TX39_DEBUG_TOE_ENABLE()                     \
{                                                       \
    HAL_IO_REGISTER _reg_ = CYG_HAL_MIPS_TX3904_CConR;  \
    CYG_WORD32 _cconr_;                                 \
    HAL_READ_UINT32( _reg_, _cconr_);                   \
    _cconr_ |= 0x04000000;                              \
    HAL_WRITE_UINT32( _reg_, _cconr_);                  \
}

#define HAL_TX39_DEBUG_TOE_DISABLE()                    \
{                                                       \
    HAL_IO_REGISTER _reg_ = CYG_HAL_MIPS_TX3904_CConR;  \
    CYG_WORD32 _cconr_;                                 \
    HAL_READ_UINT32( _reg_, _cconr_);                   \
    _cconr_ &= 0xFBFFFFFF;                              \
    HAL_WRITE_UINT32( _reg_, _cconr_);                  \
}

#else

#define HAL_TX39_DEBUG_TOE_ENABLE()

#define HAL_TX39_DEBUG_TOE_DISABLE()

#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// End of hal_intr.h
