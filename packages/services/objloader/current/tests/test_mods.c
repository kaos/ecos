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
 *  Author(s):    atonizzo@lycos.com
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/kernel/kapi.h>    // Kernel API.
#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <pkgconf/io_fileio.h>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>

#include <cyg/fileio/fileio.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

#define THREAD_STACK_SIZE   8192
#define THREAD_PRIORITY     12

cyg_uint8 thread_a_stack[THREAD_STACK_SIZE] __attribute__((__aligned__ (16)));
cyg_uint8 thread_b_stack[THREAD_STACK_SIZE] __attribute__((__aligned__ (16)));

cyg_thread thread_a_obj;
cyg_thread thread_b_obj;
cyg_handle_t thread_a_hdl;
cyg_handle_t thread_b_hdl;

#define SHOW_RESULT( _fn, _res ) \
diag_printf("<FAIL>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");

void weak_function( void )
{
    // Store the data value passed in for this thread.
    diag_printf( "This is the application function\n" );
}

void (*thread_a)( cyg_addrword_t );
void (*thread_b)( cyg_addrword_t );

// This is the main starting point for our example application.
void cyg_user_start( void )
{
    int err;
    void* lib_handle;
    void (*fn)( void );
    
    // It wouldn't be much of a basic example if some
    // kind of hello message wasn't output.
    diag_printf( "Hello eCos World!!!\n\n" );

    err = mount( "/dev/disk0/1", "/", "fatfs" );
    if( err < 0 ) 
        SHOW_RESULT( mount, err );

    err = chdir( "/" );
    if( err < 0 ) 
        SHOW_RESULT( chdir, err );

    lib_handle = cyg_ldr_open_library( (CYG_ADDRWORD)"/hello.o", 0 );
//    cyg_ldr_print_symbol_names( lib_handle );
//    cyg_ldr_print_rel_names( lib_handle );
    fn = cyg_ldr_find_symbol( lib_handle, "print_message" );
    fn();
    fn = cyg_ldr_find_symbol( lib_handle, "weak_function" );
    fn();

    thread_a = cyg_ldr_find_symbol( lib_handle, "thread_a" );
    thread_b = cyg_ldr_find_symbol( lib_handle, "thread_b" );

   // Create our two threads.
    cyg_thread_create( THREAD_PRIORITY,
                       thread_a,
                       (cyg_addrword_t) 75,
                       "Thread A",
                       (void *)thread_a_stack,
                       THREAD_STACK_SIZE,
                       &thread_a_hdl,
                       &thread_a_obj );

    cyg_thread_create( THREAD_PRIORITY + 1,
                       thread_b,
                       (cyg_addrword_t) 68,
                       "Thread B",
                       (void *)thread_b_stack,
                       THREAD_STACK_SIZE,
                       &thread_b_hdl,
                       &thread_b_obj );

    // Resume the threads so they start when the scheduler begins.
    cyg_thread_resume( thread_a_hdl );
    cyg_thread_resume( thread_b_hdl );
}

// -----------------------------------------------------------------------------
// External symbols.
// -----------------------------------------------------------------------------

// eCos semaphores
CYG_LDR_TABLE_ENTRY( cyg_thread_delay_entry,
                     "cyg_thread_delay",   
                     cyg_thread_delay );
CYG_LDR_TABLE_ENTRY( cyg_semaphore_wait_entry,
                     "cyg_semaphore_wait", 
                     cyg_semaphore_wait );
CYG_LDR_TABLE_ENTRY( cyg_semaphore_init_entry,
                     "cyg_semaphore_init", 
                     cyg_semaphore_init );
CYG_LDR_TABLE_ENTRY( cyg_semaphore_post_entry,
                     "cyg_semaphore_post", 
                     cyg_semaphore_post );

// eCos mailboxes
CYG_LDR_TABLE_ENTRY( cyg_mbox_put_entry, "cyg_mbox_put", cyg_mbox_put );

// eCos Flags
CYG_LDR_TABLE_ENTRY( cyg_flag_poll_entry, "cyg_flag_poll", cyg_flag_poll );
CYG_LDR_TABLE_ENTRY( cyg_flag_setbits_entry, 
                     "cyg_flag_setbits", 
                     cyg_flag_setbits );

// eCos misc
CYG_LDR_TABLE_ENTRY( hal_delay_us_entry, "hal_delay_us", hal_delay_us );
CYG_LDR_TABLE_ENTRY( cyg_assert_msg_entry, "cyg_assert_msg", cyg_assert_msg );
CYG_LDR_TABLE_ENTRY( cyg_assert_fail_entry, 
                     "cyg_assert_fail", 
                     cyg_assert_fail );

// memory
CYG_LDR_TABLE_ENTRY( memcpy_entry, "memcpy", memcpy );
CYG_LDR_TABLE_ENTRY( memset_entry, "memset", memset );
CYG_LDR_TABLE_ENTRY( memcmp_entry, "memcmp", memcmp );
CYG_LDR_TABLE_ENTRY( malloc_entry, "malloc", malloc );
CYG_LDR_TABLE_ENTRY( free_entry,   "free",   free );

// Strings                     
CYG_LDR_TABLE_ENTRY( strcpy_entry,  "strcpy",  strcpy );
CYG_LDR_TABLE_ENTRY( strcat_entry,  "strcat",  strcat );
CYG_LDR_TABLE_ENTRY( sprintf_entry, "sprintf", sprintf );
CYG_LDR_TABLE_ENTRY( strlen_entry,  "strlen",  strlen );
CYG_LDR_TABLE_ENTRY( strcmp_entry,  "strcmp",  strcmp );
CYG_LDR_TABLE_ENTRY( strncmp_entry, "strncmp", strncmp );

// files
CYG_LDR_TABLE_ENTRY( fopen_entry,  "fopen",  fopen );
CYG_LDR_TABLE_ENTRY( fread_entry,  "fread",  fread );
CYG_LDR_TABLE_ENTRY( fclose_entry, "fclose", fclose );
CYG_LDR_TABLE_ENTRY( fseek_entry,  "fseek",  fseek );
CYG_LDR_TABLE_ENTRY( stat_entry,   "stat",   stat );

// Diagnostic printout
CYG_LDR_TABLE_ENTRY( diag_printf_entry, "diag_printf", diag_printf );

CYG_LDR_TABLE_ENTRY( weak_function_entry, "weak_function", weak_function );
