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
// Date:        1997-09-08
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>
//		
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

//-----------------------------------------------------------------------------
// Processor saved states:

typedef struct HAL_SavedRegisters
{
    // These are common to all saved states and are in the order
    // stored and loaded by the movm instruction.
    cyg_uint32  vector;                 /* Vector number/dummy          */
    cyg_uint32  lar;                    /* Loop address register        */
    cyg_uint32  lir;                    /* Loop instruction register    */
    cyg_uint32  mdr;                    /* Multiply/Divide register     */
    cyg_uint32  a1;
    cyg_uint32  a0;
    cyg_uint32  d1;
    cyg_uint32  d0;
    cyg_uint32  a3;
    cyg_uint32  a2;
    cyg_uint32  d3;
    cyg_uint32  d2;

    /* On interrupts the PC and PSW are pushed automatically by the     */
    /* CPU and SP is pushed for debugging reasons. On a thread switch   */
    /* the saved context is made to look the same.                      */

    cyg_uint32  sp;             /* Saved copy of SP in some states      */
    cyg_uint32  psw;            /* Status word                          */
    cyg_uint32  pc;             /* Program Counter                      */
    
} HAL_SavedRegisters;

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Bit manipulation routines

externC cyg_uint32 hal_lsbit_index(cyg_uint32 mask);
externC cyg_uint32 hal_msbit_index(cyg_uint32 mask);

#define HAL_LSBIT_INDEX(index, mask) index = hal_lsbit_index(mask);

#define HAL_MSBIT_INDEX(index, mask) index = hal_msbit_index(mask);

//-----------------------------------------------------------------------------
// Context Initialization
// Initialize the context of a thread.
// Arguments:
// _sp_ name of variable containing current sp, will be written with new sp
// _thread_ thread object address, passed as argument to entry point
// _entry_ entry point address.
// _id_ bit pattern used in initializing registers, for debugging.


#define HAL_THREAD_INIT_CONTEXT( _sp_, _thread_, _entry_, _id_ )            \
{                                                                           \
    register HAL_SavedRegisters *_regs_;                                    \
    _regs_ = (HAL_SavedRegisters *)((_sp_) - sizeof(HAL_SavedRegisters)*2); \
    _regs_->d0    = (CYG_WORD)(_thread_);                                   \
    _regs_->d1    = (_id_)|0xddd1;                                          \
    _regs_->d2    = (_id_)|0xddd2;                                          \
    _regs_->d3    = (_id_)|0xddd3;                                          \
    _regs_->a0    = (_id_)|0xaaa0;                                          \
    _regs_->a1    = (_id_)|0xaaa1;                                          \
    _regs_->a2    = (_id_)|0xaaa2;                                          \
    _regs_->a3    = (_id_)|0xaaa3;                                          \
    _regs_->mdr   = 0;                                                      \
    _regs_->lir   = 0;                                                      \
    _regs_->lar   = 0;                                                      \
/*    _regs_->psw   = 0x0F00;  */                                           \
    _regs_->pc    = (CYG_WORD)(_entry_);                                    \
    _sp_          = (CYG_ADDRESS)_regs_;                                    \
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

#define HAL_REORDER_BARRIER() asm ( "" )

//-----------------------------------------------------------------------------
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen if
// executed.
// HAL_BREAKINST is the value of the breakpoint instruction and HAL_BREAKINST_SIZE
// is its size in bytes.

#define HAL_BREAKPOINT(_label_)                 \
asm volatile (" .globl	" #_label_ ";"          \
              #_label_":"                       \
              ".byte 0xFF"                      \
    );

#define HAL_BREAKINST           0xFF

#define HAL_BREAKINST_SIZE      1

//-----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )                          \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.    
#define HAL_GET_GDB_REGISTERS( _aregval_ , _regs_ )                             \
{                                                                               \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);                       \
                                                                                \
    _regval_[0]         = (_regs_)->d0;                                         \
    _regval_[1]         = (_regs_)->d1;                                         \
    _regval_[2]         = (_regs_)->d2;                                         \
    _regval_[3]         = (_regs_)->d3;                                         \
    _regval_[4]         = (_regs_)->a0;                                         \
    _regval_[5]         = (_regs_)->a1;                                         \
    _regval_[6]         = (_regs_)->a2;                                         \
    _regval_[7]         = (_regs_)->a3;                                         \
                                                                                \
    _regval_[8]         = (CYG_ADDRWORD)(_regs_) +                              \
                          sizeof(HAL_SavedRegisters);                           \
    _regval_[9]         = (_regs_)->pc;                                         \
    _regval_[10]        = (_regs_)->mdr;                                        \
    _regval_[11]        = (_regs_)->psw;                                        \
                                                                                \
    _regval_[12]        = (_regs_)->lar;                                        \
    _regval_[13]        = (_regs_)->lir;                                        \
}

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )                     \
{                                                                       \
    CYG_ADDRWORD *_regval_ = (CYG_ADDRWORD *)(_aregval_);               \
                                                                        \
    (_regs_)->d0             = _regval_[0];                             \
    (_regs_)->d1             = _regval_[1];                             \
    (_regs_)->d2             = _regval_[2];                             \
    (_regs_)->d3             = _regval_[3];                             \
    (_regs_)->a0             = _regval_[4];                             \
    (_regs_)->a1             = _regval_[5];                             \
    (_regs_)->a2             = _regval_[6];                             \
    (_regs_)->a3             = _regval_[7];                             \
                                                                        \
    (_regs_)->pc              = _regval_[9];                            \
    (_regs_)->mdr             = _regval_[10];                           \
                                                                        \
    (_regs_)->lar             = _regval_[12];                           \
    (_regs_)->lir             = _regval_[13];                           \
                                                                        \
    /* We do not allow the SP or PSW to be set. Changing the SP will    \
     * mess up the saved state. No PSW is saved on thread context       \
     * switches, so there is nowhere to save it to.                     \
     */                                                                 \
}

//-----------------------------------------------------------------------------
// HAL setjmp

#define HAL_JMP_BUF_SIZE 6

typedef cyg_uint32 hal_jmp_buf[HAL_JMP_BUF_SIZE];

externC int hal_setjmp(hal_jmp_buf env);
externC void hal_longjmp(hal_jmp_buf env, int val);

//-----------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor.

//externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_)

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ARCH_H
// End of hal_arch.h
