/* =================================================================
 *
 *      hello.c
 *
 *      Sample library to test the object loader
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

#include <cyg/kernel/kapi.h>    // Kernel API.
#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <cyg/infra/testcase.h>

cyg_sem_t sem_signal_thread;
int thread_a_count;

extern int weak_fn_called;

void weak_function(void) CYGBLD_ATTRIB_WEAK;
void weak_function(void)
{
    // Store the data value passed in for this thread.
    diag_printf("INFO:< This is the library weak function>");
    CYG_TEST_FAIL ("Libraries weak function called when apps should be called");
}

void library_open(void)
{
    // Initialize the count for thread a.
    thread_a_count = 0;

    // Initialize the semaphore with a count of zero,
    // prior to creating the threads.
    cyg_semaphore_init(&sem_signal_thread, 0);
    CYG_TEST_INFO("Library initialized");
}

void library_close(void)
{
    CYG_TEST_INFO("Library closed");
}

void print_message(void)
{
    diag_printf("INFO:<Printing a silly message...>\n");
}

// Thread A - signals thread B via semaphore.
void thread_a(cyg_addrword_t data)
{
    // Store the data value passed in for this thread.
    int msg = (int)data;

    weak_function ();
    
    while(thread_a_count < 5)
    {
        // Increment the thread a count.
        thread_a_count++;


        // Send out a message to the diagnostic port.
        diag_printf("INFO:<Thread A, count: %d  message: %d>\n", thread_a_count, msg);

        // Delay for 1 second.
        cyg_thread_delay(100);

        // Signal thread B using the semaphore.
        cyg_semaphore_post(&sem_signal_thread);
    }
    
    CYG_TEST_CHECK(weak_fn_called == 2 , "Application week function not called");
    CYG_TEST_FINISH("Object loader test finished");
}

// Thread B - waits for semaphore signal from thread A.
void thread_b(cyg_addrword_t data)
{
    // Store the data value passed in for this thread.
    int msg = (int)data;

    while(1)
    {

        // Signal thread B using the semaphore.
        cyg_semaphore_wait(&sem_signal_thread);

        // Send out a message to the diagnostic port.
        diag_printf("INFO:< Thread B, message: %d>\n", msg);
    }
}
