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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jskov,
//               jlarmour
// Date:         1999-02-19
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

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/ppc_regs.h>           // register definitions
#include <cyg/hal/hal_io.h>             // io macros
#include <cyg/infra/cyg_ass.h>          // CYG_FAIL

//--------------------------------------------------------------------------
// PowerPC exception vectors. These correspond to VSRs and are the values
// to use for HAL_VSR_GET/SET

#define CYGNUM_HAL_VECTOR_RESERVED_0        0
#define CYGNUM_HAL_VECTOR_RESET             1
#define CYGNUM_HAL_VECTOR_MACHINE_CHECK     2
#define CYGNUM_HAL_VECTOR_DSI               3
#define CYGNUM_HAL_VECTOR_ISI               4
#define CYGNUM_HAL_VECTOR_INTERRUPT         5
#define CYGNUM_HAL_VECTOR_ALIGNMENT         6
#define CYGNUM_HAL_VECTOR_PROGRAM           7
#define CYGNUM_HAL_VECTOR_FP_UNAVAILABLE    8
#define CYGNUM_HAL_VECTOR_DECREMENTER       9
#define CYGNUM_HAL_VECTOR_RESERVED_A        10
#define CYGNUM_HAL_VECTOR_RESERVED_B        11
#define CYGNUM_HAL_VECTOR_SYSTEM_CALL       12
#define CYGNUM_HAL_VECTOR_TRACE             13
#define CYGNUM_HAL_VECTOR_FP_ASSIST         14

#define CYGNUM_HAL_VSR_MIN                   CYGNUM_HAL_VECTOR_RESERVED_0
#define CYGNUM_HAL_VSR_MAX                   CYGNUM_HAL_VECTOR_FP_ASSIST
#define CYGNUM_HAL_VSR_COUNT                 ( CYGNUM_HAL_VSR_MAX + 1 )

// The decoded interrupts.
// Define decrementer as the first interrupt since it is guaranteed to
// be defined on all PowerPCs. External may expand into several interrupts
// depending on interrupt controller capabilities.
#define CYGNUM_HAL_INTERRUPT_DECREMENTER     0
#define CYGNUM_HAL_INTERRUPT_EXTERNAL        1

// CYGNUM_HAL_ISR_COUNT must match CYG_ISR_TABLE_SIZE defined in vectors.S.
#define CYGNUM_HAL_ISR_MIN                   CYGNUM_HAL_INTERRUPT_DECREMENTER
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_EXTERNAL
#define CYGNUM_HAL_ISR_COUNT                 ( CYGNUM_HAL_ISR_MAX + 1 )

// Exception vectors. These are the values used when passed out to an
// external exception handler using cyg_hal_deliver_exception()

#define CYGNUM_HAL_EXCEPTION_RESERVED_0      CYGNUM_HAL_VECTOR_RESERVED_0

#ifdef CYG_HAL_POWERPC_MPC860
// The MPC860 does not generate DSI and ISI: instead it goes to machine
// check, so that a software VM system can then call into vectors 0x300 or
// 0x400 if the address is truly invalid rather than merely not in the TLB
// right now.  Shades of IBM wanting to port OS/MVS here!
// See pp 7-9/10 in "PowerQUICC - MPC860 User's Manual"
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS     CYGNUM_HAL_VECTOR_MACHINE_CHECK
// do not define catchers for DSI and ISI - should never happen.
#else
// Sensible PowerPCs that do what the architecture suggests.  See 6-25...29
// in "PowerPC Microprocessor Family: the Programming Environments"
// (Note: eCos libc does not catch CYGNUM_HAL_EXCEPTION_MACHINE_CHECK)
#define CYGNUM_HAL_EXCEPTION_MACHINE_CHECK   CYGNUM_HAL_VECTOR_MACHINE_CHECK
#define CYGNUM_HAL_EXCEPTION_DATA_ACCESS     CYGNUM_HAL_VECTOR_DSI
#define CYGNUM_HAL_EXCEPTION_CODE_ACCESS     CYGNUM_HAL_VECTOR_ISI
#endif // !CYG_HAL_POWERPC_MPC860 : DSI and ISI exceptions generated

#define CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS  \
           CYGNUM_HAL_VECTOR_ALIGNMENT
#define CYGNUM_HAL_EXCEPTION_FPU_NOT_AVAIL   CYGNUM_HAL_VECTOR_FP_UNAVAILABLE
#define CYGNUM_HAL_EXCEPTION_RESERVED_A      CYGNUM_HAL_VECTOR_RESERVED_A
#define CYGNUM_HAL_EXCEPTION_RESERVED_B      CYGNUM_HAL_VECTOR_RESERVED_B
#define CYGNUM_HAL_EXCEPTION_SYSTEM_CALL     CYGNUM_HAL_VECTOR_SYSTEM_CALL
#define CYGNUM_HAL_EXCEPTION_TRACE           CYGNUM_HAL_VECTOR_TRACE
#define CYGNUM_HAL_EXCEPTION_FP_ASSIST       CYGNUM_HAL_VECTOR_FP_ASSIST

// decoded exception vectors
#define CYGNUM_HAL_EXCEPTION_TRAP                     (-1)
#define CYGNUM_HAL_EXCEPTION_PRIVILEGED_INSTRUCTION   (-2)
#define CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION      (-3)
#define CYGNUM_HAL_EXCEPTION_FPU                      (-4)

#define CYGNUM_HAL_EXCEPTION_MIN             CYGNUM_HAL_EXCEPTION_FPU
#define CYGNUM_HAL_EXCEPTION_MAX             CYGNUM_HAL_VECTOR_FP_ASSIST
#define CYGNUM_HAL_EXCEPTION_COUNT           \
                 ( CYGNUM_HAL_EXCEPTION_MAX - CYGNUM_HAL_EXCEPTION_MIN + 1 )


#ifdef CYG_HAL_POWERPC_MPC8xx
// Additional exceptions on the MPC8xx
#define CYGNUM_HAL_VECTOR_RESERVED_F         15
#define CYGNUM_HAL_VECTOR_SW_EMUL            16
#define CYGNUM_HAL_VECTOR_ITLB_MISS          17
#define CYGNUM_HAL_VECTOR_DTLB_MISS          18
#define CYGNUM_HAL_VECTOR_ITLB_ERROR         19
#define CYGNUM_HAL_VECTOR_DTLB_ERROR         20
#define CYGNUM_HAL_VECTOR_RESERVED_15        21
#define CYGNUM_HAL_VECTOR_RESERVED_16        22
#define CYGNUM_HAL_VECTOR_RESERVED_17        23
#define CYGNUM_HAL_VECTOR_RESERVED_18        24
#define CYGNUM_HAL_VECTOR_RESERVED_19        25
#define CYGNUM_HAL_VECTOR_RESERVED_1A        26
#define CYGNUM_HAL_VECTOR_RESERVED_1B        27
#define CYGNUM_HAL_VECTOR_DATA_BP            28
#define CYGNUM_HAL_VECTOR_INSTRUCTION_BP     29
#define CYGNUM_HAL_VECTOR_PERIPHERAL_BP      30
#define CYGNUM_HAL_VECTOR_NMI                31

#define CYGNUM_HAL_EXCEPTION_RESERVED_F      CYGNUM_HAL_VECTOR_RESERVED_F
#define CYGNUM_HAL_EXCEPTION_SW_EMUL         CYGNUM_HAL_VECTOR_SW_EMUL
#define CYGNUM_HAL_EXCEPTION_CODE_TLBMISS_ACCESS  CYGNUM_HAL_VECTOR_ITLB_MISS
#define CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_ACCESS  CYGNUM_HAL_VECTOR_DTLB_MISS
#define CYGNUM_HAL_EXCEPTION_CODE_TLBERROR_ACCESS \
               CYGNUM_HAL_VECTOR_ITLB_ERROR
#define CYGNUM_HAL_EXCEPTION_DATA_TLBERROR_ACCESS \
           CYGNUM_HAL_VECTOR_DTLB_ERROR
#define CYGNUM_HAL_EXCEPTION_RESERVED_15     CYGNUM_HAL_VECTOR_RESERVED_15
#define CYGNUM_HAL_EXCEPTION_RESERVED_16     CYGNUM_HAL_VECTOR_RESERVED_16
#define CYGNUM_HAL_EXCEPTION_RESERVED_17     CYGNUM_HAL_VECTOR_RESERVED_17
#define CYGNUM_HAL_EXCEPTION_RESERVED_18     CYGNUM_HAL_VECTOR_RESERVED_18
#define CYGNUM_HAL_EXCEPTION_RESERVED_19     CYGNUM_HAL_VECTOR_RESERVED_19
#define CYGNUM_HAL_EXCEPTION_RESERVED_1A     CYGNUM_HAL_VECTOR_RESERVED_1A
#define CYGNUM_HAL_EXCEPTION_RESERVED_1B     CYGNUM_HAL_VECTOR_RESERVED_1B
#define CYGNUM_HAL_EXCEPTION_DATA_BP         CYGNUM_HAL_VECTOR_DATA_BP
#define CYGNUM_HAL_EXCEPTION_INSTRUCTION_BP  CYGNUM_HAL_VECTOR_INSTRUCTION_BP
#define CYGNUM_HAL_EXCEPTION_PERIPHERAL_BP   CYGNUM_HAL_VECTOR_PERIPHERAL_BP
#define CYGNUM_HAL_EXCEPTION_NMI             CYGNUM_HAL_VECTOR_NMI

#undef  CYGNUM_HAL_EXCEPTION_MAX
#define CYGNUM_HAL_EXCEPTION_MAX             CYGNUM_HAL_EXCEPTION_NMI

// The first level of external interrupts
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ0            1
#define CYGNUM_HAL_INTERRUPT_SIU_LVL0            2
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ1            3
#define CYGNUM_HAL_INTERRUPT_SIU_LVL1            4
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ2            5
#define CYGNUM_HAL_INTERRUPT_SIU_LVL2            6
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ3            7
#define CYGNUM_HAL_INTERRUPT_SIU_LVL3            8
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ4            9
#define CYGNUM_HAL_INTERRUPT_SIU_LVL4           10
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ5           11
#define CYGNUM_HAL_INTERRUPT_SIU_LVL5           12
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ6           13
#define CYGNUM_HAL_INTERRUPT_SIU_LVL6           14
#define CYGNUM_HAL_INTERRUPT_SIU_IRQ7           15
#define CYGNUM_HAL_INTERRUPT_SIU_LVL7           16

// Further decoded interrups
#define CYGNUM_HAL_INTERRUPT_SIU_TB_A           17
#define CYGNUM_HAL_INTERRUPT_SIU_TB_B           18
#define CYGNUM_HAL_INTERRUPT_SIU_PIT            19
#define CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC        20
#define CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR        21
#define CYGNUM_HAL_INTERRUPT_SIU_PCMCIA_A_IRQ   22
#define CYGNUM_HAL_INTERRUPT_SIU_PCMCIA_A_CHLVL 23
#define CYGNUM_HAL_INTERRUPT_SIU_PCMCIA_B_IRQ   24
#define CYGNUM_HAL_INTERRUPT_SIU_PCMCIA_B_CHLVL 25
#define CYGNUM_HAL_INTERRUPT_SIU_CPM            26

// Even further...
#define CYGNUM_HAL_INTERRUPT_CPM_PC15           27
#define CYGNUM_HAL_INTERRUPT_CPM_SCC1           28
#define CYGNUM_HAL_INTERRUPT_CPM_SCC2           29
#define CYGNUM_HAL_INTERRUPT_CPM_SCC3           30
#define CYGNUM_HAL_INTERRUPT_CPM_SCC4           31
#define CYGNUM_HAL_INTERRUPT_CPM_PC14           32
#define CYGNUM_HAL_INTERRUPT_CPM_TIMER1         33
#define CYGNUM_HAL_INTERRUPT_CPM_PC13           34
#define CYGNUM_HAL_INTERRUPT_CPM_PC12           35
#define CYGNUM_HAL_INTERRUPT_CPM_SDMA           36
#define CYGNUM_HAL_INTERRUPT_CPM_IDMA1          37
#define CYGNUM_HAL_INTERRUPT_CPM_IDMA2          38
#define CYGNUM_HAL_INTERRUPT_CPM_RESERVED_13    39
#define CYGNUM_HAL_INTERRUPT_CPM_TIMER2         40
#define CYGNUM_HAL_INTERRUPT_CPM_RISCTT         41
#define CYGNUM_HAL_INTERRUPT_CPM_I2C            42
#define CYGNUM_HAL_INTERRUPT_CPM_PC11           43
#define CYGNUM_HAL_INTERRUPT_CPM_PC10           44
#define CYGNUM_HAL_INTERRUPT_CPM_RESERVED_0D    45
#define CYGNUM_HAL_INTERRUPT_CPM_TIMER3         46
#define CYGNUM_HAL_INTERRUPT_CPM_PC9            47
#define CYGNUM_HAL_INTERRUPT_CPM_PC8            48
#define CYGNUM_HAL_INTERRUPT_CPM_PC7            49
#define CYGNUM_HAL_INTERRUPT_CPM_RESERVED_08    50
#define CYGNUM_HAL_INTERRUPT_CPM_TIMER4         51
#define CYGNUM_HAL_INTERRUPT_CPM_PC6            52
#define CYGNUM_HAL_INTERRUPT_CPM_SPI            53
#define CYGNUM_HAL_INTERRUPT_CPM_SMC1           54
#define CYGNUM_HAL_INTERRUPT_CPM_SMC2_PIP       55
#define CYGNUM_HAL_INTERRUPT_CPM_PC5            56
#define CYGNUM_HAL_INTERRUPT_CPM_PC4            57
#define CYGNUM_HAL_INTERRUPT_CPM_ERROR          58

#define CYGNUM_HAL_INTERRUPT_CPM_FIRST       CYGNUM_HAL_INTERRUPT_CPM_PC15
#define CYGNUM_HAL_INTERRUPT_CPM_LAST        CYGNUM_HAL_INTERRUPT_CPM_ERROR

#undef  CYGNUM_HAL_ISR_MAX
#define CYGNUM_HAL_ISR_MAX                   CYGNUM_HAL_INTERRUPT_CPM_LAST
#endif

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC             CYGNUM_HAL_INTERRUPT_DECREMENTER

//--------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];
// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

//--------------------------------------------------------------------------
// Default ISRs

externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);
externC cyg_uint32 hal_default_decrementer_isr(CYG_ADDRWORD vector, 
                                               CYG_ADDRWORD data);

//--------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//--------------------------------------------------------------------------
// Interrupt control macros

#define HAL_DISABLE_INTERRUPTS(_old_)   \
    asm volatile (                      \
        "mfmsr  %0;"                    \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "not    5,4;"                   \
        "and    5,%0,5;"                \
        "mtmsr  5;"                     \
        "and    %0,%0,4"                \
        : "=r"(_old_)                   \
        :                               \
        : "r4", "r5"                    \
        );

#define HAL_ENABLE_INTERRUPTS()         \
    asm volatile (                      \
        "mfmsr  3;"                     \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "or     3,3,4;"                 \
        "mtmsr  3;"                     \
        :                               \
        :                               \
        : "r3", "r4"                    \
        );

#define HAL_RESTORE_INTERRUPTS(_old_)   \
    asm volatile (                      \
        "mfmsr  3;"                     \
        "lis    4,0;"                   \
        "ori    4,4,0x8000 ;"           \
        "and    4,%0,4;"                \
        "or     4,3,4;"                 \
        "mtmsr  4;"                     \
        :                               \
        : "r"(_old_)                    \
        : "r3", "r4"                    \
        );

// FIXME: using andi with side-effect on cc would be better but causes
// compiler problem.
#define HAL_QUERY_INTERRUPTS(_old_)     \
    asm volatile (                      \
        "mfmsr  %0;"                    \
        "lis    4,0;"                   \
        "ori    4,4,0x8000;"            \
        "and    %0,%0,4;"               \
        : "=r"(_old_)                   \
        :                               \
        : "r4"                          \
        );

//--------------------------------------------------------------------------
// Vector translation.

#ifndef HAL_TRANSLATE_VECTOR
// Basic PowerPC configuration only has two vectors; decrementer and
// external. Isr tables/chaining use same vector decoder.
#define HAL_TRANSLATE_VECTOR(_vector_,_index_) \
    (_index_) = (_vector_)
#endif

//--------------------------------------------------------------------------
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

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

#define HAL_INTERRUPT_IN_USE( _vector_, _state_)                             \
    CYG_MACRO_START                                                          \
    cyg_uint32 _index_;                                                      \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                              \
                                                                             \
    if((hal_interrupt_handlers[_index_]                                      \
            == (CYG_ADDRESS)hal_default_decrementer_isr)                     \
       || (hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr)) \
        (_state_) = 0;                                                       \
    else                                                                     \
        (_state_) = 1;                                                       \
    CYG_MACRO_END

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )            \
    CYG_MACRO_START                                                          \
    cyg_uint32 _index_;                                                      \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                              \
                                                                             \
    if((hal_interrupt_handlers[_index_]                                      \
            == (CYG_ADDRESS)hal_default_decrementer_isr)                     \
       || (hal_interrupt_handlers[_index_] == (CYG_ADDRESS)hal_default_isr)) \
    {                                                                        \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;                \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD) _data_;                 \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;              \
    }                                                                        \
    CYG_MACRO_END

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                             \
    CYG_MACRO_START                                                         \
    cyg_uint32 _index_;                                                     \
    HAL_TRANSLATE_VECTOR ((_vector_), _index_);                             \
                                                                            \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )             \
    {                                                                       \
        if (CYGNUM_HAL_INTERRUPT_DECREMENTER == (_vector_))                 \
            hal_interrupt_handlers[_index_] =                               \
                (CYG_ADDRESS)hal_default_decrementer_isr;                   \
        else                                                                \
            hal_interrupt_handlers[_index_] = (CYG_ADDRESS)hal_default_isr; \
        hal_interrupt_data[_index_] = 0;                                    \
        hal_interrupt_objects[_index_] = 0;                                 \
    }                                                                       \
    CYG_MACRO_END

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *(CYG_ADDRESS *)(_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )               \
    CYG_MACRO_START                                             \
    if( _poldvsr_ != NULL )                                     \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];    \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;               \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// Interrupt controller access

#ifdef CYG_HAL_POWERPC_MPC860

// FIXME: Should probably be put in a separate .inl file?!?

static __inline__ void
cyg_hal_interrupt_mask ( cyg_uint32 vector )
{
    switch (vector) {
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ0 ... CYGNUM_HAL_INTERRUPT_SIU_LVL7:
    {
        // SIU interrupt vectors
        cyg_uint32 simask;

        HAL_READ_UINT32 (CYGARC_REG_IMM_SIMASK, simask);
        simask &= ~(((cyg_uint32) CYGARC_REG_IMM_SIMASK_IRQ0) 
                    >> (vector - CYGNUM_HAL_INTERRUPT_SIU_IRQ0));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_SIMASK, simask);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_A:
    {
        // TimeBase A interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr &= ~(CYGARC_REG_IMM_TBSCR_REFAE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_B:
    {
        // TimeBase B interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr &= ~(CYGARC_REG_IMM_TBSCR_REFBE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_PIT:
    {
        // Periodic Interrupt
        cyg_uint16 piscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        piscr &= ~(CYGARC_REG_IMM_PISCR_PIE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC:
    {
        // Real Time Clock Second
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc &= ~(CYGARC_REG_IMM_RTCSC_SIE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR:
    {
        // Real Time Clock Alarm
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc &= ~(CYGARC_REG_IMM_RTCSC_ALE);
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    // PCMCIA_A_IRQ
    // PCMCIA_A_CHLVL
    // PCMCIA_B_IRQ
    // PCMCIA_B_CHLVL
    case CYGNUM_HAL_INTERRUPT_SIU_CPM:
    {
        // Communications Processor Module
        cyg_uint32 cicr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        cicr &= ~(CYGARC_REG_IMM_CICR_IEN);
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_CPM_FIRST ... CYGNUM_HAL_INTERRUPT_CPM_LAST:
    {
        // CPM interrupts
        cyg_uint32 cimr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CIMR, cimr);
        cimr &= ~(((cyg_uint32) 0x80000000) 
                  >> (vector - CYGNUM_HAL_INTERRUPT_CPM_FIRST));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CIMR, cimr);
        break;
    }
    default:
        CYG_FAIL("Unknown Interrupt!!!");
        break;
    }
}

static __inline__ void
cyg_hal_interrupt_unmask ( cyg_uint32 vector )
{
    switch (vector) {
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ0 ... CYGNUM_HAL_INTERRUPT_SIU_LVL7:
    {
        // SIU interrupt vectors
        cyg_uint32 simask;

        HAL_READ_UINT32 (CYGARC_REG_IMM_SIMASK, simask);
        simask |= (((cyg_uint32) CYGARC_REG_IMM_SIMASK_IRQ0) 
                   >> (vector - CYGNUM_HAL_INTERRUPT_SIU_IRQ0));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_SIMASK, simask);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_A:
    {
        // TimeBase A interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= CYGARC_REG_IMM_TBSCR_REFAE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_B:
    {
        // TimeBase B interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= CYGARC_REG_IMM_TBSCR_REFBE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_PIT:
    {
        // Periodic Interrupt
        cyg_uint16 piscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        piscr |= CYGARC_REG_IMM_PISCR_PIE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC:
    {
        // Real Time Clock Second
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc |= CYGARC_REG_IMM_RTCSC_SIE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR:
    {
        // Real Time Clock Alarm
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc |= CYGARC_REG_IMM_RTCSC_ALE;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    // PCMCIA_A_IRQ
    // PCMCIA_A_CHLVL
    // PCMCIA_B_IRQ
    // PCMCIA_B_CHLVL
    case CYGNUM_HAL_INTERRUPT_SIU_CPM:
    {
        // Communications Processor Module
        cyg_uint32 cicr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        cicr |= CYGARC_REG_IMM_CICR_IEN;
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_CPM_FIRST ... CYGNUM_HAL_INTERRUPT_CPM_LAST:
    {
        // CPM interrupts
        cyg_uint32 cimr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CIMR, cimr);
        cimr |= (((cyg_uint32) 0x80000000) 
                 >> (vector - CYGNUM_HAL_INTERRUPT_CPM_FIRST));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CIMR, cimr);
        break;
    }
    default:
        CYG_FAIL("Unknown Interrupt!!!");
        break;
    }
}

static __inline__ void
cyg_hal_interrupt_acknowledge ( cyg_uint32 vector )
{
    switch (vector) {
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ0 ... CYGNUM_HAL_INTERRUPT_SIU_LVL7:
    {
        // SIU interrupt vectors
        cyg_uint32 sipend;

        // When IRQx is configured as an edge interrupt it needs to be
        // cleared. Write to INTx and IRQ/level bits are ignore so
        // it's safe to do always.
        HAL_READ_UINT32 (CYGARC_REG_IMM_SIPEND, sipend);
        sipend |= (((cyg_uint32) CYGARC_REG_IMM_SIPEND_IRQ0) 
                   >> (vector - CYGNUM_HAL_INTERRUPT_SIU_IRQ0));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_SIPEND, sipend);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_A:
    {
        // TimeBase A interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= CYGARC_REG_IMM_TBSCR_REFA;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_TB_B:
    {
        // TimeBase B interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr |= CYGARC_REG_IMM_TBSCR_REFB;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_PIT:
    {
        // Periodic Interrupt
        cyg_uint16 piscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        piscr |= CYGARC_REG_IMM_PISCR_PS;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC:
    {
        // Real Time Clock Second
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc |= CYGARC_REG_IMM_RTCSC_SEC;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR:
    {
        // Real Time Clock Alarm
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc |= CYGARC_REG_IMM_RTCSC_ALR;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    // PCMCIA_A_IRQ
    // PCMCIA_A_CHLVL
    // PCMCIA_B_IRQ
    // PCMCIA_B_CHLVL
    case CYGNUM_HAL_INTERRUPT_SIU_CPM:
        // Communications Processor Module
        // The CPM interrupts must be acknowledged individually.
        break;
    case CYGNUM_HAL_INTERRUPT_CPM_FIRST ... CYGNUM_HAL_INTERRUPT_CPM_LAST:
    {
        // CPM interrupts
        cyg_uint32 cisr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CISR, cisr);
        cisr |= (((cyg_uint32) 0x80000000) 
                 >> (vector - CYGNUM_HAL_INTERRUPT_CPM_FIRST));
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CISR, cisr);
        break;
    }
    default:
        CYG_FAIL("Unknown Interrupt!!!");
        break;
    }
}

static __inline__ void
cyg_hal_interrupt_configure ( cyg_uint32 vector,
                              cyg_bool level,
                              cyg_bool up )
{
    switch (vector) {
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ0:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ1:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ2:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ3:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ4:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ5:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ6:
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ7:
    {
        // External interrupts
        cyg_uint32 siel, bit;

        CYG_ASSERT( level || !up, "Only falling edge is supported");    

        bit = (((cyg_uint32) CYGARC_REG_IMM_SIEL_IRQ0) 
               >> (vector - CYGNUM_HAL_INTERRUPT_SIU_IRQ0));

        HAL_READ_UINT32 (CYGARC_REG_IMM_SIEL, siel);
        siel &= ~bit;
        if (!level) {
            // Set edge detect bit.
            siel |= bit;
        }
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_SIEL, siel);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_LVL0:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL1:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL2:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL3:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL4:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL5:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL6:
    case CYGNUM_HAL_INTERRUPT_SIU_LVL7:
    case CYGNUM_HAL_INTERRUPT_SIU_TB_A:
    case CYGNUM_HAL_INTERRUPT_SIU_TB_B:
    case CYGNUM_HAL_INTERRUPT_SIU_PIT:
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC:
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR:
    // PCMCIA_A_IRQ
    // PCMCIA_A_CHLVL
    // PCMCIA_B_IRQ
    // PCMCIA_B_CHLVL
    case CYGNUM_HAL_INTERRUPT_SIU_CPM:
    case CYGNUM_HAL_INTERRUPT_CPM_FIRST ... CYGNUM_HAL_INTERRUPT_CPM_LAST:
        // These cannot be configured.
        break;

    default:
        CYG_FAIL("Unknown Interrupt!!!");
        break;
    }
}

static __inline__ void
cyg_hal_interrupt_set_level ( cyg_uint32 vector, cyg_uint32 level )
{
    // Note: highest priority has the lowest numerical value.
    CYG_ASSERT( level >= CYGARC_SIU_PRIORITY_HIGH, "Invalid priority");
    CYG_ASSERT( level <= CYGARC_SIU_PRIORITY_LOW, "Invalid priority");

    switch (vector) {
    case CYGNUM_HAL_INTERRUPT_SIU_IRQ0 ... CYGNUM_HAL_INTERRUPT_SIU_LVL7:
        // These cannot be configured.
        break;
    case CYGNUM_HAL_INTERRUPT_SIU_TB_A:
    case CYGNUM_HAL_INTERRUPT_SIU_TB_B:
    {
        // TimeBase A+B interrupt
        cyg_uint16 tbscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        tbscr &= ~(CYGARC_REG_IMM_TBSCR_IRQMASK);
        tbscr |= CYGARC_REG_IMM_TBSCR_IRQ0 >> level;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_TBSCR, tbscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_PIT:
    {
        // Periodic Interrupt
        cyg_uint16 piscr;

        HAL_READ_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        piscr &= ~(CYGARC_REG_IMM_PISCR_IRQMASK);
        piscr |= CYGARC_REG_IMM_PISCR_IRQ0 >> level;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_PISCR, piscr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_SEC:
    case CYGNUM_HAL_INTERRUPT_SIU_RTC_ALR:
    {
        // Real Time Clock Second & Real Time Clock Alarm
        cyg_uint16 rtcsc;

        HAL_READ_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        rtcsc &= ~(CYGARC_REG_IMM_RTCSC_IRQMASK);
        rtcsc |= CYGARC_REG_IMM_RTCSC_IRQ0 >> level;
        HAL_WRITE_UINT16 (CYGARC_REG_IMM_RTCSC, rtcsc);
        break;
    }
    // PCMCIA_A_IRQ
    // PCMCIA_A_CHLVL
    // PCMCIA_B_IRQ
    // PCMCIA_B_CHLVL
    case CYGNUM_HAL_INTERRUPT_SIU_CPM:
    {
        // Communications Processor Module
        cyg_uint32 cicr;

        HAL_READ_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        cicr &= ~(CYGARC_REG_IMM_CICR_IRQMASK);
        cicr |= level << CYGARC_REG_IMM_CICR_IRQ_SHIFT;
        HAL_WRITE_UINT32 (CYGARC_REG_IMM_CICR, cicr);
        break;
    }
    case CYGNUM_HAL_INTERRUPT_CPM_FIRST ... CYGNUM_HAL_INTERRUPT_CPM_LAST:
        // CPM interrupt levels are for internal priority. All interrupts
        // fed to the CPU use the CPM level set above.
        // FIXME: Control of SCdP ordering.
        break;
    default:
        CYG_FAIL("Unknown Interrupt!!!");
        break;
    }
}

// The decrementer interrupt cannnot be masked, configured or acknowledged.

#define HAL_INTERRUPT_MASK( _vector_ )                    \
    CYG_MACRO_START                                       \
    if (CYGNUM_HAL_INTERRUPT_DECREMENTER != (_vector_))   \
        cyg_hal_interrupt_mask ( (_vector_) );            \
    CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ )                  \
    CYG_MACRO_START                                       \
    if (CYGNUM_HAL_INTERRUPT_DECREMENTER != (_vector_))   \
        cyg_hal_interrupt_unmask ( (_vector_) );          \
    CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )             \
    CYG_MACRO_START                                       \
    if (CYGNUM_HAL_INTERRUPT_DECREMENTER != (_vector_))   \
        cyg_hal_interrupt_acknowledge ( (_vector_) );     \
    CYG_MACRO_END

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )              \
    CYG_MACRO_START                                                     \
    if (CYGNUM_HAL_INTERRUPT_DECREMENTER != (_vector_))                 \
        cyg_hal_interrupt_configure ( (_vector_), (_level_), (_up_) );  \
    CYG_MACRO_END

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            \
    CYG_MACRO_START                                             \
    if (CYGNUM_HAL_INTERRUPT_DECREMENTER != (_vector_))         \
        cyg_hal_interrupt_set_level ( (_vector_) , (_level_) ); \
    CYG_MACRO_END

#else

// No interrupt configuration possible without interrupt controller.

#define HAL_INTERRUPT_MASK( _vector_ )

#define HAL_INTERRUPT_UNMASK( _vector_ )

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )

#endif

//--------------------------------------------------------------------------
// Interrupt arbiters

#ifdef CYG_HAL_POWERPC_MPC860

externC cyg_uint32 hal_arbitration_isr_tb (CYG_ADDRWORD vector, 
                                           CYG_ADDRWORD data);
externC cyg_uint32 hal_arbitration_isr_pit (CYG_ADDRWORD vector,
                                            CYG_ADDRWORD data);
externC cyg_uint32 hal_arbitration_isr_rtc (CYG_ADDRWORD vector,
                                            CYG_ADDRWORD data);
externC cyg_uint32 hal_arbitration_isr_cpm (CYG_ADDRWORD vector,
                                            CYG_ADDRWORD data);

#endif // ifdef CYG_HAL_POWERPC_MPC860

//--------------------------------------------------------------------------
// Clock control

#define HAL_CLOCK_INITIALIZE( _period_ )        \
    asm volatile (                              \
        "mtdec %0;"                             \
        :                                       \
        : "r"(_period_)                         \
        );

#define HAL_CLOCK_RESET( _vector_, _period_ )   \
    asm volatile (                              \
        "mtdec %0;"                             \
        :                                       \
        : "r"(_period_)                         \
        );

#define HAL_CLOCK_READ( _pvalue_ )                              \
    CYG_MACRO_START                                             \
    register cyg_uint32 result;                                 \
    asm volatile(                                               \
        "mfdec  %0;"                                            \
        : "=r"(result)                                          \
        );                                                      \
    *(_pvalue_) = CYGNUM_KERNEL_COUNTERS_RTC_PERIOD-result;     \
    CYG_MACRO_END

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK_LATENCY
#define HAL_CLOCK_LATENCY( _pvalue_ )                           \
    CYG_MACRO_START                                             \
    register cyg_int32 result;                                  \
    asm volatile(                                               \
        "mfdec  %0;"                                            \
        : "=r"(result)                                          \
        );                                                      \
    /* Pending DEC interrupts cannot be discarded. If dec is */ \
    /* positive it''s because a DEC interrupt occured while  */ \
    /* eCos was getting ready to run. Just return 0 in that  */ \
    /* case.                                                 */ \
    if (result > 0)                                             \
        result = 0;                                             \
    *(_pvalue_) = -result;                                      \
    CYG_MACRO_END
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_INTR_H
// End of hal_intr.h
