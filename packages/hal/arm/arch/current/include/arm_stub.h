#ifndef CYGONCE_HAL_ARM_STUB_H
#define CYGONCE_HAL_ARM_STUB_H
//========================================================================
//
//      arm_stub.h
//
//      ARM-specific definitions for generic stub
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Red Hat, gthomas
// Contributors:  Red Hat, gthomas
// Date:          1998-11-26
// Purpose:       
// Description:   ARM-specific definitions for generic stub
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

#ifdef __cplusplus
extern "C" {
#endif


#define NUMREGS    (16+8+2)  // 16 GPR, 8 FPR (unused), 2 PS

#define REGSIZE( _x_ ) (((_x_) < F0 || (_x_) >= FPS) ? 4 : 12)

#ifndef TARGET_REGISTER_T_DEFINED
#define TARGET_REGISTER_T_DEFINED
typedef unsigned long target_register_t;
#endif

enum regnames {
    R0, R1, R2, R3, R4, R5, R6, R7, 
    R8, R9, R10, FP, IP, SP, LR, PC,
    F0, F1, F2, F3, F4, F5, F6, F7, 
    FPS, PS
};

#define PS_N 0x80000000
#define PS_Z 0x40000000
#define PS_C 0x20000000
#define PS_V 0x10000000

#define PS_I CPSR_IRQ_DISABLE
#define PS_F CPSR_FIQ_DISABLE

typedef enum regnames regnames_t;

/* Given a trap value TRAP, return the corresponding signal. */
extern int __computeSignal (unsigned int trap_number);

/* Return the SPARC trap number corresponding to the last-taken trap. */
extern int __get_trap_number (void);

/* Return the currently-saved value corresponding to register REG. */
extern target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
extern void put_register (regnames_t which, target_register_t value);

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */
extern void set_pc (target_register_t pc);

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
void __single_step (void);

/* Clear the single-step state. */
void __clear_single_step (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
extern int __is_breakpoint_function (void);

/* Skip the current instruction. */
extern void __skipinst (void);

extern void __install_breakpoints (void);

extern void __clear_breakpoints (void);

//------------------------------------------------------------------------
// Special definition of CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// we can only do this at all if break support is enabled:

#ifdef __thumb__
// If this macro is used from Thumb code, we need to pass this information
// along to the place_break function so it can do the right thing.
#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )                         \
do {                                                                          \
    HAL_DISABLE_INTERRUPTS(_old_);                                            \
    cyg_hal_gdb_place_break((target_register_t)((unsigned long)&&cyg_hal_gdb_break_place + 1));\
} while ( 0 )

#else

#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )                       \
do {                                                                        \
    HAL_DISABLE_INTERRUPTS(_old_);                                          \
    cyg_hal_gdb_place_break((target_register_t)&&cyg_hal_gdb_break_place ); \
} while ( 0 )

#endif // __thumb_
#endif // CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // ifndef CYGONCE_HAL_ARM_STUB_H
