#ifndef CYGONCE_HAL_INTR_H
#define CYGONCE_HAL_INTR_H

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
// Author(s):    jskov
// Contributors: jskov,
// Date:         1999-04-24
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock.
//              
// Usage:
//               #include <cyg/hal/hal_intr.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include CYGBLD_HAL_CPU_MODULES_H       // INTC module selection

// More include statements below. First part of this file must be
// usable for both assembly and C files, so only use defines here.

//--------------------------------------------------------------------------
// SH3 exception vectors. These correspond to VSRs and are the values
// to use for HAL_VSR_GET/SET
//
// Note that exceptions are decoded - there is a VSR slot for each exception
// source, while interrupts are handled via the same VSR.

#define CYGNUM_HAL_VECTOR_POWERON                0 // power-on
#define CYGNUM_HAL_VECTOR_RESET                  1 // reset
#define CYGNUM_HAL_VECTOR_TLBMISS_ACCESS         2 // TLB-miss/invalid load
#define CYGNUM_HAL_VECTOR_TLBMISS_WRITE          3 // TLB-miss/invalid store
#define CYGNUM_HAL_VECTOR_INITIAL_WRITE          4 // initial page write
#define CYGNUM_HAL_VECTOR_TLBERROR_ACCESS        5 // TLB prot violation l
#define CYGNUM_HAL_VECTOR_TLBERROR_WRITE         6 // TLB prot violation s
#define CYGNUM_HAL_VECTOR_DATA_ACCESS            7 // address error (load)
#define CYGNUM_HAL_VECTOR_DATA_WRITE             8 // address error (store)
// define CYGNUM_HAL_VECTOR_RESERVED_9           9
// define CYGNUM_HAL_VECTOR_RESERVED_10         10
#define CYGNUM_HAL_VECTOR_TRAP                  11 // unconditional trap
#define CYGNUM_HAL_VECTOR_ILLEGAL_INSTRUCTION   12 // reserved instruction
#define CYGNUM_HAL_VECTOR_ILLEGAL_SLOT_INSTRUCTION  13 
                                           // illegal instruction in delay slot
// define CYGNUM_HAL_VECTOR_RESERVED_14         14
#define CYGNUM_HAL_VECTOR_INSTRUCTION_BP        15 // user breakpoint

#define CYG_VECTOR_IS_INTERRUPT(v)   \
    (CYGNUM_HAL_VECTOR_INSTRUCTION_BP < (v))

#define CYGNUM_HAL_VECTOR_INTERRUPT             16 // all interrupts

#define CYGNUM_HAL_VSR_MIN                   CYGNUM_HAL_VECTOR_POWERON
#define CYGNUM_HAL_VSR_MAX                   CYGNUM_HAL_VECTOR_INTERRUPT
#define CYGNUM_HAL_VSR_COUNT                 ( CYGNUM_HAL_VSR_MAX + 1 )

#define CYGNUM_HAL_VSR_EXCEPTION_COUNT       (CYGNUM_HAL_VECTOR_INSTRUCTION_BP-CYGNUM_HAL_VECTOR_POWERON+1)



// The decoded interrupts.
#define CYGNUM_HAL_INTERRUPT_NMI             0
#define CYGNUM_HAL_INTERRUPT_RESERVED_1E0    1
#define CYGNUM_HAL_INTERRUPT_LVL0            2
#define CYGNUM_HAL_INTERRUPT_LVL1            3
#define CYGNUM_HAL_INTERRUPT_LVL2            4
#define CYGNUM_HAL_INTERRUPT_LVL3            5
#define CYGNUM_HAL_INTERRUPT_LVL4            6
#define CYGNUM_HAL_INTERRUPT_LVL5            7
#define CYGNUM_HAL_INTERRUPT_LVL6            8
#define CYGNUM_HAL_INTERRUPT_LVL7            9
#define CYGNUM_HAL_INTERRUPT_LVL8            10
#define CYGNUM_HAL_INTERRUPT_LVL9            11
#define CYGNUM_HAL_INTERRUPT_LVL10           12
#define CYGNUM_HAL_INTERRUPT_LVL11           13
#define CYGNUM_HAL_INTERRUPT_LVL12           14
#define CYGNUM_HAL_INTERRUPT_LVL13           15
#define CYGNUM_HAL_INTERRUPT_LVL14           16
#define CYGNUM_HAL_INTERRUPT_RESERVED_3E0    17
#define CYGNUM_HAL_INTERRUPT_TMU0_TUNI0      18
#define CYGNUM_HAL_INTERRUPT_TMU1_TUNI1      19
#define CYGNUM_HAL_INTERRUPT_TMU2_TUNI2      20
#define CYGNUM_HAL_INTERRUPT_TMU2_TICPI2     21
#define CYGNUM_HAL_INTERRUPT_RTC_ATI         22
#define CYGNUM_HAL_INTERRUPT_RTC_PRI         23
#define CYGNUM_HAL_INTERRUPT_RTC_CUI         24
#define CYGNUM_HAL_INTERRUPT_SCI_ERI         25
#define CYGNUM_HAL_INTERRUPT_SCI_RXI         26
#define CYGNUM_HAL_INTERRUPT_SCI_TXI         27
#define CYGNUM_HAL_INTERRUPT_SCI_TEI         28
#define CYGNUM_HAL_INTERRUPT_WDT_ITI         29
#define CYGNUM_HAL_INTERRUPT_REF_RCMI        30
#define CYGNUM_HAL_INTERRUPT_REF_ROVI        31

#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_REF_ROVI

//----------------------------------------------------------------------------
// Additional vectors provided by INTC V2

//#ifdef CYGARC_SH_MOD_INTC_V2 // FIXME
#define CYGNUM_HAL_INTERRUPT_RESERVED_5C0    32
#define CYGNUM_HAL_INTERRUPT_HUDI_HUDI       33
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ0        34
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ1        35
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ2        36
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ3        37
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ4        38
#define CYGNUM_HAL_INTERRUPT_IRQ_IRQ5        39
#define CYGNUM_HAL_INTERRUPT_RESERVED_6C0    40
#define CYGNUM_HAL_INTERRUPT_RESERVED_6E0    41
#define CYGNUM_HAL_INTERRUPT_PINT_PINT07     42
#define CYGNUM_HAL_INTERRUPT_PINT_PINT8F     43
#define CYGNUM_HAL_INTERRUPT_RESERVED_740    44
#define CYGNUM_HAL_INTERRUPT_RESERVED_760    45
#define CYGNUM_HAL_INTERRUPT_RESERVED_780    46
#define CYGNUM_HAL_INTERRUPT_RESERVED_7A0    47
#define CYGNUM_HAL_INTERRUPT_RESERVED_7C0    48
#define CYGNUM_HAL_INTERRUPT_RESERVED_7E0    59
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI0       50
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI1       51
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI2       52
#define CYGNUM_HAL_INTERRUPT_DMAC_DEI3       53
#define CYGNUM_HAL_INTERRUPT_IRDA_ERI1       54
#define CYGNUM_HAL_INTERRUPT_IRDA_RXI1       55
#define CYGNUM_HAL_INTERRUPT_IRDA_BRI1       56
#define CYGNUM_HAL_INTERRUPT_IRDA_TXI1       57
#define CYGNUM_HAL_INTERRUPT_SCIF_ERI2       58
#define CYGNUM_HAL_INTERRUPT_SCIF_RXI2       69
#define CYGNUM_HAL_INTERRUPT_SCIF_BRI2       60
#define CYGNUM_HAL_INTERRUPT_SCIF_TXI2       61
#define CYGNUM_HAL_INTERRUPT_ADC_ADI         62

#ifdef CYGARC_SH_MOD_INTC_V2 // FIXME

#undef  CYGNUM_HAL_ISR_MAX
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_ADC_ADI

#endif // CYGARC_SH_MOD_INTC_V2

//----------------------------------------------------------------------------
// Additional vectors provided by INTC V3

//#ifdef CYGARC_SH_MOD_INTC_V3 // FIXME
#define CYGNUM_HAL_INTERRUPT_LCDC_LCDI       63
#define CYGNUM_HAL_INTERRUPT_PCC_PCC0        64
#define CYGNUM_HAL_INTERRUPT_PCC_PCC1        65

#ifdef CYGARC_SH_MOD_INTC_V3 // FIXME

#undef  CYGNUM_HAL_ISR_MAX
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_PCC_PCC1

#endif // CYGARC_SH_MOD_INTC_V3

// CYGNUM_HAL_ISR_COUNT must match CYG_ISR_TABLE_SIZE defined in vectors.S.
#define CYGNUM_HAL_ISR_MIN                   CYGNUM_HAL_INTERRUPT_NMI
#define CYGNUM_HAL_ISR_COUNT                 ( CYGNUM_HAL_ISR_MAX + 1 )

// Exception vectors. These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception()

// The exception indexes are EXPEVT/0x20

#define CYGNUM_HAL_EXCEPTION_POWERON                0 // power-on
#define CYGNUM_HAL_EXCEPTION_RESET                  1 // reset
#define CYGNUM_HAL_EXCEPTION_TLBMISS_ACCESS         2 // TLB-miss/invalid load
#define CYGNUM_HAL_EXCEPTION_TLBMISS_WRITE          3 // TLB-miss/invalid store
#define CYGNUM_HAL_EXCEPTION_INITIAL_WRITE          4 // initial page write
#define CYGNUM_HAL_EXCEPTION_TLBERROR_ACCESS        5 // TLB prot violation l
#define CYGNUM_HAL_EXCEPTION_TLBERROR_WRITE         6 // TLB prot violation s
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS            7 // address error (load)
#define CYGNUM_HAL_EXCEPTION_DATA_WRITE             8 // address error (store)
// define CYGNUM_HAL_EXCEPTION_RESERVED_9           9
// define CYGNUM_HAL_EXCEPTION_RESERVED_10         10
#define CYGNUM_HAL_EXCEPTION_TRAP                  11 // unconditional trap
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION   12 // reserved instruction
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_SLOT_INSTRUCTION  13 
                                           // illegal instruction in delay slot
// define CYGNUM_HAL_EXCEPTION_RESERVED_14         14
#define CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP        15 // user breakpoint

#define CYGNUM_HAL_EXCEPTION_MIN          CYGNUM_HAL_EXCEPTION_POWERON
#define CYGNUM_HAL_EXCEPTION_MAX          CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP
#define CYGNUM_HAL_EXCEPTION_COUNT           \
                 ( CYGNUM_HAL_EXCEPTION_MAX - CYGNUM_HAL_EXCEPTION_MIN + 1 )

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC             CYGNUM_HAL_INTERRUPT_TMU0_TUNI0

#ifndef __ASSEMBLER__

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/sh_regs.h>            // register definitions
#include <cyg/hal/hal_io.h>             // io macros
#include <cyg/infra/cyg_ass.h>          // CYG_FAIL

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];
// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

//--------------------------------------------------------------------------
// Default ISR
// The #define is used to test whether this routine exists, and to allow
// us to call it.

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

#define HAL_DEFAULT_ISR hal_default_isr

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//--------------------------------------------------------------------------
// Interrupt control macros
//
// Note that these macros control interrupt state by setting the Imask
// of the SR rather than the (more obvious) BL. This is because a CPU
// reset is forced if execptions (such as breakpoints) are generated
// while the BL flag is set.

#define HAL_DISABLE_INTERRUPTS(_old_)           \
    CYG_MACRO_START                             \
    cyg_uint32 _tmp_;                           \
    asm volatile (                              \
        "stc    sr,%1    \n\t"                  \
        "mov    %2,%0    \n\t"                  \
        "and    %1,%0    \n\t"                  \
        "or     %2,%1    \n\t"                  \
        "ldc    %1,sr    \n\t"                  \
        : "=&r"(_old_), "=&r" (_tmp_)           \
        : "r" (CYGARC_REG_SR_IMASK)             \
        );                                      \
    CYG_MACRO_END

#define HAL_ENABLE_INTERRUPTS()                 \
    CYG_MACRO_START                             \
    cyg_uint32 _tmp_;                           \
    asm volatile (                              \
        "stc    sr,%0    \n\t"                  \
        "and    %1,%0    \n\t"                  \
        "ldc    %0,sr    \n\t"                  \
        : "=&r" (_tmp_)                         \
        : "r" (~CYGARC_REG_SR_IMASK)            \
        );                                      \
    CYG_MACRO_END

#define HAL_RESTORE_INTERRUPTS(_old_)                   \
    CYG_MACRO_START                                     \
    cyg_uint32 _tmp1_, _tmp2_;                          \
    asm volatile (                                      \
        "stc    sr,%0    \n\t"                          \
        "and    %3,%0    \n\t"                          \
        "not    %3,%1    \n\t"                          \
        "and    %2,%1    \n\t"                          \
        "or     %1,%0    \n\t"                          \
        "ldc    %0,sr    \n\t"                          \
        : "=&r" (_tmp1_), "=&r" (_tmp2_)                \
        : "r" (_old_), "r" (~CYGARC_REG_SR_IMASK)       \
        );                                              \
    CYG_MACRO_END

#define HAL_QUERY_INTERRUPTS(_old_)             \
    CYG_MACRO_START                             \
    asm volatile (                              \
        "stc    sr,%0    \n\t"                  \
        "and    %1,%0    \n\t"                  \
        : "=&r"(_old_)                          \
        : "r" (CYGARC_REG_SR_IMASK)             \
        );                                      \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// Vector translation.

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN

# define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = 0

#else

# define HAL_TRANSLATE_VECTOR(_vector_,_index_) (_index_) = (_vector_)

#endif

//--------------------------------------------------------------------------
// Routine to execute DSRs using separate interrupt stack

#ifdef  CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

externC void hal_interrupt_stack_call_pending_DSRs(void);
#define HAL_INTERRUPT_STACK_CALL_PENDING_DSRS() \
    hal_interrupt_stack_call_pending_DSRs()

// these are offered solely for stack usage testing
// if they are not defined, then there is no interrupt stack.
#define HAL_INTERRUPT_STACK_BASE cyg_interrupt_stack_base
#define HAL_INTERRUPT_STACK_TOP  cyg_interrupt_stack
// use them to declare these extern however you want:
//       extern char HAL_INTERRUPT_STACK_BASE[];
//       extern char HAL_INTERRUPT_STACK_TOP[];
// is recommended
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

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )         \
    CYG_MACRO_START                                                       \
    cyg_uint32 _index_;                                                   \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                           \
                                                                          \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr ) \
    {                                                                     \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;             \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD) _data_;              \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;           \
    }                                                                     \
    CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
    CYG_MACRO_START                                                     \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                         \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )         \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)hal_default_isr; \
        hal_interrupt_data[_index_] = 0;                                \
        hal_interrupt_objects[_index_] = 0;                             \
    }                                                                   \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *(CYG_ADDRESS *)(_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )               \
    CYG_MACRO_START                                             \
    if( _poldvsr_ != NULL )                                     \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];    \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;               \
    CYG_MACRO_END

// This is an ugly name, but what it means is: grab the VSR back to eCos
// internal handling, or if you like, the default handler.  But if
// cooperating with GDB and CygMon, the default behaviour is to pass most
// exceptions to CygMon.  This macro undoes that so that eCos handles the
// exception.  So use it with care.

externC void cyg_hal_default_interrupt_vsr( void );
externC void cyg_hal_default_exception_vsr( void );
#define HAL_VSR_SET_TO_ECOS_HANDLER( _vector_, _poldvsr_ )                    \
    CYG_MACRO_START                                                           \
    if( (void*)_poldvsr_ != (void*)NULL )                                     \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];                  \
    hal_vsr_table[_vector_] = ( CYG_VECTOR_IS_INTERRUPT( _vector_ )           \
                              ? (CYG_ADDRESS)cyg_hal_default_interrupt_vsr    \
                              : (CYG_ADDRESS)cyg_hal_default_exception_vsr ); \
    CYG_MACRO_END


//--------------------------------------------------------------------------
// Interrupt controller access
//
// Note: These macros work only on a unit basis ; if one source in a
// unit (such as SCI) is unmasked, all sources of that unit get
// unmasked.  Finer control must be done in the driver/apps.

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
externC cyg_uint8 cyg_hal_ILVL_table[];
#define HAL_UPDATE_ILVL_TABLE(_vector_, _level_)                         \
    /* Update the ILVL table, so it is easy for the interrupt entry */   \
    /* code to find out the level of a given interrupt source.      */   \
    cyg_hal_ILVL_table[(_vector_)] = (_level_)
#else
#define HAL_UPDATE_ILVL_TABLE(_vector_, _level_)
#endif

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )                     \
    CYG_MACRO_START                                                      \
    cyg_uint16 iprX;                                                     \
                                                                         \
    CYG_ASSERT((0 <= (_level_) && 15 >= (_level_)), "Illegal level");    \
    CYG_ASSERT((CYGNUM_HAL_ISR_MIN <= (_vector_)                         \
                && CYGNUM_HAL_ISR_MAX >= (_vector_)), "Illegal vector"); \
                                                                         \
    HAL_UPDATE_ILVL_TABLE(_vector_, _level_);                            \
                                                                         \
    switch( (_vector_) ) {                                               \
    case CYGNUM_HAL_INTERRUPT_NMI:                                       \
        /* fall through */                                               \
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:         \
        /* Cannot change levels */                                       \
        break;                                                           \
                                                                         \
    /* IPRA */                                                           \
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0:                                \
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);                          \
        iprX &= ~CYGARC_REG_IPRA_TMU0_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRA_TMU0_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_TMU1_TUNI1:                                \
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);                          \
        iprX &= ~CYGARC_REG_IPRA_TMU1_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRA_TMU1_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_TMU2_TUNI2:                                \
    case CYGNUM_HAL_INTERRUPT_TMU2_TICPI2:                               \
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);                          \
        iprX &= ~CYGARC_REG_IPRA_TMU2_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRA_TMU2_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_RTC_ATI:                                   \
    case CYGNUM_HAL_INTERRUPT_RTC_PRI:                                   \
    case CYGNUM_HAL_INTERRUPT_RTC_CUI:                                   \
        HAL_READ_UINT16(CYGARC_REG_IPRA, iprX);                          \
        iprX &= ~CYGARC_REG_IPRA_RTC_MASK;                               \
        iprX |= (_level_)*CYGARC_REG_IPRA_RTC_PRI1;                      \
        HAL_WRITE_UINT16(CYGARC_REG_IPRA, iprX);                         \
        break;                                                           \
                                                                         \
    /* IPRB */                                                           \
    case CYGNUM_HAL_INTERRUPT_SCI_ERI:                                   \
    case CYGNUM_HAL_INTERRUPT_SCI_RXI:                                   \
    case CYGNUM_HAL_INTERRUPT_SCI_TXI:                                   \
    case CYGNUM_HAL_INTERRUPT_SCI_TEI:                                   \
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);                          \
        iprX &= ~CYGARC_REG_IPRB_SCI_MASK;                               \
        iprX |= (_level_)*CYGARC_REG_IPRB_SCI_PRI1;                      \
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_WDT_ITI:                                   \
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);                          \
        iprX &= ~CYGARC_REG_IPRB_WDT_MASK;                               \
        iprX |= (_level_)*CYGARC_REG_IPRB_WDT_PRI1;                      \
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_REF_RCMI:                                  \
    case CYGNUM_HAL_INTERRUPT_REF_ROVI:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRB, iprX);                          \
        iprX &= ~CYGARC_REG_IPRB_REF_MASK;                               \
        iprX |= (_level_)*CYGARC_REG_IPRB_REF_PRI1;                      \
        HAL_WRITE_UINT16(CYGARC_REG_IPRB, iprX);                         \
        break;                                                           \
                                                                         \
    /* IPRC */                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          \
        iprX &= ~CYGARC_REG_IPRC_IRQ0_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRC_IRQ0_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          \
        iprX &= ~CYGARC_REG_IPRC_IRQ1_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRC_IRQ1_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          \
        iprX &= ~CYGARC_REG_IPRC_IRQ2_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRC_IRQ2_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRC, iprX);                          \
        iprX &= ~CYGARC_REG_IPRC_IRQ3_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRC_IRQ3_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRC, iprX);                         \
        break;                                                           \
                                                                         \
    /* IPRD */                                                           \
    case CYGNUM_HAL_INTERRUPT_PINT_PINT07:                               \
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          \
        iprX &= ~CYGARC_REG_IPRD_PINT07_MASK;                            \
        iprX |= (_level_)*CYGARC_REG_IPRD_PINT07_PRI1;                   \
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_PINT_PINT8F:                               \
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          \
        iprX &= ~CYGARC_REG_IPRD_PINT8F_MASK;                            \
        iprX |= (_level_)*CYGARC_REG_IPRD_PINT8F_PRI1;                   \
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          \
        iprX &= ~CYGARC_REG_IPRD_IRQ5_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRD_IRQ5_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRD, iprX);                          \
        iprX &= ~CYGARC_REG_IPRD_IRQ4_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRD_IRQ4_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRD, iprX);                         \
        break;                                                           \
                                                                         \
    /* IPRE */                                                           \
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI0:                                 \
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI1:                                 \
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI2:                                 \
    case CYGNUM_HAL_INTERRUPT_DMAC_DEI3:                                 \
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          \
        iprX &= ~CYGARC_REG_IPRE_DMAC_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRE_DMAC_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_IRDA_ERI1:                                 \
    case CYGNUM_HAL_INTERRUPT_IRDA_RXI1:                                 \
    case CYGNUM_HAL_INTERRUPT_IRDA_BRI1:                                 \
    case CYGNUM_HAL_INTERRUPT_IRDA_TXI1:                                 \
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          \
        iprX &= ~CYGARC_REG_IPRE_IRDA_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRE_IRDA_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_SCIF_ERI2:                                 \
    case CYGNUM_HAL_INTERRUPT_SCIF_RXI2:                                 \
    case CYGNUM_HAL_INTERRUPT_SCIF_BRI2:                                 \
    case CYGNUM_HAL_INTERRUPT_SCIF_TXI2:                                 \
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          \
        iprX &= ~CYGARC_REG_IPRE_SCIF_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRE_SCIF_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_ADC_ADI:                                   \
        HAL_READ_UINT16(CYGARC_REG_IPRE, iprX);                          \
        iprX &= ~CYGARC_REG_IPRE_ADC_MASK;                               \
        iprX |= (_level_)*CYGARC_REG_IPRE_ADC_PRI1;                      \
        HAL_WRITE_UINT16(CYGARC_REG_IPRE, iprX);                         \
        break;                                                           \
                                                                         \
    /* IPRF */                                                           \
    case CYGNUM_HAL_INTERRUPT_LCDC_LCDI:                                 \
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          \
        iprX &= ~CYGARC_REG_IPRF_LCDI_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRF_LCDI_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_PCC_PCC0:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          \
        iprX &= ~CYGARC_REG_IPRF_PCC0_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRF_PCC0_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         \
        break;                                                           \
    case CYGNUM_HAL_INTERRUPT_PCC_PCC1:                                  \
        HAL_READ_UINT16(CYGARC_REG_IPRF, iprX);                          \
        iprX &= ~CYGARC_REG_IPRF_PCC1_MASK;                              \
        iprX |= (_level_)*CYGARC_REG_IPRF_PCC1_PRI1;                     \
        HAL_WRITE_UINT16(CYGARC_REG_IPRF, iprX);                         \
        break;                                                           \
                                                                         \
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:                              \
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:                              \
        /* Do nothing for these reserved vectors. */                     \
        break;                                                           \
    }                                                                    \
    CYG_MACRO_END

#define HAL_INTERRUPT_MASK( _vector_ )                                    \
    CYG_MACRO_START                                                       \
    switch( (_vector_) ) {                                                \
    case CYGNUM_HAL_INTERRUPT_NMI:                                        \
        /* fall through */                                                \
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:          \
        /* Can only be masked by fiddling Imask in SR. */                 \
        break;                                                            \
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0...CYGNUM_HAL_ISR_MAX:            \
        HAL_INTERRUPT_SET_LEVEL((_vector_), 0);                           \
        break;                                                            \
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:                               \
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:                               \
        /* Do nothing for these reserved vectors. */                      \
        break;                                                            \
    default:                                                              \
        CYG_FAIL("Unknown interrupt vector");                             \
        break;                                                            \
    }                                                                     \
    CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ )                                  \
    CYG_MACRO_START                                                       \
    switch( (_vector_) ) {                                                \
    case CYGNUM_HAL_INTERRUPT_NMI:                                        \
        /* fall through */                                                \
    case CYGNUM_HAL_INTERRUPT_LVL0...CYGNUM_HAL_INTERRUPT_LVL14:          \
        /* Can only be unmasked by fiddling Imask in SR. */               \
        break;                                                            \
    case CYGNUM_HAL_INTERRUPT_TMU0_TUNI0...CYGNUM_HAL_ISR_MAX:            \
        HAL_INTERRUPT_SET_LEVEL((_vector_), 1);                           \
        break;                                                            \
    case CYGNUM_HAL_INTERRUPT_RESERVED_1E0:                               \
    case CYGNUM_HAL_INTERRUPT_RESERVED_3E0:                               \
        /* Do nothing for these reserved vectors. */                      \
        break;                                                            \
    default:                                                              \
        CYG_FAIL("Unknown interrupt vector");                             \
        break;                                                            \
    }                                                                     \
    CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                             \
    CYG_MACRO_START                                                       \
    if ( (_vector_) >= CYGNUM_HAL_INTERRUPT_IRQ_IRQ0                      \
         && (_vector_) <= CYGNUM_HAL_INTERRUPT_IRQ_IRQ5) {                \
                                                                          \
        cyg_uint8 irr0;                                                   \
                                                                          \
        HAL_READ_UINT8(CYGARC_REG_IRR0, irr0);                            \
        switch ( _vector_ ) {                                             \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ0;                                \
            break;                                                        \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ1;                                \
            break;                                                        \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ2;                                \
            break;                                                        \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ3;                                \
            break;                                                        \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ4;                                \
            break;                                                        \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:                               \
            irr0 &= ~CYGARC_REG_IRR0_IRQ5;                                \
            break;                                                        \
        }                                                                 \
        HAL_WRITE_UINT8(CYGARC_REG_IRR0, irr0);                           \
    }                                                                     \
    CYG_MACRO_END

// Note: The PINTs can be masked and configured individually, even
// though there are only two vectors. Maybe add some fake vectors just
// for masking/configuring?
#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )               \
    CYG_MACRO_START                                                      \
    if ( (_vector_) >= CYGNUM_HAL_INTERRUPT_IRQ_IRQ0                     \
         && (_vector_) <= CYGNUM_HAL_INTERRUPT_IRQ_IRQ5) {               \
                                                                         \
        cyg_uint16 icr1, ss, mask;                                       \
        ss = 0;                                                          \
        mask = CYGARC_REG_ICR1_SENSE_UP|CYGARC_REG_ICR1_SENSE_LEVEL;     \
        if (_up_) ss |= CYGARC_REG_ICR1_SENSE_UP;                        \
        if (_level_) ss |= CYGARC_REG_ICR1_SENSE_LEVEL;                  \
        CYG_ASSERT(!(_up_ && _level_), "Cannot trigger on high level!"); \
                                                                         \
        switch( (_vector_) ) {                                           \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ5:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ5_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ5_shift;                   \
            break;                                                       \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ4:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ4_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ4_shift;                   \
            break;                                                       \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ3:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ3_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ3_shift;                   \
            break;                                                       \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ2:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ2_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ2_shift;                   \
            break;                                                       \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ1:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ1_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ1_shift;                   \
            break;                                                       \
        case CYGNUM_HAL_INTERRUPT_IRQ_IRQ0:                              \
            ss <<= CYGARC_REG_ICR1_SENSE_IRQ0_shift;                     \
            mask <<= CYGARC_REG_ICR1_SENSE_IRQ0_shift;                   \
            break;                                                       \
        }                                                                \
                                                                         \
        HAL_READ_UINT16(CYGARC_REG_ICR1, icr1);                          \
        icr1 &= ~mask;                                                   \
        icr1 |= ss;                                                      \
        HAL_WRITE_UINT16(CYGARC_REG_ICR1, icr1);                         \
    }                                                                    \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// Clock control

// Using TMU counter 0, clocked at p/4, with peripheral clock
// sourced directly from the 15MHz crystal (which is the default setup
// on the EDK).  This means 150000/4 oscillations in 1/100 second.
#define HAL_CLOCK_INITIALIZE( _period_ )                                \
    CYG_MACRO_START                                                     \
    register cyg_uint8 _tstr_;                                          \
                                                                        \
    /* Disable timer while programming it. */                           \
    HAL_READ_UINT8(CYGARC_REG_TSTR, _tstr_);                            \
    _tstr_ &= ~CYGARC_REG_TSTR_STR0;                                    \
    HAL_WRITE_UINT8(CYGARC_REG_TSTR, _tstr_);                           \
                                                                        \
    /* Set counter registers. */                                        \
    HAL_WRITE_UINT32(CYGARC_REG_TCOR0, (_period_));                     \
    HAL_WRITE_UINT32(CYGARC_REG_TCNT0, (_period_));                     \
                                                                        \
    /* Set interrupt on underflow, decrement frequency at 1/4 of */     \
    /* peripheral clock.                                         */     \
    HAL_WRITE_UINT16(CYGARC_REG_TCR0, CYGARC_REG_TCR_UNIE);             \
                                                                        \
    /* Enable timer. */                                                 \
    _tstr_ |= CYGARC_REG_TSTR_STR0;                                     \
    HAL_WRITE_UINT8(CYGARC_REG_TSTR, _tstr_);                           \
                                                                        \
    CYG_MACRO_END

#define HAL_CLOCK_RESET( _vector_, _period_ )           \
    CYG_MACRO_START                                     \
    register cyg_uint16 _tcr_;                          \
                                                        \
    /* Clear underflow flag. */                         \
    HAL_READ_UINT16(CYGARC_REG_TCR0, _tcr_);            \
    _tcr_ &= ~CYGARC_REG_TCR_UNF;                       \
    HAL_WRITE_UINT16(CYGARC_REG_TCR0, _tcr_);           \
    HAL_READ_UINT16(CYGARC_REG_TCR0, _tcr_);            \
                                                        \
    CYG_MACRO_END

#define HAL_CLOCK_READ( _pvalue_ )                              \
    CYG_MACRO_START                                             \
    register cyg_uint32 _result_;                               \
                                                                \
    HAL_READ_UINT32(CYGARC_REG_TCNT0, _result_);                \
                                                                \
    *(_pvalue_) = CYGNUM_KERNEL_COUNTERS_RTC_PERIOD-_result_;   \
    CYG_MACRO_END

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ ) HAL_CLOCK_READ(_pvalue_)
#endif

extern void hal_delay_us(int);
#define HAL_DELAY_US(n) hal_delay_us(n)

#endif // __ASSEMBLER__

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_INTR_H
// End of hal_intr.h
