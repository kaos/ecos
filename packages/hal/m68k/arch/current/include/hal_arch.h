#ifndef CYGONCE_HAL_ARCH_H
#define CYGONCE_HAL_ARCH_H

//=============================================================================
//
//      hal_arch.h
//
//      Architecture specific abstractions
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2006, 2008 Free Software Foundation, Inc.            
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
//=============================================================================
//####DESCRIPTIONBEGIN####
//
// Author(s): 	bartv
// Date:	2003-06-04
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/var_arch.h>

// ----------------------------------------------------------------------------
// The default IPL level for when interrupts are enabled. Usually this will
// be set to 0, but on some platforms it may be appropriate to run with
// a higher IPL level and effectively leave some interrupts disabled.

#ifndef CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL
# define CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL    0
#endif

// ----------------------------------------------------------------------------
// setjmp/longjmp support. These only deal with the integer and fpu units.
// If there are other hardware units then they are unlikely to be used
// directly by the compiler, only by application code. Hence it is
// application code that should decide whether or not each unit's state
// should be preserved across setjmp/longjmp boundaries.
//
// Floating point registers have to be saved/restored here even if they
// are not saved during a context switch, because we are concerned
// about state within a single thread. The control and status registers
// are saved as well, but fpiar can be ignored - setjmp() is not going
// to happen while handling a floating point exception.
//
// The default implementation is in assembler and uses weak aliases. That
// code has to be kept in step with this structure definition.

#ifndef HAL_SETJMP
#define HAL_SETJMP

typedef struct {
    CYG_ADDRESS pc;
    CYG_ADDRESS sp;
    cyg_uint32  d[6];   // d2 to d7, d0 and d1 are caller-save
    CYG_ADDRESS a[5];   // a2 to a6, a0 and a1 are caller-save, a7 (sp) is separate
#ifdef CYGINT_HAL_M68K_VARIANT_FPU
    long double f[6];   // f2 to f7, f0 and f1 are caller-save
#endif
} hal_jmp_buf_t;

// This type is used by normal routines to pass the address of the
// structure into our routines without having to explicitly take the
// address of the structure.
typedef cyg_uint32 hal_jmp_buf[sizeof(hal_jmp_buf_t) / sizeof(cyg_uint32)];

externC int                         hal_m68k_setjmp(hal_jmp_buf);
externC void                        hal_m68k_longjmp(hal_jmp_buf, int);
#define hal_setjmp(_env)            hal_m68k_setjmp(_env)
#define hal_longjmp(_env, _val)     hal_m68k_longjmp(_env, _val)

#endif // HAL_SETJMP

// ----------------------------------------------------------------------------
// Thread context support. The implementation is in assembler functions
// rather than inline macros. That makes it easier to cope with
// hardware-specific units which have their own context.
//
// A thread context consists of an integer part, a floating point part
// (iff the hardware has a standard FPU and if the configuration makes
// FPU context part of the save state), and OTHER for extra hardware
// units.

#define HAL_M68K_SR_C           (0x01 << 0)
#define HAL_M68K_SR_V           (0x01 << 1)
#define HAL_M68K_SR_Z           (0x01 << 2)
#define HAL_M68K_SR_N           (0x01 << 3)
#define HAL_M68K_SR_X           (0x01 << 4)
#define HAL_M68K_SR_S           (0x01 << 13)
#define HAL_M68K_SR_T           (0x01 << 15)
#define HAL_M68K_SR_IPL_MASK    (0x07 << 8)
#define HAL_M68K_SR_IPL_SHIFT   8

typedef struct {
    // The integer context, d0-d7,a0-a6
    cyg_uint32  da[15];

    // FPU context. This is only relevant if the hardware has an FPU,
    // and then only if the configuration makes the FPU context part
    // of the save state.
#ifdef CYGIMP_HAL_M68K_FPU_SAVE
    cyg_uint32  fpsr;
    CYG_ADDRESS fpiar;
    long double f[8];
#endif

    // Some m68k variants may have additional state that should be part
    // of a thread context.
#ifdef HAL_CONTEXT_OTHER
    HAL_CONTEXT_OTHER
#endif

    // Program counter, status register, etc. The exact layout is
    // determined by the variant. The intention is that the structure
    // matches the state pushed onto the stack by the hardware when an
    // interrupt occurs, so the context can overlap this part of the
    // stack. That avoids having to copy the saved PC and SR registers
    // from the stack into the context structure. The final step of
    // a context switch is an rte instruction.
    HAL_CONTEXT_PCSR
    
} HAL_SavedRegisters;

#define HAL_CONTEXT_INTEGER_SIZE    (15 * 4)
#ifdef CYGIMP_HAL_M68K_FPU_SAVE
# define HAL_CONTEXT_FPU_SIZE       ((2 * 4) + (8 * 12))
#else
# define HAL_CONTEXT_FPU_SIZE       0
#endif
#ifndef HAL_CONTEXT_OTHER_SIZE
# define HAL_CONTEXT_OTHER_SIZE     0
#endif
#ifndef HAL_CONTEXT_PCSR_SIZE
# define HAL_CONTEXT_PCSR_SIZE      8
#endif

#define HAL_CONTEXT_FULL_SIZE       (HAL_CONTEXT_INTEGER_SIZE + HAL_CONTEXT_FPU_SIZE + HAL_CONTEXT_OTHER_SIZE + HAL_CONTEXT_PCSR_SIZE)

// Load and switch are handled by functions in hal_arch.S. One level
// of indirection is removed here for the destination thread, so that
// the actual stack pointer gets passed to assembler.
externC void    hal_thread_load_context(CYG_ADDRESS) CYGBLD_ATTRIB_NORET;
externC void    hal_thread_switch_context(CYG_ADDRESS, CYG_ADDRESS);

#define HAL_THREAD_LOAD_CONTEXT(_to_)                                       \
    CYG_MACRO_START                                                         \
    hal_thread_load_context((CYG_ADDRESS) *(_to_));                         \
    CYG_MACRO_END

#define HAL_THREAD_SWITCH_CONTEXT(_from_, _to_)                             \
    CYG_MACRO_START                                                         \
    hal_thread_switch_context((CYG_ADDRESS)(_from_), (CYG_ADDRESS)*(_to_)); \
    CYG_MACRO_END

// Init context can be done easily in C.
//
// LOAD_CONTEXT and SWITCH_CONTEXT will end up doing an rte, so at the top
// of the stack we want the SR, return PC, a dummy PC for the entry point's
// stack frame, and the argument to the function.
//
//             +----------------+
//             | _thread_ arg   |
//             +----------------+
//             | return PC      |
//             +----------------+
//             | _entry_ PC     |
//             +- - - - - - - - +
//             |     SR         |
//   SP ---->  +- - - - - - - - +
//             |     HAL        |
//             | SavedRegisters |
//             |                |
//
// FPU and OTHER contexts are handled by macros which may or may not expand.
//
// The PC/SR fields are handled by variant-specific code.

#ifdef CYGIMP_HAL_M68K_FPU_SAVE
# define HAL_CONTEXT_FPU_INIT(_regs_)                                       \
    CYG_MACRO_START                                                         \
    int _j_;                                                                \
    (_regs_)->fpsr  = 0;                                                    \
    (_regs_)->fpiar = 0;                                                    \
    for (_j_ = 0; _j_ < 8; _j_++) {                                         \
        (_regs_)->f[_j_] = 0.0;                                             \
    }                                                                       \
    CYG_MACRO_END
#else
# define HAL_CONTEXT_FPU_INIT(_regs_)
#endif
#ifndef HAL_CONTEXT_OTHER_INIT
# define HAL_CONTEXT_OTHER_INIT(_regs_)
#endif

// Only initialize with ints enabled if CYGPKG_KERNEL. RedBoot does a
// LoadContext, causing interrupts to be enabled prematurely.
#ifdef CYGPKG_KERNEL
# define _HAL_M68K_INIT_CONTEXT_SR_ (0x2000 | (CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL<<8))
#else
# define _HAL_M68K_INIT_CONTEXT_SR_ (0x2700)
#endif

#define HAL_THREAD_INIT_CONTEXT( _sparg_, _thread_, _entry_, _id_)                          \
    CYG_MACRO_START                                                                         \
    cyg_uint32* _sp_ = ((cyg_uint32*) ((cyg_uint32)(_sparg_) & ~(CYGARC_ALIGNMENT - 1)));   \
    HAL_SavedRegisters* _regs_;                                                             \
    int         _i_;                                                                        \
    *(--_sp_)   = (cyg_uint32)(_thread_);                                                   \
    *(--_sp_)   = 0xDEADC0DE;                                                               \
    _regs_      = (HAL_SavedRegisters*) ((cyg_uint32)_sp_ - sizeof(HAL_SavedRegisters));    \
    for (_i_ = 0; _i_ < 14; _i_++) {                                                        \
        _regs_->da[_i_] = _id_;                                                             \
    }                                                                                       \
    _regs_->da[14] = 0;                                                                     \
    HAL_CONTEXT_FPU_INIT(_regs_)                                                            \
    HAL_CONTEXT_OTHER_INIT(_regs_)                                                          \
    HAL_CONTEXT_PCSR_INIT(_regs_, _entry_, _HAL_M68K_INIT_CONTEXT_SR_);                     \
    (_sparg_) = (CYG_ADDRESS) (_regs_);                                                     \
    CYG_MACRO_END

// ----------------------------------------------------------------------------
// Minimal and sensible stack sizes: the intention is that applications
// will use these to provide a stack size in the first instance prior to
// proper analysis.  Idle thread stack should be this big.
//
//    THESE ARE NOT INTENDED TO BE MICROMETRICALLY ACCURATE FIGURES.
//           THEY ARE HOWEVER ENOUGH TO START PROGRAMMING.
// YOU MUST MAKE YOUR STACKS LARGER IF YOU HAVE LARGE "AUTO" VARIABLES!
//
// This is not a config option because it should not be adjusted except
// under "enough rope" sort of disclaimers.

// Stack frame overhead per call. 6 data registers, 5 address
// registers, frame pointer, and return address. We can't guess the
// local variables so just assume that using all of the registers
// averages out.
# define CYGNUM_HAL_STACK_FRAME_SIZE ((6 + 5 + 1 + 1) * 4)

// Stack needed for a context switch. Allow for sr and vector as well.
#ifndef CYGNUM_HAL_STACK_CONTEXT_SIZE
# define CYGNUM_HAL_STACK_CONTEXT_SIZE HAL_CONTEXT_FULL_SIZE
#endif

// Interrupt handling. These need to allow for nesting and a
// separate interrupt stack.
#ifndef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
# ifndef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

// No interrupt stack, no nesting. Worst case: a saved context, a
// frame for the interrupt_end() call, six frames for DSR processing,
// another saved context for an interrupt during the DSRs, and
// six frames for the ISR.
#  define CYGNUM_HAL_STACK_INTERRUPT_SIZE ((2 * CYGNUM_HAL_STACK_CONTEXT_SIZE) + (13 * CYGNUM_HAL_STACK_FRAME_SIZE))

# else
// No interrupt stack but nesting. Worst case: a saved context,
// a frame for interrupt_end(), six frames for DSR processing, then
// up to five higher priority interrupts each requiring a context
// and six frames.
#  define CYGNUM_HAL_STACK_INTERRUPT_SIZE ((6 * CYGNUM_HAL_STACK_CONTEXT_SIZE) + (37 * CYGNUM_HAL_STACK_FRAME_SIZE))
# endif

#else

# ifndef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
// An interrupt stack but no nesting. Worst case: a saved context,
// a frame for interrupt_end(), and another saved context. There
// is no need to worry about ISR or DSR frames since those will
// be on the interrupt stack. We also need to allow for nested
// interrupts before these are disabled, which will involve only
// 2 words rather than a full stack.
# define CYGNUM_HAL_STACK_INTERRUPT_SIZE ((2 * CYGNUM_HAL_STACK_CONTEXT_SIZE) + (1 * CYGNUM_HAL_STACK_FRAME_SIZE) + (5 * 2 * 4))

# else
// An interrupt stack and nesting. We need to allow for another five
// nested contexts because nested interrupts may happen after pushing
// the current context but before switching to the interrupt stack.
# define CYGNUM_HAL_STACK_INTERRUPT_SIZE ((7 * CYGNUM_HAL_STACK_CONTEXT_SIZE) + (1 * CYGNUM_HAL_STACK_FRAME_SIZE))

# endif
#endif

// We define a minimum stack size as the minimum any thread could ever
// legitimately get away with. We can throw asserts if users ask for
// less than this. This allows for a saved context for context switching
// plus eight stack frames for cals, in addition to the interrupt overhead.
#define CYGNUM_HAL_STACK_SIZE_MINIMUM (CYGNUM_HAL_STACK_INTERRUPT_SIZE + CYGNUM_HAL_STACK_CONTEXT_SIZE + (8 * CYGNUM_HAL_STACK_FRAME_SIZE))

// Now make a reasonable choice for a typical thread size. Allow for
// another 8 call frames and a K for on-stack buffers, printf(),
// and debugging overheads.
#define CYGNUM_HAL_STACK_SIZE_TYPICAL (CYGNUM_HAL_STACK_SIZE_MINIMUM + (8 * CYGNUM_HAL_STACK_FRAME_SIZE) + 1024)

// -----------------------------------------------------------------------------
// Bit manipulation routines. The vanilla 68000 has no special
// instructions for this so assembler implementations are used
// instead. Newer ColdFires do have suitable instructions so will
// define their own versions of these macro.

#ifndef HAL_LSBIT_INDEX
externC cyg_uint32 hal_lsbit_index(cyg_uint32 mask);
#define HAL_LSBIT_INDEX(index, mask) (index) = hal_lsbit_index(mask);
#endif
#ifndef HAL_MSBIT_INDEX
externC cyg_uint32 hal_msbit_index(cyg_uint32 mask);
#define HAL_MSBIT_INDEX(index, mask) (index) = hal_msbit_index(mask);
#endif

// There are some useful bit-set and bit-clear instructions which allow
// for atomic updates of a single byte of memory.
#define HAL_M68K_BSET(_address_, _bit_)                                                 \
    CYG_MACRO_START                                                                     \
    asm volatile("bset %0,(%1)\n" : : "i" (_bit_), "a" (_address_) : "cc", "memory");   \
    CYG_MACRO_END

#define HAL_M68K_BCLR(_address_, _bit_)                                                 \
    CYG_MACRO_START                                                                     \
    asm volatile("bclr %0,(%1)\n" : : "i" (_bit_), "a" (_address_) : "cc", "memory");   \
    CYG_MACRO_END
                                                     
//-----------------------------------------------------------------------------
// Idle thread code. A plain 68000 has no special support, so a no-op
// function is used. Variants may use this to go into sleep mode.

#ifndef HAL_IDLE_THREAD_ACTION
# define HAL_IDLE_THREAD_ACTION(_count_) CYG_EMPTY_STATEMENT
#endif

//-----------------------------------------------------------------------------
// Execution reorder barrier.
// When optimizing the compiler can reorder code. In multithreaded systems
// where the order of actions is vital, this can sometimes cause problems.
// This macro may be inserted into places where reordering should not happen.

#define HAL_REORDER_BARRIER() __asm__ volatile ( "" : : : "memory" )

//--------------------------------------------------------------------------
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa). The 68000 does not have any
// relevant global state so these macros are no-ops.

#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

//-----------------------------------------------------------------------------
// gdb support

// Translate a stack pointer as saved by the thread context macros
// into a pointer to a HAL_SavedRegisters structure. On the 68K
// these are equivalent.
#define HAL_THREAD_GET_SAVED_REGISTERS(_stack_, _regs_) \
    CYG_MACRO_START                                     \
    (_regs_)    = (HAL_SavedRegisters*)(_stack_);       \
    CYG_MACRO_END

// Translate between an eCos context and a gdb register set.
externC void hal_get_gdb_registers(CYG_ADDRWORD*, HAL_SavedRegisters*);
externC void hal_set_gdb_registers(HAL_SavedRegisters*, CYG_ADDRWORD*);

#define HAL_GET_GDB_REGISTERS(_regval_, _regs_)                                         \
    CYG_MACRO_START                                                                     \
    hal_get_gdb_registers((CYG_ADDRWORD*)(_regval_), (HAL_SavedRegisters*)(_regs_));    \
    CYG_MACRO_END

#define HAL_SET_GDB_REGISTERS(_regs_,_regval_)                                          \
    CYG_MACRO_START                                                                     \
    hal_set_gdb_registers((HAL_SavedRegisters*)(_regs_), (CYG_ADDRWORD*)(_regval_));    \
    CYG_MACRO_END

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

// HAL_BREAKPOINT() is a code sequence that will cause a breakpoint to happen
// if executed.
// HAL_BREAKINST is the value of the breakpoint instruction and
// HAL_BREAKINST_SIZE is its size in bytes.

#define HAL_BREAKPOINT(_label_)                     \
__asm__ volatile (" .globl  " #_label_ ";"          \
              #_label_":"                           \
              " trap #15"                           \
    );

#define HAL_BREAKINST           0x4E4F

#define HAL_BREAKINST_SIZE      2

// The GDB register definitions. as per gdb/m68k-stub.c
// FIXME: more work is needed for floating point support.
enum regnames {
    D0, D1, D2, D3, D4, D5, D6, D7,
    A0, A1, A2, A3, A4, A5, FP, SP,
    PS, PC,
#ifdef CYGINT_HAL_M68K_VARIANT_FPU
    FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7,
    FPCONTROL, FPSTATUS, FPIADDR
#endif
};

#ifdef CYGINT_HAL_M68K_VARIANT_FPU
# define NUMREGS    29
#else
# define NUMREGS    18
#endif

#define REGSIZE(_x_)    (4)

typedef enum regnames regnames_t;
typedef CYG_ADDRWORD  target_register_t;

externC int     __computeSignal(unsigned int trap_number);
externC int     __get_trap_number(void);
externC void    __install_breakpoints(void);
externC void    __clear_breakpoints(void);
externC void    __single_step(void);
externC void    __clear_single_step(void);
externC void    __skipinst(void);
externC int     __is_breakpoint_function(void);
externC void    set_pc(target_register_t);

#define HAL_STUB_PLATFORM_STUBS_FIXUP()                         \
    CYG_MACRO_START                                             \
    if (CYGNUM_HAL_VECTOR_TRAP15 == __get_trap_number())        \
        put_register(PC, get_register(PC) - 2);                 \
    CYG_MACRO_END

#endif  // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && defined(CYGPKG_HAL_EXCEPTIONS)
externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );
#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_ARCH_H
// End of hal_arch.h

