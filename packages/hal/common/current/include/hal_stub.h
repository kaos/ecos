#ifndef CYGONCE_HAL_HAL_STUB_H
#define CYGONCE_HAL_HAL_STUB_H

//=============================================================================
//
//      hal_stub.h
//
//      HAL header for GDB stub support.
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-02-12
// Purpose:     Generic HAL stub header.
// Usage:       #include <cyg/hal/hal_stub.h>
// Description: This header is included by generic-stub.c to provide an
//              interface to the eCos-specific stub implementation. It is
//              not to be included by user code, and is only placed in a
//              publically accessible directory so that platform stub packages
//              are able to include it if required.
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#define USE_GDBSTUB_PROTOTYPES 0        // avoid stub-tservice.h atm
#ifndef __ECOS__
#define __ECOS__                        // use to mark eCos hacks
#endif

#include <cyg/hal/basetype.h>           // HAL_LABEL_NAME
#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/infra/cyg_type.h>         // cyg_uint32 and CYG_BYTEORDER

#ifndef __CYGMON_TYPES
#define __CYGMON_TYPES
typedef cyg_uint32 uint32;
typedef cyg_int32  int32;
#endif // __CYGMON_TYPES

#include <cyg/hal/plf_stub.h>
#include <cyg/hal/generic-stub.h>

#ifdef __cplusplus
extern "C" {
#endif
#if 0
} // to make below format correctly.
#endif

//-----------------------------------------------------------------------------
// Definitions for generic-stub.c

#define __set_mem_fault_trap(x) ({__mem_fault = 0; x(); __mem_fault;})

#if (CYG_BYTEORDER==CYG_LSBFIRST)
# if !defined(__LITTLE_ENDIAN__)
#  define __LITTLE_ENDIAN__
# endif
# if !defined(_LITTLE_ENDIAN)
#  define _LITTLE_ENDIAN
# endif
#endif

//----------------------------------------------------------------------------
// Signal definitions to avoid 'signal.h'/
#define SIGHUP  1       /* hangup */
#define SIGINT  2       /* interrupt */
#define SIGQUIT 3       /* quit */
#define SIGILL  4       /* illegal instruction (not reset when caught) */
#define SIGTRAP 5       /* trace trap (not reset when caught) */
#define SIGIOT  6       /* IOT instruction */
#define SIGABRT 6       /* used by abort, replace SIGIOT in the future */
#define SIGEMT  7       /* EMT instruction */
#define SIGFPE  8       /* floating point exception */
#define SIGKILL 9       /* kill (cannot be caught or ignored) */
#define SIGBUS  10      /* bus error */
#define SIGSEGV 11      /* segmentation violation */
#define SIGSYS  12      /* bad argument to system call */
#define SIGPIPE 13      /* write on a pipe with no one to read it */
#define SIGALRM 14      /* alarm clock */
#define SIGTERM 15      /* software termination signal from kill */

//----------------------------------------------------------------------------
// Thread support. This setting is used in thread-pkts.h
#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#define DEBUG_THREADS 1
#else
#define DEBUG_THREADS 0
#endif

// The function stub_copy_registers() is statically defined in
// thread-packets.c, but in eCos this external stub is defined as it
// is used in dbg_gdb.cxx.
externC void __stub_copy_registers(target_register_t *dest, 
                                   target_register_t *src);

//----------------------------------------------------------------------------
// Hardware Watch/Breakpoint support. These are the possible return values
// of HAL_STUB_IS_STOPPED_BY_HARDWARE().
#define HAL_STUB_HW_STOP_NONE   0   // target did not stop for hw watch/break
#define HAL_STUB_HW_STOP_BREAK  1   // target stopped for hw breakpoint
#define HAL_STUB_HW_STOP_WATCH  2   // target stopped for write-only watchpoint
#define HAL_STUB_HW_STOP_RWATCH 3   // target stopped for read-only watchpoint
#define HAL_STUB_HW_STOP_AWATCH 4   // target stopped for access watchpoint

//----------------------------------------------------------------------------
// Memory accessor functions.
#define TARGET_HAS_OWN_MEM_FUNCS

//----------------------------------------------------------------------------
// Target extras?!
extern int __process_target_query(char * pkt, char * out, int maxOut);
extern int __process_target_set(char * pkt, char * out, int maxout);
extern int __process_target_packet(char * pkt, char * out, int maxout);

//---------------------------------------------------------------------------
// Declarations to avoid compiler warnings.

// Set the baud rate for the current serial port.
extern void __set_baud_rate (int baud);

// Write C to the current serial port.
extern void putDebugChar (int c);

// Read one character from the current serial port.
extern int getDebugChar (void);

// Push CH back onto the debug stream.
extern void ungetDebugChar (int ch);

// Reset the board.
extern void __reset (void);

// Multi-bp support.
#ifndef __set_breakpoint
extern int __set_breakpoint (target_register_t addr);
#endif
#ifndef __remove_breakpoint
extern int __remove_breakpoint (target_register_t addr);
#endif

/* Install the standard set of trap handlers for the stub. */
extern void __install_traps (void);

/* Address in text section of a breakpoint instruction.  */

#ifndef BREAKINST_DEFINED
#define BREAKINST_DEFINED
extern void _breakinst (void);
#endif

/* The opcode for a breakpoint instruction.  */

extern unsigned long __break_opcode (void);

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// This one may assume a valid saved interrupt context on some platforms
extern void cyg_hal_gdb_interrupt    (target_register_t pc);
// This one does not; use from CRITICAL_IO_REGION macros below.
extern void cyg_hal_gdb_place_break  (target_register_t pc);
// Remove a break from either above - or not if cyg_hal_gdb_running_step
extern int  cyg_hal_gdb_remove_break (target_register_t pc);
// Bool: is such a breakpoint set?
extern int  cyg_hal_gdb_break_is_set (void);

/* This is used so that the generic stub can tell
 * cyg_hal_gdb_remove_break() not to bother when we are avoiding stepping
 * through a critical region ie. hal_diag_write_char() usually - that
 * shares the GDB IO device.
 */
extern volatile int cyg_hal_gdb_running_step;

// Use these in hal_diag.c when about to write a whole $O packet to GDB.
// NB they require __builtin_return_address() to work: if your platform
// does not support this, use HAL_DISABLE_INTERRUPTS &c instead.

#if 1 // Can use the address of a label: this is more portable

// This macro may already have been defined by the architecture HAL
#ifndef CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION
#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )                        \
do {                                                                         \
    HAL_DISABLE_INTERRUPTS(_old_);                                           \
    cyg_hal_gdb_place_break( (target_register_t)&&cyg_hal_gdb_break_place ); \
} while ( 0 )
#endif

#define CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION( _old_ )                         \
do {                                                                          \
    cyg_hal_gdb_remove_break( (target_register_t)&&cyg_hal_gdb_break_place ); \
    HAL_RESTORE_INTERRUPTS(_old_);                                            \
cyg_hal_gdb_break_place:                                                      \
} while ( 0 )

#else // use __builtin_return_address instead.

#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )                        \
do {                                                                         \
    HAL_DISABLE_INTERRUPTS(_old_);                                           \
    cyg_hal_gdb_place_break((target_register_t)__builtin_return_address(0)); \
} while ( 0 )

#define CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION( _old_ )                         \
do {                                                                          \
    cyg_hal_gdb_remove_break((target_register_t)__builtin_return_address(0)); \
    HAL_RESTORE_INTERRUPTS(_old_);                                            \
} while ( 0 )

#endif

#else // NO debug_gdb_break_support

// so define these just to do interrupts:
#define CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION( _old_ )   \
    HAL_DISABLE_INTERRUPTS(_old_)

#define CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION( _old_ )   \
    HAL_RESTORE_INTERRUPTS(_old_)

#endif

//----------------------------------------------------------------------------
// eCos extensions to the stub

extern void hal_output_gdb_string(target_register_t str, int string_len);

extern target_register_t registers[];   // The current saved registers.
extern target_register_t * _registers ;
extern HAL_SavedRegisters *_hal_registers;

extern int cyg_hal_gdb_break;

//---------------------------------------------------------------------------
// Local strcpy/strlen functions removes libc dependency.
static inline char *strcpy( char *s, const char *t)
{
    char *r = s;

    while( *t ) *s++ = *t++;

    // Terminate the copied string.
    *s = 0;

    return r;
}

static inline int strlen( const char *s )
{
    int r = 0;
    while( *s++ ) r++;
    return r;
}

//-----------------------------------------------------------------------------
// Repeat the cache definitions here to avoid too much hackery in 
// generic-stub.h
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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_STUB_H
// End of hal_stub.h
