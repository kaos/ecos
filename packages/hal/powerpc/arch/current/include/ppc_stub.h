#ifndef CYGONCE_HAL_PPC_STUB_H
#define CYGONCE_HAL_PPC_STUB_H

/* ppc_stub.h - PowerPC-specific definitions for generic stub
 * 
 * Copyright (c) 1998 Cygnus Support
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

#define NUMREGS    71

#define REGSIZE( _x_ ) (((_x_) >= F0 && (_x_) <= F31) ? 8 : 4)

typedef unsigned long target_register_t;

enum regnames {
    R0, R1, R2, R3, R4, R5, R6, R7, 
    R8, R9, R10, R11, R12, R13, R14, R15,
    R16, R17, R18, R19, R20, R21, R22, R23, 
    R24, R25, R26, R27, R28, R29, R30, R31, 
    F0, F1, F2, F3, F4, F5, F6, F7, 
    F8, F9, F10, F11, F12, F13, F14, F15, 
    F16, F17, F18, F19, F20, F21, F22, F23, 
    F24, F25, F26, F27, F28, F29, F30, F31, 
    PC, PS, CND, LR, CNT, XER, MQ
};

typedef enum regnames regnames_t;

#include <cyg/hal/generic-stub.h>

/* If ENABLED is non-zero, supply a memory fault handler that traps
   subsequent memory errors and sets _mem_err to 1. Otherwise, memory
   faults are processed as exceptions. */
extern void __set_mem_fault_trap (int enable);

/* Write C to the current serial port. */
extern void putDebugChar (int c);

/* Read one character from the current serial port. */
extern int getDebugChar (void);

/* Given a trap value TRAP, return the corresponding signal. */
extern int __computeSignal (unsigned int trap_number);

/* Install the standard set of trap handlers for the stub. */
extern void __install_traps (void);

/* Return the SPARC trap number corresponding to the last-taken trap. */
extern int __get_trap_number (void);

/* Return the currently-saved value corresponding to register REG. */
extern target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
extern void put_register (regnames_t which, target_register_t value);

/* Set the currently-saved pc register value to PC. This also updates NPC
   as needed. */
extern void set_pc (target_register_t pc);

/* Flush the instruction cache. */
extern void flush_i_cache (void);

/* Reset the board. */
extern void __reset (void);

/* Set the baud rate for the current serial port. */
extern void __set_baud_rate (int baud);

/* Initialize the hardware. */
extern void initHardware (void);

/* Set things up so that the next user resume will execute one instruction.
   This may be done by setting breakpoints or setting a single step flag
   in the saved user registers, for example. */
void single_step (void);

/* Clear the single-step state. */
void clear_single_step (void);

struct gdb_packet;

/* Return 1 when a complete packet has been received, 0 if the packet
   is not yet complete, or -1 if an erroneous packet was NAKed. */
int __add_char_to_packet (int character, struct gdb_packet *packet);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
extern int __is_breakpoint_function (void);

/* Skip the current instruction. */
extern void __skipinst (void);

/* Write the 'T' packet in BUFFER. SIGVAL is the signal the program
   received. */

extern void __build_t_packet (int sigval, char *buffer);

extern void install_breakpoints (void);

/* Address in text section of a breakpoint instruction.  */

extern void _breakinst (void);

/* The opcode for a breakpoint instruction.  */

extern unsigned long __break_opcode (void);

#endif // ifndef CYGONCE_HAL_PPC_STUB_H
