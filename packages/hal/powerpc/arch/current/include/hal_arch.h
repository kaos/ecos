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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:  nickg
// Date:        1997-09-08
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
// The layout of this structure is also defined in "ppc.inc", for assembly
// code. Do not change this without changing that (or vice versa).

typedef struct 
{
    // These are common to all saved states
    cyg_uint32   d[32];                 // Data regs
    cyg_uint32   cr;                    // Condition Reg
    cyg_uint32   xer;                   // XER
    cyg_uint32   lr;                    // Link Reg
    cyg_uint32   ctr;                   // Count Reg

    // These are saved for exceptions and interrupts, but may also
    // be saved in a context switch if thread-aware debugging is enabled.
    cyg_uint32   msr;                   // Machine State Reg
    cyg_uint32   pc;                    // Program Counter

    // These are only saved for exceptions and interrupts
    cyg_uint32   vector;                // Vector number

    // These are only saved for exceptions, and are not restored
    // when continued.
    cyg_uint32   hid0;                  // Hw Implementation Dependent
    cyg_uint32   dar;                   // Data Address Reg
    cyg_uint32   dsisr;                 // DSISR
    cyg_uint32   pvr;                   // Processor Version

    // Eventually add BATs, SRs and FP registers too.
} HAL_SavedRegisters;

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Bit manipulation macros

#define HAL_LSBIT_INDEX(index, mask)    \
    asm ( "neg    11,%1;"               \
          "and    11,11,%1;"            \
          "cntlzw %0,11;"               \
          "subfic %0,%0,31;"            \
          : "=r" (index)                \
          : "r" (mask)                  \
          : "r11"                       \
        );

#define HAL_MSBIT_INDEX(index, mask)                    \
    asm ( "cntlzw %1,%0" : "=r" (mask) : "r" (index) );

//-----------------------------------------------------------------------------
// eABI
#define CYGARC_PPC_STACK_FRAME_SIZE     56      // size of a stack frame

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
    register CYG_WORD _sp_ = ((CYG_WORD)_sparg_)-CYGARC_PPC_STACK_FRAME_SIZE; \
    register HAL_SavedRegisters *_regs_;                                      \
    int _i_;                                                                  \
    _regs_ = (HAL_SavedRegisters *)((_sp_) - sizeof(HAL_SavedRegisters));     \
    for( _i_ = 0; _i_ < 32; _i_++ ) (_regs_)->d[_i_] = (_id_)|_i_;            \
    (_regs_)->d[01] = (CYG_WORD)(_sp_);        /* SP = top of stack      */   \
    (_regs_)->d[03] = (CYG_WORD)(_thread_);    /* R3 = arg1 = thread ptr */   \
    (_regs_)->cr = 0;                          /* CR = 0                 */   \
    (_regs_)->xer = 0;                         /* XER = 0                */   \
    (_regs_)->lr = (CYG_WORD)(_entry_);        /* LR = entry point       */   \
    (_regs_)->pc = (CYG_WORD)(_entry_);        /* set PC for thread dbg  */   \
    (_regs_)->ctr = 0;                         /* CTR = 0                */   \
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

#define HAL_BREAKPOINT(_label_)                 \
asm volatile (" .globl  " #_label_ ";"          \
              #_label_":"                       \
              " trap"                           \
    );

#define HAL_BREAKINST           0x7d821008

#define HAL_BREAKINST_SIZE      4

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
    for( _i_ = 0; _i_ < 32; _i_++ )                             \
        _regval_[_i_] = (_regs_)->d[_i_];                       \
                                                                \
    _regval_[64] = (_regs_)->pc;                                \
    _regval_[65] = (_regs_)->msr;                               \
    _regval_[66] = (_regs_)->cr;                                \
    _regval_[67] = (_regs_)->lr;                                \
    _regval_[68] = (_regs_)->ctr;                               \
    _regval_[69] = (_regs_)->xer;                               \
    CYG_MACRO_END

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
    CYG_MACRO_START                                             \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 32; _i_++ )                             \
        (_regs_)->d[_i_] = _regval_[_i_];                       \
                                                                \
    (_regs_)->pc  = _regval_[64];                               \
    (_regs_)->msr = _regval_[65];                               \
    (_regs_)->cr  = _regval_[66];                               \
    (_regs_)->lr  = _regval_[67];                               \
    (_regs_)->ctr = _regval_[68];                               \
    (_regs_)->xer = _regval_[69];                               \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// HAL setjmp
// Note: These definitions are repeated in context.S. If changes are required
// remember to update both sets.

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_R2        1
#define CYGARC_JMP_BUF_R13       2
#define CYGARC_JMP_BUF_R14       3
#define CYGARC_JMP_BUF_R15       4
#define CYGARC_JMP_BUF_R16       5
#define CYGARC_JMP_BUF_R17       6
#define CYGARC_JMP_BUF_R18       7
#define CYGARC_JMP_BUF_R19       8
#define CYGARC_JMP_BUF_R20       9
#define CYGARC_JMP_BUF_R21      10
#define CYGARC_JMP_BUF_R22      11
#define CYGARC_JMP_BUF_R23      12
#define CYGARC_JMP_BUF_R24      13
#define CYGARC_JMP_BUF_R25      14
#define CYGARC_JMP_BUF_R26      15
#define CYGARC_JMP_BUF_R27      16
#define CYGARC_JMP_BUF_R28      17
#define CYGARC_JMP_BUF_R29      18
#define CYGARC_JMP_BUF_R30      19
#define CYGARC_JMP_BUF_R31      20
#define CYGARC_JMP_BUF_LR       21
#define CYGARC_JMP_BUF_CR       22

#define CYGARC_JMP_BUF_SIZE     23

typedef cyg_uint32 hal_jmp_buf[CYGARC_JMP_BUF_SIZE];

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
 
// Stack frame overhead per call. The PPC ABI defines regs 13..31 as callee
// saved. callee saved variables are irrelevant for us as they would contain
// automatic variables, so we only count the caller-saved regs here
// So that makes r0..r12 + cr, xer, lr, ctr:
#define CYGNUM_HAL_STACK_FRAME_SIZE (4 * 17)

// Stack needed for a context switch (ppcref_context_size from ppc.inc) 
#define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 38)

// Interrupt + call to ISR, interrupt_end() and the DSR
#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    ((43*4 /* sizeof(HAL_SavedRegisters) */) + 2 * CYGNUM_HAL_STACK_FRAME_SIZE)

// We have lots of registers so no particular amount is added in for
// typical local variable usage.

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

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_ARCH_H
// End of hal_arch.h
