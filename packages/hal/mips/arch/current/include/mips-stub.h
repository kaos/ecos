/* mips-stub.h - MIPS-specific definitions for generic stub
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

#define NUMREGS    90

#ifdef __mips64
  #define REGSIZE(X) 8
  typedef unsigned long long target_register_t;
#else
  #define REGSIZE(X) 4
  typedef unsigned long target_register_t;
#endif

enum regnames {
	ZERO,	AT,	V0,	V1,	A0,	A1,	A2,	A3,
	T0,	T1,	T2,	T3,	T4,	T5,	T6,	T7,
	S0,	S1,	S2,	S3,	S4,	S5,	S6,	S7,
	T8,	T9,	K0,	K1,	GP,	SP,	S8,	RA,
	SR,	LO,	HI,	BAD,	CAUSE,	PC,
	CONFIG = 84,	CACHE,	DEBUG,	DEPC,	EPC
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

