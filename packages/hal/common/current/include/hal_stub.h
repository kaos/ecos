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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998, 1999 Cygnus Solutions.  
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
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#define USE_GDBSTUB_PROTOTYPES 0        // avoid stub-tservice.h atm
#define __ECOS__                        // use to mark eCos hacks

#include <cyg/hal/basetype.h>           // HAL_LABEL_NAME
#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/infra/cyg_type.h>         // cyg_uint32 and CYG_BYTEORDER
typedef cyg_uint32 uint32;

#include <cyg/hal/plf_stub.h>
#include <cyg/hal/generic-stub.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Definitions for generic-stub.c

#define __set_mem_fault_trap(x) ({__mem_fault = 0; x(); __mem_fault;})

#if (CYG_BYTEORDER==CYG_LSBFIRST) && \
    (!defined(__LITTLE_ENDIAN__) || !defined(_LITTLE_ENDIAN))
# define __LITTLE_ENDIAN__
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
extern int __set_breakpoint (target_register_t addr);
extern int __remove_breakpoint (target_register_t addr);

/* Install the standard set of trap handlers for the stub. */
extern void __install_traps (void);

/* Address in text section of a breakpoint instruction.  */

extern void CYG_LABEL_NAME(breakinst) (void);

/* The opcode for a breakpoint instruction.  */

extern unsigned long __break_opcode (void);

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
extern void cyg_hal_gdb_interrupt    (target_register_t pc);
extern int  cyg_hal_gdb_remove_break (target_register_t pc);
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
