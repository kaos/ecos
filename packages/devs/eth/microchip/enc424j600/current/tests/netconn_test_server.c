//==========================================================================
//
//      netconn_test_server.c
//
//      Utility to test enc424j600 Ethernet driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2010 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with eCos; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// As a special exception, if other files instantiate templates or use
// macros or inline functions from this file, or you compile this file
// and link it with other works to produce a work based on this file,
// this file does not by itself cause the resulting work to be covered by
// the GNU General Public License. However the source code for this file
// must still be made available in accordance with section (3) of the GNU
// General Public License v2.
//
// This exception does not invalidate any other reasons why a work based
// on this file might be covered by the GNU General Public License.
// -------------------------------------------
// ####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Ilija Stanislevik
// Contributors:
// Date:         2010-08-04
// Purpose:
// Description:  Utility to test enc424j600 (or any other) Ethernet driver
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>         // Test macros
#include <cyg/infra/cyg_ass.h>          // Assertion macros
#include <cyg/infra/diag.h>             // Diagnostic output
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <cyg/error/codes.h>
#include <cyg/io/io.h>

#include <cyg/io/spi.h>

#include <cyg/io/eth/netdev.h>
#include <network.h>

#include <lwip/netif.h>
#include <cyg/io/eth/eth_drv.h>

#define PORTNUM 2000

// Threads' data structures
typedef struct thread_s
{
    cyg_uint8 stack [CYGNUM_HAL_STACK_SIZE_TYPICAL];
    cyg_thread data;
    cyg_handle_t handle;
} thread_s_t;

typedef struct
{
    struct netconn * connection_p;
    cyg_uint32 id;
} server_argument_t;

//#define INIT_PRIORITY     5
#define INIT_PRIORITY     12
#define LISTENER_PRIORITY INIT_PRIORITY+1
#define SERVER_PRIORITY   LISTENER_PRIORITY-1

// Init thread

thread_s_t init_thread;

void run_init(void)
{
    diag_printf("i: Initializing network stack...\n");
    cyg_lwip_sequential_init();
}

// Server thread

void server(cyg_addrword_t arg_p)
{
    struct netconn *newconn = NULL;
    struct netbuf *income_buffer = NULL;
    struct ip_addr from_addr;
    struct ip_addr my_addr;
    cyg_uint16 from_port = 0;
    cyg_uint16 my_port = 0;
    cyg_uint8 *addr_byte_p = NULL;
    cyg_uint16 total_len;
    cyg_uint8 * data_buf = NULL;
    cyg_int32 retval;
    cyg_uint32 id;
    cyg_uint32 csum;

    newconn = ((server_argument_t *)arg_p) -> connection_p;
    id = ((server_argument_t *)arg_p) -> id;

    retval = netconn_addr(newconn, &my_addr, &my_port);
    if(0 != retval)
    {
        diag_printf("%u: netconn_addr() return %d. Exiting.\n", id, retval);
        netconn_delete(newconn);
        cyg_thread_exit();
    }

    retval = netconn_peer(newconn, &from_addr, &from_port);
    if(0 != retval)
    {
        diag_printf("%u: netconn_peer() return %d. Exiting.\n", id, retval);
        netconn_delete(newconn);
        cyg_thread_exit();
    }

    addr_byte_p = (cyg_uint8 *) & from_addr.addr;
    diag_printf("%u: Connected to remote peer %u.%u.%u.%u port %u, at my port %u\n", id,
        *addr_byte_p, *(addr_byte_p+1), *(addr_byte_p+2), *(addr_byte_p+3),
        from_port, my_port);
    diag_printf("newconn %x\n", (unsigned)newconn);

    csum =0;

    while((income_buffer = netconn_recv(newconn)) != NULL)
    {
        total_len = netbuf_len(income_buffer);
        if(0 != total_len)
        {
            data_buf = malloc(total_len);
            if(NULL == data_buf)
            {
                diag_printf("%u: Can not allocate %u bytes. Exiting.\n", id, total_len);
                netconn_delete(newconn);
                cyg_thread_exit();
            }

            cyg_uint8 *local_data_p = data_buf;
            do
            {
                cyg_uint8 *in_data_p;
                cyg_uint16 len;
                int i;

                netbuf_data(income_buffer, (void **)&in_data_p, &len);
                for(i=0; i<len; i++)
                {
                    csum += *local_data_p++ = *in_data_p++;
                }
            } while(netbuf_next(income_buffer) >= 0);
            diag_printf("\nReceived %u bytes\n", total_len);

            free(data_buf);
        }
        netbuf_delete(income_buffer);
    }

    diag_printf("Checksum %08x\n", csum);

    retval = netconn_close(newconn);
    if( 0 != retval)
    {
        diag_printf("%u: netconn_close(%x) return %d.\n", id, (unsigned)newconn, retval);
    }

    retval = netconn_delete(newconn);
    if( 0 != retval)
    {
        diag_printf("%u: netconn_delete(%x) return %d. Exiting.\n", id, (unsigned)newconn, retval);
        cyg_thread_exit();
    }

    diag_printf("%u: TCP connection closed.\n", id);

}

// Listener thread

thread_s_t listener_thread;

void listener(void)
{
    struct netconn *conn = NULL;
    struct netconn *newconn = NULL;
    struct netif *mynetif = NULL;
    server_argument_t server_launch_args_s;
    cyg_uint32 server_id = 0;
    thread_s_t * new_thread_data = NULL;
    char server_name[20];
    cyg_int32 retval;

    // This large malloc is just for testing
//    new_thread_data = malloc(150 * sizeof(thread_s_t));

    cyg_thread_delay(500);     // Sleep over the initialization of network

    mynetif = netif_find("et0");
    diag_printf("l: netif_find(et0) return %x\n", (unsigned int)mynetif);
    retval = netif_is_up(mynetif);
    diag_printf("l: netif_is_up() return %u\n", retval);

    diag_printf("l: Creating TCP connection...\n");
    conn = netconn_new(NETCONN_TCP);
    if (NULL == conn)
    {
        diag_printf("l: netconn_new() return NULL. Exiting.\n");
        cyg_thread_exit();
    }
    diag_printf("l: Binding to port %d...\n", PORTNUM);
    retval = netconn_bind(conn, NULL, PORTNUM);
    if (0 != retval)
    {
        diag_printf("l: netconn_bind() return %d. Exiting.\n", retval);
        cyg_thread_exit();
    }

    diag_printf("l: Setting up for listening...\n");
    retval = netconn_listen(conn);
    if( 0!= retval)
    {
        diag_printf("l: netconn_listen() return %d. Exiting.\n", retval);
        cyg_thread_exit();
    }

    do
    {
        diag_printf("l: Waiting for a new connection...\n");
        newconn = netconn_accept(conn);
        if (NULL == newconn)
        {
            diag_printf("l: netconn_accept() return NULL. Exiting.\n");
            cyg_thread_exit();
        }

        // It's time to start a server thread

        new_thread_data = malloc(sizeof(thread_s_t));
        if(NULL == new_thread_data)
        {
            diag_printf("l: Can't allocate memory for another server.\n");
            netconn_close(newconn);
            netconn_delete(newconn);
            continue;
        }
        /*
         * Thread data is never freed after the thread exit.
         * After a number of connections made, it will be impossible to
         * allocate memory for additional servers.
         */

        server_launch_args_s.connection_p = newconn;
        server_launch_args_s.id = ++server_id;
        sprintf(server_name, "s%u", server_id);

        diag_printf("l: Starting %s...\n", server_name);

        cyg_thread_create(
            SERVER_PRIORITY,
            (cyg_thread_entry_t*) server,
            (cyg_addrword_t)&server_launch_args_s,
            server_name,
            &(new_thread_data->stack[0]),
            CYGNUM_HAL_STACK_SIZE_TYPICAL,
            &(new_thread_data->handle),
            &(new_thread_data->data)
        );
        cyg_thread_resume(new_thread_data->handle);

    } while(true);
}

//---------------------------------------------------------------------------
void show_threads(void)
{
    cyg_handle_t thread = 0;
    cyg_uint16 id = 0;

    while( cyg_thread_get_next( &thread, &id ) )
    {
        cyg_thread_info info;

        if( !cyg_thread_get_info( thread, id, &info ) )
            break;

        diag_printf("ID: %04x name: %10s pri: %d\n",
                info.id, info.name?info.name:"----", info.set_pri );
        diag_printf("  stack_base %x    stack_size %d\n", info.stack_base, info.stack_size);
    }
}

void cyg_user_start(void)
{
    diag_printf("%u: Starting initializer...\n", __LINE__);
    cyg_thread_create(
        INIT_PRIORITY,
        (cyg_thread_entry_t*) run_init,
        0,
        "initializer",
        &init_thread.stack[0],
        CYGNUM_HAL_STACK_SIZE_TYPICAL,
        &init_thread.handle,
        &init_thread.data
    );
    cyg_thread_resume(init_thread.handle);

    diag_printf("%u: Starting listener...\n", __LINE__);

    cyg_thread_create(
        LISTENER_PRIORITY,
        (cyg_thread_entry_t*) listener,
        0,
        "listener",
        &listener_thread.stack[0],
        CYGNUM_HAL_STACK_SIZE_TYPICAL,
        &listener_thread.handle,
        &listener_thread.data
    );
    cyg_thread_resume(listener_thread.handle);

    show_threads();

    diag_printf("%u: Starting scheduler...\n", __LINE__);
    cyg_scheduler_start();
}

//=============================================================================

