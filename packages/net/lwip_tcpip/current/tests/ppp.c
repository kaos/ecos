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

// Simple ppp test

#include <stdio.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/kernel/kapi.h>

#include <pkgconf/net_lwip.h>

#include <lwip.h>
#include <lwip/dns.h>

#include "netif/ppp/ppp.h"
#include "netif/ppp/chat.h"

#ifdef CYGFUN_LWIP_MODE_SIMPLE
#if PPP_SUPPORT
#if LWIP_TCP

#define NUM_RUNS        5
#define CHAT_TIMEOUT    30

#define STACK_SIZE      CYGNUM_HAL_STACK_SIZE_TYPICAL

static char main_stack[STACK_SIZE];
static cyg_thread main_thread;
static cyg_handle_t main_handle;

static struct chat_item chat_items[] = {
    { CHAT_ABORT,   "NO CARRIER" },
    { CHAT_ABORT,   "NO DIALTONE" },
    { CHAT_ABORT,   "BUSY" },
    { CHAT_ABORT,   "ERROR" },
    { CHAT_SEND,    "+++" },
    { CHAT_SLEEP,   (char *) 2000 },
    { CHAT_SEND,    "AT\r\n" },
    { CHAT_WAIT,    "OK" },
    { CHAT_SEND,    "ATZ\r\n" },
    { CHAT_WAIT,    "OK" },
    { CHAT_SEND,    "ATE0\r\n" },
    { CHAT_WAIT,    "OK" },
    { CHAT_SEND_CB, (char *) 0 },   // AT+CGDCONT
    { CHAT_WAIT,    "OK" },
    { CHAT_SEND,    (char *) 1 },   // ATD
    { CHAT_WAIT,    "CONNECT" },
    { CHAT_SLEEP,   (char *) 1000 },
    { CHAT_LAST,    0 },
};

enum test_state {
    TEST_INITIAL,
    TEST_CHAT_INIT,
    TEST_CHAT_RUN,
    TEST_CHAT_FINISH,
    TEST_PPP_INIT,
    TEST_PPP_INIT_WAIT,
    TEST_PPP_UP,
    TEST_PPP_CLOSE,
    TEST_PPP_CLOSE_WAIT,
    TEST_NEXT_RUN,
    TEST_FINISH,
};

enum req_state {
    REQ_DNS_INIT,
    REQ_DNS_WAIT,
    REQ_DNS_FAILED,
    REQ_DNS_SUCCESS,
};

static enum test_state test_state;
static enum req_state req_state;
static sio_fd_t sd;
static chat_t chat;
static int pd;
static struct ip_addr host_addr;
static int run = 1;
static int success = 0;
static int chat_ok;

static void
chat_cb(chat_t chat, chat_err_t err, void *arg)
{
    switch (err) {
    case CHAT_ERR_OK:
        CYG_TEST_INFO("Chat OK");
        chat_ok = 1;
        break;
    case CHAT_ERR_ABORT:
        CYG_TEST_INFO("Chat aborted");
        chat_ok = 0;
        break;
    case CHAT_ERR_TIMEOUT:
        CYG_TEST_INFO("Chat timeout");
        chat_ok = 0;
        break;
    }

    test_state = TEST_CHAT_FINISH;
}

static void
chat_send_cb(chat_t chat, int id, char *buf, size_t len, void *arg)
{
    switch (id) {
    case 0:
        snprintf(buf, len, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", CYGDAT_NET_LWIP_PPP_TEST_APN);
        break;
    case 1:
        snprintf(buf, len, "ATD%s\r\n", CYGDAT_NET_LWIP_PPP_TEST_NUMBER);
        break;
    }
}

static void
ppp_status_cb(void *ctx, int err, void *arg)
{
    switch (err) {
    case PPPERR_NONE:
        CYG_TEST_INFO("PPP link up");
        // Setup DNS server
        {
            struct ppp_addrs *addrs = (struct ppp_addrs *) arg;
            dns_setserver(2, &addrs->dns1);
        }

        if (test_state == TEST_PPP_INIT_WAIT)
            test_state = TEST_PPP_UP;
        break;
    case PPPERR_PARAM:
        CYG_TEST_INFO("Invalid parameter");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_OPEN:
        CYG_TEST_INFO("Unable to open PPP session");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_DEVICE:
        CYG_TEST_INFO("Invalid I/O device for PPP");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_ALLOC:
        CYG_TEST_INFO("Unable to allocate resources");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_USER:
        CYG_TEST_INFO("User interrupt");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_CONNECT:
        CYG_TEST_INFO("Connection lost");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_AUTHFAIL:
        CYG_TEST_INFO("Failed authentication challenge");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    case PPPERR_PROTOCOL:
        CYG_TEST_INFO("Failed to meet protocol");
        test_state = TEST_PPP_CLOSE_WAIT;
        break;
    }
}

static void
dns_found_cb(const char *name, struct ip_addr *addr, void *arg)
{
    if (addr) {
        host_addr = *addr;
        req_state = REQ_DNS_SUCCESS;
    } else {
        req_state = REQ_DNS_FAILED;
    }
}


static void
handle_req_state(void)
{
    switch (req_state) {
    case REQ_DNS_INIT:
        CYG_TEST_INFO("Trying to resolve host name");
        if (dns_gethostbyname(CYGDAT_NET_LWIP_PPP_TEST_HOST, &host_addr,
                              dns_found_cb, NULL) == ERR_OK) {
            // Cached
            req_state = REQ_DNS_SUCCESS;
            break;
        }
        req_state = REQ_DNS_WAIT;
        break;
    case REQ_DNS_WAIT:
        break;
    case REQ_DNS_FAILED:
        CYG_TEST_INFO("Failed to resolve host name");
        test_state = TEST_PPP_CLOSE;
        break;
    case REQ_DNS_SUCCESS:
        CYG_TEST_INFO("Successfully resolved host name");
        success++;
        test_state = TEST_PPP_CLOSE;
        break;
    }
}

void
main_thread_entry(cyg_addrword_t p)
{
    CYG_TEST_INFO("Initializing lwIP");
    cyg_lwip_simple_init();

    test_state = TEST_CHAT_INIT;
    req_state = REQ_DNS_INIT;

    while (1) {
        cyg_lwip_simple_poll();
        switch (test_state) {
        case TEST_INITIAL:
            diag_printf("INFO:<Starting test run (%d/%d)>\n", run, NUM_RUNS);
            test_state = TEST_CHAT_INIT;
            break;
        case TEST_CHAT_INIT:
            CYG_TEST_INFO("Initializing chat");
            sd = sio_open(SIO_DEV_PPPOS);
            if (!sd)
                CYG_TEST_FAIL_FINISH("Cannot open serial");
            chat = chat_new(sd, chat_items, CHAT_TIMEOUT, chat_cb, chat_send_cb, NULL);
            if (!chat)
                CYG_TEST_FAIL_FINISH("Cannot allocate chat");
            test_state = TEST_CHAT_RUN;
            break;
        case TEST_CHAT_RUN:
            chat_poll(chat);
            break;
        case TEST_CHAT_FINISH:
            chat_free(chat);
            if (chat_ok)
                test_state = TEST_PPP_INIT;
            else
                test_state = TEST_NEXT_RUN;
            break;
        case TEST_PPP_INIT:
            CYG_TEST_INFO("Initializing PPP");
            if (ppp_init() != ERR_OK)
                CYG_TEST_FAIL_FINISH("Cannot initialize PPP");
            pd = ppp_open_serial(sd, ppp_status_cb, (void *) pd);
            if (pd < 0)
                CYG_TEST_FAIL_FINISH("Cannot open PPP over serial");
            ppp_set_auth(PPPAUTHTYPE_ANY, CYGDAT_NET_LWIP_PPP_TEST_USERNAME,
                                          CYGDAT_NET_LWIP_PPP_TEST_PASSWORD);
            test_state = TEST_PPP_INIT_WAIT;
            break;
        case TEST_PPP_INIT_WAIT:
            ppp_poll(pd);
            break;
        case TEST_PPP_UP:
            ppp_poll(pd);
            handle_req_state();
            break;
        case TEST_PPP_CLOSE:
            ppp_close(pd);
            test_state = TEST_PPP_CLOSE_WAIT;
            break;
        case TEST_PPP_CLOSE_WAIT:
            ppp_poll(pd);
            if (!ppp_is_open(pd))
                // Escape from data mode
                sio_write(sd, (u8_t *) "+++", 3);
                cyg_thread_delay(200);
                test_state = TEST_NEXT_RUN;
            break;
        case TEST_NEXT_RUN:
            if (run < NUM_RUNS) {
                run++;
                test_state = TEST_INITIAL;
            } else {
                test_state = TEST_FINISH;
            }
            break;
        case TEST_FINISH:
            test_state = TEST_CHAT_INIT;
            diag_printf("INFO:<Test done (%d/%d) successful runs\n",
                        success, NUM_RUNS);
            break;
        }
        cyg_thread_yield();
    }
}

void
ppptest_main(void)
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Testing ppp");

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
cyg_start(void)
{
    ppptest_main();
}

#else // LWIP_TCP
#define N_A_MSG "TCP support disabled"
#endif // LWIP_TCP

#else // PPP_SUPPORT
#define N_A_MSG "PPP support disabled"
#endif // PPP_SUPPORT

#else // CYGFUN_LWIP_MODE_SIMPLE
#define N_A_MSG "Not configured in simple mode"
#endif // CYGFUN_LWIP_MODE_SIMPLE

#ifdef N_A_MSG
externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG
