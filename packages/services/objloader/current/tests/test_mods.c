/* =================================================================
 *
 *      test_mods.c
 *
 *      Test program for the object loader.
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2005 eCosCentric Ltd.
 * 
 * eCos is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 or (at your option)
 * any later version.
 * 
 * eCos is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with eCos; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or
 * use macros or inline functions from this file, or you compile this
 * file and link it with other works to produce a work based on this
 * file, this file does not by itself cause the resulting work to be
 * covered by the GNU General Public License. However the source code
 * for this file must still be made available in accordance with
 * section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work
 * based on this file might be covered by the GNU General Public
 * License.
 *
 * -------------------------------------------
 * ####ECOSGPLCOPYRIGHTEND####
 * =================================================================
 * #####DESCRIPTIONBEGIN####
 * 
 *  Author(s):    Anthony Tonizzo (atonizzo@gmail.com)
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <pkgconf/system.h>
#include <cyg/kernel/kapi.h>    // Kernel API.
#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <cyg/infra/testcase.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>

#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

#ifdef CYGPKG_IO_FILEIO
#include <cyg/fileio/fileio.h>
#endif

// Test ROMFS data. Two example data files are generated so that
// the test will work on both big-endian and little-endian targets.
#if (CYG_BYTEORDER == CYG_LSBFIRST)
# include <cyg/objloader/testromfs_le.h>
#else
# include <cyg/objloader/testromfs_be.h>
#endif

//==========================================================================

MTAB_ENTRY(romfs_mte1,
                   "/",
                   "romfs",
                   "",
                   (CYG_ADDRWORD) &filedata[0]);


//==========================================================================

#define THREAD_STACK_SIZE   8192
#define THREAD_PRIORITY     12

cyg_uint8 thread_a_stack[THREAD_STACK_SIZE] __attribute__((__aligned__ (16)));
cyg_uint8 thread_b_stack[THREAD_STACK_SIZE] __attribute__((__aligned__ (16)));

cyg_thread thread_a_obj;
cyg_thread thread_b_obj;
cyg_handle_t thread_a_hdl;
cyg_handle_t thread_b_hdl;

#define SHOW_RESULT(_fn, _res) \
diag_printf("<FAIL>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");

int weak_fn_called = 0;

void weak_function(void)
{
  CYG_TEST_PASS("Applications weak function called");
  weak_fn_called++;
}

void (*thread_a)(cyg_addrword_t);
void (*thread_b)(cyg_addrword_t);

// This is the main starting point for our example application.
void cyg_user_start(void)
{
    int err;
    void* lib_handle;
    void (*fn)(void);
    
    CYG_TEST_INIT();

    CYG_TEST_INFO("Object loader module test started");

    err = chdir("/");

    if(err < 0) 
        SHOW_RESULT(chdir, err);

    lib_handle = cyg_ldr_open_library((CYG_ADDRWORD)"/hello.o", 0);
    CYG_TEST_CHECK(lib_handle , "Unable to load object file to load");

    fn = cyg_ldr_find_symbol(lib_handle, "print_message");
    CYG_TEST_CHECK(fn , "Unable to find print_message function");

    fn();

    fn = cyg_ldr_find_symbol(lib_handle, "weak_function");
    CYG_TEST_CHECK(fn , "Unable to find weak_function");
    
    fn();

    fn = cyg_ldr_find_symbol (lib_handle, "unresolvable_symbol");
    CYG_TEST_CHECK(!fn , "Found none existing symbol!");
    
    thread_a = cyg_ldr_find_symbol(lib_handle, "thread_a");
    thread_b = cyg_ldr_find_symbol(lib_handle, "thread_b");
    CYG_TEST_CHECK(thread_a && thread_b , "Unable to find thread functions");
    
    // Create our two threads.
    cyg_thread_create(THREAD_PRIORITY,
                       thread_a,
                       (cyg_addrword_t) 75,
                       "Thread A",
                       (void *)thread_a_stack,
                       THREAD_STACK_SIZE,
                       &thread_a_hdl,
                       &thread_a_obj);

    cyg_thread_create(THREAD_PRIORITY + 1,
                       thread_b,
                       (cyg_addrword_t) 68,
                       "Thread B",
                       (void *)thread_b_stack,
                       THREAD_STACK_SIZE,
                       &thread_b_hdl,
                       &thread_b_obj);

    // Resume the threads so they start when the scheduler begins.
    cyg_thread_resume(thread_a_hdl);
    cyg_thread_resume(thread_b_hdl);

    cyg_scheduler_start();
}

// -----------------------------------------------------------------------------
// External symbols.
// -----------------------------------------------------------------------------
CYG_LDR_TABLE_KAPI_ALARM()
CYG_LDR_TABLE_KAPI_CLOCK()
CYG_LDR_TABLE_KAPI_COND()
CYG_LDR_TABLE_KAPI_COUNTER()
CYG_LDR_TABLE_KAPI_EXCEPTIONS()
CYG_LDR_TABLE_KAPI_FLAG()
CYG_LDR_TABLE_KAPI_INTERRUPTS()
CYG_LDR_TABLE_KAPI_MBOX()
CYG_LDR_TABLE_KAPI_MEMPOOL_FIX()
CYG_LDR_TABLE_KAPI_MEMPOOL_VAR()
CYG_LDR_TABLE_KAPI_MUTEX()
CYG_LDR_TABLE_KAPI_SCHEDULER()
CYG_LDR_TABLE_KAPI_SEMAPHORE()
CYG_LDR_TABLE_KAPI_THREAD()
CYG_LDR_TABLE_STRING()
CYG_LDR_TABLE_STDIO()
CYG_LDR_TABLE_INFRA_DIAG()

// Test case infrastructure function
CYG_LDR_TABLE_ENTRY(cyg_test_output_entry, "cyg_test_output", cyg_test_output);
CYG_LDR_TABLE_ENTRY(cyg_test_exit_entry, "cyg_test_exit", cyg_test_exit);

// Test function
CYG_LDR_TABLE_ENTRY(weak_function_entry, "weak_function", weak_function);

// Test Variable
CYG_LDR_TABLE_ENTRY(weak_fn_called_entry, "weak_fn_called", &weak_fn_called);
