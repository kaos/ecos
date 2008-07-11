#ifndef CYGONCE_HAL_HAL_ARCH_H
#define CYGONCE_HAL_HAL_ARCH_H

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
// Copyright (C) 2007 eCosCentric Ltd.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   larsi
// Contributors:
// Date:        2006-05-28
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_arch.h>

#define CYG_HAL_FR30_REG CYG_ADDRWORD

//-----------------------------------------------------------------------------
// Processor saved states. This structure is also defined in fr30.inc for
// assembly code. Do not change this without changing that (or vice versa).

typedef struct HAL_SavedRegisters
{
    cyg_uint32  r[16];  // general purpose registers with
                        // r[13]: virtual accumulator (AC)
                        // r[14]: frame pointer (FP)
                        // r[15]: stack pointer (SP) r15 is not saved here!
    cyg_uint32  pc;     // program counter
    cyg_uint32  ps;     // program status (with ILM, SCR, CCR)
    cyg_uint32  tbr;    // table base register (not neccessary,
                        // as it is used system-wide not per thread)
    cyg_uint32  rp;     // return pointer
    // ssp is used system-wide for EIT processing and does not need to be saved here
    // and therefore we don't store usp, because we have it already in r15
    cyg_uint32  ssp;    // system stack pointer
    cyg_uint32  usp;    // user stack pointer
    cyg_uint32  mdh;    // multiplication and division regs /
    cyg_uint32  mdl;    // with high and low words

    cyg_uint32   last_trap; // the last taken trap (for GDB stubs)

} HAL_SavedRegisters;


//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism. FIXME
// declared in src/hal_misc.c
externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Bit manipulation routines
// declared in src/hal_misc.c

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

#define HAL_THREAD_INIT_CONTEXT( _sparg_, _thread_, _entry_, _id_ )                     \
    CYG_MACRO_START                                                                     \
    register CYG_WORD _sp_ = ((CYG_WORD)_sparg_);                                       \
    register HAL_SavedRegisters *_regs_;                                                \
    int _i_;                                                                            \
    _regs_ = (HAL_SavedRegisters *)(((_sp_) - sizeof(HAL_SavedRegisters)) & ~(CYGARC_ALIGNMENT - 1));             \
    for( _i_ = 0; _i_ <= 14; _i_++ ) (_regs_)->r[_i_] = (_id_)|_i_;                     \
    (_regs_)->r[15] = (CYG_HAL_FR30_REG)(_regs_);     /* r15 = USP = top of stack*/     \
    (_regs_)->r[04] = (CYG_HAL_FR30_REG)(_thread_);   /* R4 = arg1 = thread ptr  */     \
    (_regs_)->pc = (CYG_WORD)(_entry_);               /* PC = entry point        */     \
    (_regs_)->ps = (CYG_HAL_FR30_REG)0x1f0030;        /* 0x000F0030;  set flags  */     \
    (_regs_)->tbr = (CYG_HAL_FR30_REG)0x10ffc00;      /*system standard tbr value*/     \
    (_regs_)->rp = (CYG_HAL_FR30_REG)0x0;             /* return pointer = 0x0    */     \
    (_regs_)->ssp = (CYG_HAL_FR30_REG)0x0;            /* R4 = arg1 = thread ptr  */     \
    (_regs_)->usp = (CYG_HAL_FR30_REG)(_regs_);       /* r15 = USP = top of stack*/     \
    (_regs_)->mdh = (CYG_HAL_FR30_REG)0;              /* mdh = 0                 */     \
    (_regs_)->mdl = (CYG_HAL_FR30_REG)0;              /* mdl = 0                 */     \
    (_sparg_) = (CYG_ADDRESS)_regs_;                                                    \
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// Context switch macros.
// The arguments are pointers to locations where the stack pointer
// of the current thread is to be stored, and from where the sp of the
// next thread is to be fetched.
// declared in src/hal_misc.c

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
CYG_MACRO_START                                 \
    asm volatile (" .globl  " #_label_ ";\n"    \
                  #_label_":\n"                 \
                  "int #0x9\n"                  \
        );                                      \
CYG_MACRO_END

// 0x9F30 is the INTE instruction (vector no.9, TBR offset 0x3D8)
// 0x1f09 is the INT# 9 instruction (vector no.9, TBR offset 0x3D8)
#define HAL_BREAKINST                    0x1f09
#define HAL_BREAKINST_SIZE               2
#define HAL_BREAKINST_TYPE               unsigned short

//-----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// Default to a 32 bit register size for GDB register dumps.
#ifndef CYG_HAL_GDB_REG
#define CYG_HAL_GDB_REG CYG_WORD32
#endif

// Register layout expected by GDB
typedef struct 
{
    CYG_HAL_FR30_REG    r[16];          // was: r[0] GPR regs
    CYG_HAL_FR30_REG    pc;
    CYG_HAL_FR30_REG    ps;
    CYG_HAL_FR30_REG    tbr;
    CYG_HAL_FR30_REG    rp;
    CYG_HAL_FR30_REG    ssp;
    CYG_HAL_FR30_REG    usp;
    CYG_HAL_FR30_REG    mdh;
    CYG_HAL_FR30_REG    mdl;
} GDB_Registers;

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure declared in src/fr30_stub.h

#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )  \
         (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.
#define HAL_GET_GDB_REGISTERS( _aregval_, _regs_ )              \
    CYG_MACRO_START                                             \
    GDB_Registers *_gdb_ = (GDB_Registers *)(_aregval_);        \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ <  16; _i_++ ) {                          \
        _gdb_->r[_i_] = (_regs_)->r[_i_];                       \
    }                                                           \
                                                                \
    _gdb_->pc = (_regs_)->pc;                                   \
    _gdb_->ps = (_regs_)->ps;                                   \
    _gdb_->tbr = (_regs_)->tbr;                                 \
    _gdb_->rp = (_regs_)->rp;                                   \
    _gdb_->ssp = (_regs_)->ssp;                                 \
    _gdb_->usp = (_regs_)->usp;                                 \
    _gdb_->mdh = (_regs_)->mdh;                                 \
    _gdb_->mdl = (_regs_)->mdl;                                 \
    CYG_MACRO_END

// Copy a set of registers from a GDB_Registers structure into a
// HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ )             \
    CYG_MACRO_START                                             \
    GDB_Registers *_gdb_ = (GDB_Registers *)(_aregval_);        \
    int _i_;                                                    \
                                                                \
    for( _i_ = 0; _i_ <  16; _i_++ )                            \
        (_regs_)->r[_i_] = _gdb_->r[_i_];                       \
                                                                \
    (_regs_)->pc = _gdb_->pc;                                   \
    (_regs_)->ps = _gdb_->ps;                                   \
    (_regs_)->tbr = _gdb_->tbr;                                 \
    (_regs_)->rp = _gdb_->rp;                                   \
    (_regs_)->ssp = _gdb_->ssp;                                 \
    (_regs_)->usp = _gdb_->usp;                                 \
    (_regs_)->mdh = _gdb_->mdh;                                 \
    (_regs_)->mdl = _gdb_->mdl;                                 \
    CYG_MACRO_END


// -------------------------------------------------------------------------
// hal_setjmp/hal_longjmp


// We must save all of the registers that are preserved across routine
// calls. The assembly code assumes that this structure is defined in the
// following format. Any changes to this structure will result in changes to
// the assembly code!!

typedef struct {
    // registers
    cyg_uint32 r8;
    cyg_uint32 r9;
    cyg_uint32 r10;
    cyg_uint32 r11;
    cyg_uint32 r14;

    // SP and PC
    cyg_uint32 r15; //USP
    cyg_uint32 pc;
} hal_jmp_buf_t;

// This type is used by normal routines to pass the address of the structure
// into our routines without having to explicitly take the address
// of the structure.

typedef cyg_uint32 hal_jmp_buf[sizeof(hal_jmp_buf_t) / sizeof(cyg_uint32)];

// Define the generic setjmp and longjmp routines
externC int hal_setjmp(hal_jmp_buf env);
externC void hal_longjmp(hal_jmp_buf env, int val);
//-----------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor. (contains an empty function call at the moment)
// declared in src/hal_misc.c

externC void hal_idle_thread_action(cyg_uint32 loop_count);

#define HAL_IDLE_THREAD_ACTION(_count_) hal_idle_thread_action(_count_)

//-----------------------------------------------------------------------------
// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis.  Idle thread stack should be this big.
// 
//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!

// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.

//      Stack frame overhead per call.  13 registers (which is a maximum FIXME),
// frame pointer, and return address.  We  can't guess the local variables  so
// just assume that using all of the registers averages out.

#define CYGNUM_HAL_STACK_FRAME_SIZE ((13 + 1 + 1) * 4)

// Stack needed for a context switch.
// it should be sizeof(HAL_SavedRegisters)
//      All registers + PC + PS + RP + MDH + MDL

#ifndef CYGNUM_HAL_STACK_CONTEXT_SIZE
#define CYGNUM_HAL_STACK_CONTEXT_SIZE ((16+1+1+1+1+1)*4)
#endif // CYGNUM_HAL_STACK_CONTEXT_SIZE

// Interrupt + call to ISR, interrupt_end() and the DSR

#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
((CYGNUM_HAL_STACK_CONTEXT_SIZE) + (4*CYGNUM_HAL_STACK_FRAME_SIZE))

// We define a minimum stack size as the minimum any thread could ever
// legitimately get away with. We can throw asserts if users ask for less
// than this. Allow enough for four interrupt sources - clock, serial,
// nic, and one other

#define CYGNUM_HAL_STACK_SIZE_MINIMUM                   \
((4*CYGNUM_HAL_STACK_INTERRUPT_SIZE)            \
 + (16*CYGNUM_HAL_STACK_FRAME_SIZE))

// Now make a reasonable choice for a typical thread size. Pluck figures
// from thin air and say 30 call frames with an average of 16 words of
// automatic variables per call frame

#define CYGNUM_HAL_STACK_SIZE_TYPICAL                   \
(CYGNUM_HAL_STACK_SIZE_MINIMUM +                \
 (30 * (CYGNUM_HAL_STACK_FRAME_SIZE+(16*4))))

//--------------------------------------------------------------------------
// Memory access macros

#define CYGARC_CACHED_ADDRESS(x)                       (x)
#define CYGARC_UNCACHED_ADDRESS(x)                     (x)
#define CYGARC_PHYSICAL_ADDRESS(x)                     (x)
#define CYGARC_VIRTUAL_ADDRESS(x)                      (x)

//--------------------------------------------------------------------------
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa).

#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

//--------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ARCH_H
// End of hal_arch.h
