#ifndef CYGONCE_HAL_ARCH_H
#define CYGONCE_HAL_ARCH_H

//=============================================================================
//
//      hal_arch.h
//
//      Architecture specific abstractions
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-04-24
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>

//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Processor saved states:
// The layout of this structure is also defined in "sh.inc", for assembly
// code. Do not change this without changing that (or vice versa).

typedef struct 
{
    // These are common to all saved states
    cyg_uint32   r[16];                 // Data regs
    cyg_uint32   mach;                  // Multiply and accumulate - high
    cyg_uint32   macl;                  // Multiply and accumulate - low
    cyg_uint32   pr;                    // Procedure Reg
    cyg_uint32   sr;                    // Status Reg
    cyg_uint32   pc;                    // Program Counter

    // This marks the limit of state saved during a context switch and
    // is used to calculate necessary stack allocation for context switches.
    // It would probably be better to have a union instead...
    cyg_uint32   context_size[0];

    // These are only saved on interrupts
    cyg_uint32   vbr;                   // Vector Base Register
    cyg_uint32   gbr;                   // Global Base Register

    // These are only saved on interrupts
    cyg_uint32   event;                 // EXPEVT or INTEVT
} HAL_SavedRegisters;

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//--------------------------------------------------------------------------
// Bit manipulation routines

externC cyg_uint32 hal_lsbit_index(cyg_uint32 mask);
externC cyg_uint32 hal_msbit_index(cyg_uint32 mask);

#define HAL_LSBIT_INDEX(index, mask) index = hal_lsbit_index(mask);

#define HAL_MSBIT_INDEX(index, mask) index = hal_msbit_index(mask);

//-----------------------------------------------------------------------------
// Context Initialization
// Initialize the context of a thread.
// Arguments:
// _sparg_ name of variable containing current sp, will be written with new sp
// _thread_ thread object address, passed as argument to entry point
// _entry_ entry point address.
// _id_ bit pattern used in initializing registers, for debugging.

#define HAL_THREAD_INIT_CONTEXT( _sparg_, _thread_, _entry_, _id_ )           \
    CYG_MACRO_START                                                           \
    register HAL_SavedRegisters *_regs_;                                      \
    int _i_;                                                                  \
    _regs_ = (HAL_SavedRegisters *)((_sparg_) - sizeof(HAL_SavedRegisters));  \
    for( _i_ = 0; _i_ < 16; _i_++ ) (_regs_)->r[_i_] = (_id_)|_i_;            \
    (_regs_)->r[15] = (CYG_WORD)(_regs_);      /* SP = top of stack      */   \
    (_regs_)->r[04] = (CYG_WORD)(_thread_);    /* R4 = arg1 = thread ptr */   \
    (_regs_)->mach = 0;                        /* MACH = 0               */   \
    (_regs_)->macl = 0;                        /* MACL = 0               */   \
    (_regs_)->pr = (CYG_WORD)(_entry_);        /* PR = entry point       */   \
    (_regs_)->sr = 0;                          /* SR = enable interrupts */   \
    (_regs_)->pc = (CYG_WORD)(_entry_);        /* set PC for thread dbg  */   \
    _sparg_ = (CYG_ADDRESS)_regs_;                                            \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// Context switch macros.
// The arguments are pointers to locations where the stack pointer
// of the current thread is to be stored, and from where the sp of the
// next thread is to be fetched.

externC void hal_thread_switch_context( CYG_ADDRESS to, CYG_ADDRESS from );
externC void hal_thread_load_context( CYG_ADDRESS to )
    __attribute__ ((noreturn));

#define HAL_THREAD_SWITCH_CONTEXT(_fspptr_,_tspptr_)                    \
        hal_thread_switch_context((CYG_ADDRESS)_tspptr_,(CYG_ADDRESS)_fspptr_);

#define HAL_THREAD_LOAD_CONTEXT(_tspptr_)                               \
        hal_thread_load_context( (CYG_ADDRESS)_tspptr_ );

//-----------------------------------------------------------------------------
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

//-----------------------------------------------------------------------------
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen
// if executed.
// HAL_BREAKINST is the value of the breakpoint instruction and 
// HAL_BREAKINST_SIZE is its size in bytes.

// Enable interrupts before doing the trap, or the CPU will reset.
#define HAL_BREAKPOINT(_label_)                 \
    CYG_MACRO_START                             \
    HAL_ENABLE_INTERRUPTS();                    \
    asm volatile (" .globl  _" #_label_ ";_"    \
                  #_label_":"                   \
                  " trapa #0xc3;nop;nop;nop");  \
    CYG_MACRO_END

#define HAL_BREAKINST           0xc3c3

#define HAL_BREAKINST_SIZE      2

//-----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )  \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.    
#define HAL_GET_GDB_REGISTERS( _aregval_, _regs_ )              \
    CYG_MACRO_START                                             \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 16; _i_++ )                             \
        _regval_[_i_] = (_regs_)->r[_i_];                       \
                                                                \
    _regval_[16] = (_regs_)->pc;                                \
    _regval_[17] = (_regs_)->pr;                                \
    _regval_[18] = (_regs_)->gbr;                               \
    _regval_[19] = (_regs_)->vbr;                               \
    _regval_[20] = (_regs_)->mach;                              \
    _regval_[21] = (_regs_)->macl;                              \
    _regval_[22] = (_regs_)->sr;                                \
                                                                \
    /* 23-51 not used atm. */                                   \
    CYG_MACRO_END

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
    CYG_MACRO_START                                             \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 16; _i_++ )                             \
        (_regs_)->r[_i_] = _regval_[_i_];                       \
                                                                \
    (_regs_)->pc = _regval_[16];                                \
    (_regs_)->pr = _regval_[17];                                \
    (_regs_)->gbr  = _regval_[18];                              \
    (_regs_)->vbr  = _regval_[19];                              \
    (_regs_)->mach = _regval_[20];                              \
    (_regs_)->macl = _regval_[21];                              \
    (_regs_)->sr = _regval_[22];                                \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// HAL setjmp

typedef struct {
    cyg_uint32 sp;
    cyg_uint32 pr;
    cyg_uint32 r8;
    cyg_uint32 r9;
    cyg_uint32 r10;
    cyg_uint32 r11;
    cyg_uint32 r12;
    cyg_uint32 r13;
    cyg_uint32 r14;
} hal_jmp_buf_t;

typedef cyg_uint32 hal_jmp_buf[sizeof(hal_jmp_buf_t) / sizeof(cyg_uint32)];

externC int hal_setjmp(hal_jmp_buf env);
externC void hal_longjmp(hal_jmp_buf env, int val);

//-----------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor.

externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_) hal_idle_thread_action(_count_)

//-----------------------------------------------------------------------------
// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis.  Idle thread stack should be this big.

//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!
 
// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.
 
// Stack frame overhead per call. Space to save FP, PR and 8 registers.
#define CYGNUM_HAL_STACK_FRAME_SIZE (4 * 10)

// Stack needed for a context switch (shreg_context_size from sh.inc)
#define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 21)

// Interrupt + call to ISR, interrupt_end() and the DSR
#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    ((24*4 /* sizeof(HAL_SavedRegisters) */) + 2 * CYGNUM_HAL_STACK_FRAME_SIZE)

// We define a minimum stack size as the minimum any thread could ever
// legitimately get away with. We can throw asserts if users ask for less
// than this. Allow enough for three interrupt sources - clock, serial and
// one other

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 

// An interrupt stack which is large enough for all possible interrupt
// conditions (and only used for that purpose) exists.  "User" stacks
// can therefore be much smaller

# define CYGNUM_HAL_STACK_SIZE_MINIMUM \
         (16*CYGNUM_HAL_STACK_FRAME_SIZE + 2*CYGNUM_HAL_STACK_INTERRUPT_SIZE)

#else

// No separate interrupt stack exists.  Make sure all threads contain
// a stack sufficiently large
# define CYGNUM_HAL_STACK_SIZE_MINIMUM                  \
        (((2+3)*CYGNUM_HAL_STACK_INTERRUPT_SIZE) +      \
         (16*CYGNUM_HAL_STACK_FRAME_SIZE))
#endif

// Now make a reasonable choice for a typical thread size. Pluck figures
// from thin air and say 30 call frames with an average of 16 words of
// automatic variables per call frame
#define CYGNUM_HAL_STACK_SIZE_TYPICAL                \
        (CYGNUM_HAL_STACK_SIZE_MINIMUM +             \
         30 * (CYGNUM_HAL_STACK_FRAME_SIZE+(16*4)))

//--------------------------------------------------------------------------
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa).
#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_ARCH_H
// End of hal_arch.h
