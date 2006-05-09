#ifndef CYGONCE_HAL_ARCH_H
#define CYGONCE_HAL_ARCH_H

//=============================================================================
//
//      hal_arch.h
//
//      Architecture specific abstractions
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria
// Contributors:  Wade Jensen
// Date:          2005-25-06
// Purpose:       
// Description:   Definitions for the ColdFire architecture HAL.
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

// Include some variant specific architectural defines
#include <cyg/hal/var_arch.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/coldfire_stub.h>
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

// ----------------------------------------------------------------------------
// Macros to deal with exception stack frame fields

// The ColdFire family of processors has a simplified exception stack
// frame that looks like the following:
//
//          8 +----------------+----------------+
//            |         Program Counter         |
//          4 +----------------+----------------+
//            |Fmt/FS/Vector/FS|      SR        |
//  SP -->  0 +----------------+----------------+
// The stack self-aligns to a 4-byte boundary at an exception, with
// the Fmt/FS/Vector/FS field indicating the size of the adjustment
// (SP += 0,1,2,3 bytes).

// Define the Fmt/FS/Vector/FS word.
// Bits 31-28 are the format word which tells the
// RTI instruction how to align the stack.
#define HAL_CF_EXCEPTION_FORMAT_MSK ((CYG_WORD16)0xF000)
// Bits 25-18 are the vector number of the exception.
#define HAL_CF_EXCEPTION_VECTOR_MSK ((CYG_WORD16)0x03FC)
// Bits 27-26, and 17-16 are the fault status used
// for bus and address errors.
#define HAL_CF_EXCEPTION_FS32_MSK   ((CYG_WORD16)0x0C00)
#define HAL_CF_EXCEPTION_FS10_MSK   ((CYG_WORD16)0x0003)

// Macros to access fields in the format vector word.

#define HAL_CF_EXCEPTION_FORMAT(_fmt_vec_word_)                            \
    ((((CYG_WORD16)(_fmt_vec_word_)) & HAL_CF_EXCEPTION_FORMAT_MSK) >> 12)

#define HAL_CF_EXCEPTION_VECTOR(_fmt_vec_word_)                            \
    ((((CYG_WORD16)(_fmt_vec_word_)) & HAL_CF_EXCEPTION_VECTOR_MSK) >> 2)

#define HAL_CF_EXCEPTION_FS(_fmt_vec_word_)                                \
     (((((CYG_WORD16)(_fmt_vec_word_)) & HAL_CF_EXCEPTION_FS32_MSK) >> 8)  \
     | (((CYG_WORD16)(_fmt_vec_word_)) & HAL_CF_EXCEPTION_FS10_MSK))

// ----------------------------------------------------------------------------
// HAL_SavedRegisters -- Saved by a context switch or by an exception/interrupt

typedef struct
{
    // These are common to all saved states and are in the order
    // stored and loaded by the movem instruction.

    // Data regs D0-D7
    CYG_WORD32 d[8];

    // Address regs A0-A7
    CYG_ADDRESS a[8];

#ifdef CYGHWR_HAL_COLDFIRE_MAC
    // MAC registers
    CYG_WORD32 macc;
    CYG_WORD32 macsr;
    CYG_WORD32 mask;
#endif

    // On exception/interrupt PC, SR and exception are pushed on the
    // stack automatically, so there is no need to allocate the entire
    // structure.
    
    // 16-bit format/vector word
    CYG_WORD16 fmt_vec_word;

    // Status register
    CYG_WORD16 sr;

    // Program counter
    CYG_ADDRESS pc;

} __attribute__ ((aligned, packed)) HAL_SavedRegisters;

#ifndef HAL_THREAD_SWITCH_CONTEXT

// ***************************************************************************
// HAL_THREAD_SWITCH_CONTEXT
// 
// This macro saves the state of the currently running thread and writes
// its stack pointer to *(_fspptr_).
// It then switches to the thread context that *(_tspptr_) points to.
// 
// INPUT:
//  _fspptr_: A pointer to the location to save the current thread's stack
//      pointer to.
// 
//   _tspptr_: A pointer to the location containing the stack pointer of
//      the thread context to switch to.
// 
// OUTPUT:
//   *(_fspptr_): Contains the stack pointer of the previous thread's context.
// 
// ***************************************************************************

externC void hal_thread_switch_context( CYG_ADDRESS to, CYG_ADDRESS from );
externC void hal_thread_load_context( CYG_ADDRESS to )
    CYGBLD_ATTRIB_NORET;

#define HAL_THREAD_SWITCH_CONTEXT(_fspptr_,_tspptr_)                    \
        hal_thread_switch_context((CYG_ADDRESS)_tspptr_,(CYG_ADDRESS)_fspptr_);
#endif // HAL_THREAD_SWITCH_CONTEXT

#ifndef HAL_THREAD_LOAD_CONTEXT

// ***************************************************************************
// hal_thread_load_context
// 
// This routine loads the thread context that *(_tspptr_) points to.
// This routine does not return.
// 
// INPUT:
//   _tspptr_: A pointer to  the location containing  the stack pointer  of
//      the thread context to switch to.
// 
// ***************************************************************************

#define HAL_THREAD_LOAD_CONTEXT(_tspptr_)                               \
        hal_thread_load_context( (CYG_ADDRESS)_tspptr_ );
#endif // HAL_THREAD_LOAD_CONTEXT


#ifndef HAL_THREAD_INIT_CONTEXT

// ***************************************************************************
// HAL_THREAD_INIT_CONTEXT -- Context Initialization
// 
// Initialize the context of a thread.
// 
// INPUT:
//   _sparg_: The name of the variable containing the current sp. This
//      will be written with the new sp.
// 
//   _thread_: The thread object address, passed as argument to entry
//      point.
// 
//   _entry_: The thread's entry point address.
// 
//   _id_: A bit pattern used in initializing registers, for debugging.
// 
// OUTPUT:
//   _sparg_: Updated with the value of the new sp.
// 
// ***************************************************************************

#define HAL_THREAD_INIT_CONTEXT(_sparg_, _thread_, _entry_, _id_)           \
    CYG_MACRO_START                                                         \
    CYG_WORD32 * _sp_ = ((CYG_WORD32*)((CYG_WORD32)(_sparg_) & ~15));       \
    HAL_SavedRegisters * _regs_;                                            \
    int _i_;                                                                \
                                                                            \
    /* Thread's parameter. */                                               \
    *(--_sp_) = (CYG_WORD32)(_thread_);                                     \
    /* Fake thread's return addr. Needed because thread is a function */    \
    /* and parameters to functions are always follwed by the return   */    \
    /* address on the stack.                                          */    \
    *(--_sp_) = (CYG_WORD32)(0xDEADC0DE);                                   \
     /* Thread's return addr. (used by hal_thread_load_context) */          \
    *(--_sp_) = (CYG_WORD32)(_entry_);                                      \
                                                                            \
    _regs_ = (HAL_SavedRegisters *)                                         \
              ((CYG_WORD32)_sp_ - sizeof(HAL_SavedRegisters));              \
                                                                            \
    for (_i_=0; _i_ < 8; _i_++)                                             \
        _regs_->a[_i_] = _regs_->d[_i_] = (_id_);                           \
    /* A6, initial frame pointer should be null */                          \
    _regs_->a[6] = (CYG_ADDRESS)0;                                          \
                                                                            \
    /* Thread's starting SR. All interrupts enabled. */                     \
    _regs_->sr = 0x3000;                                                    \
                                                                            \
    /* Thread's starting PC */                                              \
    _regs_->pc = (CYG_ADDRESS)(_entry_);                                    \
                                                                            \
    (_sparg_) = (CYG_ADDRESS)_regs_;                                        \
    CYG_MACRO_END
#endif // HAL_THREAD_INIT_CONTEXT
    
// ----------------------------------------------------------------------------
// Bit manipulation routines.

externC cyg_uint32 hal_lsbit_index(cyg_uint32 mask);
externC cyg_uint32 hal_msbit_index(cyg_uint32 mask);

#define HAL_LSBIT_INDEX(index, mask) (index) = hal_lsbit_index(mask);

#define HAL_MSBIT_INDEX(index, mask) (index) = hal_msbit_index(mask);

// ----------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor.

externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_) hal_idle_thread_action(_count_)

// ----------------------------------------------------------------------------
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() asm volatile ( "" : : : "memory" )

// ----------------------------------------------------------------------------
// Breakpoint support
// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen
// if executed.
// HAL_BREAKINST is the value of the breakpoint instruction and
// HAL_BREAKINST_SIZE is its size in bytes.

// The host side of GDB debugger uses trap #15 to install breakpoints.

#define CYGNUM_HAL_VECTOR_DEBUGTRAP         47

#define HAL_BREAKPOINT(_label_)                 \
asm volatile (" .globl  " #_label_ ";"          \
              #_label_":"                       \
              " trap #15"                       \
    );

#define HAL_BREAKINST           0x4E4F

#define HAL_BREAKINST_SIZE      2


// ----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

typedef struct {
    cyg_uint32  d[8];
    cyg_uint32  a[8];
    cyg_uint32  pc;
    cyg_uint32  sr;
} GDB_Registers;

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )  \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)


// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.
#define HAL_GET_GDB_REGISTERS( _aregval_, _regs_ )              \
    CYG_MACRO_START                                             \
    union __gdbreguniontype {                                   \
      __typeof__(_aregval_) _aregval2_;                         \
      GDB_Registers *_gdbr;                                     \
    } __gdbregunion;                                            \
    __gdbregunion._aregval2_ = (_aregval_);                     \
    GDB_Registers *_gdb_ = __gdbregunion._gdbr;                 \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 8; _i_++ )                              \
    {                                                           \
        _gdb_->d[_i_] = (_regs_)->d[_i_];                       \
        _gdb_->a[_i_] = (_regs_)->a[_i_];                       \
    }                                                           \
                                                                \
    _gdb_->pc = (_regs_)->pc;                                   \
    _gdb_->sr = (cyg_uint32) ((_regs_)->sr);                    \
    CYG_MACRO_END

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
    CYG_MACRO_START                                             \
    union __gdbreguniontype {                                   \
      __typeof__(_aregval_) _aregval2_;                         \
      GDB_Registers *_gdbr;                                     \
    } __gdbregunion;                                            \
    __gdbregunion._aregval2_ = (_aregval_);                     \
    GDB_Registers *_gdb_ = __gdbregunion._gdbr;                 \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ < 8; _i_++ )                              \
    {                                                           \
        (_regs_)->d[_i_] = _gdb_->d[_i_];                       \
        (_regs_)->a[_i_] = _gdb_->a[_i_];                       \
    }                                                           \
                                                                \
    (_regs_)->pc = _gdb_->pc;                                   \
    (_regs_)->sr = (CYG_WORD16)(_gdb_->sr);                     \
    CYG_MACRO_END


#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && \
      defined(CYGPKG_HAL_EXCEPTIONS)

// ----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.

externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

#endif // defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)

// ----------------------------------------------------------------------------
// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis. Idle thread stack should be this big.

//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!

// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.

// Stack frame overhead per call: 6 data registers, 5 address registers,
// frame pointer, and return address. We can't guess the local variables so
// just assume that using all of the registers averages out.

#define CYGNUM_HAL_STACK_FRAME_SIZE ((6 + 5 + 1 + 1) * 4)

// Stack needed for a context switch.
// All registers + pc + sr + vector.

#ifndef CYGNUM_HAL_STACK_CONTEXT_SIZE
#define CYGNUM_HAL_STACK_CONTEXT_SIZE ((8+8+1)*4 + (1+1)*2)
#endif // CYGNUM_HAL_STACK_CONTEXT_SIZE

// Interrupt (rounded up) + call to ISR, interrupt_end() and the DSR.

#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    ((CYGNUM_HAL_STACK_CONTEXT_SIZE) + (2*CYGNUM_HAL_STACK_FRAME_SIZE))

// We define a minimum stack size as the minimum any thread could ever
// legitimately get away with. We can throw asserts if users ask for less
// than this. Allow enough for four interrupt sources - clock, serial,
// nic, and one other.

// No separate interrupt stack exists. Make sure all threads contain
// a stack sufficiently large.

#define CYGNUM_HAL_STACK_SIZE_MINIMUM                   \
        ((4*CYGNUM_HAL_STACK_INTERRUPT_SIZE)            \
         + (16*CYGNUM_HAL_STACK_FRAME_SIZE))

// Now make a reasonable choice for a typical thread size. Pluck figures
// from thin air and say 30 call frames with an average of 16 words of
// automatic variables per call frame.

#define CYGNUM_HAL_STACK_SIZE_TYPICAL                   \
        (CYGNUM_HAL_STACK_SIZE_MINIMUM +                \
         (30 * (CYGNUM_HAL_STACK_FRAME_SIZE+(16*4))))

// -------------------------------------------------------------------------
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa).

#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

// -------------------------------------------------------------------------
// hal_setjmp/hal_longjmp


// We must save all of the registers that are preserved across routine
// calls. The assembly code assumes that this structure is defined in the
// following format. Any changes to this structure will result in changes to
// the assembly code!!

typedef struct {
    // D registers
    cyg_uint32 d2;
    cyg_uint32 d3;
    cyg_uint32 d4;
    cyg_uint32 d5;
    cyg_uint32 d6;
    cyg_uint32 d7;

    // A registers
    cyg_uint32 a2;
    cyg_uint32 a3;
    cyg_uint32 a4;
    cyg_uint32 a5;
    cyg_uint32 a6;
    
#ifdef CYGHWR_HAL_COLDFIRE_MAC
    // MAC registers
    cyg_uint32 macc;
    cyg_uint32 macsr;
    cyg_uint32 mask;
#endif

    // SP and PC
    cyg_uint32 sp;
    cyg_uint32 pc;
} hal_jmp_buf_t;

// This type is used by normal routines to pass the address of the structure
// into our routines without having to explicitly take the address
// of the structure.

typedef cyg_uint32 hal_jmp_buf[sizeof(hal_jmp_buf_t) / sizeof(cyg_uint32)];

// Define the generic setjmp and longjmp routines
externC int hal_setjmp(hal_jmp_buf env);
externC void hal_longjmp(hal_jmp_buf env, int val);
#define hal_setjmp(_env) hal_setjmp(_env)
#define hal_longjmp(_env, _val) hal_longjmp(_env, _val)

// ---------------------------------------------------------------------------
// End of hal_arch.h
#endif // CYGONCE_HAL_ARCH_H
