#ifndef CYGONCE_HAL_ARM_STUB_H
#define CYGONCE_HAL_ARM_STUB_H

/* arm_stub.h - ARM-specific definitions for generic stub
 * 
 * Copyright (c) 1998,1999 Cygnus Solutions
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#define NUMREGS    (16+8+2)  // 16 GPR, 8 FPR (unused), 2 PS

#define REGSIZE( _x_ ) (((_x_) < F0 || (_x_) >= FPS) ? 4 : 12)

typedef unsigned long target_register_t;

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

#endif // ifndef CYGONCE_HAL_ARM_STUB_H
