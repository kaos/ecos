//==========================================================================
//
//      lib/bootp_support.c
//
//      Minimal BOOTP functions
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
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
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// BOOTP support

#include <network.h>

// This function sets up the interface it the simplest configuration.
// Just enough to broadcast a BOOTP request and get a response.
// It returns 'true' if a response was obtained.
cyg_bool_t
do_bootp(const char *intf, struct bootp *recv)
{
    struct sockaddr_in *addrp;
    struct ifreq ifr;
    struct sockaddr_in cli_addr, serv_addr, bootp_server_addr;
    struct ecos_rtentry route;
    int s, addrlen;
    int one = 1;
    struct bootp bootp_xmit;
    unsigned char mincookie[] = {99,130,83,99,255} ;
    struct timeval tv;

    // Ensure clean slate
    route_reinit();  // Force any existing routes to be forgotten

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return false;
    }

    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one))) {
        perror("setsockopt");
        return false;
    }

    addrp = (struct sockaddr_in *) &ifr.ifr_addr;
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;

    strcpy(ifr.ifr_name, intf);
    if (ioctl(s, SIOCSIFADDR, &ifr)) {
        perror("SIOCSIFADDR");
        return false;
    }

    if (ioctl(s, SIOCSIFNETMASK, &ifr)) {
        perror("SIOCSIFNETMASK");
        return false;
    }

    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    if (ioctl(s, SIOCSIFBRDADDR, &ifr)) {
        perror("SIOCSIFBRDADDR");
        return false;
    }

    ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
    if (ioctl(s, SIOCSIFFLAGS, &ifr)) {
        perror("SIOCSIFFLAGS");
        return false;
    }

    if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
        perror("SIOCGIFHWADDR");
        return false;
    }

    // Set up routing
    /* the broadcast address is 255.255.255.255 */
    memset(&addrp->sin_addr, 255, sizeof(addrp->sin_addr));
    memset(&route, 0, sizeof(route));
    memcpy(&route.rt_gateway, addrp, sizeof(*addrp));

    addrp->sin_family = AF_INET;
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;
    memcpy(&route.rt_dst, addrp, sizeof(*addrp));
    memcpy(&route.rt_genmask, addrp, sizeof(*addrp));

    route.rt_dev = ifr.ifr_name;
    route.rt_flags = RTF_UP|RTF_GATEWAY;
    route.rt_metric = 0;

    if (ioctl(s, SIOCADDRT, &route)) {
        if (errno != EEXIST) {
            perror("SIOCADDRT 3");
            return false;
        }
    }

    memset((char *) &cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cli_addr.sin_port = htons(IPPORT_BOOTPC);
    
    if(bind(s, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        perror("bind error");
        return false;
    }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }
    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
        perror("setsockopt SO_REUSEPORT");
        return false;
    }
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    serv_addr.sin_port = htons(IPPORT_BOOTPS);

    // Fill in the BOOTP request
    bzero(&bootp_xmit, sizeof(bootp_xmit));
    if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
        perror("SIOCGIFHWADDR");
        return false;
    }
    bootp_xmit.bp_htype = ifr.ifr_hwaddr.sa_family;
    bootp_xmit.bp_hlen = IFHWADDRLEN;
    bcopy(ifr.ifr_hwaddr.sa_data, &bootp_xmit.bp_chaddr, bootp_xmit.bp_hlen);

    bootp_xmit.bp_secs = 0;
    bcopy(mincookie, bootp_xmit.bp_vend, sizeof(mincookie));

    bootp_xmit.bp_op = BOOTREQUEST;

    if(sendto(s, &bootp_xmit, sizeof(struct bootp), 0, 
              (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("sendto error");
        return false;
    }

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    addrlen = sizeof(bootp_server_addr);
    if (recvfrom(s, recv, sizeof(struct bootp), 0,
                 (struct sockaddr *)&bootp_server_addr, &addrlen) < 0) {
        perror("recvfrom error");
        return false;
    }


    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr.s_addr = INADDR_ANY;

    strcpy(ifr.ifr_name, intf);
    if (ioctl(s, SIOCDIFADDR, &ifr)) {
        perror("SIOCDIFADDR");
    }

    // Shut down interface so it can be reinitialized
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);
    if (ioctl(s, SIOCSIFFLAGS, &ifr)) {
        perror("SIOCSIFFLAGS");
        return false;
    }

    // All done with socket
    close(s);
    return true;
}

static char *_bootp_op[] = {"", "REQUEST", "REPLY"};
static char *_bootp_hw_type[] = {"", "Ethernet", "Exp Ethernet", "AX25",
                                     "Pronet", "Chaos", "IEEE802", "Arcnet"};

void
show_bootp(const char *intf, struct bootp *bp)
{
    int i, len;
    unsigned char *op, *ap = 0;
    unsigned char name[128];
    struct in_addr addr[32];
    diag_printf("BOOTP[%s] op: %s\n", intf, _bootp_op[bp->bp_op]);
    diag_printf("       hw_type: %s\n", _bootp_hw_type[bp->bp_htype]);
    diag_printf("       hw_addr: ");
    for (i = 0;  i < bp->bp_hlen;  i++) {
        diag_printf("%02x", bp->bp_chaddr[i]);
        if (i != (bp->bp_hlen-1)) diag_printf(":");
    }
    diag_printf("\n");
    diag_printf("     client IP: %s\n", inet_ntoa(bp->bp_ciaddr));
    diag_printf("         my IP: %s\n", inet_ntoa(bp->bp_yiaddr));
    diag_printf("     server IP: %s\n", inet_ntoa(bp->bp_siaddr));
    diag_printf("    gateway IP: %s\n", inet_ntoa(bp->bp_giaddr));
    if (bp->bp_sname[0]) {
        diag_printf("        server: %s\n", bp->bp_sname);
    }
    if (bp->bp_file[0]) {
        diag_printf("          file: %s\n", bp->bp_file);
    }
    if (bp->bp_vend[0]) {
        diag_printf("  options:\n");
        op = &bp->bp_vend[4];
        while (*op != TAG_END) {
            switch (*op) {
            case TAG_SUBNET_MASK:
            case TAG_GATEWAY:
            case TAG_IP_BROADCAST:
            case TAG_DOMAIN_SERVER:
                ap = (unsigned char *)&addr[0];
                len = *(op+1);
                for (i = 0;  i < len;  i++) {
                    *ap++ = *(op+i+2);
                }
                if (*op == TAG_SUBNET_MASK)   ap =  "  subnet mask";
                if (*op == TAG_GATEWAY)       ap =  "      gateway";
                if (*op == TAG_IP_BROADCAST)  ap =  " IP broadcast";
                if (*op == TAG_DOMAIN_SERVER) ap =  "domain server";
                diag_printf("      %s: ", ap);
                ap = (unsigned char *)&addr[0];
                while (len > 0) {
                    diag_printf("%s", inet_ntoa(*(struct in_addr *)ap));
                    len -= sizeof(struct in_addr);
                    ap += sizeof(struct in_addr);
                    if (len) diag_printf(", ");
                }
                diag_printf("\n");
                break;
            case TAG_DOMAIN_NAME:
            case TAG_HOST_NAME:
                for (i = 0;  i < *(op+1);  i++) {
                    name[i] = *(op+i+2);
                }
                name[*(op+1)] = '\0';
                if (*op == TAG_DOMAIN_NAME) ap =  " domain name";
                if (*op == TAG_HOST_NAME)   ap =  "   host name";
                diag_printf("       %s: %s\n", ap, name);
                break;
            default:
                diag_printf("Unknown option: %x.%d\n", *op, *(op+1));
            }                
            op += *(op+1)+2;
        }
    }
}

cyg_bool_t
get_bootp_option(struct bootp *bp, unsigned char tag, void *opt)
{
    unsigned char *op;
    unsigned char *val = (unsigned char *)opt;
    int i;
    op = &bp->bp_vend[4];
    while (*op != TAG_END) {
        if (*op == tag) {
            for (i = 0;  i < *(op+1);  i++) {
                *val++ = *(op+i+2);
            }
            return true;
        }
        op += *(op+1)+2;
    }
    return false;
}

// [Re]initialize the network interface with the info passed from BOOTP
cyg_bool_t
init_net(const char *intf, struct bootp *bp)
{
    struct sockaddr_in *addrp;
    struct ifreq ifr;
    int s;
    int one = 1;
    struct ecos_rtentry route;
    struct in_addr netmask, gateway;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return false;
    }

    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one))) {
        perror("setsockopt");
        return false;
    }

    addrp = (struct sockaddr_in *) &ifr.ifr_addr;
    memset(addrp, 0, sizeof(*addrp));
    addrp->sin_family = AF_INET;
    addrp->sin_len = sizeof(*addrp);
    addrp->sin_port = 0;
    addrp->sin_addr = bp->bp_yiaddr;  // The address BOOTP gave us

    strcpy(ifr.ifr_name, intf);
    if (ioctl(s, SIOCSIFADDR, &ifr)) {
        perror("SIOCIFADDR");
        return false;
    }

    if (get_bootp_option(bp, TAG_SUBNET_MASK, &addrp->sin_addr)) {
        netmask = addrp->sin_addr;
        if (ioctl(s, SIOCSIFNETMASK, &ifr)) {
            perror("SIOCSIFNETMASK");
            return false;
        }
    }

    if (get_bootp_option(bp, TAG_IP_BROADCAST, &addrp->sin_addr)) {
        if (ioctl(s, SIOCSIFBRDADDR, &ifr)) {
            perror("SIOCSIFBRDADDR");
            return false;
        }
    
    }

    ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
    if (ioctl(s, SIOCSIFFLAGS, &ifr)) {
        perror("SIOCSIFFLAGS");
        return false;
    }

    // Set up routing
    if (get_bootp_option(bp, TAG_GATEWAY, &gateway)) {
        memset(&route, 0, sizeof(route));
        addrp->sin_family = AF_INET;
        addrp->sin_port = 0;
        addrp->sin_addr.s_addr = *(unsigned int *)&bp->bp_yiaddr & *(unsigned int *)&netmask;
        memcpy(&route.rt_dst, addrp, sizeof(*addrp));
        addrp->sin_addr = netmask;
        memcpy(&route.rt_genmask, addrp, sizeof(*addrp));
        addrp->sin_addr = gateway;
        memcpy(&route.rt_gateway, addrp, sizeof(*addrp));

        route.rt_dev = ifr.ifr_name;
        route.rt_flags = RTF_UP|RTF_GATEWAY;
        route.rt_metric = 0;

        if (ioctl(s, SIOCADDRT, &route)) {
            diag_printf("Route - dst: %s", inet_ntoa(((struct sockaddr_in *)&route.rt_dst)->sin_addr));
            diag_printf(", mask: %s", inet_ntoa(((struct sockaddr_in *)&route.rt_genmask)->sin_addr));
            diag_printf(", gateway: %s\n", inet_ntoa(((struct sockaddr_in *)&route.rt_gateway)->sin_addr));
            if (errno != EEXIST) {
                perror("SIOCADDRT 3");
                return false;
            }
        }
    }
    close(s);
    return true;
}
