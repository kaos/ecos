#ifndef CYGONCE_HAL_IMP_INTR_H
#define CYGONCE_HAL_IMP_INTR_H

//==========================================================================
//
//      imp_intr.h
//
//      TX39 Interrupt and clock support
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Contributors: nickg, jskov,
//               gthomas, jlarmour
// Date:         1999-02-16
// Purpose:      TX39 Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for variants of the TX39 MIPS
//               architecture.
//              
// Usage:
//              #include <cyg/hal/imp_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/plf_intr.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

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

#define CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt controller access.

#ifndef CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#if defined(CYGPKG_HAL_MIPS_TX3904)

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

#define CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#else

#error Unspecified TX39 variant

#endif

#endif

//--------------------------------------------------------------------------
// Clock control

#ifndef CYGHWR_HAL_CLOCK_CONTROL_DEFINED

#if defined(CYGPKG_HAL_MIPS_TX3904)

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

// We need to sync and check the coprocessor 0 condition - this
// indicates whether data is present in the write buffer. We need to
// wait until the data to be written is flushed out. This works
// around a tx39 bug.  gcc will insert a NOP after the asm insns.

# define HAL_CLOCK_READ( _pvalue_ )                                     \
CYG_MACRO_START                                                         \
    asm volatile (                                                      \
        "sync; nop; 1: ; bc0f 1b"                                       \
        :                                                               \
        :                                                               \
        : "$0"                                                          \
        );                                                              \
     HAL_READ_UINT32( CYG_HAL_MIPS_TX3904_TIMER_RR, *(_pvalue_) );      \
CYG_MACRO_END

#else

# define HAL_CLOCK_READ( _pvalue_ )                                     \
CYG_MACRO_START                                                         \
    HAL_READ_UINT32( CYG_HAL_MIPS_TX3904_TIMER_RR, *(_pvalue_) );       \
CYG_MACRO_END

#endif

#if defined(CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY) && \
    !defined(HAL_CLOCK_LATENCY)
#define HAL_CLOCK_LATENCY( _pvalue_ ) HAL_CLOCK_READ(_pvalue_)
#endif

#define CYGHWR_HAL_CLOCK_CONTROL_DEFINED

#else

#error Unspecified TX39 variant

#endif

#endif

//--------------------------------------------------------------------------
// Timeout exception support. This is TX39 specific.

#if defined(CYGPKG_HAL_MIPS_TX3904)

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
#endif // ifndef CYGONCE_HAL_IMP_INTR_H
// End of imp_intr.h
