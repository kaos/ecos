//==========================================================================
//
//      tests/dns2.c
//
//      Simple test of DNS client support. This time use DHCP
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andrew.lunn
// Contributors: andrew.lunn, jskov
// Date:         2001-12-03
// Purpose:      
// Description:  Test DNS functions. Use the server and domain name the
//               DHCP server told us.
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <network.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <cyg/infra/testcase.h>

#ifdef CYGPKG_NET_DHCP

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + 0x1000)
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

#define __string(_x) #_x
#define __xstring(_x) __string(_x)

#define _DNS_IP            __xstring(172.16.19.254) // farmnet dns0 address
#define _LOOKUP_FQDN       __xstring(dell-paw-2.cambridge.redhat.com.)
#define _LOOKUP_DOMAINNAME __xstring(cambridge.redhat.com)
#define _LOOKUP_HOSTNAME   __xstring(dell-paw-2)
#define _LOOKUP_IP         __xstring(195.224.55.226)
#define _LOOKUP_IP_BAD     __xstring(10.0.0.0)

void
dns_test(cyg_addrword_t p)
{
    struct in_addr addr;
    struct hostent *hent;
    char dn[256];

    CYG_TEST_INIT();

    init_all_network_interfaces();

    getdomainname(dn,sizeof(dn));
    diag_printf("INFO:<DHCP said domain name is %s>\n",dn);
    CYG_TEST_CHECK(!strncmp(dn,_DNS_IP,sizeof(_DNS_IP)),
                   "DHCP got the wrong domainname");
    
    /* Expect _LOOKUP_IP as the answer. This is a CNAME lookup */
    inet_aton(_LOOKUP_IP, &addr);
    hent = gethostbyname(_LOOKUP_FQDN);
    if (hent != NULL) {
        diag_printf("PASS:<%s is %s>\n", hent->h_name, inet_ntoa(*(struct in_addr *)hent->h_addr));
        if (0 != memcmp((void*)&addr, (void*)(hent->h_addr), sizeof(struct in_addr))) {
          diag_printf("FAIL:<expected " _LOOKUP_FQDN " to be " _LOOKUP_IP ">\n");
        }
    } else {
        diag_printf("FAIL:<Asked for " _LOOKUP_FQDN ". No answer: %s>\n", hstrerror(h_errno));
    }

    /* Now just the hostname */
    hent = gethostbyname(_LOOKUP_HOSTNAME);
    if (hent != NULL) {
        diag_printf("PASS:<%s is %s>\n", _LOOKUP_HOSTNAME, inet_ntoa(*(struct in_addr *)hent->h_addr));
    } else {
        diag_printf("FAIL:<Asked for " _LOOKUP_HOSTNAME ". No answer: %s>\n", hstrerror(h_errno));
    }

    CYG_TEST_FINISH("dns1 test completed");
}

void
cyg_start(void)
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      dns_test,          // entry
                      0,                 // entry parameter
                      "DNS test",        // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
}

#else // CYGPKG_NET_DHCP

externC void
cyg_start( void )
{
    CYG_TEST_INIT();

    CYG_TEST_NA("This test needs DHCP enabled");
}

#endif
