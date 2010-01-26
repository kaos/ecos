//==========================================================================
//
//      httpd_sequential.c
//
//      A simple HTTP server using the netconn API in 'Sequential' mode.
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
// Date:         2009-05-14
// Purpose:
// Description:  A simple HTTP server using the netconn API in 'Sequential'
//               mode.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/kernel/kapi.h>

#include <lwip.h>
#include <lwip/init.h>

#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
#if LWIP_TCP
#if LWIP_NETCONN

struct http_state {
    char buf[256];
    char *file;
    u32_t left;
    u8_t retries;
};

static int request_counter = 1;

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static char main_stack[STACK_SIZE];
static cyg_thread main_thread;
static cyg_handle_t main_handle;


// Sends a text.
static void
send_text(struct netconn *conn, const char *text)
{
    const char *src = text;
    u16_t len = 0;
    
    while (*src++ != '\0')
        len++;
    
    netconn_write(conn, text, len, NETCONN_COPY);
}

// Sends a decimal value.
static void
send_dec(struct netconn *conn, int value)
{
    char tmp1[16], tmp2[16];
    int i = 0, j = 0;
    
    do {
        tmp1[i++] = '0' + value % 10; 
        value /= 10;
    } while (value > 0);

    while (i-- > 0)
        tmp2[j++] = tmp1[i];
    
    netconn_write(conn, tmp2, j, NETCONN_COPY);
}

// Sends the HTTP page.
static void
send_page(struct netconn *conn)
{
    send_text(conn, "HTTP/1.0 200 OK\r\n");
    send_text(conn, "Content-Type: text/html\r\n");
    send_text(conn, "\r\n");
    send_text(conn, "<html>\r\n");
    send_text(conn, "<h1>eCos - HTTP test server running on lwIP</h1>\r\n");
    send_text(conn, "<table><tr><td>Version:</td><td>");
    send_dec(conn, LWIP_VERSION_MAJOR);
    send_text(conn, ".");
    send_dec(conn, LWIP_VERSION_MINOR);
    send_text(conn, ".");
    send_dec(conn, LWIP_VERSION_REVISION);
    if (LWIP_VERSION_IS_RELEASE)
        send_text(conn, " (release)");
    if (LWIP_VERSION_IS_DEVELOPMENT)
        send_text(conn, " (development)");
    if (LWIP_VERSION_IS_RC)
        send_text(conn, " (rc)");
    send_text(conn, "</td></tr><tr><td>Requests:</td><td>");
    send_dec(conn, request_counter++);
    send_text(conn, "</td></tr></table>\r\n");
    send_text(conn, "</html>");
}

static void
http_server_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t len;

    // Read the data from the port, blocking if nothing yet there.
    // We assume the request (the part we care about) is in one netbuf.
    inbuf = netconn_recv(conn);
    if (!inbuf)
        return;

    if (netconn_err(conn) == ERR_OK) {

        // Get data from the netbuf
        netbuf_data(inbuf, (void **) &buf, &len);
        
        // Is this an HTTP GET command? Only check the first 5 chars, since
        // there are other formats for GET, and we're keeping it very simple.
        if (len >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' &&
            buf[3] == ' ' && buf[4] == '/' ) {
            
            // Send the response
            send_page(conn);
        }
    }
    
    // Delete the buffer (netconn_recv gives us ownership, so we have to make
    // sure to deallocate the buffer).
    netbuf_delete(inbuf);
}


// Main thread.
void
main_thread_entry(cyg_addrword_t p)
{
    struct netconn *server, *client;
    
    CYG_TEST_INFO("Initializing lwIP");
    cyg_lwip_sequential_init();
    
    // Create a new TCP connection handle
    CYG_TEST_INFO("Initializing server");
    server = netconn_new(NETCONN_TCP);
    if (server == NULL)
        CYG_TEST_FAIL_FINISH("Cannot allocate server netconn");
    
    // Bind to port 80 (HTTP) with default IP address
    netconn_bind(server, NULL, 80);

    // Put the connection into LISTEN state
    netconn_listen(server);

    // Accept client connections
    CYG_TEST_INFO("Running");
    while(1) {
        client = netconn_accept(server);
        http_server_serve(client);
        netconn_close(client);
        netconn_delete(client);
    }
}

void
httpd_main(void)
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Testing httpd");

    cyg_thread_create(
        10,                 // Priority
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
cyg_start(void)
{
    httpd_main();
}

#else // LWIP_NETCONN
#define N_A_MSG "Netconn support disabled"
#endif // LWIP_NETCONN

#else // LWIP_TCP
#define N_A_MSG "TCP support disabled"
#endif // LWIP_TCP

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
