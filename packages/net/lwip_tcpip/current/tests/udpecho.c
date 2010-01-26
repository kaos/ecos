//==========================================================================
//
//      udpecho.c
//
//      Simple test-case for the netconn UDP API: echo on UDP port 7.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2009 Free Software Foundation
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2009-05-18
// Purpose:
// Description:  Simple test-case for the netconn UDP API: echo on UDP port 7.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/kernel/kapi.h>

#include <lwip.h>

#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
#if LWIP_NETCONN
#if LWIP_UDP

#define UDP_PORT 7

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static char main_stack[STACK_SIZE];
static cyg_thread main_thread;
static cyg_handle_t main_handle;

static char udpecho_stack[STACK_SIZE];

static char buffer[100];

void
udpecho_thread_entry(void *arg)
{
    struct netconn *conn;
    struct netbuf *buf;
    struct ip_addr *addr;
    unsigned short port;

    conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, NULL, UDP_PORT);

    while (1) {
        buf = netconn_recv(conn);
        addr = netbuf_fromaddr(buf);
        port = netbuf_fromport(buf);
        netconn_connect(conn, addr, port);
        netconn_send(conn, buf);
        netbuf_copy(buf, buffer, sizeof(buffer));
        netbuf_delete(buf);
    }
}

void
main_thread_entry(cyg_addrword_t p)
{
    cyg_lwip_sequential_init();
    cyg_lwip_thread_new("udpecho", udpecho_thread_entry, NULL,
                        udpecho_stack, STACK_SIZE, 7);
}

void
udpecho_main(void)
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Testing UDP");
    diag_printf("This test will echo UDP packets on local port %d\n", UDP_PORT);

    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(
        10,                 // Priority - just a number
        main_thread_entry,  // Entry
        0,                  // Entry parameter
        "main",             // Name
        main_stack,         // Stack
        STACK_SIZE,         // Size
        &main_handle,       // Handle
        &main_thread        // Thread data structure
    );
    cyg_thread_resume(main_handle);
    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    udpecho_main();
}

#else // LWIP_UDP
#define N_A_MSG "UDP support disabled"
#endif // LWIP_UDP

#else // LWIP_NETCONN
#define N_A_MSG "NETCONN support disabled"
#endif // LWIP_NETCONN

#else // CYGFUN_LWIP_MODE_SEQUENTIAL
#define N_A_MSG "Not configured in 'Sequential' mode"
#endif // CYGFUN_LWIP_MODE_SEQUENTIAL

#ifdef N_A_MSG
externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG
