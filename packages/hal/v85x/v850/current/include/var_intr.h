#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//==========================================================================
//
//      var_intr.h
//
//      NEC V850 Interrupt and clock support
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                  
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jskov, gthomas, jlarmour
// Date:         2001-03-21
// Purpose:      NEC V850 Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for variants of the NEC V85x
//               architecture.
//              
// Usage:
//              #include <cyg/hal/var_intr.h>
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
// NEC V850/SA1 (70301x) and V850/SB1 (70303x) vectors. 

// These are the exception codes presented in the Cause register and
// correspond to VSRs. These values are the ones to use for HAL_VSR_GET/SET

#define CYGNUM_HAL_VECTOR_RESET                0x00
#define CYGNUM_HAL_VECTOR_NMI                  0x01
#define CYGNUM_HAL_VECTOR_INTWDT               0x02 // watchdog timer NMI
#define CYGNUM_HAL_VECTOR_TRAP00               0x04
#define CYGNUM_HAL_VECTOR_TRAP10               0x05
#define CYGNUM_HAL_VECTOR_ILGOP                0x06 // illegal opcode
#define CYGNUM_HAL_VECTOR_INTWDTM              0x08 // watchdog timer maskable
#define CYGNUM_HAL_VECTOR_INTP0                0x09
#define CYGNUM_HAL_VECTOR_INTP1                0x0A
#define CYGNUM_HAL_VECTOR_INTP2                0x0B
#define CYGNUM_HAL_VECTOR_INTP3                0x0C
#define CYGNUM_HAL_VECTOR_INTP4                0x0D
#define CYGNUM_HAL_VECTOR_INTP5                0x0E
#define CYGNUM_HAL_VECTOR_INTP6                0x0F

#if CYGINT_HAL_V850_VARIANT_SA1
#define CYGNUM_HAL_VECTOR_INTWTNI              0x10
#define CYGNUM_HAL_VECTOR_INTTM00              0x11
#define CYGNUM_HAL_VECTOR_INTTM01              0x12
#define CYGNUM_HAL_VECTOR_INTTM10              0x13
#define CYGNUM_HAL_VECTOR_INTTM11              0x14
#define CYGNUM_HAL_VECTOR_INTTM2               0x15
#define CYGNUM_HAL_VECTOR_INTTM3               0x16
#define CYGNUM_HAL_VECTOR_INTTM4               0x17
#define CYGNUM_HAL_VECTOR_INTTM5               0x18
#define CYGNUM_HAL_VECTOR_INTIIC0              0x19
#define CYGNUM_HAL_VECTOR_INTCSI0              0x19
#define CYGNUM_HAL_VECTOR_INTSER0              0x1A
#define CYGNUM_HAL_VECTOR_INTSR0               0x1B
#define CYGNUM_HAL_VECTOR_INTCSI1              0x1B
#define CYGNUM_HAL_VECTOR_INTST0               0x1C
#define CYGNUM_HAL_VECTOR_INTCSI2              0x1D
#define CYGNUM_HAL_VECTOR_INTSER1              0x1E
#define CYGNUM_HAL_VECTOR_INTSR1               0x1F
#define CYGNUM_HAL_VECTOR_INTST1               0x20
#define CYGNUM_HAL_VECTOR_INTAD                0x21
#define CYGNUM_HAL_VECTOR_INTDMA0              0x22
#define CYGNUM_HAL_VECTOR_INTDMA1              0x23
#define CYGNUM_HAL_VECTOR_INTDMA2              0x24
#define CYGNUM_HAL_VECTOR_INTWTN               0x25

#define CYGNUM_HAL_VSR_MIN                     0
#define CYGNUM_HAL_VSR_MAX                     0x25
#define CYGNUM_HAL_VSR_COUNT                   0x26

#elif CYGINT_HAL_V850_VARIANT_SB1

#define CYGNUM_HAL_VECTOR_INTWTNI              0x14
#define CYGNUM_HAL_VECTOR_INTTM00              0x15
#define CYGNUM_HAL_VECTOR_INTTM01              0x16
#define CYGNUM_HAL_VECTOR_INTTM10              0x17
#define CYGNUM_HAL_VECTOR_INTTM11              0x18
#define CYGNUM_HAL_VECTOR_INTTM2               0x19
#define CYGNUM_HAL_VECTOR_INTTM3               0x1A
#define CYGNUM_HAL_VECTOR_INTTM4               0x1B
#define CYGNUM_HAL_VECTOR_INTTM5               0x1C
#define CYGNUM_HAL_VECTOR_INTTM6               0x1D
#define CYGNUM_HAL_VECTOR_INTTM7               0x1E
#define CYGNUM_HAL_VECTOR_INTIIC0              0x1F
#define CYGNUM_HAL_VECTOR_INTCSI0              0x1F
#define CYGNUM_HAL_VECTOR_INTSER0              0x20
#define CYGNUM_HAL_VECTOR_INTSR0               0x21
#define CYGNUM_HAL_VECTOR_INTCSI1              0x21
#define CYGNUM_HAL_VECTOR_INTST0               0x22
#define CYGNUM_HAL_VECTOR_INTCSI2              0x23
#define CYGNUM_HAL_VECTOR_INTIIC1              0x24
#define CYGNUM_HAL_VECTOR_INTSER1              0x25
#define CYGNUM_HAL_VECTOR_INTSR1               0x26
#define CYGNUM_HAL_VECTOR_INTCSI3              0x26
#define CYGNUM_HAL_VECTOR_INTST1               0x27
#define CYGNUM_HAL_VECTOR_INTCSI4              0x28
#ifdef __SB2
#define CYGNUM_HAL_VECTOR_INTIE1               0x29
#define CYGNUM_HAL_VECTOR_INTIE2               0x2A
#endif
#define CYGNUM_HAL_VECTOR_INTAD                0x2B
#define CYGNUM_HAL_VECTOR_INTDMA0              0x2C
#define CYGNUM_HAL_VECTOR_INTDMA1              0x2D
#define CYGNUM_HAL_VECTOR_INTDMA2              0x2E
#define CYGNUM_HAL_VECTOR_INTDMA3              0x2F
#define CYGNUM_HAL_VECTOR_INTDMA4              0x30
#define CYGNUM_HAL_VECTOR_INTDMA5              0x31
#define CYGNUM_HAL_VECTOR_INTWTN               0x32
#define CYGNUM_HAL_VECTOR_INTKR                0x33


#define CYGNUM_HAL_VSR_MIN                     0
#define CYGNUM_HAL_VSR_MAX                     0x33
#define CYGNUM_HAL_VSR_COUNT                   ((CYGNUM_HAL_VSR_MAX-CYGNUM_HAL_VSR_MIN)+1)

#else
# error No v850 variant implemented!
#endif

// Min/Max exception numbers and how many there are
#define CYGNUM_HAL_EXCEPTION_MIN                0
#define CYGNUM_HAL_EXCEPTION_MAX                7
#define CYGNUM_HAL_EXCEPTION_COUNT              ((CYGNUM_HAL_EXCEPTION_MAX-CYGNUM_HAL_EXCEPTION_MIN)+1)

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     0x08
#define CYGNUM_HAL_ISR_MAX                     CYGNUM_HAL_VSR_MAX
#define CYGNUM_HAL_ISR_COUNT                   ((CYGNUM_HAL_ISR_MAX-CYGNUM_HAL_ISR_MIN)+1)

// The vector used by the Real time clock.
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_VECTOR_INTTM10

// Mapping from interrupt numbers to hardware registers
#if CYGINT_HAL_V850_VARIANT_SA1
#define CYG_HAL_V85X_INTERRUPT_CONTROL_REGISTERS        \
    (volatile unsigned char *)V850_REG_WDTIC,           \
    (volatile unsigned char *)V850_REG_PIC0,            \
    (volatile unsigned char *)V850_REG_PIC1,            \
    (volatile unsigned char *)V850_REG_PIC2,            \
    (volatile unsigned char *)V850_REG_PIC3,            \
    (volatile unsigned char *)V850_REG_PIC4,            \
    (volatile unsigned char *)V850_REG_PIC5,            \
    (volatile unsigned char *)V850_REG_PIC6,            \
    (volatile unsigned char *)V850_REG_WTNIIC,          \
    (volatile unsigned char *)V850_REG_TMIC00,          \
    (volatile unsigned char *)V850_REG_TMIC01,          \
    (volatile unsigned char *)V850_REG_TMIC10,          \
    (volatile unsigned char *)V850_REG_TMIC11,          \
    (volatile unsigned char *)V850_REG_TMIC2,           \
    (volatile unsigned char *)V850_REG_TMIC3,           \
    (volatile unsigned char *)V850_REG_TMIC4,           \
    (volatile unsigned char *)V850_REG_TMIC5,           \
    (volatile unsigned char *)V850_REG_CSIC0,           \
    (volatile unsigned char *)V850_REG_SERIC0,          \
    (volatile unsigned char *)V850_REG_CSIC1,           \
    (volatile unsigned char *)V850_REG_STIC0,           \
    (volatile unsigned char *)V850_REG_CSIC2,           \
    (volatile unsigned char *)V850_REG_SERIC1,          \
    (volatile unsigned char *)V850_REG_SRIC1,           \
    (volatile unsigned char *)V850_REG_STIC1,           \
    (volatile unsigned char *)V850_REG_ADIC,            \
    (volatile unsigned char *)V850_REG_DMAIC0,          \
    (volatile unsigned char *)V850_REG_DMAIC1,          \
    (volatile unsigned char *)V850_REG_DMAIC2,          \
    (volatile unsigned char *)V850_REG_WTNIC

#elif CYGINT_HAL_V850_VARIANT_SB1
#define CYG_HAL_V85X_INTERRUPT_CONTROL_REGISTERS        \
    (volatile unsigned char *)V850_REG_WDTIC,           \
    (volatile unsigned char *)V850_REG_PIC0,            \
    (volatile unsigned char *)V850_REG_PIC1,            \
    (volatile unsigned char *)V850_REG_PIC2,            \
    (volatile unsigned char *)V850_REG_PIC3,            \
    (volatile unsigned char *)V850_REG_PIC4,            \
    (volatile unsigned char *)V850_REG_PIC5,            \
    (volatile unsigned char *)V850_REG_PIC6,            \
    NULL,                                               \
    NULL,                                               \
    NULL,                                               \
    NULL,                                               \
    (volatile unsigned char *)V850_REG_WTNIIC,          \
    (volatile unsigned char *)V850_REG_TMIC00,          \
    (volatile unsigned char *)V850_REG_TMIC01,          \
    (volatile unsigned char *)V850_REG_TMIC10,          \
    (volatile unsigned char *)V850_REG_TMIC11,          \
    (volatile unsigned char *)V850_REG_TMIC2,           \
    (volatile unsigned char *)V850_REG_TMIC3,           \
    (volatile unsigned char *)V850_REG_TMIC4,           \
    (volatile unsigned char *)V850_REG_TMIC5,           \
    (volatile unsigned char *)V850_REG_TMIC6,           \
    (volatile unsigned char *)V850_REG_TMIC7,           \
    (volatile unsigned char *)V850_REG_CSIC0,           \
    (volatile unsigned char *)V850_REG_SERIC0,          \
    (volatile unsigned char *)V850_REG_CSIC1,           \
    (volatile unsigned char *)V850_REG_STIC0,           \
    (volatile unsigned char *)V850_REG_CSIC2,           \
    (volatile unsigned char *)V850_REG_IICIC1,          \
    (volatile unsigned char *)V850_REG_SERIC1,          \
    (volatile unsigned char *)V850_REG_CSIC3,           \
    (volatile unsigned char *)V850_REG_STIC1,           \
    (volatile unsigned char *)V850_REG_CSIC4,           \
    (volatile unsigned char *)V850_REG_IEBIC1,          \
    (volatile unsigned char *)V850_REG_IEBIC2,          \
    (volatile unsigned char *)V850_REG_ADIC,            \
    (volatile unsigned char *)V850_REG_DMAIC0,          \
    (volatile unsigned char *)V850_REG_DMAIC1,          \
    (volatile unsigned char *)V850_REG_DMAIC2,          \
    (volatile unsigned char *)V850_REG_DMAIC3,          \
    (volatile unsigned char *)V850_REG_DMAIC4,          \
    (volatile unsigned char *)V850_REG_DMAIC5,          \
    (volatile unsigned char *)V850_REG_WTNIC,           \
    (volatile unsigned char *)V850_REG_KRIC

#endif // elif CYGINT_HAL_V850_VARIANT_SB1

//--------------------------------------------------------------------------
// Clock control

// This is handled by the default code

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
// End of var_intr.h
