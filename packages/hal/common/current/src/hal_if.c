//=============================================================================
//
//      hal_if.c
//
//      ROM/RAM interfacing functions
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
// Date:        2000-06-07
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_ass.h>          // assertions

#include <cyg/hal/hal_arch.h>           // set/restore GP

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_if.h>             // our interface

#include <cyg/hal/hal_diag.h>           // Diag IO

#include <cyg/hal/hal_stub.h>           // stub functionality
#include <cyg/hal/plf_stub.h>           // reset entry - FIXME, should be moved


//--------------------------------------------------------------------------

externC void patch_dbg_syscalls(void * vector);
externC void init_thread_syscall(void * vector);

externC CYG_ADDRWORD hal_interrupt_handlers[];
externC CYG_ADDRWORD hal_vsr_table[];


//--------------------------------------------------------------------------
// Configuration control for the services in this feil.
#if	defined(CYG_HAL_STARTUP_ROM) ||			\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYGSEM_HAL_USE_ROM_MONITOR))
# define CYGPRI_IMPLEMENTS_IF_SERVICES
#endif

//--------------------------------------------------------------------------
// Implementations and function wrappers for monitor services
#ifdef CYGPRI_IMPLEMENTS_IF_SERVICES

static void
reset(void)
{
    // With luck, the platform defines some magic that will cause a hardware
    // reset.
    HAL_STUB_PLATFORM_RESET();

#ifdef HAL_STUB_PLATFORM_RESET_ENTRY
    // If that's not the case (above is an empty statement) there may
    // be defined an address we can jump to - and effectively
    // reinitialize the system. Not quite as good as a reset, but it
    // is often enough.
    goto *HAL_STUB_PLATFORM_RESET_ENTRY;

#else
#error " no RESET_ENTRY"
#endif
}

// This is the system's default kill signal routine. Unless overridden
// by the application, it will cause a board reset when GDB quits the
// connection. (The user can avoid the reset by using the GDB 'detach'
// command instead of 'kill' or 'quit').
static int
kill_by_reset(int __irq_nr, void* __regs)
{
    reset();
    return 0;
}

static int
nop_service(void)
{
    // This is the default service. It always returns false (0), and
    // _does_ not trigger any assertions. Clients must either cope
    // with the service failure or assert.
    return 0;
}

//----------------------------------
// Comm controls

static hal_virtual_comm_table_t comm_channels[CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS];

static int
set_debug_comm(int __comm_id)
{
    static int __selected_id;

    CYG_ASSERT(-1 == __comm_id
               || __comm_id < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS,
               "Invalid channel");

    if (-1 == __comm_id)
        return __selected_id;

    __selected_id = __comm_id;
    
    CYGACC_CALL_IF_DEBUG_PROCS_SET(comm_channels[__comm_id]);

    return 1;
}

static int
set_console_comm(int __comm_id)
{
    static int __selected_id;

    CYG_ASSERT(-1 == __comm_id
               || __comm_id < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS,
               "Invalid channel");

    if (-1 == __comm_id)
        return __selected_id;

    __selected_id = __comm_id;
    
    CYGACC_CALL_IF_CONSOLE_PROCS_SET(comm_channels[__comm_id]);

    return 1;
}


#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//----------------------------------
// eCos debug procedure wrappers
static void
debug_write(void* __ch_data, const cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        HAL_STUB_PLATFORM_PUT_CHAR(*__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
debug_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        *__buf++ = HAL_STUB_PLATFORM_GET_CHAR();

    CYGARC_HAL_RESTORE_GP();
}


static void
debug_putc(void* __ch_data, cyg_uint8 __ch)
{
    CYGARC_HAL_SAVE_GP();

    HAL_STUB_PLATFORM_PUT_CHAR(__ch);

    CYGARC_HAL_RESTORE_GP();
}

static cyg_uint8
debug_getc(void* __ch_data)
{
    cyg_uint8 __ch;
    CYGARC_HAL_SAVE_GP();

    __ch = HAL_STUB_PLATFORM_GET_CHAR();

    CYGARC_HAL_RESTORE_GP();
    return __ch;
}

static int
debug_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    // Do nothing (yet).
    return 0;
}
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//----------------------------------
// Cache functions

static void
flush_icache(void *__p, int __nbytes)
{
#ifdef HAL_ICACHE_FLUSH
    HAL_ICACHE_FLUSH( __p , __nbytes );
#elif defined(HAL_ICACHE_INVALIDATE)
    HAL_ICACHE_INVALIDATE();
#endif
}

static void
flush_dcache(void *__p, int __nbytes)
{
#ifdef HAL_DCACHE_FLUSH
    HAL_DCACHE_FLUSH( __p , __nbytes );
#elif defined(HAL_DCACHE_INVALIDATE)
    HAL_DCACHE_INVALIDATE();
#endif
}

#endif

#if !defined(CYGPRI_CONSOLE_PROCS_HANDLED) \
    && defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
// --------------------
// Console wrapper functions. These are used for pumping data to the device
// via low-level functions provided by the platform HAL. Mangling has
// happened (if applicable) by the time the stream hits this code.
static void
console_write(void* __ch_data, const cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        HAL_PLF_DIAG_RAW_WRITE_CHAR(*__buf++);

    CYGARC_HAL_RESTORE_GP();
}

static void
console_read(void* __ch_data, cyg_uint8* __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while(__len-- > 0)
        HAL_PLF_DIAG_RAW_READ_CHAR(*__buf++);

    CYGARC_HAL_RESTORE_GP();
}


static void
console_putc(void* __ch_data, cyg_uint8 __ch)
{
    CYGARC_HAL_SAVE_GP();

    HAL_PLF_DIAG_RAW_WRITE_CHAR(__ch);

    CYGARC_HAL_RESTORE_GP();
}

static cyg_uint8
console_getc(void* __ch_data)
{
    cyg_uint8 __ch;
    CYGARC_HAL_SAVE_GP();

    HAL_PLF_DIAG_RAW_READ_CHAR(__ch);

    CYGARC_HAL_RESTORE_GP();
    return __ch;
}

static int
console_control(void *__ch_data, __comm_control_cmd_t __func, ...)
{
    // Do nothing (yet).
    return 0;
}
#endif

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)
//-----------------------------------------------------------------------------
// Console IO functions that adhere to the virtual vector table semantics in
// order to ensure proper debug agent mangling when required.
//
// The platform HAL must specify the channel used, and provide raw IO
// routines for that channel. The platform HAL also has the necessary
// information to determine if the channel is already initialized by
// a debug agent (either builtin or in ROM).
void 
hal_if_diag_init(void)
{
#ifndef CYGPRI_CONSOLE_PROCS_HANDLED
    // Set up console procs for the channel and initialize it.
    hal_virtual_comm_table_t* comm;
    // Hook console wrappers up on the diag channel
    CYGACC_CALL_IF_SET_CONSOLE_COMM()(CYGNUM_HAL_VIRTUAL_VECTOR_CONSOLE_CHANNEL);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS();

    CYGACC_COMM_IF_WRITE_SET(*comm, console_write);
    CYGACC_COMM_IF_READ_SET(*comm, console_read);
    CYGACC_COMM_IF_PUTC_SET(*comm, console_putc);
    CYGACC_COMM_IF_GETC_SET(*comm, console_getc);
    CYGACC_COMM_IF_CONTROL_SET(*comm, console_control);
    
    // Init device.
    HAL_PLF_DIAG_RAW_INIT();
#endif
}

void 
hal_if_diag_write_char(char c)
{
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        CYGACC_COMM_IF_PUTC(*__chan)(*__chan, c);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        CYGACC_COMM_IF_PUTC(*__chan)(*__chan, c);
    }
}

void 
hal_if_diag_read_char(char *c)
{
    hal_virtual_comm_table_t* __chan = CYGACC_CALL_IF_CONSOLE_PROCS();
    
    if (__chan)
        *c = CYGACC_COMM_IF_GETC(*__chan)(*__chan);
    else {
        __chan = CYGACC_CALL_IF_DEBUG_PROCS();
        *c = CYGACC_COMM_IF_GETC(*__chan)(*__chan);
    }
}
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_DIAG

//--------------------------------------------------------------------------
// Init function. It should be called from the platform initialization code.
// For monitor configurations it will initialize the calling interface table,
// for client configurations it will patch the existing table as per
// configuration.
void
hal_if_init(void)
{
    // Set up services provided by monitors
#ifdef CYGPRI_IMPLEMENTS_IF_SERVICES
    {
        hal_virtual_comm_table_t* comm;
        int i, j;
        // Initialize tables with the NOP service.
        // This should only be done for service routine entries - data
        // pointers should be NULLed.
        for (i = 0; i < CYGNUM_CALL_IF_TABLE_SIZE; i++)
            hal_virtual_vector_table[i] = (CYG_ADDRWORD) &nop_service;

        for (j = 0; j < CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS; j++)
            for (i = 0; i < CYGNUM_COMM_IF_TABLE_SIZE; i++)
                comm_channels[j][i] = (CYG_ADDRWORD) &nop_service;


        // Fill in supported services.

        // Version number
        CYGACC_CALL_IF_VERSION_SET(CYGNUM_CALL_IF_TABLE_VERSION);

        // ICTRL and EXC tables - I assume these to be the equivalents of
        // ISR and VSR tables.
        CYGACC_CALL_IF_ICTRL_TABLE_SET(hal_interrupt_handlers);
        CYGACC_CALL_IF_EXC_TABLE_SET(hal_vsr_table);

        // Reset and kill vector
        CYGACC_CALL_IF_RESET_SET(reset);
        CYGACC_CALL_IF_KILL_VECTOR_SET(kill_by_reset);

        // Comm controls
        CYGACC_CALL_IF_SET_DEBUG_COMM_SET(set_debug_comm);
        CYGACC_CALL_IF_SET_CONSOLE_COMM_SET(set_console_comm);

        // Cache functions
        CYGACC_CALL_IF_FLUSH_ICACHE_SET(flush_icache);
        CYGACC_CALL_IF_FLUSH_DCACHE_SET(flush_dcache);

        // Clear console procs entry. If platform has been to
        // configured to use a separate console port, it will be set
        // up later (hal_diag_init). Alternatively (if this is a stub)
        // it will be initialized with the output mangler
        // (O-packetizer for GDB) which uses the debug comms.
        CYGACC_CALL_IF_CONSOLE_PROCS_SET(0);

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        // Hook debug wrappers up on the debug channel
        set_debug_comm(CYGNUM_HAL_VIRTUAL_VECTOR_DEBUG_CHANNEL);
        comm = CYGACC_CALL_IF_DEBUG_PROCS();
        CYGACC_COMM_IF_WRITE_SET(*comm, debug_write);
        CYGACC_COMM_IF_READ_SET(*comm, debug_read);
        CYGACC_COMM_IF_PUTC_SET(*comm, debug_putc);
        CYGACC_COMM_IF_GETC_SET(*comm, debug_getc);
        CYGACC_COMM_IF_CONTROL_SET(*comm, debug_control);
#endif

        // Data entries not currently supported in eCos
        CYGACC_CALL_IF_CPU_DATA_SET(0);
        CYGACC_CALL_IF_BOARD_DATA_SET(0);
        CYGACC_CALL_IF_DBG_DATA_SET(0);
    }
#endif

    // Set up services provided by clients
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   &&  \
    ( defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs) \
      || defined(CYGSEM_HAL_USE_ROM_MONITOR_CygMon))

    patch_dbg_syscalls( (void *)(hal_virtual_vector_table) );
#endif

    // Init client services
#if !defined(CYGPKG_KERNEL) && defined(CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT)
    // Only include this code if we do not have a kernel. Otherwise
    // the kernel supplies the functionality for the app we are linked
    // with.

    // Prepare for application installation of thread info function in
    // vector table.
    init_thread_syscall( (void *)&hal_virtual_vector_table[CYGNUM_CALL_IF_DBG_SYSCALL] );
#endif

    // Finally, install async breakpoint handler if it is configured in.
    // FIXME: this should probably check for STUBS instead (but code is
    //        conditional on BREAK for now)
#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
    // Install async breakpoint handler into vector table.
    CYGACC_CALL_IF_INSTALL_BPT_FN_SET(&cyg_hal_gdb_interrupt);
#endif
}
