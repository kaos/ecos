#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//==========================================================================
//
//      var_intr.h
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
//               interrupts and the clock.
//               This file contains info about interrupts and
//               peripherals that are common on all PCs; for example,
//               the clock always activates irq 0 and would therefore
//               be listed here; an ethernet card is configured for
//               the individual system and would be in plf_intr.h
//               instead.
//              
// Usage:
//               #include <cyg/hal/var_intr.h>
//               ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/plf_intr.h>
#include <pkgconf/hal_i386_pc.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

// Physical interrupts connected to the CPU.
#define CYGNUM_HAL_VECTOR_DIV0                    0
#define CYGNUM_HAL_VECTOR_DEBUG                   1
#define CYGNUM_HAL_VECTOR_NMI                     2
#define CYGNUM_HAL_VECTOR_BREAKPOINT              3
#define CYGNUM_HAL_VECTOR_OVERFLOW                4
#define CYGNUM_HAL_VECTOR_BOUND                   5
#define CYGNUM_HAL_VECTOR_OPCODE                  6
#define CYGNUM_HAL_VECTOR_NO_DEVICE               7
#define CYGNUM_HAL_VECTOR_DOUBLE_FAULT            8
#define CYGNUM_HAL_VECTOR_INVALID_TSS            10
#define CYGNUM_HAL_VECTOR_SEGV                   11
#define CYGNUM_HAL_VECTOR_STACK_FAULT            12
#define CYGNUM_HAL_VECTOR_PROTECTION             13
#define CYGNUM_HAL_VECTOR_PAGE                   14
#define CYGNUM_HAL_VECTOR_FPE                    16
#define CYGNUM_HAL_VECTOR_ALIGNMENT              17

#define CYGNUM_HAL_ISR_MIN                        0
#define CYGNUM_HAL_ISR_MAX                       47
#define CYGNUM_HAL_ISR_COUNT    (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)

#define CYGNUM_HAL_VSR_MIN                        0
#define CYGNUM_HAL_VSR_MAX                       47
#define CYGNUM_HAL_VSR_COUNT    (CYGNUM_HAL_VSR_MAX - CYGNUM_HAL_VSR_MIN + 1)


// Common exception vectors.
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION CYGNUM_HAL_VECTOR_OPCODE
#define CYGNUM_HAL_EXCEPTION_INTERRUPT           CYGNUM_HAL_VECTOR_BREAKPOINT
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS         CYGNUM_HAL_VECTOR_PROTECTION
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS         CYGNUM_HAL_VECTOR_PAGE
#define CYGNUM_HAL_EXCEPTION_TRAP                CYGNUM_HAL_VECTOR_DEBUG
#define CYGNUM_HAL_EXCEPTION_FPU                 CYGNUM_HAL_VECTOR_FPE
#define CYGNUM_HAL_EXCEPTION_STACK_OVERFLOW      CYGNUM_HAL_VECTOR_SEGV
#define CYGNUM_HAL_EXCEPTION_MIN                 0
#define CYGNUM_HAL_EXCEPTION_MAX                 31
#define CYGNUM_HAL_EXCEPTION_COUNT (CYGNUM_HAL_EXCEPTION_MAX - CYGNUM_HAL_EXCEPTION_MIN + 1)

// These really are wild guesses on my part...
#define CYGNUM_HAL_VECTOR_SIGBUS                CYGNUM_HAL_EXCEPTION_DATA_ACCESS
#define CYGNUM_HAL_VECTOR_SIGFPE                CYGNUM_HAL_EXCEPTION_FPU
#define CYGNUM_HAL_VECTOR_SIGSEGV               CYGNUM_HAL_VECTOR_SEGV

#define PC_HARDWARE_IRQ_OFFSET                  32

#define CYGNUM_HAL_INTERRUPT_RTC                (PC_HARDWARE_IRQ_OFFSET + 0)


//--------------------------------------------------------------------------

externC void hal_pc_interrupt_attach(CYG_ADDRWORD vector, CYG_ADDRESS isr,
			CYG_ADDRWORD parameter, void * object) ;
#define HAL_INTERRUPT_ATTACH(_vector_, _isr_, _data_, _object_) \
		hal_pc_interrupt_attach(_vector_, (CYG_ADDRESS)_isr_, _data_, _object_)

#define HAL_ENABLE_INTERRUPTS()                 \
CYG_MACRO_START                                 \
    asm ("sti") ;                               \
CYG_MACRO_END

#define HAL_DISABLE_INTERRUPTS(_old_)           \
CYG_MACRO_START                                 \
    register int x ;                            \
    asm volatile (                              \
        "pushfl ;"                              \
        "popl %0 ;"                             \
        "cli"                                   \
        :	"=r" (x)                        \
        ) ;                                     \
    (_old_) = (x & 0x200);                      \
CYG_MACRO_END

#define HAL_RESTORE_INTERRUPTS(_old_)           \
CYG_MACRO_START                                 \
    register int x = _old_;                     \
    asm volatile ( "pushfl ;"                   \
                   "popl %%eax ;"               \
                   "andl $0xFFFFFDFF,%%eax;"    \
                   "orl  %0,%%eax;"             \
                   "pushl %%eax;"               \
                   "popfl ;"                    \
                   : /* No outputs */           \
                   : "r"(x)                     \
                   : "eax"                      \
                 );                             \
CYG_MACRO_END

#define HAL_QUERY_INTERRUPTS(_old_)             \
CYG_MACRO_START                                 \
    register int x ;                            \
    asm volatile ("pushfl ;"                    \
         "popl %0"                              \
         :	"=r" (x)                        \
        );                                      \
    (_old_) = (x & 0x200);                      \
CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )           \
CYG_MACRO_START                                         \
    int x = (_vector_) - PC_HARDWARE_IRQ_OFFSET;        \
    if ((x >= 8) && (x < 16))                           \
    {                                                   \
        asm(                                            \
            "movb $0x20, %%al;"                         \
            "outb %%al, $0xA0;"                         \
            :	/* No outputs. */                       \
            :	/* No inputs. */                        \
            :	"eax"                                   \
            );                                          \
    }                                                   \
    if ((x >= 0) && (x < 8))                            \
    {                                                   \
        asm(                                            \
            "movb $0x20, %%al;"                         \
            "outb %%al, $0x20;"                         \
            :	/* No outputs. */                       \
            :	/* No inputs. */                        \
            :	"eax"                                   \
            );                                          \
    }                                                   \
CYG_MACRO_END

#undef HAL_INTERRUPT_ACKNOWLEGE
#define HAL_INTERRUPT_ACKNOWLEGE(_vector_)
	/* Handled automatically in the interrupt routine.  See platform.inc. */

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) ((_index_) = (_vector_))

#define HAL_INTERRUPT_MASK( _vector_ )                  \
CYG_MACRO_START                                         \
    int x = (_vector_) - PC_HARDWARE_IRQ_OFFSET;        \
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
    int x = (_vector_) - PC_HARDWARE_IRQ_OFFSET;        \
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


#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )      

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            

//---------------------------------------------------------------------------
// Clock support.

externC void hal_pc_clock_initialize(cyg_uint32) ;
externC void hal_pc_clock_read(cyg_uint32 *) ;

#define HAL_CLOCK_INITIALIZE(_period_)        hal_pc_clock_initialize(_period_)
#define HAL_CLOCK_RESET(_vec_, _period_)      /* Clock automatically reloads. */
#define HAL_CLOCK_READ(_pvalue_)              hal_pc_clock_read(_pvalue_)

#define PC_PIT_CONTROL  (0x43)
#define PC_PIT_CLOCK_0	(0x40)
#define PC_PIT_CLOCK_1	(0x41)
#define PC_PIT_CLOCK_2	(0x42)


//---------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
// End of var_intr.h
