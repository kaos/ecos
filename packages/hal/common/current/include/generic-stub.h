/* Definitions for the generic GDB remote stub
 *
 * Copyright (c) 1997, 1998 Cygnus Solutions
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
#ifndef GENERIC_STUB_H
#define GENERIC_STUB_H

/* Two typedefs must be provided before including this file.
   target_register_t should correspond to the largest register value on
   this target processor.
   regnames_t corresponds to a type representing a register number. */

#ifndef ASM

struct gdb_packet;

/* Set the current serial port being used for stub I/O to PORT. */
extern void __set_serial_port (int port);

/* Set the baud rate for the current serial port. */
extern void __set_baud_rate (int baud);

/* Write C to the current serial port. */
extern void putDebugChar (int c);

/* Read one character from the current serial port. */
extern int getDebugChar (void);

/* Push CH back onto the debug stream. */
extern void ungetDebugChar (int ch);

/* Return the currently-saved value corresponding to register REG. */
extern target_register_t get_register (regnames_t reg);

/* Store VALUE in the register corresponding to WHICH. */
extern void put_register (regnames_t which, target_register_t value);

/* Set the next instruction to be executed when the user program resumes
   execution to PC. */
extern void set_pc (target_register_t pc);

/* Set the return value of the currently-pending syscall to VALUE. */
extern void set_syscall_value (target_register_t value);

/* Return the Nth argument to the currently-pending syscall (starting from
   0). */
extern target_register_t get_syscall_arg (int n);

/* Initialize the stub. This will also install any trap handlers needed by
   the stub. */
extern void initialize_stub (void);

/* Initialize the hardware. */
extern void initHardware (void);

/* Skip the current instruction. */
extern void __skipinst (void);

/* If the breakpoint we hit is in the breakpoint() instruction, return a
   non-zero value. */
extern int __is_breakpoint_function (void);

/* Execute a breakpoint instruction. Restarting will cause the instruction
   to be skipped. */
extern void breakpoint (void);

/* Return the syscall # corresponding to this system call. */
extern int __get_syscall_num (void);

/* Transfer exception event processing to the stub. */
extern void __switch_to_stub (void);

/* Send an exit packet containing the specified status. */
extern void __send_exit_status (int status);

/* Copy COUNT bytes of memory from SRC to DST. A non-zero value will be
   returned if a memory fault occurs during the copy. */
extern int __read_mem_safe (unsigned char *buf, target_register_t addr, int count);
extern int __write_mem_safe (unsigned char *buf, target_register_t addr, int count);

/* Reset the board. */
extern void __reset (void);

/* Flush the instruction cache. */
extern void flush_i_cache (void);

/* Flush the data cache. */
extern void __flush_d_cache (void);

typedef enum {
  CACHE_NOOP, CACHE_ENABLE, CACHE_DISABLE, CACHE_FLUSH
} cache_control_t;

/* Perform the specified operation on the instruction cache. 
   Returns 1 if the cache is enabled, 0 otherwise. */
extern int __instruction_cache (cache_control_t request);
/* Perform the specified operation on the data cache. 
   Returns 1 if the cache is enabled, 0 otherwise. */
extern int __data_cache (cache_control_t request);

/* Write the 'T' packet in BUFFER. SIGVAL is the signal the program
   received. */

extern void __build_t_packet (int sigval, char *buffer);

/* Return 1 when a complete packet has been received, 0 if the packet
   is not yet complete, or -1 if an erroneous packet was NAKed. */
int __add_char_to_packet (int character, struct gdb_packet *packet);

typedef int (*__PFI)(int);
typedef void (*__PFV)(void);

/* When an exception occurs, __process_exception_vec will be invoked with
   the signal number corresponding to the trap/exception. The function
   should return zero if it wishes execution to resume from the saved
   register values; a non-zero value indicates that the exception handler
   should be reinvoked. */
extern volatile __PFI __process_exception_vec;
extern volatile __PFV __process_exit_vec;
extern volatile __PFI __process_syscall_vec;
extern volatile __PFI __process_signal_vec;
extern volatile __PFI __interruptible_control;

/* Send an 'O' packet to GDB containing STR. */
extern int __output_gdb_string (const char *str, int string_len);

/* Request MAXLEN bytes of input from GDB to be stored in DEST. If BLOCK
   is set, GDB should block until MAXLEN bytes are available to be
   read; otherwise, it will return immediately with whatever data is
   available. 
   The return value is the number of bytes written into DEST. */
extern int __get_gdb_input (char *dest, int maxlen, int block);

/* Choose which tty port should be used for GDB debugging. Note that this
   function is completely target-dependent; it may do nothing at all. */
extern void __setTty (int which);

/* Return which port is currently being used for debugging. */
extern int __getTty (void);

extern char __tohex (int c);

unsigned char * __mem2hex (unsigned char *mem, unsigned char *dest, int count, int may_fault);
#endif

#endif
