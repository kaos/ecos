#ifndef CYGONCE_HAL_HAL_INTR_H
#define CYGONCE_HAL_HAL_INTR_H

//=============================================================================
//
//	hal_intr.h
//
//	HAL Interrupt and clock support
//
//=============================================================================
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-02-17
// Purpose:	Define Interrupt support
// Description:	The macros defined here provide the HAL APIs for handling
//              interrupts and the clock.
//              
// Usage:
//		#include <cyg/hal/hal_intr.h>
//		...
//		
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

#include <pkgconf/hal.h>

//-----------------------------------------------------------------------------

// The MN10300 has a somewhat complex interrupt structure. Besides the
// reset and NMI vectors there are seven maskable interrupt vectors
// which must point to code in the 64k starting at 0x40000000. There
// are also 25 Interrupt control groups, each of which can have 4
// interrupt lines attached, for a theoretical total of 100 interrupts
// (!). Some of these are dedicated to specific devices, other to
// external pins, and others are not connected to anything, resulting
// in only 45 that can actually be delivered. Each control group may
// be assigned one of seven interrupt levels, and is delivered to the
// corresponding vector. Software can then use a register to determine
// the delivering group and detect from there which interrupt has been
// delivered.
//
// The approach we will adopt at present is for the code attached to
// each vector to save state and jump via a table to a VSR. The
// default VSR will fully decode the delivered interrupt into a table
// of isr/data/object entries. VSR replacement will operate on the
// first level indirection table rather than the hardware
// vectors. This is the fastest mechanism, however it needs 100*3*4 +
// 7*4 = 1228 bytes for the tables.
// 

//-----------------------------------------------------------------------------
// Interrupt vectors.

// The level-specific hardware vectors
#define CYG_VECTOR_0                    0
#define CYG_VECTOR_1                    1
#define CYG_VECTOR_2                    2
#define CYG_VECTOR_3                    3
#define CYG_VECTOR_4                    4
#define CYG_VECTOR_5                    5
#define CYG_VECTOR_6                    6
#define CYG_VECTOR_NMI                  7
#define CYG_VECTOR_TRAP                 8

#define CYG_VSR_MIN                     0
#define CYG_VSR_MAX                     8
#define CYG_VSR_COUNT                   9

#define CYG_EXCEPTION_MIN               0
#define CYG_EXCEPTION_MAX               3
#define CYG_EXCEPTION_COUNT             4


#if defined(CYG_HAL_MN10300_MN103000)

// The decoded interrupts

#define CYG_VECTOR_NMIRQ                0
#define CYG_VECTOR_WATCHDOG             1
#define CYG_VECTOR_SYSTEM_ERROR         2
#define CYG_VECTOR_RESERVED_3           3

#define CYG_VECTOR_RESERVED_4           4
#define CYG_VECTOR_RESERVED_5           5
#define CYG_VECTOR_RESERVED_6           6
#define CYG_VECTOR_RESERVED_7           7

#define CYG_VECTOR_TIMER_0              8
#define CYG_VECTOR_TIMER_1              9
#define CYG_VECTOR_TIMER_2              10
#define CYG_VECTOR_TIMER_3              11

#define CYG_VECTOR_TIMER_4              12
#define CYG_VECTOR_TIMER_5              13
#define CYG_VECTOR_TIMER_6              14
#define CYG_VECTOR_TIMER_7              15

#define CYG_VECTOR_TIMER_8              16
#define CYG_VECTOR_TIMER_8_COMPARE_A    17
#define CYG_VECTOR_TIMER_8_COMPARE_B    18
#define CYG_VECTOR_RESERVED_19          19

#define CYG_VECTOR_TIMER_9              20
#define CYG_VECTOR_TIMER_9_COMPARE_A    21
#define CYG_VECTOR_TIMER_9_COMPARE_B    22
#define CYG_VECTOR_RESERVED_23          23

#define CYG_VECTOR_TIMER_10             24
#define CYG_VECTOR_TIMER_10_COMPARE_A   25
#define CYG_VECTOR_TIMER_10_COMPARE_B   26
#define CYG_VECTOR_TIMER_10_COMPARE_C   27

#define CYG_VECTOR_TIMER_11             28
#define CYG_VECTOR_TIMER_11_COMPARE_A   29
#define CYG_VECTOR_TIMER_11_COMPARE_B   30
#define CYG_VECTOR_TIMER_11_COMPARE_C   31

#define CYG_VECTOR_TIMER_12             32
#define CYG_VECTOR_TIMER_12_COMPARE_A   33
#define CYG_VECTOR_TIMER_12_COMPARE_B   34
#define CYG_VECTOR_TIMER_12_COMPARE_C   35

#define CYG_VECTOR_TIMER_11_COMPARE_D   36
#define CYG_VECTOR_TIMER_12_COMPARE_D   37
#define CYG_VECTOR_RESERVED_38          38
#define CYG_VECTOR_RESERVED_39          39

#define CYG_VECTOR_DMA0                 40
#define CYG_VECTOR_RESERVED_41          41
#define CYG_VECTOR_RESERVED_42          42
#define CYG_VECTOR_RESERVED_43          43

#define CYG_VECTOR_DMA1                 44
#define CYG_VECTOR_RESERVED_45          45
#define CYG_VECTOR_RESERVED_46          46
#define CYG_VECTOR_RESERVED_47          47

#define CYG_VECTOR_DMA2                 48
#define CYG_VECTOR_RESERVED_49          49
#define CYG_VECTOR_RESERVED_50          50
#define CYG_VECTOR_RESERVED_51          51

#define CYG_VECTOR_DMA3                 52
#define CYG_VECTOR_RESERVED_53          53
#define CYG_VECTOR_RESERVED_54          54
#define CYG_VECTOR_RESERVED_55          55

#define CYG_VECTOR_SERIAL_0_RX          56
#define CYG_VECTOR_SERIAL_0_TX          57
#define CYG_VECTOR_RESERVED_58          58
#define CYG_VECTOR_RESERVED_59          59

#define CYG_VECTOR_SERIAL_1_RX          60
#define CYG_VECTOR_SERIAL_1_TX          61
#define CYG_VECTOR_RESERVED_62          62
#define CYG_VECTOR_RESERVED_63          63

#define CYG_VECTOR_EXTERNAL_0           64
#define CYG_VECTOR_RESERVED_65          65
#define CYG_VECTOR_RESERVED_66          66
#define CYG_VECTOR_RESERVED_67          67

#define CYG_VECTOR_EXTERNAL_1           68
#define CYG_VECTOR_RESERVED_69          69
#define CYG_VECTOR_RESERVED_70          70
#define CYG_VECTOR_RESERVED_71          71

#define CYG_VECTOR_EXTERNAL_2           72
#define CYG_VECTOR_RESERVED_73          73
#define CYG_VECTOR_RESERVED_74          74
#define CYG_VECTOR_RESERVED_75          75

#define CYG_VECTOR_EXTERNAL_3           76
#define CYG_VECTOR_RESERVED_77          77
#define CYG_VECTOR_RESERVED_78          78
#define CYG_VECTOR_RESERVED_79          79

#define CYG_VECTOR_EXTERNAL_4           80
#define CYG_VECTOR_RESERVED_81          81
#define CYG_VECTOR_RESERVED_82          82
#define CYG_VECTOR_RESERVED_83          83

#define CYG_VECTOR_EXTERNAL_5           84
#define CYG_VECTOR_RESERVED_85          85
#define CYG_VECTOR_RESERVED_86          86
#define CYG_VECTOR_RESERVED_87          87

#define CYG_VECTOR_EXTERNAL_6           88
#define CYG_VECTOR_RESERVED_89          89
#define CYG_VECTOR_RESERVED_90          90
#define CYG_VECTOR_RESERVED_91          91

#define CYG_VECTOR_EXTERNAL_7           92
#define CYG_VECTOR_RESERVED_93          93
#define CYG_VECTOR_RESERVED_94          94
#define CYG_VECTOR_RESERVED_95          95

#define CYG_VECTOR_AD_CONVERSION        96
#define CYG_VECTOR_RESERVED_97          97
#define CYG_VECTOR_RESERVED_98          98
#define CYG_VECTOR_RESERVED_99          99

#define CYG_ISR_MIN                     0
#define CYG_ISR_MAX                     99

#define CYG_ISR_COUNT                   100

#elif defined(CYG_HAL_MN10300_MN103002) 

// The decoded interrupts

#define CYG_VECTOR_NMIRQ                0
#define CYG_VECTOR_WATCHDOG             1
#define CYG_VECTOR_SYSTEM_ERROR         2
#define CYG_VECTOR_RESERVED_3           3

#define CYG_VECTOR_RESERVED_4           4
#define CYG_VECTOR_RESERVED_5           5
#define CYG_VECTOR_RESERVED_6           6
#define CYG_VECTOR_RESERVED_7           7

#define CYG_VECTOR_TIMER_0              8
#define CYG_VECTOR_RESERVED_9           9
#define CYG_VECTOR_RESERVED_10          10
#define CYG_VECTOR_RESERVED_11          11

#define CYG_VECTOR_TIMER_1              12
#define CYG_VECTOR_RESERVED_13          13
#define CYG_VECTOR_RESERVED_14          14
#define CYG_VECTOR_RESERVED_15          15

#define CYG_VECTOR_TIMER_2              16
#define CYG_VECTOR_RESERVED_17          17
#define CYG_VECTOR_RESERVED_18          18
#define CYG_VECTOR_RESERVED_19          19

#define CYG_VECTOR_TIMER_3              20
#define CYG_VECTOR_RESERVED_21          21
#define CYG_VECTOR_RESERVED_22          22
#define CYG_VECTOR_RESERVED_23          23

#define CYG_VECTOR_TIMER_4              24
#define CYG_VECTOR_RESERVED_25          25
#define CYG_VECTOR_RESERVED_26          26
#define CYG_VECTOR_RESERVED_27          27

#define CYG_VECTOR_TIMER_5              28
#define CYG_VECTOR_RESERVED_29          29
#define CYG_VECTOR_RESERVED_30          30
#define CYG_VECTOR_RESERVED_31          31

#define CYG_VECTOR_TIMER_6              32
#define CYG_VECTOR_RESERVED_33          33
#define CYG_VECTOR_RESERVED_34          34
#define CYG_VECTOR_RESERVED_35          35

#define CYG_VECTOR_TIMER_6_COMPARE_A    36
#define CYG_VECTOR_RESERVED_37          37
#define CYG_VECTOR_RESERVED_38          38
#define CYG_VECTOR_RESERVED_39          39

#define CYG_VECTOR_TIMER_6_COMPARE_B    40
#define CYG_VECTOR_RESERVED_41          41
#define CYG_VECTOR_RESERVED_42          42
#define CYG_VECTOR_RESERVED_43          43

#define CYG_VECTOR_RESERVED_44          44
#define CYG_VECTOR_RESERVED_45          45
#define CYG_VECTOR_RESERVED_46          46
#define CYG_VECTOR_RESERVED_47          47

#define CYG_VECTOR_DMA0                 48
#define CYG_VECTOR_RESERVED_49          49
#define CYG_VECTOR_RESERVED_50          50
#define CYG_VECTOR_RESERVED_51          51

#define CYG_VECTOR_DMA1                 52
#define CYG_VECTOR_RESERVED_53          53
#define CYG_VECTOR_RESERVED_54          54
#define CYG_VECTOR_RESERVED_55          55

#define CYG_VECTOR_DMA2                 56
#define CYG_VECTOR_RESERVED_57          57
#define CYG_VECTOR_RESERVED_58          58
#define CYG_VECTOR_RESERVED_59          59

#define CYG_VECTOR_DMA3                 60
#define CYG_VECTOR_RESERVED_61          61
#define CYG_VECTOR_RESERVED_62          62
#define CYG_VECTOR_RESERVED_63          63

#define CYG_VECTOR_SERIAL_0_RX          64
#define CYG_VECTOR_RESERVED_65          65
#define CYG_VECTOR_RESERVED_66          66
#define CYG_VECTOR_RESERVED_67          67

#define CYG_VECTOR_SERIAL_0_TX          68
#define CYG_VECTOR_RESERVED_69          69
#define CYG_VECTOR_RESERVED_70          70
#define CYG_VECTOR_RESERVED_71          71

#define CYG_VECTOR_SERIAL_1_RX          72
#define CYG_VECTOR_RESERVED_73          73
#define CYG_VECTOR_RESERVED_74          74
#define CYG_VECTOR_RESERVED_75          75

#define CYG_VECTOR_SERIAL_1_TX          76
#define CYG_VECTOR_RESERVED_77          77
#define CYG_VECTOR_RESERVED_78          78
#define CYG_VECTOR_RESERVED_79          79

#define CYG_VECTOR_SERIAL_2_RX          80
#define CYG_VECTOR_RESERVED_81          81
#define CYG_VECTOR_RESERVED_82          82
#define CYG_VECTOR_RESERVED_83          83

#define CYG_VECTOR_SERIAL_2_TX          84
#define CYG_VECTOR_RESERVED_85          85
#define CYG_VECTOR_RESERVED_86          86
#define CYG_VECTOR_RESERVED_87          87

#define CYG_VECTOR_RESERVED_88          88
#define CYG_VECTOR_RESERVED_89          89
#define CYG_VECTOR_RESERVED_90          90
#define CYG_VECTOR_RESERVED_91          91

#define CYG_VECTOR_EXTERNAL_0           92
#define CYG_VECTOR_RESERVED_93          93
#define CYG_VECTOR_RESERVED_94          94
#define CYG_VECTOR_RESERVED_95          95

#define CYG_VECTOR_EXTERNAL_1           96
#define CYG_VECTOR_RESERVED_97          97
#define CYG_VECTOR_RESERVED_98          98
#define CYG_VECTOR_RESERVED_99          99

#define CYG_VECTOR_EXTERNAL_2           100
#define CYG_VECTOR_RESERVED_101         101
#define CYG_VECTOR_RESERVED_102         102
#define CYG_VECTOR_RESERVED_103         103

#define CYG_VECTOR_EXTERNAL_3           104
#define CYG_VECTOR_RESERVED_105         105
#define CYG_VECTOR_RESERVED_106         106
#define CYG_VECTOR_RESERVED_107         107

#define CYG_VECTOR_EXTERNAL_4           108
#define CYG_VECTOR_RESERVED_109         109
#define CYG_VECTOR_RESERVED_110         110
#define CYG_VECTOR_RESERVED_111         111

#define CYG_VECTOR_EXTERNAL_5           112
#define CYG_VECTOR_RESERVED_113         113
#define CYG_VECTOR_RESERVED_114         114
#define CYG_VECTOR_RESERVED_115         115

#define CYG_VECTOR_EXTERNAL_6           116
#define CYG_VECTOR_RESERVED_117         117
#define CYG_VECTOR_RESERVED_118         118
#define CYG_VECTOR_RESERVED_119         119

#define CYG_VECTOR_EXTERNAL_7           120
#define CYG_VECTOR_RESERVED_121         121
#define CYG_VECTOR_RESERVED_122         122
#define CYG_VECTOR_RESERVED_123         123

#define CYG_ISR_MIN                     0
#define CYG_ISR_MAX                     123

#define CYG_ISR_COUNT                   (3+((CYG_ISR_MAX+1)/4))

#endif

// The vector used by the Real time clock

#ifdef CYG_HAL_MN10300_SIM
#  define CYG_VECTOR_RTC                  CYG_VECTOR_TIMER_5
//# define CYG_VECTOR_RTC                  CYG_VECTOR_EXTERNAL_1
#else
# ifdef CYG_HAL_MN10300_MN103000
#  define CYG_VECTOR_RTC                  CYG_VECTOR_TIMER_8
# endif
# ifdef CYG_HAL_MN10300_MN103002
#  define CYG_VECTOR_RTC                  CYG_VECTOR_TIMER_5
# endif
#endif

//-----------------------------------------------------------------------------
// Timer control registers.

// On simulator we use simulated external interrupt

#if defined(CYG_HAL_MN10300_MN103002)

// On the mn103002 we use timers 4 and 5

#define TIMER4_CR        0x340010a0
#define TIMER4_BR        0x34001090
#define TIMER4_MD        0x34001080

#define TIMER5_CR        0x340010a2
#define TIMER5_BR        0x34001092
#define TIMER5_MD        0x34001082

#define TIMER_CR        TIMER5_CR
#define TIMER_BR        TIMER5_BR
#define TIMER_MD        TIMER5_MD

#define TIMER0_MD       0x34001000
#define TIMER0_BR       0x34001010
#define TIMER0_CR       0x34001020

#elif defined(CYG_HAL_MN10300_MN103000)

// on the mn103000 we use timers 4 and 5
#define TIMER4_CR        0x340010a0
#define TIMER4_BR        0x34001090
#define TIMER4_MD        0x34001080

#define TIMER5_CR        0x340010a2
#define TIMER5_BR        0x34001092
#define TIMER5_MD        0x34001082

#define TIMER_CR        TIMER5_CR
#define TIMER_BR        TIMER5_BR
#define TIMER_MD        TIMER5_MD

#define TIMER0_MD       0x34001000
#define TIMER0_BR       0x34001010
#define TIMER0_CR       0x34001020

#endif

//-----------------------------------------------------------------------------
// Static data used by HAL

// ISR tables
externC volatile CYG_ADDRESS    hal_interrupt_handlers[CYG_ISR_COUNT];
externC volatile CYG_ADDRWORD   hal_interrupt_data[CYG_ISR_COUNT];
externC volatile CYG_ADDRESS    hal_interrupt_objects[CYG_ISR_COUNT];        

// VSR table
externC volatile CYG_ADDRESS    hal_vsr_table[CYG_VSR_COUNT];

// MN10300 interrupt control registers, mapped by linker script.
externC volatile cyg_uint16     mn10300_interrupt_control[0x300/2];

//-----------------------------------------------------------------------------
// Interrupt state storage

typedef cyg_uint32 CYG_INTERRUPT_STATE;

//-----------------------------------------------------------------------------
// Interrupt control macros

#define HAL_DISABLE_INTERRUPTS(_old_)   \
        asm volatile (                  \
            "mov        psw,%0;"        \
            "mov        0xF7FF,d0;"     \
            "and        %0,d0;"         \
            "mov        d0,psw;"        \
            "and        0x0800,%0;"     \
            : "=d"(_old_)               \
            :                           \
            : "d0"                      \
            );

#define HAL_ENABLE_INTERRUPTS()         \
        asm volatile (                  \
            "mov        psw,d0;"        \
            "or         0x0800,d0;"     \
            "mov        d0,psw;"        \
            :                           \
            :                           \
            : "d0"                      \
            );

#define HAL_RESTORE_INTERRUPTS(_old_)   \
        asm volatile (                  \
            "mov        psw,d1;"        \
            "or         %0,d1;"         \
            "mov        d1,psw;"        \
            :                           \
            : "d"(_old_)                \
            : "d1"                      \
            );

#define HAL_QUERY_INTERRUPTS(_old_)     \
        asm volatile (                  \
            "mov        psw,%0;"        \
            "and        0x0800,%0;"     \
            : "=d"(_old_)               \
            );

//-----------------------------------------------------------------------------
// Translate a vector number into an ISR table index.
// If we have chained interrupts we have just a single ISR per priority
// level. On the MN103000 there are several interrupts per controller,
// so we have to decode to one of 100 vectors. On the MN103002 there is
// only one interrupt per controller, so we can have just one ISR per
// controller, except for the NMI vectors which occupy the first 3 slots.

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN

#define HAL_TRANSLATE_VECTOR(_vector_,_index_)                  \
{                                                               \
    /* ICRs are 16 bit regs at 32 bit spacing */                \
    cyg_ucount16 _ix_ = ((_vector_)>>2)<<1;                     \
                                                                \
    /* read the appropriate interrupt control register */       \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_ix_];         \
                                                                \
    /* extract interrupt priority level */                      \
    _index_ = (_icr_ >> 12) & 0x7;                              \
}

#else

#if defined(CYG_HAL_MN10300_MN103000)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_) _index_ = (_vector_)

#elif defined(CYG_HAL_MN10300_MN103002)

//#define HAL_TRANSLATE_VECTOR(_vector_,_index_) _index_ = ((_vector_)>>2)

#define HAL_TRANSLATE_VECTOR(_vector_,_index_)                          \
              _index_ = (((_vector_)<=CYG_VECTOR_SYSTEM_ERROR) ?        \
                         (_vector_) :                                   \
                         (((_vector_)>>2)+CYG_VECTOR_RESERVED_3))

#endif

#endif


//-----------------------------------------------------------------------------
// Interrupt and VSR attachment macros

#define HAL_INTERRUPT_ATTACH( _vector_, _isr_, _data_, _object_ )       \
{                                                                       \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR(_vector_,_index_);                             \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)NULL )          \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)_isr_;           \
        hal_interrupt_data[_index_] = (CYG_ADDRWORD)_data_;             \
        hal_interrupt_objects[_index_] = (CYG_ADDRESS)_object_;         \
    }                                                                   \
}

#define HAL_INTERRUPT_DETACH( _vector_, _isr_ )                         \
{                                                                       \
    cyg_uint32 _index_;                                                 \
    HAL_TRANSLATE_VECTOR(_vector_,_index_);                             \
                                                                        \
    if( hal_interrupt_handlers[_index_] == (CYG_ADDRESS)_isr_ )         \
    {                                                                   \
        hal_interrupt_handlers[_index_] = (CYG_ADDRESS)NULL;            \
        hal_interrupt_data[_index_] = 0;                                \
        hal_interrupt_objects[_index_] = 0;                             \
    }                                                                   \
}

#define HAL_VSR_GET( _vector_, _pvsr_ )                                 \
    *((CYG_ADDRESS *)_pvsr_) = hal_vsr_table[_vector_];
    

#define HAL_VSR_SET( _vector_, _vsr_, _poldvsr_ )                       \
    if( _poldvsr_ != NULL )                                             \
        *(CYG_ADDRESS *)_poldvsr_ = hal_vsr_table[_vector_];            \
    hal_vsr_table[_vector_] = (CYG_ADDRESS)_vsr_;


//-----------------------------------------------------------------------------
// Interrupt controller access
// Read interrupt control registers back after writing to them. This
// ensures that the written value is not sitting in the store buffers
// when interrupts are re-enabled.
#define HAL_INTERRUPT_MASK( _vector_ )                                  \
{                                                                       \
    /* ICRs are 16 bit regs at 32 bit spacing */                        \
    cyg_ucount16 _index_ = ((_vector_)>>2)<<1;                          \
                                                                        \
    /* read the appropriate interrupt control register */               \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];              \
                                                                        \
    /* clear interrupt enable bit for this vector */                    \
    _icr_ &= ~(0x0100<<((_vector_)&3));                                 \
                                                                        \
    /* restore the interrupt control register */                        \
    mn10300_interrupt_control[_index_] = _icr_;                         \
    _icr_ = mn10300_interrupt_control[_index_];                         \
}

#define HAL_INTERRUPT_UNMASK( _vector_ )                                \
{                                                                       \
    /* ICRs are 16 bit regs at 32 bit spacing */                        \
    cyg_ucount16 _index_ = (_vector_>>2)<<1;                            \
                                                                        \
    /* read the appropriate interrupt control register */               \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];              \
                                                                        \
    /* set interrupt enable bit for this vector */                      \
    _icr_ |= (0x0100<<(_vector_&3));                                    \
                                                                        \
    /* restore the interrupt control register */                        \
    mn10300_interrupt_control[_index_] = _icr_;                         \
    _icr_ = mn10300_interrupt_control[_index_];                         \
}

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                   \
{                                                               \
    /* ICRs are 16 bit regs at 32 bit spacing */                \
    cyg_ucount16 _index_ = ((_vector_)>>2)<<1;                  \
                                                                \
    /* read the appropriate interrupt control register */       \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];      \
                                                                \
    /* clear interrupt request bit for this vector */           \
    _icr_ &= ~(0x0010<<((_vector_)&3));                         \
                                                                \
    /* set interrupt detect bit for this vector */              \
    _icr_ |= (0x0001<<((_vector_)&3));                          \
                                                                \
    /* restore the interrupt control register */                \
    mn10300_interrupt_control[_index_] = _icr_;                 \
    _icr_ = mn10300_interrupt_control[_index_];                 \
}

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )      \
{                                                               \
    cyg_vector _v_ = _vector_;                                  \
    /* adjust vector to bit offset in EXTMD */                  \
    _v_ -= CYG_VECTOR_EXTERNAL_0;                               \
    _v_ >>= 1;                                                  \
                                                                \
    cyg_uint16 _val_ = 0;                                       \
                                                                \
    /* set bits according to requirements */                    \
    if( _up_ ) _val_ |= 1;                                      \
    if( !(_level_) ) _val_ |= 2;                                \
                                                                \
    /* get EXTMD */                                             \
    cyg_uint16 _reg_ = mn10300_interrupt_control[0x180>>1];     \
                                                                \
    /* clear old value and set new */                           \
    _reg_ &= ~(3<<_v_);                                         \
    _reg_ |= _val_<<_v_;                                        \
                                                                \
    /* restore EXTMD */                                         \
    mn10300_interrupt_control[0x180>>1] = _reg_;                \
}

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )            \
{                                                               \
    /* ICRs are 16 bit regs at 32 bit spacing */                \
    cyg_ucount16 _index_ = (_vector_>>2)<<1;                    \
                                                                \
    /* read the appropriate interrupt control register */       \
    cyg_uint16 _icr_ = mn10300_interrupt_control[_index_];      \
                                                                \
    /* set interrupt level for this group of vectors */         \
    _icr_ &= 0x0FFF;                                            \
    _icr_ |= (_level_)<<12;                                     \
                                                                \
    /* restore the interrupt control register */                \
    mn10300_interrupt_control[_index_] = _icr_;                 \
    _icr_ = mn10300_interrupt_control[_index_];                 \
}

//-----------------------------------------------------------------------------
// Clock control


#if 0 // defined(CYG_HAL_MN10300_SIM)

#define OEA_DEV                 0x31000000

#define HAL_SWAP(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|        \
                (((x)&0xff0000)>>8)|(((x)&0xff000000)>>24))

#define PAL_COUNTDOWN_TIMER     0x20  // one shot on IR0
#define PAL_COUNTDOWN_VALUE     0x24
#define PAL_PERIODIC_TIMER      0x28  // repeating on IR1
#define PAL_PERIODIC_VALUE      0x2c


// IRQ1 set to receive clock intrs
#define HAL_CLOCK_INITIALIZE( _period_ )                        \
{                                                               \
    volatile cyg_uint32 *timer = (cyg_uint32 *)                 \
        (OEA_DEV + PAL_PERIODIC_TIMER);                         \
    cyg_uint32 p = _period_;                                    \
                                                                \
    *timer = HAL_SWAP(p);                                       \
}

#elif defined(CYG_HAL_MN10300_MN103000)

#define HAL_CLOCK_INITIALIZE( _period_ )                        \
{                                                               \
    volatile cyg_uint16 *timer_ctr = (cyg_uint16 *)TIMER_BR;    \
    volatile cyg_uint16 *timer_mode = (cyg_uint16 *)TIMER_MD;   \
    volatile cyg_uint8 *timer_a_mode = (cyg_uint8 *)0x34001084; \
                                                                \
    *timer_a_mode = 0x04;                                       \
                                                                \
    *timer_ctr = 0xf000;                                        \
                                                                \
    *timer_mode = 0x0013;                                       \
    *timer_mode = 0x4013;                                       \
    *timer_mode = 0x0013;                                       \
    *timer_mode = 0x8013;                                       \
}

#elif defined(CYG_HAL_MN10300_MN103002)

#define HAL_CLOCK_INITIALIZE( _period_ )                                \
{                                                                       \
    volatile cyg_uint16 *timer4_br      = (cyg_uint16 *)TIMER4_BR;      \
    volatile cyg_uint8 *timer4_md       = (cyg_uint8 *)TIMER4_MD;       \
    volatile cyg_uint16 *timer5_br      = (cyg_uint16 *)TIMER5_BR;      \
    volatile cyg_uint8 *timer5_md       = (cyg_uint8 *)TIMER5_MD;       \
                                                                        \
    /* Set timers 4 and 5 into cascade mode */                          \
                                                                        \
    *timer5_br = (_period_)>>16;                                        \
                                                                        \
    *timer5_md = 0x40;                                                  \
    *timer5_md = 0x83;                                                  \
                                                                        \
    *timer4_br = (_period_)&0x0000FFFF;                                 \
                                                                        \
    *timer4_md = 0x40;                                                  \
    *timer4_md = 0x80;                                                  \
}

#else

#error Undefined MN10300 model

#endif    

#define HAL_CLOCK_RESET( _vector_, _period_ )

#if 0 //def CYG_HAL_MN10300_SIM

// This timer counts down, so subtract from set value.
#define HAL_CLOCK_READ( _pvalue_ )                                      \
{                                                                       \
    volatile cyg_uint32 *timer = (cyg_uint32 *)                         \
        (OEA_DEV + PAL_PERIODIC_TIMER);                                 \
    volatile cyg_uint32 *value = (cyg_uint32 *)                         \
        (OEA_DEV + PAL_PERIODIC_VALUE);                                 \
    cyg_uint32 t,v;                                                     \
    t = *timer;                                                         \
    v = *value;                                                         \
    *(_pvalue_) = HAL_SWAP(t) - HAL_SWAP(v);                            \
}

#else // CYG_HAL_MN10300_SIM not
#define HAL_CLOCK_READ( _pvalue_ )                                      \
{                                                                       \
    volatile cyg_uint16 *timer4_cr = (cyg_uint16 *)TIMER4_CR;           \
    volatile cyg_uint16 *timer5_cr = (cyg_uint16 *)TIMER5_CR;           \
                                                                        \
    cyg_uint16 t5;                                                      \
    cyg_uint16 t4;                                                      \
                                                                        \
    /* Loop reading the two timers until we can read t5 twice   */      \
    /* with the same value. This avoids getting silly times if  */      \
    /* the timers carry between reading the two regs.           */      \
    do {                                                                \
        t5 = *timer5_cr;                                                \
        t4 = *timer4_cr;                                                \
    } while( t5 != *timer5_cr );                                        \
                                                                        \
    *(_pvalue_) = CYGNUM_KERNEL_COUNTERS_RTC_PERIOD - ((t5<<16) + t4);  \
}
#endif // CYG_HAL_MN10300_SIM
//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_INTR_H
// End of hal_intr.h
