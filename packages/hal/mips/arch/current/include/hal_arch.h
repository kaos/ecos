#ifndef CYGONCE_HAL_HAL_ARCH_H
#define CYGONCE_HAL_HAL_ARCH_H

//=============================================================================
//
//	hal_arch.h
//
//	Architecture specific abstractions
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
// Author(s):   nickg
// Contributors:  nickg
// Date:        1998-02-05
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>
//		
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Processor saved states:
// The layout of this structure is also defined in "mips.inc", for assembly
// code. Do not change this without changing that (or vice versa).

typedef struct 
{
    // These are common to all saved states
    cyg_uint32  d[32];                  /* Data regs                    */
    cyg_uint32  hi;                     /* hi word of mpy/div reg       */
    cyg_uint32  lo;                     /* lo word of mpy/div reg       */

    // These are only saved for exceptions and interrupts
    cyg_uint32  vector;                 /* Vector number                */
    cyg_uint32  pc;                     /* Program Counter              */
    cyg_uint32  sr;                     /* Status Reg                   */
    cyg_uint32  cache;                  /* Cache control register       */


    // These are only saved for exceptions, and are not restored
    // when continued.
    cyg_uint32  cause;                  /* Exception cause register     */
    cyg_uint32  badvr;                  /* Bad virtual address reg      */
    cyg_uint32  prid;                   /* Processor Version            */
    cyg_uint32  config;                 /* Config register              */
} HAL_SavedRegisters;

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Bit manipulation macros

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
{                                                                             \
    register CYG_WORD _sp_ = ((CYG_WORD)_sparg_)-56;                          \
    register HAL_SavedRegisters *_regs_;                                      \
    int _i_;                                                                  \
    _regs_ = (HAL_SavedRegisters *)((_sp_) - sizeof(HAL_SavedRegisters));     \
    for( _i_ = 0; _i_ < 32; _i_++ ) (_regs_)->d[_i_] = (_id_)|_i_;            \
    (_regs_)->d[29] = (CYG_WORD)(_sp_);         /* SP = top of stack       */ \
    (_regs_)->d[04] = (CYG_WORD)(_thread_);     /* R4 = arg1 = thread ptr  */ \
    (_regs_)->lo = 0;                           /* LO = 0                  */ \
    (_regs_)->hi = 0;                           /* HI = 0                  */ \
    (_regs_)->d[30] = (CYG_WORD)(_sp_);         /* FP = top of stack       */ \
    (_regs_)->d[31] = (CYG_WORD)(_entry_);      /* LR(d[31]) = entry point */ \
    (_regs_)->pc = (CYG_WORD)(_entry_);         /* PC = entry point        */ \
    _sparg_ = (CYG_ADDRESS)_regs_;                                            \
}

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
// The "memory" keyword is potentially unnecessary, but it is harmless to
// keep it.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

//-----------------------------------------------------------------------------
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen if
// executed.
// HAL_BREAKINST is the value of the breakpoint instruction and HAL_BREAKINST_SIZE
// is its size in bytes.

#define HAL_BREAKPOINT(_label_)                 \
asm volatile (" .globl	" #_label_ ";"          \
              #_label_":"                       \
              " break	5"                      \
    );

#define HAL_BREAKINST           0x0005000d

#define HAL_BREAKINST_SIZE      4

//-----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )                          \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.    
#define HAL_GET_GDB_REGISTERS( _aregval_ , _regs_ )                     \
{                                                                       \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);               \
    int _i_;                                                            \
                                                                        \
    for( _i_ = 0; _i_ < 32; _i_++ )                                     \
        _regval_[_i_] = (_regs_)->d[_i_];                               \
                                                                        \
    _regval_[32] = (_regs_)->sr;                                        \
    _regval_[33] = (_regs_)->lo;                                        \
    _regval_[34] = (_regs_)->hi;                                        \
    _regval_[35] = (_regs_)->badvr;                                     \
    _regval_[36] = (_regs_)->cause;                                     \
    _regval_[37] = (_regs_)->pc;                                        \
}

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
{                                                               \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);       \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 32; _i_++ )                             \
        (_regs_)->d[_i_] = _regval_[_i_];                       \
                                                                \
    (_regs_)->sr = _regval_[32];                                \
    (_regs_)->lo = _regval_[33];                                \
    (_regs_)->hi = _regval_[34];                                \
    (_regs_)->badvr = _regval_[35];                             \
    (_regs_)->cause = _regval_[36];                             \
    (_regs_)->pc = _regval_[37];                                \
}

//-----------------------------------------------------------------------------
// HAL setjmp

#define HAL_JMP_BUF_SIZE 11

typedef cyg_uint32 hal_jmp_buf[HAL_JMP_BUF_SIZE];

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
#endif // CYGONCE_HAL_HAL_ARCH_H
// End of hal_arch.h


