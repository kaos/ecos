//==========================================================================
//
//        ecos_bsp.c
//
//        eCos BSP (for building Cygmon)
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1999-10-11
// Description:   Wrapper functions which provide BSP environment for Cygmon
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration headers
#include <pkgconf/hal.h>
#include <pkgconf/cygmon.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>
#include <cyg/io/io.h>
#include "bsp/common/bsp_if.h"
#include <signal.h>
#include CYGHWR_MEMORY_LAYOUT_H

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_MINIMUM
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

char *build_date = __DATE__;

extern void monitor_main(int, char *);
extern int  stub_is_active;

void
cygmon_main(void)
{
    _bsp_init();
    monitor_main(0, 0);  // Null argument list
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      cygmon_main,       // entry
                      0,                 // entry parameter
                      "Cygmon",          // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
extern void *_hal_registers;  // Used by eCos GDB stubs
extern void (*__init_vec)(void);
extern void (*__cleanup_vec)(void);
extern void __install_traps(void);
#endif

void
_bsp_handle_exception(cyg_addrword_t data, cyg_code_t num, cyg_addrword_t info)
{
    if (num == CYGNUM_HAL_EXCEPTION_INTERRUPT) {
        if (machine_syscall(info)) {
            return;
        }
        // Fall through to "normal" exception handling if system call failed
    }
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    _hal_registers = (void *)info;  // Used by eCos GDB stubs
    __cleanup_vec();
#endif
    bsp_invoke_dbg_handler(num, (void *)info);
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    __init_vec();
#endif
}

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
extern void __handle_exception(void);
#else

int
unhandled_exception(int num, void *args)
{
    diag_printf("Unhandled exception: %d/%x\n", num, args);
    while (1) ;
}
#endif

void
_bsp_cpu_init(void)
{
    cyg_exception_handler_t _bsp_breakpoint_handler;
    int d0;
    cyg_exception_set_handler(CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION,
                              _bsp_handle_exception,
                              &d0,
                              0,
                              0);
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    bsp_install_dbg_handler((bsp_handler_t)__handle_exception);
    __install_traps();
#else
    bsp_install_dbg_handler(unhandled_exception);
#endif
}

#define FAIL() diag_printf("fail: %s\n", __FUNCTION__);  while (1) ;

struct BSP_IO {
    char            *name;
    cyg_io_handle_t  chan;
};

static void 
uart_putchar(void *base, char c)
{
    char buf;
    int len;
    struct BSP_IO *io = (struct BSP_IO *)base;
    if (io->chan) {
        len = 1;
        buf = c;
        cyg_io_write(io->chan, &buf, &len);
    }
}

// This function is mostly used by the 'write()' system call
static void
uart_write(void *base, const char *buf, int len)
{
    struct BSP_IO *io = (struct BSP_IO *)base;
    if (io->chan) {
        if (stub_is_active) {
            // We are running in 'GDB' mode
            __output_gdb_string(buf, len);
        } else {
            cyg_io_write(io->chan, buf, &len);
        }
    }
}

static int
uart_read(void *base, char *buf, int len)
{
    struct BSP_IO *io = (struct BSP_IO *)base;
    if (io->chan) {
        cyg_io_read(io->chan, buf, &len);
        return len;
    }
    return 0;
}

static int
uart_getchar(void *base)
{
    char buf = '\0';
    int len;
    struct BSP_IO *io = (struct BSP_IO *)base;
    if (io->chan) {
        len = 1;
        cyg_io_read(io->chan, &buf, &len);
    }
    return buf;
}

static int
uart_control(void *base, int func, ...)
{
    // FIXME - need to fill this in!
}

/*
 * Setup the bsp_comm_channel data structure
 */
struct BSP_IO bsp_comm_io[] = { 
    {CYGDAT_CYGMON_CONSOLE_DEV, 0},  // Console device
};

struct bsp_comm_channel _bsp_comm_list[] = 
{
    {
	{ "UART 0", 
          BSP_COMM_SERIAL, 
          BSP_PROTO_NONE },
	{ (void*)&bsp_comm_io[0], 
          uart_write, 
          uart_read,
	  uart_putchar, 
          uart_getchar, 
          uart_control }
    },
};
int _bsp_num_comms = sizeof(_bsp_comm_list)/sizeof(_bsp_comm_list[0]);

void
_bsp_init_board_comm(void)
{
    int i;
    for (i = 0;  i < _bsp_num_comms;  i++) {
        Cyg_ErrNo err;
        struct BSP_IO *io;
        io = (struct BSP_IO *)_bsp_comm_list[i].procs.ch_data;
        if ((err = cyg_io_lookup(io->name, &io->chan)) != ENOERR) {
            diag_printf("Can't open '%s'\n", io->name);
        }
    }
}

/*
 * Array of memory region descriptors. We just list RAM.
 */
#ifdef CYGMEM_REGION_ram
#define RAM_VIRTUAL_BASE CYGMEM_REGION_ram
#define RAM_TOTAL_SIZE   CYGMEM_REGION_ram_SIZE
#else
#define RAM_VIRTUAL_BASE 0x00008000
#define RAM_TOTAL_SIZE   0x00FF8000
#endif

struct bsp_mem_info _bsp_memory_list[] = 
{
    { (void *)RAM_VIRTUAL_BASE,   
      (void *)RAM_VIRTUAL_BASE,   
      0, 
      RAM_TOTAL_SIZE,   
      BSP_MEM_RAM 
    },
};

/*
 * Number of memory region descriptors.
 */
int _bsp_num_mem_regions = sizeof(_bsp_memory_list)/sizeof(_bsp_memory_list[0]);

void
_bsp_install_cpu_irq_controllers(void)
{
}

void
_bsp_install_board_irq_controllers(void)
{
}

void
__init_irq_controllers(void)
{
}

void
_bsp_board_init(void)
{
    /*
     * Define platform info.
     */
#ifdef HAL_PLATFORM_CPU
    _bsp_platform_info.cpu = HAL_PLATFORM_CPU;
#else
    _bsp_platform_info.cpu = "Unknown CPU";
#endif
#ifdef HAL_PLATFORM_BOARD
    _bsp_platform_info.board = HAL_PLATFORM_BOARD;
#else
    _bsp_platform_info.board = "Unknown board";
#endif
#ifdef HAL_PLATFORM_EXTRA
    _bsp_platform_info.extra = HAL_PLATFORM_EXTRA;
#else
    _bsp_platform_info.extra = "";
#endif
}

extern char *strchr(char *, char);
char *
index(char *string, char key)
{
    return strchr(string, key);
}

void
flush_i_cache(void)
{
    HAL_ICACHE_SYNC();
}

void
ecos_bsp_console_putc(char c)
{
    if (bsp_shared_data) {
        bsp_console_putc(c);
    }
}

char
ecos_bsp_console_getc(void)
{
    if (bsp_shared_data) {
        return bsp_console_getc();
    } else {
        return '?';
    }
}
