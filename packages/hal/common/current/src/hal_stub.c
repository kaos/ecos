//=============================================================================
//
//      hal_stub.c
//
//      Helper functions for stub, specific to eCos HAL
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
// Author(s):   jskov (based on powerpc/cogent hal_stub.c)
// Contributors:jskov
// Date:        1999-02-12
// Purpose:     Helper functions for stub, specific to eCos HAL
// Description: Parts of the GDB stub requirements are provided by
//              the eCos HAL, rather than target and/or board specific
//              code. 
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>           // Our header

#include <cyg/hal/hal_arch.h>           // HAL_BREAKINST
#include <cyg/hal/hal_cache.h>          // HAL_xCACHE_x
#include <cyg/hal/hal_intr.h>           // interrupt disable/restore

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif


//-----------------------------------------------------------------------------
// Extra eCos data.

// Saved registers.
HAL_SavedRegisters *_hal_registers;
target_register_t registers[NUMREGS];
target_register_t alt_registers[NUMREGS] ;  // Thread or saved process state
target_register_t * _registers = registers; // Pointer to current set of registers

// Interrupt control.
static volatile __PFI __interruptible_control;

//-----------------------------------------------------------------------------
// Register access

// Return the currently-saved value corresponding to register REG of
// the exception context.
target_register_t 
get_register (regnames_t reg)
{
    return _registers[reg];
}

// Store VALUE in the register corresponding to WHICH in the exception
// context.
void 
put_register (regnames_t which, target_register_t value)
{
    _registers[which] = value;
}

//-----------------------------------------------------------------------------
// Serial stuff

// Write C to the current serial port.
void 
putDebugChar (int c)
{
    HAL_STUB_PLATFORM_PUT_CHAR(c);
}

// Read one character from the current serial port.
int 
getDebugChar (void)
{
    return HAL_STUB_PLATFORM_GET_CHAR();
}

// Set the baud rate for the current serial port.
void 
__set_baud_rate (int baud) 
{
    HAL_STUB_PLATFORM_SET_BAUD_RATE(baud);
}

//-----------------------------------------------------------------------------
// GDB interrupt (BREAK) support.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

#if (HAL_BREAKINST_SIZE == 2)
typedef cyg_uint16 t_inst;
#elif (HAL_BREAKINST_SIZE == 4)
typedef cyg_uint32 t_inst;
#else
#error "Don't know how to handle that size"
#endif

typedef struct
{
  t_inst *targetAddr;
  t_inst savedInstr;
} instrBuffer;

static instrBuffer break_buffer;

void 
cyg_hal_gdb_interrupt (target_register_t pc)
{
    if (NULL == break_buffer.targetAddr) {
        break_buffer.targetAddr = (t_inst*) pc;
        break_buffer.savedInstr = *(t_inst*)pc;
        *(t_inst*)pc = (t_inst)HAL_BREAKINST;

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
    }
}

int 
cyg_hal_gdb_remove_break (target_register_t pc)
{
    if ((t_inst*)pc == break_buffer.targetAddr) {
        *(t_inst*)pc = break_buffer.savedInstr;
        break_buffer.targetAddr = NULL;

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
        return 1;
    }
    return 0;
}

#endif // CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

// Use this function to disable serial interrupts whenever reply
// characters are expected from GDB.  The reason we want to control
// whether the target can be interrupted or not is simply that GDB on
// the host will be sending acknowledge characters/commands while the
// stub is running - if serial interrupts were still active, the
// characters would never reach the (polling) getDebugChar.
static void
interruptible(int state)
{
    static int __interrupts_suspended = 0;

    if (__interruptible_control) {
        if (state) {
            __interrupts_suspended--;
            if (0 >= __interrupts_suspended) {
                __interrupts_suspended = 0;
                __interruptible_control(1);
            }
        } else {
            __interrupts_suspended++;
            if (1 == __interrupts_suspended)
                __interruptible_control(0);
        }
    }
}

//-----------------------------------------------------------------------------
// eCos stub entry and exit magic.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
int cyg_hal_gdb_break;
#endif

// Called at stub *entry*
static void 
handle_exception_cleanup( void )
{
    interruptible(0);

    // Expand the HAL_SavedRegisters structure into the GDB register
    // array format.
    HAL_GET_GDB_REGISTERS(&registers[0], _hal_registers);
    _registers = &registers[0];

#ifdef HAL_STUB_PLATFORM_STUBS_FIXUP
    // Some architectures may need to fix the PC in case of a partial
    // or fully executed trap instruction. GDB only takes correct action
    // when the PC is pointing to the breakpoint instruction it set.
    // 
    // Most architectures would leave PC pointing at the trap
    // instruction itself though, and so do not need to do anything
    // special.
    HAL_STUB_PLATFORM_STUBS_FIXUP();
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // FIXME: (there may be a better way to do this)
    // If we hit a breakpoint set by the gdb interrupt stub, make it
    // seem like an interrupt rather than having hit a breakpoint.
    if (cyg_hal_gdb_remove_break(get_register (PC)))
        cyg_hal_gdb_break = 1;
    else
        cyg_hal_gdb_break = 0;
#endif
}

// Called at stub *exit*
static void 
handle_exception_init( void )
{
    // Compact register array again.
    HAL_SET_GDB_REGISTERS(_hal_registers, &registers[0]);

    interruptible(1);
}


//-----------------------------------------------------------------------------
// Initialization.

// Signal handler.
int 
cyg_hal_process_signal (int signal)
{
    // We don't care about the signal (atm).
    return 0;
}

// Install the standard set of trap handlers for the stub.
void 
__install_traps (void)
{
    // Set signal handling vector so we can treat 'C<signum>' as 'c'.
    __process_signal_vec = &cyg_hal_process_signal;
    // Set exit vector to reset vector. This will allow automatic reset on
    // some platforms.
    __process_exit_vec = &__reset;

    __cleanup_vec = &handle_exception_cleanup;
    __init_vec    = &handle_exception_init;

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // Control of GDB interrupts.
    __interruptible_control = HAL_STUB_PLATFORM_INTERRUPTIBLE;
#endif

    // Nothing further to do, handle_exception will be called when an
    // exception occurs.
}

// Initialize the hardware.
void 
initHardware (void) 
{
    static int initialized = 0;

    if (!initialized) {
        initialized = 1;

#ifdef HAL_STUB_PLATFORM_INIT
        // If the platform defines any initialization code, call it here.
        HAL_STUB_PLATFORM_INIT();
#endif        
                
        // Get serial port initialized.
        HAL_STUB_PLATFORM_INIT_SERIAL();

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        // Get interrupt handler initialized.
        HAL_STUB_PLATFORM_INIT_BREAK_IRQ();
#endif

#ifdef CYG_HAL_STARTUP_STUBS
        // There may be extra initialization required when configured
        // for stubs startup.
        HAL_STUB_PLATFORM_STUBS_INIT();
#endif
    }
}

// Reset the board.
void 
__reset (void)
{
    HAL_STUB_PLATFORM_RESET();
}

//-----------------------------------------------------------------------------
// Breakpoint support.

#ifndef CYGPKG_HAL_ARM
// This function will generate a breakpoint exception.  It is used at
// the beginning of a program to sync up with a debugger and can be
// used otherwise as a quick means to stop program execution and
// "break" into the debugger.
void
breakpoint()
{
    HAL_BREAKPOINT(_breakinst);
}

// This function returns the opcode for a 'trap' instruction.
unsigned long 
__break_opcode ()
{
  return HAL_BREAKINST;
}
#endif

// FIXME: Add support for multiple breakpoints (libstub/bplist-dynamic.c)
int __set_breakpoint (target_register_t addr)    { return 0; }
int __remove_breakpoint (target_register_t addr) { return 0; }

//-----------------------------------------------------------------------------
// Write the 'T' packet in BUFFER. SIGVAL is the signal the program received.
void 
__build_t_packet (int sigval, char *buf)
{
    target_register_t addr;
    char *ptr = buf;
    target_register_t sp;

    sp = (target_register_t) get_register (SP);

    *ptr++ = 'T';
    *ptr++ = __tohex (sigval >> 4);
    *ptr++ = __tohex (sigval);

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
    // Include thread ID if thread manipulation is required.
    {
        int id;
        
        *ptr++ = 't';
        *ptr++ = 'h';
        *ptr++ = 'r';
        *ptr++ = 'e';
        *ptr++ = 'a';
        *ptr++ = 'd';
        *ptr++ = ':';

#if (CYG_BYTEORDER == CYG_MSBFIRST)
        id = dbg_currthread_id ();
#else
        // FIXME: Temporary workaround for PR 18903. Thread ID must be
        // big-endian in the T packet.
        {
            unsigned char* bep = (unsigned char*)&id;
            int be_id;

            be_id = dbg_currthread_id ();
            *bep++ = (be_id >> 24) & 0xff ;
            *bep++ = (be_id >> 16) & 0xff ;
            *bep++ = (be_id >> 8) & 0xff ;
            *bep++ = (be_id & 0xff) ;
        }
#endif
        ptr = __mem2hex((char *)&id, ptr, sizeof(id), 0);
        *ptr++ = ';';
    }
#endif

    *ptr++ = __tohex (PC >> 4);
    *ptr++ = __tohex (PC);
    *ptr++ = ':';
    addr = get_register (PC);
    ptr = __mem2hex((char *)&addr, ptr, sizeof(addr), 0);
    *ptr++ = ';';

    *ptr++ = __tohex (SP >> 4);
    *ptr++ = __tohex (SP);
    *ptr++ = ':';
    ptr = __mem2hex((char *)&sp, ptr, sizeof(sp), 0);
    *ptr++ = ';';
    
    *ptr++ = 0;
}


//-----------------------------------------------------------------------------
// Cache functions.

// Perform the specified operation on the instruction cache. 
// Returns 1 if the cache is enabled, 0 otherwise.
int 
__instruction_cache (cache_control_t request)
{
    int state = 1;

    switch (request) {
    case CACHE_ENABLE:
        HAL_ICACHE_ENABLE();
        break;
    case CACHE_DISABLE:
        HAL_ICACHE_DISABLE();
        state = 0;
        break;
    case CACHE_FLUSH:
        HAL_ICACHE_SYNC();
#ifndef CYGPKG_HAL_MIPS
        // SYNC and INVALIDATE_ALL are the same on the MIPS,
        // so avoid doing it twice.
        HAL_ICACHE_INVALIDATE_ALL();
#endif        
        break;
    case CACHE_NOOP:
        /* fall through */
    default:
        break;
    }

#ifdef HAL_ICACHE_IS_ENABLED
    HAL_ICACHE_IS_ENABLED(state);
#endif

    return state;
}

// Perform the specified operation on the data cache. 
// Returns 1 if the cache is enabled, 0 otherwise.
int 
__data_cache (cache_control_t request)
{
    int state = 1;

    switch (request) {
    case CACHE_ENABLE:
        HAL_DCACHE_ENABLE();
        break;
    case CACHE_DISABLE:
        HAL_DCACHE_DISABLE();
        state = 0;
        break;
    case CACHE_FLUSH:
        HAL_DCACHE_SYNC();
#ifndef CYGPKG_HAL_MIPS
        // SYNC and INVALIDATE_ALL are the same on the MIPS,
        // so avoid doing it twice.
        HAL_DCACHE_INVALIDATE_ALL();
#endif        
        break;
    case CACHE_NOOP:
        /* fall through */
    default:
        break;
    }
#ifdef HAL_DCACHE_IS_ENABLED
    HAL_DCACHE_IS_ENABLED(state);
#endif

    return state;
}

//-----------------------------------------------------------------------------
// Memory accessor functions.

void *__mem_fault_handler = (void *)0;

/* These are the "arguments" to __do_read_mem and __do_write_mem, 
   which are passed as globals to avoid squeezing them thru
   __set_mem_fault_trap.  */

static volatile target_register_t memCount;

static void
__do_copy_mem (unsigned char* src, unsigned char* dst)
{
  __mem_fault = 1;                      // Defaults to 'fail'. Is cleared
                                        // when the copy loop completes.
  __mem_fault_handler = &&err;

  while (memCount)
    {
      *dst++ = *src++;
      memCount--;
    }

  __mem_fault = 0;

 err:
  __mem_fault_handler = (void *)0;
}

/*
 * __read_mem_safe:
 * Get contents of target memory, abort on error.
 */

int
__read_mem_safe (unsigned char *dst, target_register_t src, int count)
{
  memCount = count;
  __do_copy_mem((unsigned char*) src, (unsigned char*) dst);
  return count - memCount;      // return number of bytes successfully read
}

/*
 * __write_mem_safe:
 * Set contents of target memory, abort on error.
 */

int
__write_mem_safe (unsigned char *src, target_register_t dst, int count)
{
  memCount = count;
  __do_copy_mem((unsigned char*) src, (unsigned char*) dst);
  return count - memCount;      // return number of bytes successfully written
}

//-----------------------------------------------------------------------------
// Target extras?!
int 
__process_target_query(char * pkt, char * out, int maxOut)
{ return 0 ; }
int 
__process_target_set(char * pkt, char * out, int maxout)
{ return 0 ; }
int 
__process_target_packet(char * pkt, char * out, int maxout)
{ return 0 ; }

// GDB string output, making sure interrupts are disabled.
// This function gets used by some diag output functions.
void 
hal_output_gdb_string(target_register_t str, int string_len)
{
    unsigned long __state;
    HAL_DISABLE_INTERRUPTS(__state);
    __output_gdb_string(str, string_len);
    HAL_RESTORE_INTERRUPTS(__state);
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
