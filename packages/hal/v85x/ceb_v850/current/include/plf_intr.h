#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H

//==========================================================================
//
//      plf_intr.h
//
//      NEC/V85x Interrupt and clock support
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
// Author(s):    nickg
// Contributors: nickg, jskov,
//               gthomas, jlarmour
// Date:         1999-02-16
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for the VRC4373 board.
//              
// Usage:
//              #include <cyg/hal/plf_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// NEC (70F3017) vectors. 

// These are the exception codes presented in the Cause register and
// correspond to VSRs. These values are the ones to use for HAL_VSR_GET/SET

#define CYGNUM_HAL_VECTOR_RESET                0x00
#define CYGNUM_HAL_VECTOR_NMI                  0x01
#define CYGNUM_HAL_VECTOR_WATCHDOG_TIMER       0x02
#define CYGNUM_HAL_VECTOR_TRAP0                0x04
#define CYGNUM_HAL_VECTOR_TRAP1                0x05
#define CYGNUM_HAL_VECTOR_ILLEGAL_OPCODE       0x06
#define CYGNUM_HAL_VECTOR_WATCHDOG_TIMER_M     0x08
#define CYGNUM_HAL_VECTOR_INTP0                0x09
#define CYGNUM_HAL_VECTOR_INTP1                0x0A
#define CYGNUM_HAL_VECTOR_INTP2                0x0B
#define CYGNUM_HAL_VECTOR_INTP3                0x0C
#define CYGNUM_HAL_VECTOR_INTP4                0x0D
#define CYGNUM_HAL_VECTOR_INTP5                0x0E
#define CYGNUM_HAL_VECTOR_INTP6                0x0F
#define CYGNUM_HAL_VECTOR_INTWTI               0x10
#define CYGNUM_HAL_VECTOR_INTTM00              0x11
#define CYGNUM_HAL_VECTOR_INTTM01              0x12
#define CYGNUM_HAL_VECTOR_INTTM10              0x13
#define CYGNUM_HAL_VECTOR_INTTM11              0x14
#define CYGNUM_HAL_VECTOR_INTTM2               0x15
#define CYGNUM_HAL_VECTOR_INTTM3               0x16
#define CYGNUM_HAL_VECTOR_INTTM4               0x17
#define CYGNUM_HAL_VECTOR_INTTM5               0x18
#define CYGNUM_HAL_VECTOR_INTCSI0              0x19
#define CYGNUM_HAL_VECTOR_INTSER0              0x1A
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
#define CYGNUM_HAL_VECTOR_INTWT                0x25

#define CYGNUM_HAL_VSR_MIN                     0
#define CYGNUM_HAL_VSR_MAX                     0x25
#define CYGNUM_HAL_VSR_COUNT                   0x26

// Min/Max exception numbers and how many there are
#define CYGNUM_HAL_EXCEPTION_MIN                0
#define CYGNUM_HAL_EXCEPTION_MAX                7
#define CYGNUM_HAL_EXCEPTION_COUNT              ((CYGNUM_HAL_EXCEPTION_MAX-CYGNUM_HAL_EXCEPTION_MIN)+1)

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     0x08
#define CYGNUM_HAL_ISR_MAX                     0x25
#define CYGNUM_HAL_ISR_COUNT                   ((CYGNUM_HAL_ISR_MAX-CYGNUM_HAL_ISR_MIN)+1)

// The vector used by the Real time clock.
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_VECTOR_INTTM10

// Vector used to detect ^C
#define CYGHWR_HAL_GDB_PORT_VECTOR             CYGNUM_HAL_VECTOR_INTCSI1

// Mapping from interrupt numbers to hardware registers
#define CYG_HAL_V85X_INTERRUPT_CONTROL_REGISTERS        \
    (volatile unsigned char *)V850_REG_WDTIC,           \
    (volatile unsigned char *)V850_REG_PIC0,            \
    (volatile unsigned char *)V850_REG_PIC1,            \
    (volatile unsigned char *)V850_REG_PIC2,            \
    (volatile unsigned char *)V850_REG_PIC3,            \
    (volatile unsigned char *)V850_REG_PIC4,            \
    (volatile unsigned char *)V850_REG_PIC5,            \
    (volatile unsigned char *)V850_REG_PIC6,            \
    (volatile unsigned char *)V850_REG_WTIIC,           \
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
    (volatile unsigned char *)V850_REG_SRIC0,           \
    (volatile unsigned char *)V850_REG_STIC0,           \
    (volatile unsigned char *)V850_REG_CSIC2,           \
    (volatile unsigned char *)V850_REG_SERIC1,          \
    (volatile unsigned char *)V850_REG_SRIC1,           \
    (volatile unsigned char *)V850_REG_STIC1,           \
    (volatile unsigned char *)V850_REG_ADIC,            \
    (volatile unsigned char *)V850_REG_DMAIC0,          \
    (volatile unsigned char *)V850_REG_DMAIC1,          \
    (volatile unsigned char *)V850_REG_DMAIC2,          \
    (volatile unsigned char *)V850_REG_WTIC

#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)

extern struct Hal_SavedRegisters *hal_saved_interrupt_state;
extern void hal_ctrlc_isr_init(void);
extern cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);
#define HAL_CTRLC_ISR(_v_,_d_)   hal_ctrlc_isr(_v_, _d_)
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
