//==========================================================================
//
//      ipv6_routing_thread.c
//
//      IPv6 routing support
//
//==========================================================================
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2002-04-16
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// IPv6 routing support

#include <pkgconf/net.h>
#undef _KERNEL
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

#include <cyg/kernel/kapi.h>

#include <net/if.h>
#include <ifaddrs.h>
#include <net/if_var.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netdb.h>
#include <netinet6/in6_var.h>
#include <netinet6/nd6.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALLROUTER       "ff02::2"

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL+2048
static char rs_stack[STACK_SIZE];
static cyg_thread rs_thread_data;
static cyg_handle_t rs_thread_handle;

static void
cyg_rs_exit(void)
{
    cyg_thread_exit();
}

static void
cyg_rs(cyg_addrword_t param)
{
    struct timeval tv;
    struct addrinfo hints, *res;
    int s, len, err;
    struct sockaddr_in6 to;
    struct icmp6_hdr *icp;
    u_int hlim = 255;
    u_char outpack[sizeof(struct icmp6_hdr)];
    int cc = sizeof(struct icmp6_hdr);
    fd_set fdset;
    char msg[1024];

    _show_all_interfaces();

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    err = getaddrinfo(ALLROUTER, NULL, &hints, &res);
    if (err) {
        diag_printf("%s - failed to get ALL ROUTER: %s", 
                    __FUNCTION__, gai_strerror(err));
        cyg_rs_exit();
    }
    memcpy(&to, res->ai_addr, res->ai_addrlen);
    *(u_int16_t *)&to.sin6_addr.s6_addr[2] = htons(1);

    if ((s = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) {
        diag_printf("%s - can't open socket: %s\n", 
                    __FUNCTION__, strerror(errno));
        cyg_rs_exit();
    }
    if (setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
                   &hlim, sizeof(hlim)) < 0) {
        diag_printf("%s - can set IPV6 MULTICAST HOPS: %s\n",
                    __FUNCTION__, strerror(errno));
        cyg_rs_exit();
    }

    tv.tv_sec = 15;
    tv.tv_usec = 0;
    select(0, 0, 0, 0, &tv);  // Sleeps for tv

    icp = (struct icmp6_hdr *)outpack;
    icp->icmp6_type = ND_ROUTER_SOLICIT;
    icp->icmp6_code = 0;
    icp->icmp6_cksum = 0;
    icp->icmp6_data32[0] = 0; /* RS reserved field */

    while (true) {
	if (sendto(s, outpack, cc, 0, (struct sockaddr *)&to, to.sin6_len) < 0) {
           diag_printf("%s - can't send RS: %s\n",
                       __FUNCTION__, strerror(errno));
        }
        // See if a router gives back an answer
	FD_ZERO(&fdset);
	FD_SET(s, &fdset);
	tv.tv_sec = RTR_SOLICITATION_INTERVAL;
	tv.tv_usec = 0;
	if (select(s + 1, &fdset, NULL, NULL, &tv) == 1) {
            len = read(s, msg, sizeof(msg));
            if (len < 0) {
                diag_printf("%s - can't read router message: %s\n",
                            __FUNCTION__, strerror(errno));
            } else {
                diag_printf("%s - router message\n", __FUNCTION__);
                diag_dump_buf(msg, len);
            }
	}
        tv.tv_sec = CYGINT_NET_IPV6_ROUTING_THREAD_PERIOD - RTR_SOLICITATION_INTERVAL;
        tv.tv_usec = 0;
        select(0, 0, 0, 0, &tv);  // Sleeps for tv
    }
}

void
ipv6_start_routing_thread(void)
{
    cyg_thread_create(
        CYGINT_NET_IPV6_ROUTING_THREAD_PRIORITY, // Priority
        cyg_rs,                                  // entry
        0,                                       // parameter
        "IPv6 routing",                          // Name
        &rs_stack[0],                            // Stack
        STACK_SIZE,                              // Size
        &rs_thread_handle,                       // Handle
        &rs_thread_data                          // Thread data structure
        );
    cyg_thread_resume(rs_thread_handle);    // Start it
    diag_printf("IPv6 routing thread started\n");
}
