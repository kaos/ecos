//=============================================================================
//
//      dns.c
//
//      DNS client code
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   andrew.lunn
// Contributors:andrew.lunn, jskov
// Date:        2001-09-18
// Description: Provides DNS lookup as per RFC 1034/1035.
// 
// Note:        Does only support A and PTR lookups. Maybe add for other
//              types as well?
//
//              parse_answer() only returns the first found record.
//
//              Add tracing and assertions.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/system.h>
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
# include <cyg/kernel/kapi.h>
#endif
#include <cyg/hal/drv_api.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         /* Tracing support */

#include <netdb.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cyg/ns/dns/dns_priv.h>

static short id = 0;              /* ID of the last query */
static int s = -1;                /* Socket to the DNS server */
static cyg_drv_mutex_t dns_mutex; /* Mutex to stop multiple queries as once */
static cyg_ucount32 ptdindex;     /* Index for the per thread data */
static char * domainname=NULL;    /* Domain name used for queries */

/* Allocate space for string of length len. Return NULL on failure. */
static inline char*
alloc_string(int len)
{
    return malloc(len);
}

static inline void
free_string(char* s)
{
    free(s);
}

/* Deallocate the memory taken to hold a hent structure */
static void
free_hent(struct hostent * hent)
{
    if (hent->h_name) {
        free_string(hent->h_name);
    }

    if (hent->h_addr_list) {
        int i = 0;
        while (hent->h_addr_list[i]) {
            free(hent->h_addr_list[i]);
            i++;
        }
        free(hent->h_addr_list);
    }
    free(hent);
}

/* Allocate hent structure with room for one in_addr. Returns NULL on
   failure. */
static struct hostent*
alloc_hent(void)
{
    struct hostent *hent;

    hent = malloc(sizeof(struct hostent));
    if (hent) {
        memset(hent, 0, sizeof(struct hostent));
        hent->h_addr_list = malloc(sizeof(char *)*2);
        if (!hent->h_addr_list) {
            free_hent(hent);
            return NULL;
        }
        hent->h_addr_list[0] = malloc(sizeof(struct in_addr));
        if (!hent->h_addr_list[0]) {
            free_hent(hent);
            return NULL;
        }
        hent->h_addr_list[1] = NULL;
    }

    return hent;
}

/* Thread destructor used to free stuff stored in per-thread data slot. */
static void
thread_destructor(CYG_ADDRWORD data)
{
    struct hostent *hent;
    hent = (struct hostent *)cyg_thread_get_data(index);
    if (hent)
        free_hent(hent);
    return;
    data=data;
}

/* Store the hent away in the per-thread data. */
static void
store_hent(struct hostent *hent)
{
    // Prevent memory leaks by setting a destructor to be
    // called on thread exit to free per-thread data.
    cyg_thread_add_destructor( &thread_destructor, 0 );
    cyg_thread_set_data(index, (CYG_ADDRWORD)hent);
}

/* If there is an answer to an old query, free the memory it uses. */
static void
free_stored_hent(void)
{
    struct hostent *hent;
    hent = (struct hostent *)cyg_thread_get_data(index);
    if (hent) {
        free_hent(hent);
        cyg_thread_set_data(index, (CYG_ADDRWORD)NULL);
        cyg_thread_rem_destructor( &thread_destructor, 0 );
    }
}

/* Send the query to the server and read the response back. Return -1
   if it fails, otherwise put the response back in msg and return the
   length of the response. */
static int 
send_recv(char * msg, int len, int msglen)
{
    struct dns_header *dns_hdr;
    struct timeval timeout;
    int finished = false;
    int backoff = 1;
    fd_set readfds;
    int written;
    int ret;

    CYG_REPORT_FUNCNAMETYPE( "send_recv", "returning %d" );
    CYG_REPORT_FUNCARG3( "msg=%08x, len=%d, msglen", msg, len, msglen );

    CYG_CHECK_DATA_PTR( msg, "msg is not a valid pointer!" );

    dns_hdr = (struct dns_header *) msg;

    do { 
        written = write(s, msg, len);
        if (written < 0) {
            ret = -1;
            break;
        }

        FD_ZERO(&readfds);
        FD_SET(s, &readfds);
    
        timeout.tv_sec = backoff;
        timeout.tv_usec = 0;
        backoff = backoff << 1;

        ret = select(s+1, &readfds, NULL, NULL, &timeout);
        if (ret < 0) {
            ret = -1;
            break;
        }
        /* Timeout */
        if (ret == 0) {
            if (backoff > 16) {
                h_errno = TRY_AGAIN;
                ret = -1;
                break;
            }
        }
        if (ret == 1) {
            ret = read(s, msg, msglen);
            if (ret < 0) {
                ret = -1;
                break;
            }
      
            /* Reply to an old query. Ignore it */
            if (ntohs(dns_hdr->id) != (id-1)) {
                continue;
            }
            finished = true;
        }
    } while (!finished);

    CYG_REPORT_RETVAL( ret );

    return ret;
}

/* Include the DNS client implementation code */
#include <cyg/ns/dns/dns_impl.inl>

/* Initialise the resolver. Open a socket and bind it to the address
   of the server.  return -1 if something goes wrong, otherwise 0. If
   we are being called a second time we have to be careful to allow
   any ongoing lookups to finish before we close the socket and
   connect to a different DNS server. The danger here is that we may
   have to wait for upto 32 seconds if the DNS server is down.
 */
int  
cyg_dns_res_init(struct in_addr *dns_server)
{
    struct sockaddr_in server;
    struct servent *sent;
    static int init =0;

    CYG_REPORT_FUNCNAMETYPE( "cyg_dns_res_init", "returning %d" );
    CYG_REPORT_FUNCARG1( "dns_server=%08x", dns_server );

    CYG_CHECK_DATA_PTR( dns_server, "dns_server is not a valid pointer!" );

    if (init) {
      cyg_drv_mutex_lock(&dns_mutex);
      cyg_thread_free_data_index(ptdindex);
      if (s >= 0) {
        close(s);
      }
    } else {
      init = 1;
      cyg_drv_mutex_init(&dns_mutex);
      cyg_drv_mutex_lock(&dns_mutex);
    }
    

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        cyg_drv_mutex_unlock(&dns_mutex);
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
  
    sent = getservbyname("domain", "udp");
    if (sent == (struct servent *)0) {
        s = -1;
        cyg_drv_mutex_unlock(&dns_mutex);
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
  
    memcpy((char *)&server.sin_addr, dns_server, sizeof(server.sin_addr));
    server.sin_port = sent->s_port;
    server.sin_family = AF_INET;
    server.sin_len = sizeof(server);

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        s = -1;
        cyg_drv_mutex_unlock(&dns_mutex);
        CYG_REPORT_RETVAL( -1 );
        return -1;
    }
    ptdindex = cyg_thread_new_data_index();  
  
    cyg_drv_mutex_unlock(&dns_mutex);
  
    CYG_REPORT_RETVAL( 0 );
    return 0;
}
