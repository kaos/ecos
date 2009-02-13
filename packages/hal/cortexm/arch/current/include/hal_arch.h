#ifndef CYGONCE_HAL_ARCH_H
#define CYGONCE_HAL_ARCH_H
/*==========================================================================
//
//      hal_arch.h
//
//      Cortex-M architecture abstractions
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
// Description:  Define architecture abstractions
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_arch.h>

//==========================================================================
// CPU save state
//
// This is a discriminated union of different save states for threads,
// exceptions and interrupts. State is saved in the most efficient way
// for each context. This makes the GDB state get/put slightly more
// complex, but that is a suitable compromise.

typedef struct 
{
    union
    {
        cyg_uint32              type;           // State type

        struct
        {
            cyg_uint32          type;           // State type
            cyg_uint32          basepri;        // BASEPRI
            cyg_uint32          sp;             // SP (R13)
            cyg_uint32          r[13];          // R0..R12
            cyg_uint32          pc;             // PC/LR
        } thread;

        struct
        {
            cyg_uint32          type;           // State type
            cyg_uint32          vector;         // Exception vector number
            cyg_uint32          basepri;        // BASEPRI
            
            cyg_uint32          r4_11[8];       // Remaining CPU registers
            cyg_uint32          xlr;            // Exception return LR
            
            // The following are saved and restored automatically by the CPU
            // for exceptions or interrupts.

            cyg_uint32          r0;
            cyg_uint32          r1;
            cyg_uint32          r2;
            cyg_uint32          r3;
            cyg_uint32          r12;
            cyg_uint32          lr;
            cyg_uint32          pc;
            cyg_uint32          psr;
        } exception;

        struct
        {
            cyg_uint32          type;           // State type
            
            // The following are saved and restored automatically by the CPU
            // for exceptions or interrupts.

            cyg_uint32          r0;
            cyg_uint32          r1;
            cyg_uint32          r2;
            cyg_uint32          r3;
            cyg_uint32          r12;
            cyg_uint32          lr;
            cyg_uint32          pc;
            cyg_uint32          psr;
            
        } interrupt;
    } u;

} HAL_SavedRegisters;

#define HAL_SAVEDREGISTERS_EXCEPTION    1
#define HAL_SAVEDREGISTERS_THREAD       2
#define HAL_SAVEDREGISTERS_INTERRUPT    3

//==========================================================================
// Thread context initialization

#define HAL_THREAD_INIT_CONTEXT( __sparg, __thread, __entry, __id )     \
{                                                                       \
    register CYG_WORD __sp = ((CYG_WORD)__sparg) & ~7;                  \
    register CYG_WORD *__ep = (CYG_WORD *)(__sp -= sizeof(CYG_WORD));   \
    register HAL_SavedRegisters *__regs;                                \
    int __i;                                                            \
    __sp = ((CYG_WORD)__sp) &~15;                                       \
    __regs = (HAL_SavedRegisters *)((__sp) - sizeof(__regs->u.thread)); \
    __regs->u.type = HAL_SAVEDREGISTERS_THREAD;                         \
    for( __i = 1; __i < 13; __i++ )                                     \
        __regs->u.thread.r[__i] = 0;                                    \
    *__ep = (CYG_WORD)(__entry);                                        \
    __regs->u.thread.sp       = (CYG_WORD)(__sp);                       \
    __regs->u.thread.r[0]     = (CYG_WORD)(__thread);                   \
    __regs->u.thread.r[1]     = (CYG_WORD)(__id);                       \
    __regs->u.thread.r[11]     = (CYG_WORD)(__ep);                      \
    __regs->u.thread.pc       = (CYG_WORD)__entry;                      \
    __regs->u.thread.basepri  = 0;                                      \
    __sparg = (CYG_ADDRESS)__regs;                                      \
}

//==========================================================================
// Context switch macros.
// The arguments are pointers to locations where the stack pointer
// of the current thread is to be stored, and from where the SP of the
// next thread is to be fetched.

__externC void hal_thread_switch_context( CYG_ADDRESS to, CYG_ADDRESS from );
__externC void hal_thread_load_context( CYG_ADDRESS to ) __attribute__ ((noreturn));

#define HAL_THREAD_SWITCH_CONTEXT(__fspptr,__tspptr)                    \
        hal_thread_switch_context((CYG_ADDRESS)__tspptr,                \
                                  (CYG_ADDRESS)__fspptr);

#define HAL_THREAD_LOAD_CONTEXT(__tspptr)                               \
        hal_thread_load_context( (CYG_ADDRESS)__tspptr );


//==========================================================================
// Fetch PC from saved state

#define CYGARC_HAL_GET_PC_REG(__regs,__val)                             \
{                                                                       \
    switch( (__regs)->u.type )                                          \
    {                                                                   \
    case HAL_SAVEDREGISTERS_THREAD   : (__val) = (__regs)->u.thread.pc; break; \
    case HAL_SAVEDREGISTERS_EXCEPTION: (__val) = (__regs)->u.exception.pc; break; \
    case HAL_SAVEDREGISTERS_INTERRUPT: (__val) = (__regs)->u.interrupt.pc; break; \
    }                                                                   \
}

//==========================================================================
// Exception handling function
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//==========================================================================
// Bit manipulation macros

#define HAL_LSBIT_INDEX(__index, __mask)                                \
{                                                                       \
    register cyg_uint32 __bit = (__mask);                               \
    register int __count;                                               \
    __bit = __bit & -__bit;                                             \
    __asm__ volatile ("clz %0,%1" : "=r"(__count) : "r"(__bit) );       \
    (__index) = 31-__count;                                             \
}

#define HAL_MSBIT_INDEX(__index, __mask)                                \
{                                                                       \
    register cyg_uint32 __bit = (__mask);                               \
    register int __count;                                               \
    __asm__ volatile ("clz %0,%1" : "=r"(__count) : "r"(__bit) );       \
    (__index) = 31-__count;                                             \
}

//==========================================================================
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

//==========================================================================
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen
// if executed.
// HAL_BREAKINST is the value of the breakpoint instruction and 
// HAL_BREAKINST_SIZE is its size in bytes.

#define HAL_BREAKINST           0xbebe         // BKPT

# define HAL_BREAKINST_SIZE      2
# define HAL_BREAKINST_TYPE      cyg_uint16

#define _stringify1(__arg) #__arg
#define _stringify(__arg) _stringify1(__arg)

# define HAL_BREAKPOINT(_label_)                        \
__asm__ volatile (" .globl  " #_label_ ";"              \
              #_label_":"                               \
              " .short  " _stringify(HAL_BREAKINST)     \
    );

//==========================================================================
// GDB support

// Register layout expected by GDB
typedef struct
{
    cyg_uint32  gpr[16];
    cyg_uint32  f0[3];
    cyg_uint32  f1[3];
    cyg_uint32  f2[3];
    cyg_uint32  f3[3];
    cyg_uint32  f4[3];
    cyg_uint32  f5[3];
    cyg_uint32  f6[3];
    cyg_uint32  f7[3];
    cyg_uint32  fps;
    cyg_uint32  ps;
} HAL_CORTEXM_GDB_Registers;

// Translate a stack pointer as saved by the thread context macros
// into a pointer to a HAL_SavedRegisters structure. On the Cortex-M
// these are equivalent.

#define HAL_THREAD_GET_SAVED_REGISTERS(__stack, __regs) \
    CYG_MACRO_START                                     \
    (__regs)    = (HAL_SavedRegisters*)(__stack);       \
    CYG_MACRO_END


__externC void hal_get_gdb_registers( HAL_CORTEXM_GDB_Registers *gdbreg, HAL_SavedRegisters *regs );
__externC void hal_set_gdb_registers( HAL_CORTEXM_GDB_Registers *gdbreg, HAL_SavedRegisters *regs );

#define HAL_GET_GDB_REGISTERS(__regval, __regs) hal_get_gdb_registers( (HAL_CORTEXM_GDB_Registers *)(__regval), (HAL_SavedRegisters *)(__regs) )
#define HAL_SET_GDB_REGISTERS(__regs, __regval) hal_set_gdb_registers( (HAL_CORTEXM_GDB_Registers *)(__regval), (HAL_SavedRegisters *)(__regs) )

//==========================================================================
// HAL setjmp

#define CYGARC_JMP_BUF_SIZE     16

typedef cyg_uint32 hal_jmp_buf[CYGARC_JMP_BUF_SIZE];

__externC int hal_setjmp(hal_jmp_buf env);
__externC void hal_longjmp(hal_jmp_buf env, int val);


//==========================================================================
// Idle thread code.
//
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor. Here we only supply a default fallback if the variant/platform
// doesn't define anything.

#ifndef HAL_IDLE_THREAD_ACTION
#define HAL_IDLE_THREAD_ACTION(__count) __asm__ volatile ( "wfi\n" )
#endif

//==========================================================================
// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis.  Idle thread stack should be this big.

//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!

// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.

// A minimal, optimized stack frame - space for return link plus four
// arguments or local variables.
#define CYGNUM_HAL_STACK_FRAME_SIZE (4 * 20)

// Stack needed for a context switch
#define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 20)

// Interrupt + call to ISR, interrupt_end() and the DSR
#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    (CYGNUM_HAL_STACK_CONTEXT_SIZE + 2 * CYGNUM_HAL_STACK_FRAME_SIZE)

// Space for the maximum number of nested interrupts, plus room to call functions
#define CYGNUM_HAL_MAX_INTERRUPT_NESTING 4

// Minimum stack size. Space for the given number of nested
// interrupts, plus a thread context switch plus a couple of function
// calls.
#define CYGNUM_HAL_STACK_SIZE_MINIMUM \
        ((CYGNUM_HAL_MAX_INTERRUPT_NESTING+1) * CYGNUM_HAL_STACK_INTERRUPT_SIZE + \
         2 * CYGNUM_HAL_STACK_FRAME_SIZE)

// Typical stack size -- used mainly for test programs. The minimum
// stack size plus enough space for some function calls.
#define CYGNUM_HAL_STACK_SIZE_TYPICAL \
        (CYGNUM_HAL_STACK_SIZE_MINIMUM + 32 * CYGNUM_HAL_STACK_FRAME_SIZE)

//==========================================================================
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa).

#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

//==========================================================================
#endif //CYGONCE_HAL_ARCH_H

