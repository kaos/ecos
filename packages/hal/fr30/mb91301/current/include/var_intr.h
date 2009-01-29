#ifndef CYGONCE_HAL_IMP_INTR_H
#define CYGONCE_HAL_IMP_INTR_H

//==========================================================================
//
//      var_intr.h
//
//      MB91301 Interrupt and clock support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):    larsi
// Contributors: larsi
// Date:         2006-07-14
// Purpose:      MB91301 Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for variants of the MB91301
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

#include <cyg/hal/hal_io.h>

#include <cyg/hal/plf_intr.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#define CYGNUM_HAL_INTERRUPT_0                     16
#define CYGNUM_HAL_INTERRUPT_1                     17
#define CYGNUM_HAL_INTERRUPT_2                     18
#define CYGNUM_HAL_INTERRUPT_3                     19
#define CYGNUM_HAL_INTERRUPT_4                     20
#define CYGNUM_HAL_INTERRUPT_5                     21
#define CYGNUM_HAL_INTERRUPT_6                     22
#define CYGNUM_HAL_INTERRUPT_7                     23
#define CYGNUM_HAL_INTERRUPT_RELOAD_TIMER0         24
#define CYGNUM_HAL_INTERRUPT_RELOAD_TIMER1         25
#define CYGNUM_HAL_INTERRUPT_RELOAD_TIMER2         26
#define CYGNUM_HAL_INTERRUPT_UART0_RX              27
#define CYGNUM_HAL_INTERRUPT_UART1_RX              28
#define CYGNUM_HAL_INTERRUPT_UART2_RX              29
#define CYGNUM_HAL_INTERRUPT_UART0_TX              30
#define CYGNUM_HAL_INTERRUPT_UART1_TX              31
#define CYGNUM_HAL_INTERRUPT_UART2_TX              32
#define CYGNUM_HAL_INTERRUPT_DMAC0                 33
#define CYGNUM_HAL_INTERRUPT_DMAC1                 34
#define CYGNUM_HAL_INTERRUPT_DMAC2                 35
#define CYGNUM_HAL_INTERRUPT_DMAC3                 36
#define CYGNUM_HAL_INTERRUPT_DMAC4                 37
#define CYGNUM_HAL_INTERRUPT_AD                    38
#define CYGNUM_HAL_INTERRUPT_PPG0                  39
#define CYGNUM_HAL_INTERRUPT_PPG1                  40
#define CYGNUM_HAL_INTERRUPT_PPG2                  41
#define CYGNUM_HAL_INTERRUPT_PPG3                  42
// system reserved #define CYGNUM_HAL_INTERRUPT_                43
#define CYGNUM_HAL_INTERRUPT_UTIMER0               44
#define CYGNUM_HAL_INTERRUPT_UTIMER1               45
#define CYGNUM_HAL_INTERRUPT_UTIMER2               46
#define CYGNUM_HAL_INTERRUPT_TIMEBASE_OVERFLOW     47
#define CYGNUM_HAL_INTERRUPT_I2C0                  48
#define CYGNUM_HAL_INTERRUPT_I2C1                  49
// system reserved #define CYGNUM_HAL_INTERRUPT_                 50
// system reserved #define CYGNUM_HAL_INTERRUPT_                 51
#define CYGNUM_HAL_INTERRUPT_FREERUN_TIMER         52
#define CYGNUM_HAL_INTERRUPT_ICU0                  53
#define CYGNUM_HAL_INTERRUPT_ICU1                  54
#define CYGNUM_HAL_INTERRUPT_ICU2                  55
#define CYGNUM_HAL_INTERRUPT_ICU3                  56
// system reserved #define CYGNUM_HAL_INTERRUPT_             57
// system reserved #define CYGNUM_HAL_INTERRUPT_             58
// system reserved #define CYGNUM_HAL_INTERRUPT_             59
// system reserved #define CYGNUM_HAL_INTERRUPT_             60
// system reserved #define CYGNUM_HAL_INTERRUPT_             61
// system reserved #define CYGNUM_HAL_INTERRUPT_             62
#define CYGNUM_HAL_INTERRUPT_DELAYED_IRQ           63

// The interrupt vector used by the RTC, aka tick timer
#define CYGNUM_HAL_INTERRUPT_RTC            CYGNUM_HAL_INTERRUPT_RELOAD_TIMER1

#define CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt controller access.

// currently only external interrupts are masked using the external
// interrupt controller. This means only vectors 16 to 23 are valid.
// Other interrupts may be masked in the future
// using the mask mechanism for interrupt levels, if needed.

#ifndef CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#define CYG_HAL_FR30_MB91301_ENIR     0x41
#define CYG_HAL_FR30_MB91301_EIRR     0x40
#define CYG_HAL_FR30_MB91301_ELVR     0x42
#define CYG_HAL_FR30_MB91301_ICR00    0x440

// Array which stores the configured priority levels for the configured
// interrupts.
// this will be useful, if we implement masking of non external interrupts
// externC volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];


#define HAL_INTERRUPT_MASK( _vector_ )                       \
{                                                            \
    CYG_WORD32 _ilr_;                                        \
    if ((_vector_ >= CYGNUM_HAL_INTERRUPT_7) &&              \
          (_vector_ <= CYGNUM_HAL_INTERRUPT_0)){             \
        HAL_READ_UINT8( CYG_HAL_FR30_MB91301_ENIR, _ilr_ );  \
        _ilr_ &= ~(1<<((_vector_)>>4));                      \
        HAL_WRITE_UINT8( CYG_HAL_FR30_MB91301_ENIR, _ilr_ ); \
    }                                                        \
    /* Handle RTC masking special */                         \
    if (_vector_ == CYGNUM_HAL_INTERRUPT_RTC)                \
        asm volatile("ldi:8     #0x57,  r0;\n"               \
                     "bandl     #0x7,   @r0;\n"              \
                      : : :"r0");                            \
}

#define HAL_INTERRUPT_UNMASK( _vector_ )                     \
{                                                            \
    CYG_WORD32 _ilr_;                                        \
    if ((_vector_ >= CYGNUM_HAL_INTERRUPT_7) &&              \
          (_vector_ <= CYGNUM_HAL_INTERRUPT_0)){             \
        HAL_READ_UINT8( CYG_HAL_FR30_MB91301_ENIR, _ilr_ );  \
        _ilr_ |= (1<<((_vector_)>>4));                       \
        HAL_WRITE_UINT8( CYG_HAL_FR30_MB91301_ENIR, _ilr_ ); \
    }                                                        \
    /* Handle RTC unmasking special */                       \
    if (_vector_ == CYGNUM_HAL_INTERRUPT_RTC)                \
        asm volatile("ldi:8     #0x57,  r0;\n"               \
                     "borl      #0x8,   @r0;\n"              \
                      : : :"r0");                            \
}

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                \
{                                                            \
    CYG_WORD32 _ilr_;                                        \
    if ((_vector_ >= CYGNUM_HAL_INTERRUPT_7) &&              \
          (_vector_ <= CYGNUM_HAL_INTERRUPT_0)){             \
        HAL_READ_UINT8( CYG_HAL_FR30_MB91301_EIRR, _ilr_ );  \
        _ilr_ &= ~(1<<((_vector_)>>4));                      \
        HAL_WRITE_UINT8( CYG_HAL_FR30_MB91301_EIRR, _ilr_ ); \
    }                                                        \
    /* Handle RTC acknowledging special */                   \
    if (_vector_ == CYGNUM_HAL_INTERRUPT_RTC)                \
        asm volatile("ldi:8     #0x57,  r0;\n"               \
                     "bandl     #0xb,   @r0;\n"              \
                      : : :"r0");                            \
}

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )              \
{                                                                       \
        /* subtract 15 from vector */                                   \
        cyg_uint32 _v_ = _vector_ >> 4;                                 \
        cyg_uint16 _val_ = 0;                                           \
        cyg_uint16 _reg_;                                               \
                                                                        \
            if ((_vector_ >= CYGNUM_HAL_INTERRUPT_7) &&                 \
                (_vector_ <= CYGNUM_HAL_INTERRUPT_0)){                  \
                                                                        \
        /* set bits according to requirements */                        \
        if( _up_ ) _val_ |= 1;                                          \
        if( !(_level_) ) _val_ |= 2;                                    \
                                                                        \
        /* get old ELVR */                                              \
        HAL_READ_UINT16( CYG_HAL_FR30_MB91301_ELVR, _reg_ );            \
                                                                        \
        /* clear old value and set new */                               \
        _reg_ &= ~(3 << _v_);                                           \
        _reg_ |= _val_ << _v_;                                          \
        HAL_WRITE_UINT16( CYG_HAL_FR30_MB91301_ELVR, _reg_ );           \
    }                                                                   \
}

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )                    \
{                                                                       \
    /* subtract 15 from vector */                                       \
    cyg_uint32 _v_ = _vector_ >> 4;                                     \
    CYG_WORD32 _ilr_;                                                   \
/*    HAL_READ_UINT8( CYG_HAL_FR30_MB91301_ICR00 + _vector_, _ilr_ );*/ \
/* reading before writing is only needed, if UINT8 writing is not */    \
/* possible to IO 0x440   */                                              \
    _ilr_ = (_level_);                                                  \
    HAL_WRITE_UINT8( CYG_HAL_FR30_MB91301_ICR00 + _vector_, _ilr_ );    \
/*  for later use:  */                                                    \
/*    hal_interrupt_level[_vector_] = _level_;      */                    \
}

#define CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#endif

//--------------------------------------------------------------------------
// Clock control registers

// MB91301 series has 3 built-in timer channels.
// Timer 2 is used for delay and timer 1 for RTC/*delay*/.
// Timer 0 and 1 can activate DMA and this feature is propably needed by
// the application. Timer 0 is free to use by the application.

#define CYG_HAL_FR30_DLY_TMCSR          0x5e
#define CYG_HAL_FR30_DLY_TMR            0x5a
#define CYG_HAL_FR30_DLY_TMRLR          0x58
#define CYG_HAL_FR30_RTC_TMCSR          0x56
#define CYG_HAL_FR30_RTC_TMR            0x52
#define CYG_HAL_FR30_RTC_TMRLR          0x50



//--------------------------------------------------------------------------
// Control-C support.

#if defined(CYGDBG_HAL_FR30_DEBUG_GDB_CTRLC_SUPPORT)

#define CYGHWR_HAL_GDB_PORT_VECTOR CYGNUM_HAL_INTERRUPT_DUART

externC cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

#define HAL_CTRLC_ISR hal_ctrlc_isr

#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_IMP_INTR_H
// End of imp_intr.h
