#ifndef CYGONCE_HAL_HAL_ARCH_H
#define CYGONCE_HAL_HAL_ARCH_H

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
// Author(s):   proven
// Contributors:proven, pjo, nickg
// Date:        1998-10-05
// Purpose:     Define architecture abstractions
// Usage:       #include <cyg/hal/hal_arch.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

#include <cyg/hal/var_arch.h>

//-----------------------------------------------------------------------------
// Processor saved states. This structure is also defined in i386.inc for
// assembly code. Do not change this without changing that (or vice versa).

#ifdef CYGHWR_HAL_I386_FPU

typedef struct
{
    cyg_uint32  fpstate_valid;
    cyg_uint32  fpstate[108/sizeof(cyg_uint32)];
} HAL_FPU_Context;

#endif

typedef struct 
{
#ifdef CYGHWR_HAL_I386_FPU
#ifdef CYGHWR_HAL_I386_FPU_SWITCH_LAZY
    HAL_FPU_Context     *fpucontext;
#else
    HAL_FPU_Context     fpucontext;    
#endif    
#endif    
    cyg_uint32  edi;
    cyg_uint32  esi;
    cyg_uint32  ebp;
    cyg_uint32  esp;
    cyg_uint32  ebx;
    cyg_uint32  edx;
    cyg_uint32  ecx;
    cyg_uint32  eax;    
    cyg_uint32  vector; // if saved on interrupt contains intr vector
    cyg_uint32  pc;
    cyg_uint32  cs;
    cyg_uint32  eflags;
} HAL_SavedRegisters;


//-----------------------------------------------------------------------------
// Exception handling function.
// This function is defined by the kernel according to this prototype. It is
// invoked from the HAL to deal with any CPU exceptions that the HAL does
// not want to deal with itself. It usually invokes the kernel's exception
// delivery mechanism.
externC void cyg_hal_deliver_exception( CYG_WORD code, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Bit manipulation routines

#define HAL_LSBIT_INDEX(index, mask)            \
CYG_MACRO_START                                 \
    asm volatile( "bsfl %1,%0\n"                \
                  : "=r" (index)                \
                  : "r" (mask)                  \
                );                              \
CYG_MACRO_END

#define HAL_MSBIT_INDEX(index, mask)            \
CYG_MACRO_START                                 \
    asm volatile( "bsrl %1,%0\n"                \
                  : "=r" (index)                \
                  : "r" (mask)                  \
                );                              \
CYG_MACRO_END

//-----------------------------------------------------------------------------
// Context Initialization
// Initialize the context of a thread.
// Arguments:
// _sp_ name of variable containing current sp, will be written with new sp
// _thread_ thread object address, passed as argument to entry point
// _entry_ entry point address.
// _id_ bit pattern used in initializing registers, for debugging.

#ifndef CYG_HAL_DEFAULT_CS
#define CYG_HAL_DEFAULT_CS 0x0008
#endif

#ifdef CYGHWR_HAL_I386_FPU
# ifdef CYGHWR_HAL_I386_FPU_SWITCH_LAZY
# define HAL_THREAD_INIT_FPU_CONTEXT_SPACE( __sp, __fpspace )   \
         __sp -= sizeof(HAL_FPU_Context);                       \
         __fpspace = __sp;
# define HAL_THREAD_INIT_FPU_CONTEXT( __regs, __fpspace )                               \
CYG_MACRO_START                                                                         \
    cyg_ucount8 __i;                                                                    \
    HAL_FPU_Context *__fpspace_ = (HAL_FPU_Context *)(__fpspace);                       \
    (__regs)->fpucontext = __fpspace_;                                                  \
    __fpspace_->fpstate_valid = 0;                                                      \
    for( __i = 0; __i < (sizeof(__fpspace_->fpstate)/sizeof(cyg_uint32)); __i++ )       \
        __fpspace_->fpstate[__i] = 0;                                                   \
CYG_MACRO_END
# else
# define HAL_THREAD_INIT_FPU_CONTEXT_SPACE( __sp, __fpspace )                   \
         (__fpspace) = (__fpspace);
# define HAL_THREAD_INIT_FPU_CONTEXT( __regs, __fpspace )                               \
CYG_MACRO_START                                                                         \
    cyg_ucount8 __i;                                                                    \
    HAL_FPU_Context *__fpspace_ = &((__regs)->fpucontext);                              \
    __fpspace_->fpstate_valid = 0;                                                      \
    for( __i = 0; __i < (sizeof(__fpspace_->fpstate)/sizeof(cyg_uint32)); __i++ )       \
        __fpspace_->fpstate[__i] = 0;                                                   \
CYG_MACRO_END
# endif
#else
# define HAL_THREAD_INIT_FPU_CONTEXT_SPACE( __sp, __fpspace )                   \
         (__fpspace) = (__fpspace);
# define HAL_THREAD_INIT_FPU_CONTEXT( __regs, __fpspace )
#endif


#define HAL_THREAD_INIT_CONTEXT( _sparg_, _thread_, _entry_, _id_ )     \
CYG_MACRO_START                                                         \
    register CYG_WORD* _sp_ = ((CYG_WORD*)((_sparg_) &~15));            \
    register CYG_WORD *_fpspace_ = NULL;                                \
    register HAL_SavedRegisters *_regs_;                                \
                                                                        \
    HAL_THREAD_INIT_FPU_CONTEXT_SPACE( _sp_, _fpspace_ );               \
    *(--_sp_) = (CYG_WORD)(0);                                          \
    *(--_sp_) = (CYG_WORD)(0);                                          \
    *(--_sp_) = (CYG_WORD)(_thread_);                                   \
    *(--_sp_) = (CYG_WORD)(0);                                          \
                                                                        \
    _regs_ = (HAL_SavedRegisters *)                                     \
               ((unsigned long)_sp_ - sizeof(HAL_SavedRegisters));      \
    HAL_THREAD_INIT_FPU_CONTEXT( _regs_, _fpspace_ );                   \
    _regs_->eflags = (CYG_WORD)(0x00000200);                            \
    _regs_->cs     = (CYG_WORD)(CYG_HAL_DEFAULT_CS);                    \
    _regs_->pc	   = (CYG_WORD)(_entry_);                               \
    _regs_->vector = (CYG_WORD)(_id_);                                  \
    _regs_->esp    = (CYG_WORD) _sp_-4;                                 \
    _regs_->ebp    = (CYG_WORD)(_id_);                                  \
    _regs_->esi    = (CYG_WORD)(_id_);                                  \
    _regs_->edi    = (CYG_WORD)(_id_);                                  \
    _regs_->eax    = (CYG_WORD)(_id_);                                  \
    _regs_->ebx    = (CYG_WORD)(_id_);                                  \
    _regs_->ecx    = (CYG_WORD)(_id_);                                  \
    _regs_->edx    = (CYG_WORD)(_id_);                                  \
    (_sparg_)      = (CYG_ADDRESS) _regs_;                              \
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
CYG_MACRO_START                                 \
    asm volatile (" .globl  " #_label_ ";"      \
                  #_label_":"                   \
                  "int $3"                      \
        );                                      \
CYG_MACRO_END

#define HAL_BREAKINST                    0xCC
#define HAL_BREAKINST_SIZE               1

//-----------------------------------------------------------------------------
// Thread register state manipulation for GDB support.

// Translate a stack pointer as saved by the thread context macros above into
// a pointer to a HAL_SavedRegisters structure.
#define HAL_THREAD_GET_SAVED_REGISTERS( _sp_, _regs_ )  \
        (_regs_) = (HAL_SavedRegisters *)(_sp_)

// Copy a set of registers from a HAL_SavedRegisters structure into a
// GDB ordered array.    

externC void hal_get_gdb_registers(CYG_ADDRWORD *, HAL_SavedRegisters *);
externC void hal_set_gdb_registers(HAL_SavedRegisters *, CYG_ADDRWORD *);

#define HAL_GET_GDB_REGISTERS( _aregval_, _regs_ ) \
		hal_get_gdb_registers((CYG_ADDRWORD *)(_aregval_), (_regs_))

// Copy a GDB ordered array into a HAL_SavedRegisters structure.
#define HAL_SET_GDB_REGISTERS( _regs_ , _aregval_ ) \
		hal_set_gdb_registers((_regs_), (CYG_ADDRWORD *)(_aregval_))

//-----------------------------------------------------------------------------
// HAL setjmp

#define CYGARC_JMP_BUF_SP        0
#define CYGARC_JMP_BUF_EBP       1
#define CYGARC_JMP_BUF_EBX       2
#define CYGARC_JMP_BUF_ESI       3
#define CYGARC_JMP_BUF_EDI       4
#define CYGARC_JMP_BUF_PC        5

#define CYGARC_JMP_BUF_SIZE      6

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
 
// Stack frame overhead per call. Four arguments, 3 local registers
// (edi, esi, ebx), four local variables and return address.
#define CYGNUM_HAL_STACK_FRAME_SIZE (12 * 4)

// Stack needed for a context switch ( sizeof(HAL_SavedRegisters) ).
#ifdef CYGHWR_HAL_I386_FPU
# ifdef CYGHWR_HAL_I386_FPU_SWITCH_LAZY
#  define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 13)
# else
#  define CYGNUM_HAL_STACK_CONTEXT_SIZE ((4 * 12) + 108)
# endif
#else
# define CYGNUM_HAL_STACK_CONTEXT_SIZE (4 * 12)
#endif

// Interrupt + call to ISR, interrupt_end() and the DSR
#define CYGNUM_HAL_STACK_INTERRUPT_SIZE \
    ((4*CYGNUM_HAL_STACK_CONTEXT_SIZE) + 4 * CYGNUM_HAL_STACK_FRAME_SIZE)

// We define a minimum stack size as the minimum any thread could ever
// legitimately get away with. We can throw asserts if users ask for less
// than this. Allow enough for three interrupt sources - clock, serial and
// one other

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK)

// An interrupt stack which is large enough for all possible interrupt
// conditions (and only used for that purpose) exists.  "User" stacks
// can therefore be much smaller

# define CYGNUM_HAL_STACK_SIZE_MINIMUM \
         (2*CYGNUM_HAL_STACK_FRAME_SIZE + 2*CYGNUM_HAL_STACK_INTERRUPT_SIZE)

#else

// No separate interrupt stack exists.  Make sure all threads contain
// a stack sufficiently large

# define CYGNUM_HAL_STACK_SIZE_MINIMUM                  \
        (((2+3+10)*CYGNUM_HAL_STACK_INTERRUPT_SIZE) +   \
         (2*CYGNUM_HAL_STACK_FRAME_SIZE))

#endif

// Now make a reasonable choice for a typical thread size. Pluck figures
// from thin air and say 15 call frames with an average of 16 words of
// automatic variables per call frame

#define CYGNUM_HAL_STACK_SIZE_TYPICAL                \
        (CYGNUM_HAL_STACK_SIZE_MINIMUM +             \
         15 * (CYGNUM_HAL_STACK_FRAME_SIZE+(16*4)))

//--------------------------------------------------------------------------
// Memory access macros

#define CYGARC_CACHED_ADDRESS(x)                       (x)
#define CYGARC_UNCACHED_ADDRESS(x)                     (x)
#define CYGARC_PHYSICAL_ADDRESS(x)                     (x)

//--------------------------------------------------------------------------
// Region size finder

#if CYGINT_HAL_I386_MEM_REAL_REGION_TOP

externC cyg_uint8 *hal_i386_mem_real_region_top( cyg_uint8 *_regionend_ );
                                                
# define HAL_MEM_REAL_REGION_TOP( _regionend_ ) \
    hal_i386_mem_real_region_top( _regionend_ )
#endif

//--------------------------------------------------------------------------
// Macros for switching context between two eCos instances (jump from
// code in ROM to code in RAM or vice versa).

#define CYGARC_HAL_SAVE_GP()
#define CYGARC_HAL_RESTORE_GP()

//--------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ARCH_H
// End of hal_arch.h
