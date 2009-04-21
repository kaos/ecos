#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H
//==========================================================================
//
//      var_intr.h
//
//      mcfxxxx Variant interrupt and clock support
//
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2006, 2007, 2008 Free Software Foundation, Inc.      
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
//####DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
// 
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

// Include any processor specific interrupt definitions.
#include <cyg/hal/proc_intr.h>

// ColdFire defines some extra exceptions
// The debug vector is for hardware breakpoints. These are not used
// at present.
#define CYGNUM_HAL_VECTOR_DEBUG             12
#define CYGNUM_HAL_VECTOR_FORMAT            14

// The following implementation should suffice for most platforms. If
// any of them need special VSR's then they can define their own version.
#if !defined(HAL_VSR_SET_TO_ECOS_HANDLER) && !defined(_HAL_M68K_NO_VSR_SET_)
# define HAL_VSR_SET_TO_ECOS_HANDLER( _vector_, _poldvsr_)                  \
    CYG_MACRO_START                                                         \
    if ( (_vector_) <= CYGNUM_HAL_VECTOR_TRAPLAST) {                        \
        HAL_VSR_SET( (_vector_), &hal_m68k_exception_vsr, (_poldvsr_) );    \
    } else {                                                                \
        HAL_VSR_SET( (_vector_), &hal_m68k_interrupt_vsr, (_poldvsr_) );    \
    }                                                                       \
    CYG_MACRO_END
#endif

// ----------------------------------------------------------------------------
// On ColdFires with 5282-style interrupt controllers, many of the
// interrupt-related macros can be provided here rather than duplicated
// in the processor HALs. Of course there complications, for example there
// may be one or two interrupt controllers.

#ifdef HAL_MCFxxxx_HAS_MCF5282_INTC

# ifndef HAL_INTERRUPT_MASK
// Masking interrupts is straightforward: there are 32-bit read-write
// mask registers. Often the vector argument will be a constant so the
// compiler gets a chance to optimise these macros. There is also some
// compile-time optimization based around the maximum number of
// interrupt vectors. to avoid worrying about interrupts 32-63 if only
// the bottom half of INTC0 is of interest, or interrupts 96-127 if
// only the bottom half of INTC1 is of interest.
//
// Some newer ColdFires have additional set/clear mask registers.

#  ifdef HAL_MCFxxxx_INTCx_SIMR
#   if (1 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_MASK(_vector_)                                                \
     CYG_MACRO_START                                                                    \
     HAL_WRITE_UINT8(HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_SIMR, (_vector_));      \
     CYG_MACRO_END
#   elif (2 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_MASK(_vector_)                                                \
     CYG_MACRO_START                                                                    \
     cyg_uint32 _vec_ = (_vector_);                                                     \
     if (_vec_ < 64) {                                                                  \
         HAL_WRITE_UINT8(HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_SIMR, _vec_);       \
     } else {                                                                           \
         HAL_WRITE_UINT8(HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_SIMR, _vec_ - 64);  \
     }                                                                                  \
     CYG_MACRO_END
#   else
#    error At most two interrupt controllers supported at present.
#   endif
#  else // ! SIMR
#   if   (1 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_MASK(_vector_)                                    \
     CYG_MACRO_START                                                        \
     cyg_uint32  _shift_ = (_vector_);                                      \
     cyg_uint32  _reg_;                                                     \
     cyg_uint32  _bits_;                                                    \
     if ( (CYGNUM_HAL_ISR_MAX < 32) || (_shift_ < 32)) {                    \
         _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRL;         \
     } else {                                                               \
         _reg_    = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRH;        \
         _shift_ -= 32;                                                     \
     }                                                                      \
     HAL_READ_UINT32(_reg_, _bits_);                                        \
     _bits_ |= (0x01 << _shift_);                                           \
     HAL_WRITE_UINT32(_reg_, _bits_);                                       \
     CYG_MACRO_END
#   elif (2 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_MASK(_vector_)                                    \
     CYG_MACRO_START                                                        \
     cyg_uint32  _shift_ = (_vector_);                                      \
     cyg_uint32  _reg_;                                                     \
     cyg_uint32  _bits_;                                                    \
     if ( (CYGNUM_HAL_ISR_MAX < 32) || (_shift_ < 32)) {                    \
         _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRL;         \
     } else if ((CYGNUM_HAL_ISR_MAX < 64) || (_shift_ < 64)) {              \
         _reg_    = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRH;        \
         _shift_ -= 32;                                                     \
     } else if ((CYGNUM_HAL_ISR_MAX < 96) || (_shift_ < 96)) {              \
         _reg_    = HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_IMRL;        \
         _shift_ -= 64;                                                     \
     } else {                                                               \
         _reg_    = HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_IMRH;        \
         _shift_ -= 96;                                                     \
     }                                                                      \
     HAL_READ_UINT32(_reg_, _bits_);                                        \
     _bits_ |= (0x01 << _shift_);                                           \
     HAL_WRITE_UINT32(_reg_, _bits_);                                       \
     CYG_MACRO_END
#   else
#    error At most two interrupt controllers supported at present.
#   endif   // HAL_MCFxxxx_HAS_MCF5282_INTC == 1/2/many
#  endif    // SIMR
# endif     // !defined(HAL_INTERRUPT_MASK)

// Care has to be taken with bit 0 of the IMRL registers. Writing a 1 to
// this masks all interrupts.
# ifndef HAL_INTERRUPT_UNMASK
#  ifdef HAL_MCFxxxx_INTCx_CIMR
#   if (1 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_UNMASK(_vector_)                                              \
     CYG_MACRO_START                                                                    \
     HAL_WRITE_UINT8(HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_CIMR, (_vector_));      \
     CYG_MACRO_END
#   elif (2 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_UNMASK(_vector_)                                              \
     CYG_MACRO_START                                                                    \
     cyg_uint32 _vec_ = (_vector_);                                                     \
     if (_vec_ < 64) {                                                                  \
         HAL_WRITE_UINT8(HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_CIMR, _vec_);       \
     } else {                                                                           \
         HAL_WRITE_UINT8(HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_CIMR, _vec_ - 64);  \
     }                                                                                  \
     CYG_MACRO_END
#   else
#    error At most two interrupt controllers supported at present.
#   endif
#  else
#   if   (1 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_UNMASK(_vector_)                                  \
     CYG_MACRO_START                                                        \
     cyg_uint32  _vec_ = (_vector_);                                        \
     cyg_uint32  _reg_;                                                     \
     cyg_uint32  _bits_;                                                    \
     if ( (CYGNUM_HAL_ISR_MAX < 32) || (_vec_ < 32)) {                      \
         _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRL;         \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~((0x01 << _vec_) | 0x01);                               \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     } else {                                                               \
         _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRH;         \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~(0x01 << (_vec_ - 32));                                 \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     }                                                                      \
     CYG_MACRO_END
#   elif (2 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#    define HAL_INTERRUPT_UNMASK(_vector_)                                  \
     CYG_MACRO_START                                                        \
     cyg_uint32  _vec_ = (_vector_);                                        \
     cyg_uint32  _reg_;                                                     \
     cyg_uint32  _bits_;                                                    \
     if ( (CYGNUM_HAL_ISR_MAX < 32) || (_vec_ < 32)) {                      \
         _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRL;         \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~((0x01 << _vec_) | 0x01);                               \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     } else if ((CYGNUM_HAL_ISR_MAX < 64) || (_vec_ < 64)) {                \
         _reg_    = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_IMRH;        \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~(0x01 << (_vec_ - 32));                                 \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     } else if ((CYGNUM_HAL_ISR_MAX < 96) || (_vec_ < 96)) {                \
         _reg_    = HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_IMRL;        \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~((0x01 << (_vec_ - 64)) | 0x01);                        \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     } else {                                                               \
         _reg_    = HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_IMRH;        \
         HAL_READ_UINT32(_reg_, _bits_);                                    \
         _bits_ &= ~(0x01 << (_vec_ - 96));                                 \
         HAL_WRITE_UINT32(_reg_, _bits_);                                   \
     }                                                                      \
     CYG_MACRO_END
#   else
#    error At most two interrupt controllers supported at present.
#   endif    // HAL_MCFxxxx_HAS_MCF5282_INTC == 1/2/many
#  endif    // HAL_MCFxxxx_INTCx_CIMR
# endif     // !defined(HAL_INTERRUPT_UNMASK)

# ifndef HAL_INTERRUPT_SET_LEVEL
// Each interrupt vector has its own priority register. This consists
// of an M68K IPL level between 1 and 7, plus a priority within each
// level between 0 and 7, giving a total of 56 legal priorities. All
// active interrupt vectors within each interrupt vector must be given
// unique priorities, otherwise the system's behaviour is undefined
// (and can include the interrupt controller supplying the wrong
// interrupt vector in the IACK cycle).
#  if (1 == HAL_MCFxxxx_HAS_MCF5282_INTC)

#   define HAL_INTERRUPT_SET_LEVEL(_vector_, _prilevel_)                    \
    CYG_MACRO_START                                                         \
    cyg_uint32  _vec_   = (_vector_);                                       \
    cyg_uint32  _reg_;                                                      \
    _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_ICR00 + _vec_;     \
    HAL_WRITE_UINT8(_reg_, _prilevel_);                                     \
    CYG_MACRO_END

#  elif (2 == HAL_MCFxxxx_HAS_MCF5282_INTC)
#   define HAL_INTERRUPT_SET_LEVEL(_vector_, _prilevel_)                        \
    CYG_MACRO_START                                                             \
    cyg_uint32  _vec_   = (_vector_);                                           \
    cyg_uint32  _reg_;                                                          \
    if( _vec_ < 64 )                                                            \
        _reg_   = HAL_MCFxxxx_INTC0_BASE + HAL_MCFxxxx_INTCx_ICR00 + _vec_;     \
    else                                                                        \
        _reg_   = HAL_MCFxxxx_INTC1_BASE + HAL_MCFxxxx_INTCx_ICR00 + _vec_ - 64;\
    HAL_WRITE_UINT8(_reg_, _prilevel_);                                         \
    CYG_MACRO_END

#  else
#   error At most two interrupt controllers supported at present.
#  endif    // HAL_MCFxxxx_HAS_MCF5282_INTC == 1/2/many
# endif     // !defined(HAL_INTERRUPT_SET_LEVEL)
    
#endif  // HAL_MCFxxxx_HAS_MCF5282_INTC
    
#ifdef HAL_MCFxxxx_HAS_MCF5282_EPORT
// There is no acknowledgement support inside the interrupt
// controller. Instead each device has its own way of clearing the
// interrupt, so it is left to device drivers to clear interrupts at
// an appropriate time. The exception is for the edge port interrupts
// which can be handled easily here.
# ifndef HAL_INTERRUPT_ACKNOWLEDGE
#  if (1 == HAL_MCFxxxx_HAS_MCF5282_EPORT)

#   define HAL_INTERRUPT_ACKNOWLEDGE(_vector_)                                                  \
     CYG_MACRO_START                                                                            \
     cyg_uint32 _vec_ = (_vector_);                                                             \
     if ((_vec_ >= HAL_MCFxxxx_EPORT0_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT0_VECMAX)) {        \
         HAL_WRITE_UINT8(HAL_MCFxxxx_EPORT0_BASE + HAL_MCFxxxx_EPORTx_EPFR,                     \
                         0x01 << (_vec_ - HAL_MCFxxxx_EPORT0_VECBASE));                         \
     }                                                                                          \
     CYG_MACRO_END

#  elif (2 == HAL_MCFxxxx_HAS_MCF5282_EPORT)
#   define HAL_INTERRUPT_ACKNOWLEDGE(_vector_)                                                  \
     CYG_MACRO_START                                                                            \
     cyg_uint32 _vec_ = (_vector_);                                                             \
     if ((_vec_ >= HAL_MCFxxxx_EPORT0_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT0_VECMAX)) {        \
         HAL_WRITE_UINT8(HAL_MCFxxxx_EPORT0_BASE + HAL_MCFxxxx_EPORTx_EPFR,                     \
                         0x01 << (_vec_ - HAL_MCFxxxx_EPORT0_VECBASE));                         \
     } else if ((_vec_ >= HAL_MCFxxxx_EPORT1_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT1_VECMAX)) { \
         HAL_WRITE_UINT8(HAL_MCFxxxx_EPORT1_BASE + HAL_MCFxxxx_EPORTx_EPFR,                     \
                         0x01 << (_vec_ - HAL_MCFxxxx_EPORT1_VECBASE));                         \
     }                                                                                          \
     CYG_MACRO_END


#  else
#   error At most two edge port modules are supported at present.
#  endif    // HAL_MCFxxxx_HAS_MCF5282_EPORT == 1/2/many
# endif     // !defined(HAL_INTERRUPT_ACKNOWLEDGE)

# ifndef HAL_INTERRUPT_CONFIGURE

// Interrupt level/edge control only applies to the edge port
// interrupts. It is possible to select level-low, rising edge, or
// falling edge. The hardware does not support level-high. It does
// support rising-or-falling edge, but the eCos API does not export
// that functionality. Instead code can manipulate the edge port
// registers directly.
//
// This macro also manipulates the edge port data direction and
// interrupt enable registers, to ensure that the edge port really
// will generate interrupts.

#  if (1 == HAL_MCFxxxx_HAS_MCF5282_EPORT)

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_triggered_, _up_)                     \
    CYG_MACRO_START                                                                     \
    cyg_uint32 _vec_ = (_vector_);                                                      \
    if ((_vec_ >= HAL_MCFxxxx_EPORT0_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT0_VECMAX)) { \
        cyg_uint32  _base_;                                                             \
        cyg_uint16  _reg16_;                                                            \
        cyg_uint8   _reg8_;                                                             \
        _base_   = HAL_MCFxxxx_EPORT0_BASE;                                             \
        _vec_   -= HAL_MCFxxxx_EPORT0_VECBASE;                                          \
        HAL_READ_UINT16(_base_ + HAL_MCFxxxx_EPORTx_EPPAR, _reg16_);                    \
        _reg16_ &= ~(0x03 << (2 * _vec_));                                              \
        if (_level_triggered_) {                                                        \
            /* 00 is level-triggered so nothing to be done */                           \
        } else if (_up_) {                                                              \
            _reg16_ |= (HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_RISING << (2 * _vec_));          \
        } else {                                                                        \
            _reg16_ |= (HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_FALLING << (2 * _vec_));         \
        }                                                                               \
        HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_EPORTx_EPPAR, _reg16_);                   \
        HAL_READ_UINT8( _base_ + HAL_MCFxxxx_EPORTx_EPDDR, _reg8_);                     \
        _reg8_ &= ~(0x01 << _vec_);                                                     \
        HAL_WRITE_UINT8(_base_ + HAL_MCFxxxx_EPORTx_EPDDR, _reg8_);                     \
        HAL_READ_UINT8( _base_ + HAL_MCFxxxx_EPORTx_EPIER, _reg8_);                     \
        _reg8_ |= (0x01 << _vec_);                                                      \
        HAL_WRITE_UINT8(_base_ + HAL_MCFxxxx_EPORTx_EPIER, _reg8_);                     \
    }                                                                                   \
    CYG_MACRO_END

#  elif (2 == HAL_MCFxxxx_HAS_MCF5282_EPORT)

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_triggered_, _up_)                         \
    CYG_MACRO_START                                                                         \
    cyg_uint32 _vec_ = (_vector_);                                                          \
    if (((_vec_ >= HAL_MCFxxxx_EPORT0_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT0_VECMAX)) ||   \
        ((_vec_ >= HAL_MCFxxxx_EPORT1_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT1_VECMAX))) {   \
        cyg_uint32  _base_;                                                                 \
        cyg_uint16  _reg16_;                                                                \
        cyg_uint8   _reg8_;                                                                 \
        if ((_vec_ >= HAL_MCFxxxx_EPORT0_VECMIN) && (_vec_ <= HAL_MCFxxxx_EPORT0_VECMAX)) { \
            _base_   = HAL_MCFxxxx_EPORT0_BASE;                                             \
            _vec_   -= HAL_MCFxxxx_EPORT0_VECBASE;                                          \
        } else {                                                                            \
            _base_   = HAL_MCFxxxx_EPORT1_BASE;                                             \
            _vec_   -= HAL_MCFxxxx_EPORT1_VECBASE;                                          \
        }                                                                                   \
        HAL_READ_UINT16(_base_ + HAL_MCFxxxx_EPORTx_EPPAR, _reg16_);                        \
        _reg16_ &= ~(0x03 << (2 * _vec_));                                                  \
        if (_level_triggered_) {                                                            \
            /* 00 is level-triggered so nothing to be done */                               \
        } else if (_up_) {                                                                  \
            _reg16_ |= (HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_RISING << (2 * _vec_));              \
        } else {                                                                            \
            _reg16_ |= (HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_FALLING << (2 * _vec_));             \
        }                                                                                   \
        HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_EPORTx_EPPAR, _reg16_);                       \
        HAL_READ_UINT8( _base_ + HAL_MCFxxxx_EPORTx_EPDDR, _reg8_);                         \
        _reg8_ &= ~(0x01 << _vec_);                                                         \
        HAL_WRITE_UINT8(_base_ + HAL_MCFxxxx_EPORTx_EPDDR, _reg8_);                         \
        HAL_READ_UINT8( _base_ + HAL_MCFxxxx_EPORTx_EPIER, _reg8_);                         \
        _reg8_ |= (0x01 << _vec_);                                                          \
        HAL_WRITE_UINT8(_base_ + HAL_MCFxxxx_EPORTx_EPIER, _reg8_);                         \
    }                                                                                       \
    CYG_MACRO_END
        
#  else
#  endif    // HAL_MCFxxxx_HAS_MCF5282_EPORT == 1/2/many
# endif     // !defined(HAL_INTERRUPT_CONFIGURE)

#endif

#ifndef HAL_INTERRUPT_MASK
# error Processor or platform HAL_ should have provided HAL_INTERRUPT_MASK() macro
#endif
#ifndef HAL_INTERRUPT_UNMASK
# error Processor or platform HAL_ should have provided HAL_INTERRUPT_UNMASK() macro
#endif
#ifndef HAL_INTERRUPT_SET_LEVEL
# error Processor or platform HAL_ should have provided HAL_INTERRUPT_SET_LEVEL() macro
#endif
#ifndef HAL_INTERRUPT_ACKNOWLEDGE
# error Processor or platform HAL_ should have provided HAL_INTERRUPT_ACKNOWLEDGE() macro
#endif
#ifndef HAL_INTERRUPT_CONFIGURE
# error Processor or platform HAL_ should have provided HAL_INTERRUPT_CONFIGURE() macro
#endif

// ----------------------------------------------------------------------------
// On ColdFires with 5282-style programmable interrupt timers, typically
// one of those will be used for the system clock.
#ifdef HAL_MCFxxxx_HAS_MCF5282_PIT

# if (!defined(HAL_CLOCK_INIITALIZE) || !defined(HAL_CLOCK_RESET) || !defined(HAL_CLOCK_READ))
#  if !defined(_HAL_MCFxxxx_CLOCK_PIT_BASE_)
#   error The processor HAL should specify the programmable interrupt timer used for the system clock
#  elif !defined(_HAL_MCFxxxx_CLOCK_PIT_PRE_)
#   error The processor or platform HAL should specify the timer prescaler
#  else

#   if !defined(HAL_CLOCK_INITIALIZE)
#     define HAL_CLOCK_INITIALIZE(_period_)                                     \
      CYG_MACRO_START                                                           \
      cyg_uint32 _base_  = _HAL_MCFxxxx_CLOCK_PIT_BASE_;                        \
      HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_PITx_PCSR,                          \
                       _HAL_MCFxxxx_CLOCK_PIT_PRE_ |                            \
                       HAL_MCFxxxx_PITx_PCSR_OVW | HAL_MCFxxxx_PITx_PCSR_PIE |  \
                       HAL_MCFxxxx_PITx_PCSR_PIF | HAL_MCFxxxx_PITx_PCSR_RLD);  \
      HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_PITx_PMR, _period_);                \
      HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_PITx_PCSR,                          \
                       _HAL_MCFxxxx_CLOCK_PIT_PRE_ |                            \
                       HAL_MCFxxxx_PITx_PCSR_OVW | HAL_MCFxxxx_PITx_PCSR_PIE |  \
                       HAL_MCFxxxx_PITx_PCSR_PIF | HAL_MCFxxxx_PITx_PCSR_RLD |  \
                       HAL_MCFxxxx_PITx_PCSR_EN);                               \
      CYG_MACRO_END
#   endif

#   if !defined(HAL_CLOCK_RESET)
// The clock resets automatically but the interrupt must be cleared. This could
// be done by rewriting the period, but that introduces a risk of drift. A
// better approach is to write the PIF bit in the control register, which means
// writing all the other bits as well.
//
// Note: this could interfere with power management since the doze/halted
// bits may get cleared inadvertently.
#    define HAL_CLOCK_RESET(_vector_, _period_)                                 \
      CYG_MACRO_START                                                           \
      cyg_uint32 _base_  = _HAL_MCFxxxx_CLOCK_PIT_BASE_;                        \
      HAL_WRITE_UINT16(_base_ + HAL_MCFxxxx_PITx_PCSR,                          \
                       _HAL_MCFxxxx_CLOCK_PIT_PRE_  |                           \
                       HAL_MCFxxxx_PITx_PCSR_OVW | HAL_MCFxxxx_PITx_PCSR_PIE |  \
                       HAL_MCFxxxx_PITx_PCSR_PIF | HAL_MCFxxxx_PITx_PCSR_RLD |  \
                       HAL_MCFxxxx_PITx_PCSR_EN);                               \
      CYG_MACRO_END
#   endif

#   if !defined(HAL_CLOCK_READ)
#    define HAL_CLOCK_READ(_pvalue_)                                            \
     CYG_MACRO_START                                                            \
     cyg_uint32 _base_  = _HAL_MCFxxxx_CLOCK_PIT_BASE_;                         \
     cyg_uint16 _period_, _counter_;                                            \
     HAL_READ_UINT16(_base_ + HAL_MCFxxxx_PITx_PMR, _period_);                  \
     HAL_READ_UINT16(_base_ + HAL_MCFxxxx_PITx_PCNTR, _counter_);               \
     *(_pvalue_) = _period_ - _counter_;                                        \
     CYG_MACRO_END
#   endif
#  endif
# endif
#endif

#ifndef HAL_CLOCK_INITIALIZE
# error Processor or platform HAL should have provided HAL_CLOCK_INITIALIZE() macro
#endif
#ifndef HAL_CLOCK_RESET
# error Processor or platform HAL should have provided HAL_CLOCK_RESET() macro
#endif
#ifndef HAL_CLOCK_READ
# error Processor or platform HAL should have provided HAL_CLOCK_READ() macro
#endif

#ifndef HAL_CLOCK_LATENCY
# define HAL_CLOCK_LATENCY(_pvalue_)   HAL_CLOCK_READ(_pvalue_)
#endif

// ----------------------------------------------------------------------------
// Where possible use a 5282-compatible reset controller to generate a
// software reset. The architectural HAL has a fall-back
// implementation which goes through the reset exception vector slot.
#if !defined(HAL_PLATFORM_RESET) && defined(HAL_MCFxxxx_HAS_MCF5282_RST)
# define HAL_PLATFORM_RESET()                                           \
    CYG_MACRO_START                                                     \
    HAL_WRITE_UINT8(HAL_MCFxxxx_RST_BASE + HAL_MCFxxxx_RST_RCR,         \
                    HAL_MCFxxxx_RST_RCR_SOFTRST);                       \
    CYG_MACRO_END
# endif

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
