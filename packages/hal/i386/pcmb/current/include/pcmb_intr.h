#ifndef CYGONCE_HAL_PCMB_INTR_H
#define CYGONCE_HAL_PCMB_INTR_H

//==========================================================================
//
//      pcmb_intr.h
//
//      i386/pc Interrupt and clock support
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
// Author(s):    proven
// Contributors: proven, jskov, pjo
// Date:         1999-10-15
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock on a standard PC Motherboard.
//               This file contains info about interrupts and
//               peripherals that are common on all PCs; for example,
//               the clock always activates irq 0 and would therefore
//               be listed here.
//              
// Usage:
//               #include <cyg/hal/pcmb_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>
#include <pkgconf/hal_i386_pcmb.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/plf_intr.h>
#include <cyg/hal/hal_io.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#define CYGNUM_HAL_INTERRUPT_IRQ0                32
#define CYGNUM_HAL_INTERRUPT_IRQ1                33
#define CYGNUM_HAL_INTERRUPT_IRQ2                34
#define CYGNUM_HAL_INTERRUPT_IRQ3                35
#define CYGNUM_HAL_INTERRUPT_IRQ4                36
#define CYGNUM_HAL_INTERRUPT_IRQ5                37
#define CYGNUM_HAL_INTERRUPT_IRQ6                38
#define CYGNUM_HAL_INTERRUPT_IRQ7                39
#define CYGNUM_HAL_INTERRUPT_IRQ8                40
#define CYGNUM_HAL_INTERRUPT_IRQ9                41
#define CYGNUM_HAL_INTERRUPT_IRQ10               42
#define CYGNUM_HAL_INTERRUPT_IRQ11               43
#define CYGNUM_HAL_INTERRUPT_IRQ12               44
#define CYGNUM_HAL_INTERRUPT_IRQ13               45
#define CYGNUM_HAL_INTERRUPT_IRQ14               46
#define CYGNUM_HAL_INTERRUPT_IRQ15               47

#define CYGNUM_HAL_INTERRUPT_TIMER               32
#define CYGNUM_HAL_INTERRUPT_KEYBOARD            33
#define CYGNUM_HAL_INTERRUPT_SLAVE8259           34
#define CYGNUM_HAL_INTERRUPT_COM2                35
#define CYGNUM_HAL_INTERRUPT_COM1                36
#define CYGNUM_HAL_INTERRUPT_LPT2                37
#define CYGNUM_HAL_INTERRUPT_FDD                 38
#define CYGNUM_HAL_INTERRUPT_LPT1                39
#define CYGNUM_HAL_INTERRUPT_WALLCLOCK           40
#define CYGNUM_HAL_INTERRUPT_SLAVE8259REDIR      41
#define CYGNUM_HAL_INTERRUPT_COPRO               45
#define CYGNUM_HAL_INTERRUPT_HDD                 46

#define CYGNUM_HAL_ISR_MIN                       32
#define CYGNUM_HAL_ISR_MAX                       255
#define CYGNUM_HAL_ISR_COUNT    (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)

#define CYGNUM_HAL_INTERRUPT_RTC                 CYGNUM_HAL_INTERRUPT_TIMER


//--------------------------------------------------------------------------
// Interrupt vector translation

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) \
        ((_index_) = ((_vector_)-CYGNUM_HAL_ISR_MIN))


//--------------------------------------------------------------------------
// PIC interrupt acknowledge


#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )   \
CYG_MACRO_START                                 \
    int x;                                      \
    HAL_TRANSLATE_VECTOR( _vector_, x );        \
    if ((x >= 8) && (x < 16))                   \
        HAL_WRITE_UINT8( 0xa0, 0x20 );          \
    if ((x >= 0) && (x < 16))                   \
        HAL_WRITE_UINT8( 0x20, 0x20 );          \
CYG_MACRO_END


//--------------------------------------------------------------------------
// PIC per-interrupt source masking


#define HAL_INTERRUPT_MASK( _vector_ )                  \
CYG_MACRO_START                                         \
    int x;                                              \
    HAL_TRANSLATE_VECTOR( _vector_, x );                \
    if (x >= 8)                                         \
    {                                                   \
        x = 1 << (x - 8) ;                              \
        asm(                                            \
            "inb $0xA1, %%al;"                          \
            "orl %0, %%eax;"                            \
            "outb %%al, $0xA1;"                         \
            :	/* No outputs. */                       \
            :	"g" (x)                                 \
            :	"eax"                                   \
            );                                          \
    }                                                   \
    else                                                \
    {                                                   \
        x = 1 << x ;                                    \
        asm(                                            \
            "inb $0x21, %%al;"                          \
            "orl %0, %%eax;"                            \
            "outb %%al, $0x21;"                         \
            :	/* No outputs. */                       \
            :	"g" (x)                                 \
            :	"eax"                                   \
            );                                          \
    }                                                   \
CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ )                \
CYG_MACRO_START                                         \
    int x;                                              \
    HAL_TRANSLATE_VECTOR( _vector_, x );                \
    if (x >= 8)                                         \
    {                                                   \
        x = ~(1 << (x - 8)) ;                           \
        asm(                                            \
            "inb $0xA1, %%al;"                          \
            "andl %0, %%eax;"                           \
            "outb %%al, $0xA1;"                         \
            :	/* No outputs. */                       \
            :	"g" (x)                                 \
            :	"eax"                                   \
            );                                          \
    }                                                   \
    else                                                \
    {                                                   \
        x = ~(1 << x) ;                                 \
        asm(                                            \
            "inb $0x21, %%al;"                          \
            "andl %0, %%eax;"                           \
            "outb %%al, $0x21;"                         \
            :	/* No outputs. */                       \
            :	"g" (x)                                 \
            :	"eax"                                   \
            );                                          \
    }                                                   \
CYG_MACRO_END


//--------------------------------------------------------------------------
// PIC interrupt configuration
// Nothing supported here at present

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )      

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            


//---------------------------------------------------------------------------
// Clock support.

externC void hal_pc_clock_initialize(cyg_uint32) ;
externC void hal_pc_clock_read(cyg_uint32 *) ;

#define HAL_CLOCK_INITIALIZE(_period_)        hal_pc_clock_initialize(_period_)
#define HAL_CLOCK_RESET(_vec_, _period_)      /* Clock automatically reloads. */
#define HAL_CLOCK_READ(_pvalue_)              hal_pc_clock_read(_pvalue_)

// Timer IO ports
#define PC_PIT_CONTROL  (0x43)
#define PC_PIT_CLOCK_0	(0x40)
#define PC_PIT_CLOCK_1	(0x41)
#define PC_PIT_CLOCK_2	(0x42)

//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PCMB_INTR_H
// End of pcmb_intr.h
