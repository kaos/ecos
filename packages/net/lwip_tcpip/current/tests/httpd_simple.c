//==========================================================================
//
//      httpd_simple.c
//
//      A simple HTTP server using raw tcp pcbs in 'Simple' mode.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation
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
// Date:         2008-12-01
// Purpose:
// Description:  A simple HTTP server using raw tcp pcbs in 'Simple' mode.
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

#ifdef CYGFUN_LWIP_MODE_SIMPLE
#if LWIP_TCP

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


// Appends text to the buffer.
static void
buf_append_text(char *buf, const char *text)
{
    char *dst = buf;
    const char *src = text;
    
    while (*dst != '\0')
        dst++;
    
    while (*src != '\0')
        *dst++ = *src++;
    
    *dst = '\0';
}

// Appends a decimal value to the buffer.
static void
buf_append_dec(char *buf, int value)
{
    char *dst = buf;
    char tmp[16];
    int i = 0;
    
    while (*dst != '\0')
        dst++;
    
    do {
        tmp[i++] = '0' + value % 10; 
        value /= 10;
    } while (value > 0);
    
    while (i-- > 0)
        *dst++ = tmp[i];
    
    *dst = '\0';
}

// Returns the length of the buffer.
static size_t
buf_length(char *buf)
{
    size_t len = 0;
    
    while (*buf++ != '\0')
        len++;
    
    return len;
}

// Prepares the HTTP page.
static void
prepare_page(struct http_state *hs)
{
    hs->buf[0] = '\0';
    buf_append_text(hs->buf, "HTTP/1.0 200 OK\r\n");
    buf_append_text(hs->buf, "Content-Type: text/html\r\n");
    buf_append_text(hs->buf, "\r\n");
    buf_append_text(hs->buf, "<html>\r\n");
    buf_append_text(hs->buf, "<h1>eCos - HTTP test server running on lwIP</h1>\r\n");
    buf_append_text(hs->buf, "<table><tr><td>Version:</td><td>");
    buf_append_dec(hs->buf, LWIP_VERSION_MAJOR);
    buf_append_text(hs->buf, ".");
    buf_append_dec(hs->buf, LWIP_VERSION_MINOR);
    buf_append_text(hs->buf, ".");
    buf_append_dec(hs->buf, LWIP_VERSION_REVISION);
    if (LWIP_VERSION_IS_RELEASE)
        buf_append_text(hs->buf, " (release)");
    if (LWIP_VERSION_IS_DEVELOPMENT)
        buf_append_text(hs->buf, " (development)");
    if (LWIP_VERSION_IS_RC)
        buf_append_text(hs->buf, " (rc)");
    buf_append_text(hs->buf, "</td></tr><tr><td>Requests:</td><td>");
    buf_append_dec(hs->buf, request_counter++);
    buf_append_text(hs->buf, "</td></tr></table>\r\n");
    buf_append_text(hs->buf, "</html>");
    
    hs->file = hs->buf;
    hs->left = buf_length(hs->buf);
}

// Called when an error occured. Frees the allocated resources.
static void
http_err(void *arg, err_t err)
{
    struct http_state *hs;

    CYG_TEST_INFO("Connection error");

    hs = arg;
    mem_free(hs);
}

// Closes the TCP connection and frees the allocated resources.
static void
close_conn(struct tcp_pcb *pcb, struct http_state *hs)
{
    CYG_TEST_INFO("Closing connection");

    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    mem_free(hs);
    tcp_close(pcb);
}

// Sends the next chunk of data.
static void
send_data(struct tcp_pcb *pcb, struct http_state *hs)
{
    err_t err;
    u16_t len;
    
    CYG_TEST_INFO("Sending data");

    // We cannot send more data than space available in the send buffer
    len = tcp_sndbuf(pcb) < hs->left ? tcp_sndbuf(pcb) : hs->left;
    
    do {
        err = tcp_write(pcb, hs->file, len, 0);
        if (err == ERR_MEM) {
            len /= 2;
        }
    } while (err == ERR_MEM && len > 1);

    if (err == ERR_OK) {
        hs->file += len;
        hs->left -= len;
    }
}

// Called in a regular interval. Implements retries.
static err_t
http_poll(void *arg, struct tcp_pcb *pcb)
{
    struct http_state *hs;

    CYG_TEST_INFO("Polling");

    hs = arg;

    if (hs == NULL) {
        tcp_abort(pcb);
        return ERR_ABRT;
    } else {
        ++hs->retries;
        if (hs->retries == 4) {
            tcp_abort(pcb);
            return ERR_ABRT;
        }
        send_data(pcb, hs);
    }

    return ERR_OK;
}

// Called when data has been sent.
static err_t
http_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    struct http_state *hs;

    CYG_TEST_INFO("Sent data");

    hs = arg;

    hs->retries = 0;

    if (hs->left > 0) {
        send_data(pcb, hs);
    } else {
        close_conn(pcb, hs);
    }

    return ERR_OK;
}

// Called when data has been received. Checks for HTTP requests.
static err_t
http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    int i;
    char *data;
    struct http_state *hs;

    CYG_TEST_INFO("Received data");

    hs = arg;

    if (err == ERR_OK && p != NULL) {
        // Inform TCP that we have taken the data
        tcp_recved(pcb, p->tot_len);

        // Check for HTTP requests
        if (hs->file == NULL) {
            data = p->payload;

            if (*data == 'G') {
                for (i = 0; i < 40; i++) {
                    if (((char *) data + 4)[i] == ' ' ||
                        ((char *) data + 4)[i] == '\r' ||
                        ((char *) data + 4)[i] == '\n') {
                        ((char *)data + 4)[i] = 0;
                    }
                }
                
                prepare_page(hs);

                pbuf_free(p);
                send_data(pcb, hs);

                // Tell TCP that we wish be to informed of data that has been
                // successfully sent by a call to the http_sent() function.
                tcp_sent(pcb, http_sent);
            } else {
                pbuf_free(p);
                close_conn(pcb, hs);
            }
        } else {
            pbuf_free(p);
        }
    }

    if (err == ERR_OK && p == NULL) {
        close_conn(pcb, hs);
    }

    return ERR_OK;
}

// Called when a new connection was accepted.
static err_t
http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct http_state *hs;

    CYG_TEST_INFO("Incoming connection");

    tcp_setprio(pcb, TCP_PRIO_MIN);

    // Allocate memory for the structure that holds the state of the connection
    hs = mem_malloc(sizeof(struct http_state));
    if (hs == NULL)
        return ERR_MEM;

    // Initialize the structure
    hs->file = NULL;
    hs->left = 0;
    hs->retries = 0;

    // Tell TCP that this is the structure we wish to be passed for our
    // callbacks
    tcp_arg(pcb, hs);

    // Register callbacks
    tcp_recv(pcb, http_recv);
    tcp_err(pcb, http_err);
    tcp_poll(pcb, http_poll, 4);

    CYG_TEST_INFO("Connection accepted");

    return ERR_OK;
}

// Main thread.
void
main_thread_entry(cyg_addrword_t p)
{
    struct tcp_pcb *pcb;

    CYG_TEST_INFO("Initializing lwIP");
    cyg_lwip_simple_init();

    CYG_TEST_INFO("Initializing tcb");
    pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_accept);

    CYG_TEST_INFO("Running");
    while (1) {
        cyg_lwip_simple_poll();
        cyg_thread_delay(1);
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

#else // LWIP_TCP
#define N_A_MSG "TCP support disabled"
#endif // LWIP_TCP

#else // CYGFUN_LWIP_MODE_SIMPLE
#define N_A_MSG "Not configured in 'Simple' mode"
#endif // CYGFUN_LWIP_MODE_SIMPLE

#ifdef N_A_MSG
externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG
