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
#ifdef CYGPKG_CYGMON
#include <pkgconf/cygmon.h>
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>           // Our header

#include <cyg/hal/hal_arch.h>           // HAL_BREAKINST
#include <cyg/hal/hal_cache.h>          // HAL_xCACHE_x
#include <cyg/hal/hal_intr.h>           // interrupt disable/restore

#include <cyg/hal/hal_if.h>             // ROM calling interface
#include <cyg/hal/hal_misc.h>           // Helper functions

#ifdef CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT
#include <cyg/hal/dbg-threads-api.h>    // dbg_currthread_id
#endif

#ifdef USE_LONG_NAMES_FOR_ENUM_REGNAMES
#define PC REG_PC
#define SP REG_SP
#endif

//-----------------------------------------------------------------------------
// Extra eCos data.

// Saved registers.
HAL_SavedRegisters *_hal_registers;
target_register_t registers[NUMREGS];
target_register_t alt_registers[NUMREGS] ;  // Thread or saved process state
target_register_t * _registers = registers; // Pointer to current set of registers

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // this should go away
// Interrupt control.
static volatile __PFI __interruptible_control;
#endif

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
#ifdef CYGPKG_CYGMON
extern void ecos_bsp_console_putc(char);
extern char ecos_bsp_console_getc(void);
#endif

// Write C to the current serial port.
void 
putDebugChar (int c)
{
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    __call_if_debug_procs_t __debug_procs = CYGACC_CALL_IF_DEBUG_PROCS();
    CYGACC_COMM_IF_PUTC(*__debug_procs, c);
#elif defined(CYGPKG_CYGMON)
    ecos_bsp_console_putc(c);
#else
    HAL_STUB_PLATFORM_PUT_CHAR(c);
#endif
}

// Read one character from the current serial port.
int 
getDebugChar (void)
{
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    __call_if_debug_procs_t __debug_procs = CYGACC_CALL_IF_DEBUG_PROCS();
    return CYGACC_COMM_IF_GETC(*__debug_procs);
#elif defined(CYGPKG_CYGMON)
    return ecos_bsp_console_getc();
#else
    return HAL_STUB_PLATFORM_GET_CHAR();
#endif
}

// Set the baud rate for the current serial port.
void 
__set_baud_rate (int baud) 
{
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    __call_if_debug_procs_t __debug_procs = CYGACC_CALL_IF_DEBUG_PROCS();
    CYGACC_COMM_IF_CONTROL(*__debug_procs, __COMMCTL_SETBAUD, baud);
#elif defined(CYGPKG_CYGMON)
    // FIXME!
#else
    HAL_STUB_PLATFORM_SET_BAUD_RATE(baud);
#endif
}

//-----------------------------------------------------------------------------
// GDB interrupt (BREAK) support.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

#ifndef CYGPKG_HAL_ARM

#if (HAL_BREAKINST_SIZE == 1)
typedef cyg_uint8 t_inst;
#elif (HAL_BREAKINST_SIZE == 2)
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

volatile int cyg_hal_gdb_running_step = 0;

void 
cyg_hal_gdb_place_break (target_register_t pc)
{
    cyg_hal_gdb_interrupt( pc ); // Let's hope this becomes a drop-through:
}

void 
cyg_hal_gdb_interrupt (target_register_t pc)
{
    CYGARC_HAL_SAVE_GP();

    // Clear flag that we Continued instead of Stepping
    cyg_hal_gdb_running_step = 0;
    // and override existing break? So that a ^C takes effect...
    if (NULL != break_buffer.targetAddr)
        cyg_hal_gdb_remove_break( (target_register_t)break_buffer.targetAddr );

    if (NULL == break_buffer.targetAddr) {
        break_buffer.targetAddr = (t_inst*) pc;
        break_buffer.savedInstr = *(t_inst*)pc;
        *(t_inst*)pc = (t_inst)HAL_BREAKINST;

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
    }

    CYGARC_HAL_RESTORE_GP();
}

int 
cyg_hal_gdb_remove_break (target_register_t pc)
{
    if ( cyg_hal_gdb_running_step )
        return 0;

    if ((t_inst*)pc == break_buffer.targetAddr) {
        *(t_inst*)pc = break_buffer.savedInstr;
        break_buffer.targetAddr = NULL;

        __data_cache(CACHE_FLUSH);
        __instruction_cache(CACHE_FLUSH);
        return 1;
    }
    return 0;
}

int 
cyg_hal_gdb_break_is_set (void)
{
    if (NULL != break_buffer.targetAddr) {
        return 1;
    }
    return 0;
}

#endif // CYGPKG_HAL_ARM

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

    if (state) {
        __interrupts_suspended--;
        if (0 >= __interrupts_suspended) {
            __interrupts_suspended = 0;
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // this _check_ should go away
            {
                hal_virtual_comm_table_t* __chan;
                __chan = CYGACC_CALL_IF_DEBUG_PROCS();
                CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_IRQ_ENABLE);
            }
#else                
            if (__interruptible_control)
                __interruptible_control(1);
#endif
        }
    } else {
        __interrupts_suspended++;
        if (1 == __interrupts_suspended)
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // this _check_ should go away
            {
                hal_virtual_comm_table_t* __chan;
                __chan = CYGACC_CALL_IF_DEBUG_PROCS();
                CYGACC_COMM_IF_CONTROL(*__chan, __COMMCTL_IRQ_DISABLE);
            }
#else                
            if (__interruptible_control)
                __interruptible_control(0);
#endif
    }
}

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
//-----------------------------------------------------------------------------
// GDB O-packetizer function.

// This gets called via the virtual vector debug comms entry and
// handles O-packetization. The debug comms entries are used for the
// actual device IO.

static cyg_uint8
cyg_hal_gdb_diag_getc(void* __ch_data)
{
    cyg_uint8 __ch;
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_DEBUG_PROCS();
    CYGARC_HAL_SAVE_GP();

    __ch = CYGACC_COMM_IF_GETC(*__chan);

    CYGARC_HAL_RESTORE_GP();

    return __ch;
}


static void
cyg_hal_gdb_diag_putc(void* __ch_data, cyg_uint8 c)
{
    static char line[100];
    static int pos = 0;
    CYGARC_HAL_SAVE_GP();

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {
        CYG_INTERRUPT_STATE old;
        hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_DEBUG_PROCS();

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
#ifdef CYG_HAL_STARTUP_ROM
        HAL_DISABLE_INTERRUPTS(old);
#else
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#endif
        
        while(1)
        {
            static const char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0, c1;
            int i;
        
            CYGACC_COMM_IF_PUTC(*__chan, '$');
            CYGACC_COMM_IF_PUTC(*__chan, 'O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                CYGACC_COMM_IF_PUTC(*__chan, h);
                CYGACC_COMM_IF_PUTC(*__chan, l);
                csum += h;
                csum += l;
            }
            CYGACC_COMM_IF_PUTC(*__chan, '#');
            CYGACC_COMM_IF_PUTC(*__chan, hex[(csum>>4)&0xF]);
            CYGACC_COMM_IF_PUTC(*__chan, hex[csum&0xF]);

            c1 = CYGACC_COMM_IF_GETC(*__chan);
            if( c1 == '+' ) break;

            if( cyg_hal_is_break( &c1 , 1 ) ) {
                // Caller's responsibility to react on this.
                CYGACC_CALL_IF_CONSOLE_INTERRUPT_FLAG_SET(1);
                break;
            }
        }

        pos = 0;
        // And re-enable interrupts
#ifdef CYG_HAL_STARTUP_ROM
        HAL_RESTORE_INTERRUPTS(old);
#else
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#endif
    }

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_gdb_diag_write(void* __ch_data, const cyg_uint8* __buf, 
                       cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        cyg_hal_gdb_diag_putc(__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
cyg_hal_gdb_diag_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = cyg_hal_gdb_diag_getc(__ch_data);

    CYGARC_HAL_RESTORE_GP();
}

static int
cyg_hal_gdb_diag_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    // Do nothing (yet).
    return 0;
}
#endif


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
    // If we continued instead of stepping, when there was a break set
    // ie. we were stepping within a critical region, clear the break, and
    // that flag.  If we stopped for some other reason, this has no effect.
    if ( cyg_hal_gdb_running_step ) {
        cyg_hal_gdb_running_step = 0;
        cyg_hal_gdb_remove_break(get_register (PC));
    }

    // FIXME: (there may be a better way to do this)
    // If we hit a breakpoint set by the gdb interrupt stub, make it
    // seem like an interrupt rather than having hit a breakpoint.
    cyg_hal_gdb_break = cyg_hal_gdb_remove_break(get_register (PC));
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

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // this should go away
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // Control of GDB interrupts.
    __interruptible_control = HAL_STUB_PLATFORM_INTERRUPTIBLE;
#endif
#endif

    // Nothing further to do, handle_exception will be called when an
    // exception occurs.
}

// Initialize the hardware.
void 
initHardware (void) 
{
    static int initialized = 0;

    if (initialized)
        return;
    initialized = 1;

    // Get serial port initialized.
    HAL_STUB_PLATFORM_INIT_SERIAL();

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    {
        hal_virtual_comm_table_t* comm;
        int cur = CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

        // Initialize mangler procs
        CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_MANGLER);
        comm = CYGACC_CALL_IF_CONSOLE_PROCS();
        CYGACC_COMM_IF_WRITE_SET(*comm, cyg_hal_gdb_diag_write);
        CYGACC_COMM_IF_READ_SET(*comm, cyg_hal_gdb_diag_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, cyg_hal_gdb_diag_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, cyg_hal_gdb_diag_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, cyg_hal_gdb_diag_control);

        // Now either restore the previous console channel, or let the
        // mangler stay in its place. The latter happens if the
        // console channel was previously unspecified, or if the
        // previous channel matches the used for GDB communication.
        if (CYGNUM_CALL_IF_SET_COMM_ID_EMPTY != cur
            && CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL != cur)
            CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);

        // Set the debug channel.
        CYGACC_CALL_IF_SET_DEBUG_COMM(CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL);
    }
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT

#ifdef HAL_STUB_PLATFORM_INIT
    // If the platform defines any initialization code, call it here.
    HAL_STUB_PLATFORM_INIT();
#endif        

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT // this should go away
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // Get interrupt handler initialized.
    HAL_STUB_PLATFORM_INIT_BREAK_IRQ();
#endif
#endif // !CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
}

// Reset the board.
void 
__reset (void)
{
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    __call_if_reset_t __rom_reset = CYGACC_CALL_IF_RESET_GET();
    if (__rom_reset)
        __rom_reset();
#else
    HAL_STUB_PLATFORM_RESET();
#endif
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

//-----------------------------------------------------------------------------
// Write the 'T' packet in BUFFER. SIGVAL is the signal the program received.
void 
__build_t_packet (int sigval, char *buf)
{
    target_register_t addr;
    char *ptr = buf;

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
#ifdef CYGARC_REGSIZE_DIFFERS_FROM_TARGET_REGISTER_T
    ptr = __mem2hex((char *)&addr, ptr, sizeof(addr), 0);
    if (sizeof(addr) < REGSIZE(PC))
    {
        // GDB is expecting REGSIZE(PC) number of bytes.
        // We only have sizeof(addr) number.  Let's fill
        // the appropriate number of bytes intelligently.
        target_register_t extend_val = 0;
#ifdef CYGARC_SIGN_EXTEND_REGISTERS
        {
            unsigned long bits_in_addr = (sizeof(addr) << 3);  // ie Size in bytes * 8
            target_register_t sign_bit_mask = (1 << (bits_in_addr - 1));
            if ((addr & sign_bit_mask) == sign_bit_mask)
                extend_val = ~0;
        }
#endif
        ptr = __mem2hex((char *)&extend_val, ptr, REGSIZE(PC) - sizeof(addr), 0);
    }
#else
    ptr = __mem2hex((char *)&addr, ptr, REGSIZE(PC), 0);
#endif
    *ptr++ = ';';

    *ptr++ = __tohex (SP >> 4);
    *ptr++ = __tohex (SP);
    *ptr++ = ':';
    addr = (target_register_t) get_register (SP);
#ifdef CYGARC_REGSIZE_DIFFERS_FROM_TARGET_REGISTER_T
    ptr = __mem2hex((char *)&addr, ptr, sizeof(addr), 0);
    if (sizeof(addr) < REGSIZE(SP))
    {
        // GDB is expecting REGSIZE(SP) number of bytes.
        // We only have sizeof(addr) number.  Let's fill
        // the appropriate number of bytes intelligently.
        target_register_t extend_val = 0;
#ifdef CYGARC_SIGN_EXTEND_REGISTERS
        {
            unsigned long bits_in_addr = (sizeof(addr) << 3);  // ie Size in bytes * 8
            target_register_t sign_bit_mask = (1 << (bits_in_addr - 1));
            if ((addr & sign_bit_mask) == sign_bit_mask)
                extend_val = ~0;
        }
#endif
        ptr = __mem2hex((char *)&extend_val, ptr, REGSIZE(SP) - sizeof(addr), 0);
    }
#else
    ptr = __mem2hex((char *)&addr, ptr, REGSIZE(SP), 0);
#endif
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

volatile void *__mem_fault_handler = (void *)0;

/* These are the "arguments" to __do_read_mem and __do_write_mem, 
   which are passed as globals to avoid squeezing them thru
   __set_mem_fault_trap.  */

static volatile target_register_t memCount;

static void
__do_copy_mem (unsigned char* src, unsigned char* dst)
{
    unsigned long *long_dst;
    unsigned long *long_src;
    unsigned short *short_dst;
    unsigned short *short_src;

    __mem_fault = 1;                      /* Defaults to 'fail'. Is cleared */
                                          /* when the copy loop completes.  */
    __mem_fault_handler = &&err;

    // See if it's safe to do multi-byte, aligned operations
    while (memCount) {
        if ((memCount >= sizeof(long)) &&
            (((target_register_t)dst & (sizeof(long)-1)) == 0) &&
            (((target_register_t)src & (sizeof(long)-1)) == 0)) {
        
            long_dst = (unsigned long *)dst;
            long_src = (unsigned long *)src;

            *long_dst++ = *long_src++;
            memCount -= sizeof(long);

            dst = (unsigned char *)long_dst;
            src = (unsigned char *)long_src;
        } else if ((memCount >= sizeof(short)) &&
                   (((target_register_t)dst & (sizeof(short)-1)) == 0) &&
                   (((target_register_t)src & (sizeof(short)-1)) == 0)) {
            
            short_dst = (unsigned short *)dst;
            short_src = (unsigned short *)src;

            *short_dst++ = *short_src++;
            memCount -= sizeof(short);

            dst = (unsigned char *)short_dst;
            src = (unsigned char *)short_src;
        } else {
            *dst++ = *src++;
            memCount--;
        }
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
__read_mem_safe (void *dst, void *src, int count)
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
__write_mem_safe (void *src, void *dst, int count)
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
