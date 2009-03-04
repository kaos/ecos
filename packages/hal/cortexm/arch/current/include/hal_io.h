#ifndef CYGONCE_HAL_IO_H
#define CYGONCE_HAL_IO_H
/*==========================================================================
//
//      hal_io.h
//
//      Cortex-M architecture IO register definitions 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Description:  Define IO registers
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_io.h>

//==========================================================================
// Handy macros for defining register bits and fields:
//

#define BIT_(__n)       (1<<(__n))
#define MASK_(__n,__s)  (((1<<(__s))-1)<<(__n))
#define VALUE_(__n,__v) ((__v)<<(__n))

//==========================================================================
// SysTick timer
//
// This is really part of the NVIC, but we break it out into a
// separate definition for convenience.

#define CYGARC_REG_SYSTICK_BASE                 0xE000E010

#define CYGARC_REG_SYSTICK_CSR                  0
#define CYGARC_REG_SYSTICK_RELOAD               4
#define CYGARC_REG_SYSTICK_VALUE                8
#define CYGARC_REG_SYSTICK_CAL                  12

#define CYGARC_REG_SYSTICK_CSR_COUNTFLAG        BIT_(16)
#define CYGARC_REG_SYSTICK_CSR_CLK_EXT          VALUE_(2,0)
#define CYGARC_REG_SYSTICK_CSR_CLK_INT          VALUE_(2,1)
#define CYGARC_REG_SYSTICK_CSR_TICKINT          BIT_(1)
#define CYGARC_REG_SYSTICK_CSR_ENABLE           BIT_(0)

#define CYGARC_REG_SYSTICK_CAL_NOREF            BIT_(31)
#define CYGARC_REG_SYSTICK_CAL_SKEW             BIT_(30)
#define CYGARC_REG_SYSTICK_CAL_TENMS            MASK_(0,23)

//==========================================================================
// NVIC registers

#define CYGARC_REG_NVIC_BASE                    0xE000E000

#if defined(CYGHWR_HAL_CORTEXM_M3)
#define CYGARC_REG_NVIC_TYPE                    0x004
#endif

#define CYGARC_REG_NVIC_SER0                    0x100
#define CYGARC_REG_NVIC_CER0                    0x180
#define CYGARC_REG_NVIC_SPR0                    0x200
#define CYGARC_REG_NVIC_CPR0                    0x280
#define CYGARC_REG_NVIC_ABR0                    0x300
#define CYGARC_REG_NVIC_PR0                     0x400

// Generate address of 32 bit control register for interrupt
#define CYGARC_REG_NVIC_SER(__intr)             (CYGARC_REG_NVIC_SER0+4*((__intr)>>5))
#define CYGARC_REG_NVIC_CER(__intr)             (CYGARC_REG_NVIC_CER0+4*((__intr)>>5))
#define CYGARC_REG_NVIC_SPR(__intr)             (CYGARC_REG_NVIC_SPR0+4*((__intr)>>5))
#define CYGARC_REG_NVIC_CPR(__intr)             (CYGARC_REG_NVIC_CPR0+4*((__intr)>>5))
#define CYGARC_REG_NVIC_ABR(__intr)             (CYGARC_REG_NVIC_ABR0+4*((__intr)>>5))

// Generate bit in register for interrupt
#define CYGARC_REG_NVIC_IBIT(__intr)            BIT_((__intr)&0x1F)

// Generate byte address of interrupt's priority register.
#define CYGARC_REG_NVIC_PR(__intr)              (CYGARC_REG_NVIC_PR0+(__intr))


#if defined(CYGHWR_HAL_CORTEXM_M3)

#define CYGARC_REG_NVIC_CPUID                   0xD00
#define CYGARC_REG_NVIC_ICSR                    0xD04
#define CYGARC_REG_NVIC_VTOR                    0xD08
#define CYGARC_REG_NVIC_AIRCR                   0xD0C
#define CYGARC_REG_NVIC_SCR                     0xD10
#define CYGARC_REG_NVIC_CCR                     0xD14
#define CYGARC_REG_NVIC_SHPR0                   0xD18
#define CYGARC_REG_NVIC_SHPR1                   0xD1C
#define CYGARC_REG_NVIC_SHPR2                   0xD20
#define CYGARC_REG_NVIC_SHCSR                   0xD24
#define CYGARC_REG_NVIC_CFSR                    0xD28
#define CYGARC_REG_NVIC_HFSR                    0xD2C
#define CYGARC_REG_NVIC_DFSR                    0xD30
#define CYGARC_REG_NVIC_MMAR                    0xD34
#define CYGARC_REG_NVIC_BFAR                    0xD38
#define CYGARC_REG_NVIC_AFSR                    0xD3C
#define CYGARC_REG_NVIC_PFR0                    0xD40
#define CYGARC_REG_NVIC_PFR1                    0xD44
#define CYGARC_REG_NVIC_DFR0                    0xD48
#define CYGARC_REG_NVIC_AFR0                    0xD4C
#define CYGARC_REG_NVIC_MMFR0                   0xD50
#define CYGARC_REG_NVIC_MMFR1                   0xD54
#define CYGARC_REG_NVIC_MMFR2                   0xD58
#define CYGARC_REG_NVIC_MMFR3                   0xD5C
#define CYGARC_REG_NVIC_ISAR0                   0xD60
#define CYGARC_REG_NVIC_ISAR1                   0xD64
#define CYGARC_REG_NVIC_ISAR2                   0xD68
#define CYGARC_REG_NVIC_ISAR3                   0xD6C
#define CYGARC_REG_NVIC_ISAR4                   0xD70
#define CYGARC_REG_NVIC_STIR                    0xF00
#define CYGARC_REG_NVIC_PID4                    0xFD0
#define CYGARC_REG_NVIC_PID5                    0xFD4
#define CYGARC_REG_NVIC_PID6                    0xFD8
#define CYGARC_REG_NVIC_PID7                    0xFDC
#define CYGARC_REG_NVIC_PID0                    0xFE0
#define CYGARC_REG_NVIC_PID1                    0xFE4
#define CYGARC_REG_NVIC_PID2                    0xFE8
#define CYGARC_REG_NVIC_PID3                    0xFEC
#define CYGARC_REG_NVIC_CID0                    0xFF0
#define CYGARC_REG_NVIC_CID1                    0xFF4
#define CYGARC_REG_NVIC_CID2                    0xFF8
#define CYGARC_REG_NVIC_CID3                    0xFFC

// ICSR

#define CYGARC_REG_NVIC_ICSR_NMIPENDSET         BIT_(31)
#define CYGARC_REG_NVIC_ICSR_PENDSVSET          BIT_(28)
#define CYGARC_REG_NVIC_ICSR_PENDSVCLR          BIT_(27)
#define CYGARC_REG_NVIC_ICSR_PENDSTSET          BIT_(26)
#define CYGARC_REG_NVIC_ICSR_PENDSTCLR          BIT_(25)
#define CYGARC_REG_NVIC_ICSR_ISRPREEMPT         BIT_(23)
#define CYGARC_REG_NVIC_ICSR_ISRPENDING         BIT_(22)
#define CYGARC_REG_NVIC_ICSR_VECTPENDING        MASK_(12,9)
#define CYGARC_REG_NVIC_ICSR_RETTOBASE          BIT_(11)
#define CYGARC_REG_NVIC_ICSR_VECTACTIVE         MASK_(0,9)

// VTOR

#define CYGARC_REG_NVIC_VTOR_TBLOFF(__o)        VALUE_(7,__o)
#define CYGARC_REG_NVIC_VTOR_TBLBASE_CODE       0
#define CYGARC_REG_NVIC_VTOR_TBLBASE_SRAM       BIT_(29)

// AI/RCR

#define CYGARC_REG_NVIC_AIRCR_KEY               VALUE_(16,0x5FA)
#define CYGARC_REG_NVIC_AIRCR_BIGENDIAN         BIT_(15)
#define CYGARC_REG_NVIC_AIRCR_PRIGROUP(__p)     VALUE_(8,__p)
#define CYGARC_REG_NVIC_AIRCR_SYSRESETREQ       BIT_(2)
#define CYGARC_REG_NVIC_AIRCR_VECTCLRACTIVE     BIT_(1)
#define CYGARC_REG_NVIC_AIRCR_VECTRESET         BIT_(0)

// SCR

#define CYGARC_REG_NVIC_SCR_SLEEPONEXIT         BIT_(1)
#define CYGARC_REG_NVIC_SCR_DEEPSLEEP           BIT_(2)
#define CYGARC_REG_NVIC_SCR_SEVONPEND           BIT_(4)

// SHCSR

#define CYGARC_REG_NVIC_SHCSR_USGFAULTENA       BIT_(18)
#define CYGARC_REG_NVIC_SHCSR_BUSFAULTENA       BIT_(17)
#define CYGARC_REG_NVIC_SHCSR_MEMFAULTENA       BIT_(16)
#define CYGARC_REG_NVIC_SHCSR_SVCALLPENDED      BIT_(15)
#define CYGARC_REG_NVIC_SHCSR_BUSFAULTPENDED    BIT_(14)
#define CYGARC_REG_NVIC_SHCSR_MEMFAULTPENDED    BIT_(13)
#define CYGARC_REG_NVIC_SHCSR_USGFAULTPENDED    BIT_(12)
#define CYGARC_REG_NVIC_SHCSR_SYSTICKACT        BIT_(11)
#define CYGARC_REG_NVIC_SHCSR_PENDSVACT         BIT_(10)
#define CYGARC_REG_NVIC_SHCSR_MONITORACT        BIT_(8)
#define CYGARC_REG_NVIC_SHCSR_SVCALLACT         BIT_(7)
#define CYGARC_REG_NVIC_SHCSR_USGFAULTACT       BIT_(3)
#define CYGARC_REG_NVIC_SHCSR_BUSFAULTACT       BIT_(1)
#define CYGARC_REG_NVIC_SHCSR_MEMFAULTACT       BIT_(0)

#endif

//==========================================================================
// Debug registers

#if defined(CYGHWR_HAL_CORTEXM_M3)

#define CYGARC_REG_DEBUG_BASE                   0xE000EDF0

#define CYGARC_REG_DEBUG_DHSR                   0x00
#define CYGARC_REG_DEBUG_DCRSR                  0x04
#define CYGARC_REG_DEBUG_DCRDR                  0x08
#define CYGARC_REG_DEBUG_DEMCR                  0x0C


#define CYGARC_REG_DEBUG_DHSR_DBGKEY            VALUE_(16,0xA05F)
#define CYGARC_REG_DEBUG_DHSR_S_RESET           BIT_(25)
#define CYGARC_REG_DEBUG_DHSR_S_RETIRE          BIT_(24)
#define CYGARC_REG_DEBUG_DHSR_S_LOCKUP          BIT_(19)
#define CYGARC_REG_DEBUG_DHSR_S_SLEEP           BIT_(18)
#define CYGARC_REG_DEBUG_DHSR_S_HALT            BIT_(17)
#define CYGARC_REG_DEBUG_DHSR_S_REGRDY          BIT_(16)
#define CYGARC_REG_DEBUG_DHSR_C_SNAPSTALL       BIT_(5)
#define CYGARC_REG_DEBUG_DHSR_C_MASKINTS        BIT_(3)
#define CYGARC_REG_DEBUG_DHSR_C_STEP            BIT_(2)
#define CYGARC_REG_DEBUG_DHSR_C_HALT            BIT_(1)
#define CYGARC_REG_DEBUG_DHSR_C_DEBUGEN         BIT_(0)


#define CYGARC_REG_DEBUG_DCRSR_REG_WRITE        BIT_(16)
#define CYGARC_REG_DEBUG_DCRSR_REG_READ         0
#define CYGARC_REG_DEBUG_DCRSR_REG(__x)         VALUE_(0,__x)


#define CYGARC_REG_DEBUG_DEMCR_TRCENA           BIT_(24)
#define CYGARC_REG_DEBUG_DEMCR_MON_REQ          BIT_(19)
#define CYGARC_REG_DEBUG_DEMCR_MON_STEP         BIT_(18)
#define CYGARC_REG_DEBUG_DEMCR_MON_PEND         BIT_(17)
#define CYGARC_REG_DEBUG_DEMCR_MON_EN           BIT_(16)
#define CYGARC_REG_DEBUG_DEMCR_VC_HARDERR       BIT_(10)
#define CYGARC_REG_DEBUG_DEMCR_VC_INTERR        BIT_(9)
#define CYGARC_REG_DEBUG_DEMCR_VC_BUSERR        BIT_(8)
#define CYGARC_REG_DEBUG_DEMCR_VC_STATERR       BIT_(7)
#define CYGARC_REG_DEBUG_DEMCR_VC_CHKERR        BIT_(6)
#define CYGARC_REG_DEBUG_DEMCR_VC_NOCPERR       BIT_(5)
#define CYGARC_REG_DEBUG_DEMCR_VC_MMERR         BIT_(4)
#define CYGARC_REG_DEBUG_DEMCR_VC_CORERESET     BIT_(0)

#endif

//==========================================================================
// IO Register address.
// This type is for recording the address of an IO register.

typedef volatile CYG_ADDRWORD HAL_IO_REGISTER;

//-----------------------------------------------------------------------------
// HAL IO macros.

#ifndef HAL_IO_MACROS_DEFINED

//-----------------------------------------------------------------------------
// BYTE Register access.
// Individual and vectorized access to 8 bit registers.


#define HAL_READ_UINT8( _register_, _value_ ) \
        ((_value_) = *((volatile CYG_BYTE *)(_register_)))

#define HAL_WRITE_UINT8( _register_, _value_ ) \
        (*((volatile CYG_BYTE *)(_register_)) = (_value_))

#define HAL_READ_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )     \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        (_buf_)[_i_] = ((volatile CYG_BYTE *)(_register_))[_j_];        \
    CYG_MACRO_END

#define HAL_WRITE_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        ((volatile CYG_BYTE *)(_register_))[_j_] = (_buf_)[_i_];        \
    CYG_MACRO_END

#define HAL_READ_UINT8_STRING( _register_, _buf_, _count_ )             \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        (_buf_)[_i_] = ((volatile CYG_BYTE *)(_register_))[_i_];        \
    CYG_MACRO_END

#define HAL_WRITE_UINT8_STRING( _register_, _buf_, _count_ )            \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        ((volatile CYG_BYTE *)(_register_)) = (_buf_)[_i_];             \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// 16 bit access.
// Individual and vectorized access to 16 bit registers.
    

#define HAL_READ_UINT16( _register_, _value_ ) \
        ((_value_) = *((volatile CYG_WORD16 *)(_register_)))

#define HAL_WRITE_UINT16( _register_, _value_ ) \
        (*((volatile CYG_WORD16 *)(_register_)) = (_value_))

#define HAL_READ_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        (_buf_)[_i_] = ((volatile CYG_WORD16 *)(_register_))[_j_];      \
    CYG_MACRO_END

#define HAL_WRITE_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        ((volatile CYG_WORD16 *)(_register_))[_j_] = (_buf_)[_i_];      \
    CYG_MACRO_END

#define HAL_READ_UINT16_STRING( _register_, _buf_, _count_)             \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        (_buf_)[_i_] = ((volatile CYG_WORD16 *)(_register_))[_i_];      \
    CYG_MACRO_END

#define HAL_WRITE_UINT16_STRING( _register_, _buf_, _count_)            \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        ((volatile CYG_WORD16 *)(_register_))[_i_] = (_buf_)[_i_];      \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// 32 bit access.
// Individual and vectorized access to 32 bit registers.

#define HAL_READ_UINT32( _register_, _value_ ) \
        ((_value_) = *((volatile CYG_WORD32 *)(_register_)))

#define HAL_WRITE_UINT32( _register_, _value_ ) \
        (*((volatile CYG_WORD32 *)(_register_)) = (_value_))

#define HAL_READ_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        (_buf_)[_i_] = ((volatile CYG_WORD32 *)(_register_))[_j_];      \
    CYG_MACRO_END

#define HAL_WRITE_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_,_j_;                                                \
    for( _i_ = 0, _j_ = 0; _i_ < (_count_); _i_++, _j_ += (_step_))     \
        ((volatile CYG_WORD32 *)(_register_))[_j_] = (_buf_)[_i_];      \
    CYG_MACRO_END

#define HAL_READ_UINT32_STRING( _register_, _buf_, _count_)             \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        (_buf_)[_i_] = ((volatile CYG_WORD32 *)(_register_))[_i_];      \
    CYG_MACRO_END

#define HAL_WRITE_UINT32_STRING( _register_, _buf_, _count_)            \
    CYG_MACRO_START                                                     \
    cyg_count32 _i_;                                                    \
    for( _i_ = 0; _i_ < (_count_); _i_++)                               \
        ((volatile CYG_WORD32 *)(_register_))[_i_] = (_buf_)[_i_];      \
    CYG_MACRO_END


#define HAL_IO_MACROS_DEFINED

#endif // !HAL_IO_MACROS_DEFINED

// Enforce a flow "barrier" to prevent optimizing compiler from reordering 
// operations.
#define HAL_IO_BARRIER()


//==========================================================================
#endif //CYGONCE_HAL_IO_H
