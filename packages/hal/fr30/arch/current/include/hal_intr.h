#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//==========================================================================
//
//      hal_intr.h
//
//      FR30 HAL Interrupt and clock support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
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
// Date:         2006-06-09
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//               #include <cyg/hal/hal_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_fr30.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_intr.h>

//--------------------------------------------------------------------------
// Exception vectors.
// Standard exception,interrupt and trap(EIT) vectors supported by FR30 CPUs
// These values are the ones to use for HAL_VSR_GET/SET
// The FR30 vector table lies in reversed in memory, therefore we do
// the 255-x thing here.

#define CYGNUM_HAL_VECTOR_RESET                   0
// Values from 1 - 8 are system reserved
#define CYGNUM_HAL_VECTOR_MODE_VECTOR             1
#define CYGNUM_HAL_VECTOR_COPR_NOT_FOUND          7
#define CYGNUM_HAL_VECTOR_COPR_ERROR              8
#define CYGNUM_HAL_VECTOR_BREAKPOINT              9
#define CYGNUM_HAL_VECTOR_INTE                    CYGNUM_HAL_VECTOR_BREAKPOINT
// Values 10 and 11 are system reserved
#define CYGNUM_HAL_VECTOR_INSTR_BREAK_EXCEPTION   10
#define CYGNUM_HAL_VECTOR_OPERAND_BREAK_TRAP      11
#define CYGNUM_HAL_VECTOR_DEBUG                   12
#define CYGNUM_HAL_VECTOR_STEP_TRACE              CYGNUM_HAL_VECTOR_DEBUG
// Value 13 is system reserved
#define CYGNUM_HAL_VECTOR_NMI_INTR_TOOL           13
// Value 14 undefined instruction exception
#define CYGNUM_HAL_VECTOR_OPCODE                  14
// NMI (special non maskable interrupt)
#define CYGNUM_HAL_VECTOR_NMI                     15
// interrupts
// Note that these defines are for C code and have to be the same like those in
// arch.inc for assembler code !
#define CYGNUM_HAL_VECTOR_INTRFIRST               15
#define CYGNUM_HAL_VECTOR_INTRLAST                63
#define CYGNUM_HAL_VECTOR_NUMINTRS                (CYGNUM_HAL_VECTOR_INTRLAST-CYGNUM_HAL_VECTOR_INTRFIRST+1)
// Values 64 and 65 are reserved for system
// traps
#define CYGNUM_HAL_VECTOR_TRAPFIRST               80
#define CYGNUM_HAL_VECTOR_SYSTEM_CALL             CYGNUM_HAL_VECTOR_TRAPFIRST
#define CYGNUM_HAL_VECTOR_TRAPLAST                255
#define CYGNUM_HAL_VECTOR_NUMTRAPS                (CYGNUM_HAL_VECTOR_TRAPLAST-CYGNUM_HAL_VECTOR_TRAPFIRST+1)

// The default size of the VSR table is 256 entries.
#ifndef CYGNUM_HAL_VSR_MIN
#define CYGNUM_HAL_VSR_MIN                        0
#define CYGNUM_HAL_VSR_MAX                        255
#define CYGNUM_HAL_VSR_COUNT                      256
#endif

// For ecos fr30 interrupts are interrupts and fr30 exceptions and
// fr30 traps are both exceptions

// Common interrupt vectors
#ifndef CYGNUM_HAL_ISR_MIN
#define CYGNUM_HAL_ISR_MIN                       CYGNUM_HAL_VECTOR_INTRFIRST
#define CYGNUM_HAL_ISR_MAX                       CYGNUM_HAL_VECTOR_INTRLAST
#define CYGNUM_HAL_ISR_COUNT                     (CYGNUM_HAL_VECTOR_NUMINTRS)
#endif
// Common exception vectors. (so these are fr30 exceptions and traps)
#define CYGNUM_HAL_EXCEPTION_RESET               CYGNUM_HAL_VECTOR_RESET
#define CYGNUM_HAL_EXCEPTION_FPU_NOT_AVAIL       CYGNUM_HAL_VECTOR_COPR_NOT_FOUND
#define CYGNUM_HAL_EXCEPTION_FPU                 CYGNUM_HAL_VECTOR_COPR_ERROR
#define CYGNUM_HAL_EXCEPTION_TRAP                CYGNUM_HAL_VECTOR_INTE
// #define CYGNUM_HAL_EXCEPTION_INTERRUPT           CYGNUM_HAL_VECTOR_BREAKPOINT
// #define CYGNUM_HAL_EXCEPTION_TRAP                CYGNUM_HAL_VECTOR_STEP_TRACE
#define CYGNUM_HAL_EXCEPTION_SINGLE_STEP         CYGNUM_HAL_VECTOR_STEP_TRACE
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION CYGNUM_HAL_VECTOR_OPCODE

// common interrupt vectors
// I am sure more defines can be moved from variant to architecture HAL here
#define CYGNUM_HAL_INTERRUPT_NMI                 15

// here we define exceptions and traps as the fr30 docs termini
#define CYGNUM_HAL_EXCEPTION_FR30_MIN            0
#define CYGNUM_HAL_EXCEPTION_FR30_MAX            14
#define CYGNUM_HAL_EXCEPTION_FR30_COUNT (CYGNUM_HAL_EXCEPTION_FR30_MAX - CYGNUM_HAL_EXCEPTION_FR30_MIN + 1)

#define CYGNUM_HAL_TRAP_FR30_MIN                 80
#define CYGNUM_HAL_TRAP_FR30_MAX                 255
#define CYGNUM_HAL_TRAP_FR30_COUNT (CYGNUM_HAL_TRAP_FR30_MAX - CYGNUM_HAL_TRAP_FR30_MIN + 1)

// here we define the ecos ones
// nicht mehr aktuell:(calculated from fr30 ones) FIXME maybe have to
// change to correspons to durchgaengig vector numbers. In this table
// exceptions are unterbrochen from interrupts
#define CYGNUM_HAL_EXCEPTION_MIN                 0
#define CYGNUM_HAL_EXCEPTION_MAX                 255
#define CYGNUM_HAL_EXCEPTION_COUNT           \
                 ( CYGNUM_HAL_EXCEPTION_MAX - CYGNUM_HAL_EXCEPTION_MIN + 1 )

//--------------------------------------------------------------------------
// Interrupt levels
// Lower numbers mean stronger interrupt levels
// values 0 - 14 are system reserved and can not be set by a program
// (setting them would add 16 to the value automatically)
// value 15 is for NMI
// value 31 disables the interrupt

#ifndef CYGHWR_HAL_INTERRUPT_LEVELS_DEFINED

#define CYGNUM_HAL_INTERRUPT_LEVEL_0                     16
#define CYGNUM_HAL_INTERRUPT_LEVEL_1                     17
#define CYGNUM_HAL_INTERRUPT_LEVEL_2                     18
#define CYGNUM_HAL_INTERRUPT_LEVEL_3                     19
#define CYGNUM_HAL_INTERRUPT_LEVEL_4                     20
#define CYGNUM_HAL_INTERRUPT_LEVEL_5                     21
#define CYGNUM_HAL_INTERRUPT_LEVEL_6                     22
#define CYGNUM_HAL_INTERRUPT_LEVEL_7                     23
#define CYGNUM_HAL_INTERRUPT_LEVEL_8                     24
#define CYGNUM_HAL_INTERRUPT_LEVEL_9                     25
#define CYGNUM_HAL_INTERRUPT_LEVEL_10                    26
#define CYGNUM_HAL_INTERRUPT_LEVEL_11                    27
#define CYGNUM_HAL_INTERRUPT_LEVEL_12                    28
#define CYGNUM_HAL_INTERRUPT_LEVEL_13                    29
#define CYGNUM_HAL_INTERRUPT_LEVEL_14                    30
#define CYGNUM_HAL_INTERRUPT_LEVEL_15                    31
#define CYGNUM_HAL_INTERRUPT_LEVEL_DISABLE              \
  CYGNUM_HAL_INTERRUPT_LEVEL_15

#define CYGHWR_HAL_INTERRUPT_LEVELS_DEFINED

#endif

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS  hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS  hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
externC volatile CYG_ADDRESS  hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//---------------------------------------------------------------------------
// Default ISR
externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

#define HAL_DEFAULT_ISR hal_default_isr

//--------------------------------------------------------------------------
// CPU interrupt enable/disable macros

#define HAL_ENABLE_INTERRUPTS()                 \
CYG_MACRO_START                                 \
    asm volatile ("orccr #0x10;\n") ;           \
CYG_MACRO_END

#define HAL_DISABLE_INTERRUPTS(_old_)           \
CYG_MACRO_START                                 \
    register int x ;                            \
    asm volatile ("st   r1, @-r15 ;\n"          \
         "MOV PS, r1 ;\n"                       \
         "LDI:8 #0x10,%0 ;\n"                   \
         "AND r1, %0 ;\n"                       \
         "LSR #1,%0 ;\n"                        \
         "ld    @r15+,  r1 ;\n"                 \
         "ANDCCR #0xEF \n"                      \
         :  "=r" (x)                            \
        );                                      \
    (_old_) = (x);                              \
CYG_MACRO_END

#define HAL_RESTORE_INTERRUPTS(_old_)           \
CYG_MACRO_START                                 \
    register int x = _old_;                     \
    asm volatile ( "CMP #8, %0 ;\n"             \
                   "BEQ 0f ;\n"                 \
                   "ANDCCR #0xEF ;\n"           \
                   "BRA 1f;\n"                  \
                   "0:\n"                       \
                   "ORCCR #0x10 ;\n"            \
                   "1:\n"                       \
                   : /* No outputs */           \
                   : "r"(x)                     \
                 );                             \
CYG_MACRO_END

// 5th bit (0x10 / #10H) is interrupt flag
// it is shifted right to be able to work with 4 bit immediate in the other macros
#define HAL_QUERY_INTERRUPTS(_old_)             \
CYG_MACRO_START                                 \
    register int x ;                            \
    asm volatile ("MOV PS,__tmp_reg__ ;\n"      \
         "LDI:8 #10H,%0 ;\n"                    \
         "AND __tmp_reg__,%0 ;\n"               \
         "LSR #1,%0 ;\n"                        \
         :  "=r" (x)                            \
        );                                      \
    (_old_) = (x);                              \
CYG_MACRO_END

//---------------------------------------------------------------------------
// Interrupt and VSR attachment macros


#define HAL_INTERRUPT_IN_USE( _vector_, _state_)        \
    CYG_MACRO_START                                     \
    cyg_uint32 _index_;                                 \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);         \
                                                        \
    if (hal_interrupt_handlers[_index_]                 \
        ==(CYG_ADDRESS)HAL_DEFAULT_ISR)                 \
        (_state_) = 0;                                  \
    else                                                \
        (_state_) = 1;                                  \
    CYG_MACRO_END

#ifndef HAL_INTERRUPT_ATTACH
externC void __default_interrupt_vsr(void);
#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )               \
    CYG_MACRO_START                                                             \
    cyg_uint32 _index_;                                                         \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);                                  \
                                                                                \
    HAL_VSR_SET( _vector_, &__default_interrupt_vsr , NULL);                    \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)HAL_DEFAULT_ISR )       \
    {                                                                           \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)(_isr_);                 \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD)(_data_);                   \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)(_object_);               \
    }                                                                           \
    CYG_MACRO_END
#endif /* HAL_INTERRUPT_ATTACH */

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ ) \
    CYG_MACRO_START                             \
    cyg_uint32 _index_;                         \
    HAL_TRANSLATE_VECTOR((_vector_), _index_);  \
                                                \
    if (hal_interrupt_handlers[_index_]         \
        == (CYG_ADDRESS)(_isr_))                \
    {                                           \
        hal_interrupt_handlers[_index_] =       \
            (CYG_ADDRESS)HAL_DEFAULT_ISR;       \
        hal_interrupt_data[_index_] = 0;        \
        hal_interrupt_objects[_index_] = 0;     \
    }                                           \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                         \
    *((CYG_ADDRESS *)(_pvsr_)) = hal_vsr_table[(_vector_)];


#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )               \
    CYG_MACRO_START                                             \
    CYG_ADDRESS *__poldvsr = (CYG_ADDRESS *)(_poldvsr_);        \
    if( __poldvsr != NULL )                                     \
        *__poldvsr = hal_vsr_table[(_vector_)];                 \
    hal_vsr_table[(_vector_)] = (CYG_ADDRESS)(_vsr_);           \
    CYG_MACRO_END

// This is an ugly name, but what it means is: grab the VSR back to eCos
// internal handling, or if you like, the default handler.  But if
// cooperating with GDB and CygMon, the default behaviour is to pass most
// exceptions to CygMon.  This macro undoes that so that eCos handles the
// exception.  So use it with care.

externC void __default_exception_vsr(void);
externC void __default_interrupt_vsr(void);

#define HAL_VSR_SET_TO_ECOS_HANDLER( _vector_, _poldvsr_ )                  \
CYG_MACRO_START                                                             \
    HAL_VSR_SET( _vector_, _vector_ > CYGNUM_HAL_EXCEPTION_MAX              \
                              ? (CYG_ADDRESS)__default_interrupt_vsr        \
                                : (CYG_ADDRESS)__default_exception_vsr,     \
                 _poldvsr_ );                                               \
CYG_MACRO_END

//--------------------------------------------------------------------------
// Vector translation.
// For chained interrupts we only have a single vector though which all
// are passed. For unchained interrupts we have a vector per interrupt.

#ifndef HAL_TRANSLATE_VECTOR

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = 0

#else

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)

#endif

#endif

//--------------------------------------------------------------------------
// Clock control.
// This code uses the 16 bit reload timer 1. The defines are used to specify
// the IO adress of the registers, that are different in FR30 variants.

#ifndef CYGHWR_HAL_CLOCK_CONTROL_DEFINED

extern CYG_WORD32 cyg_hal_clock_period;

#define CYGHWR_HAL_CLOCK_PERIOD_DEFINED

#define HAL_CLOCK_INITIALIZE( _period_ )                    \
CYG_MACRO_START                                             \
    HAL_WRITE_UINT16( CYG_HAL_FR30_RTC_TMRLR , _period_);   \
    HAL_WRITE_UINT16( CYG_HAL_FR30_RTC_TMCSR , 0x081b);     \
    cyg_hal_clock_period = _period_;                        \
CYG_MACRO_END

// This clears the interrupt request for reload timer 1 (RTC)
#define HAL_CLOCK_RESET( _vector_, _period_ )               \
CYG_MACRO_START                                             \
asm volatile("ldi:8     #0x57,  r0;\n"                      \
             "bandl     #0xb,   @r0;\n"                     \
             : : :"r0");                                    \
CYG_MACRO_END

#define HAL_CLOCK_READ( _pvalue_ )                          \
CYG_MACRO_START                                             \
    CYG_FAIL("clock_read");                                 \
    register CYG_WORD32 result;                             \
    HAL_READ_UINT16( CYG_HAL_FR30_RTC_TMR, result);         \
    *(_pvalue_) = cyg_hal_clock_period - result;            \
CYG_MACRO_END

#define CYGHWR_HAL_CLOCK_CONTROL_DEFINED

#endif


#if defined(CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY) && \
    !defined(HAL_CLOCK_LATENCY)
#define HAL_CLOCK_LATENCY( _pvalue_ )                       \
CYG_MACRO_START                                             \
    register CYG_WORD32 _cval_;                             \
    HAL_CLOCK_READ(&_cval_);                                \
    *(_pvalue_) = _cval_ - cyg_hal_clock_period;            \
CYG_MACRO_END
#endif

//----------------------------------------------------------------------------
// Reset
// this clears BIT4 in STCR(0x481), which should issue a reset

#define HAL_PLATFORM_RESET()                                \
    asm volatile (                                          \
        "ldi:20 #0x481, r0;\n"                              \
        "bandh  #1,     @r0;\n"                             \
        : : :"r0");

externC void _start(void);
#define HAL_PLATFORM_RESET_ENTRY        (&_start)

//--------------------------------------------------------------------------
// Microsecond delay
// This uses reload timer 2, because timer 0 and 1 can cause DMA transfers.
// Timer 2 is only used for delay service. We maybe support it out of the
// scheduler clock in the future.

externC void hal_delay_us(cyg_int32 usecs);
#define HAL_DELAY_US(_millis_)           hal_delay_us(_millis_);

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// End of hal_intr.h
