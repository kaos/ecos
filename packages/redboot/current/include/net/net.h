//==========================================================================
//
//      net/net.h
//
//      Stand-alone networking support for RedBoot
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _NET_H_
#define _NET_H_

#include <pkgconf/redboot.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/basetype.h>

extern bool net_debug;

extern unsigned long do_ms_tick(void);
#define MS_TICKS() do_ms_tick()

/* #define NET_SUPPORT_RARP  1 */
#define NET_SUPPORT_ICMP 1
#define NET_SUPPORT_UDP  1
#define NET_SUPPORT_TCP  1

#if (CYG_BYTEORDER == CYG_LSBFIRST)
#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#endif
extern unsigned long  ntohl(unsigned long x);
extern unsigned long  ntohs(unsigned short x);
#else
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#endif

#define	htonl(x)	ntohl(x)
#define	htons(x)	ntohs(x)

/*
 * Minimum ethernet packet length.
 */
#define ETH_MIN_PKTLEN  60
#define ETH_MAX_PKTLEN  1514

typedef unsigned char enet_addr_t[6];
typedef unsigned char ip_addr_t[4];

typedef unsigned char  octet;
typedef unsigned short word;
typedef unsigned int   dword;

#ifndef NULL
#define NULL 0
#endif

// IPv4 support
typedef struct in_addr {
    unsigned long  s_addr;  // IPv4 address
} in_addr_t;

// Socket/connection information
struct sockaddr_in {
    struct in_addr sin_addr;
    unsigned short sin_port;
    unsigned short sin_family;
    short          sin_len;
};
#define AF_INET      1
#define INADDR_ANY   0

struct timeval {
    unsigned long tv_sec;
    unsigned long tv_usec;
};

/*
 * Simple timer support structure.
 */
typedef void (*tmr_handler_t)(void *user_data);

/*
 * Timer structure.
 * When expiration time is met or exceeded, the handler is
 * called and the timer struct is removed from the list.
 */
typedef struct _timer {
    struct _timer *next;        /* next timer in list */
    unsigned long delay;	/* expiration time relative to start time */
    unsigned long start;	/* when the timer was set */
    tmr_handler_t handler;      /* user procedure to call when timer 'fires' */
    void          *user_data;   /* user pointer passed to above procedure */
} timer_t;


/*
 * Ethernet header.
 */
typedef struct {
    enet_addr_t   destination;
    enet_addr_t   source;
    word          type;
#define ETH_TYPE_IP   0x800
#define ETH_TYPE_ARP  0x806
#define ETH_TYPE_RARP 0x8053
} eth_header_t;


/*
 * ARP/RARP header.
 */
typedef struct {
    word	hw_type;
#define ARP_HW_ETHER 1
#define ARP_HW_EXP_ETHER 2
    word	protocol;
    octet	hw_len;
    octet	proto_len;
    word	opcode;
#define	ARP_REQUEST	1
#define	ARP_REPLY	2
#define	RARP_REQUEST	3
#define	RARP_REPLY	4
    enet_addr_t	sender_enet;
    ip_addr_t	sender_ip;
    enet_addr_t	target_enet;
    ip_addr_t	target_ip;
} arp_header_t;


#define ARP_PKT_SIZE  (sizeof(arp_header_t) + sizeof(eth_header_t))

/*
 * Internet Protocol header.
 */
typedef struct {
#ifdef __LITTLE_ENDIAN__
    octet       hdr_len:4,
                version:4;
#else
    octet       version:4,
                hdr_len:4;
#endif
    octet       tos;
    word        length;
    word        ident;
    word        fragment;
    octet       ttl;
    octet       protocol;
#define IP_PROTO_ICMP  1
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP  17
    word        checksum;
    ip_addr_t   source;
    ip_addr_t   destination;
} ip_header_t;


#define IP_PKT_SIZE (60 + sizeof(eth_header_t))


/*
 * A IP<->ethernet address mapping.
 */
typedef struct {
    ip_addr_t    ip_addr;
    enet_addr_t  enet_addr;
} ip_route_t;


/*
 * UDP header.
 */
typedef struct {
    word	src_port;
    word	dest_port;
    word	length;
    word	checksum;
} udp_header_t;


/*
 * TCP header.
 */
typedef struct {
    word	src_port;
    word	dest_port;
    dword	seqnum;
    dword	acknum;
#ifdef __LITTLE_ENDIAN__
    octet       reserved:4,
                hdr_len:4;
#else
    octet       hdr_len:4,
                reserved:4;
#endif
    octet	flags;
#define TCP_FLAG_FIN  1
#define TCP_FLAG_SYN  2
#define TCP_FLAG_RST  4
#define TCP_FLAG_PSH  8
#define TCP_FLAG_ACK 16
#define TCP_FLAG_URG 32
    word	window;
    word	checksum;
    word	urgent;
} tcp_header_t;


/*
 * ICMP header.
 */
typedef struct {
    octet	type;
#define ICMP_TYPE_ECHOREPLY   0
#define ICMP_TYPE_ECHOREQUEST 8
    octet	code;
    word	checksum;
    word	ident;
    word	seqnum;
} icmp_header_t;


typedef struct _pktbuf {
    struct _pktbuf *next;
#if 0
    eth_header_t   *eth_hdr;		/* pointer to ethernet header */
#endif
    union {
	ip_header_t *__iphdr;		/* pointer to IP header */
	arp_header_t *__arphdr;		/* pointer to ARP header */
    } u1;
#define ip_hdr u1.__iphdr
#define arp_hdr u1.__arphdr
    union {
	udp_header_t *__udphdr;		/* pointer to UDP header */
	tcp_header_t *__tcphdr;		/* pointer to TCP header */
	icmp_header_t *__icmphdr;	/* pointer to ICMP header */
    } u2;
#define udp_hdr u2.__udphdr
#define tcp_hdr u2.__tcphdr
#define icmp_hdr u2.__icmphdr
    word	pkt_bytes;		/* number of data bytes in buf */
    word        bufsize;                /* size of buf */
    word	*buf;
} pktbuf_t;


/* protocol handler */
typedef void (*pkt_handler_t)(pktbuf_t *pkt);


typedef struct _udp_socket {
    struct _udp_socket	*next;
    word		our_port;
    word		pad;
    void                (*handler)(struct _udp_socket *skt, char *buf, int len,
				   ip_route_t *src_route, word src_port);
} udp_socket_t;


typedef void (*udp_handler_t)(udp_socket_t *skt, char *buf, int len,
			      ip_route_t *src_route, word src_port);


typedef struct _tcp_socket {
    struct _tcp_socket *next;
    int		       state;       /* connection state */
#define _CLOSED      0
#define _LISTEN      1
#define _SYN_RCVD    2
#define _SYN_SENT    3
#define _ESTABLISHED 4
#define _CLOSE_WAIT  5
#define _LAST_ACK    6
#define _FIN_WAIT_1  7
#define _FIN_WAIT_2  8
#define _CLOSING     9
#define _TIME_WAIT  10
    ip_route_t         his_addr;    /* address of other end of connection */
    word               our_port;
    word               his_port;
    word               data_bytes;   /* number of data bytes in pkt */
    timer_t            timer;
    pktbuf_t           pkt;         /* dedicated xmit packet */
    pktbuf_t           *rxlist;     /* list of unread incoming data packets */
    char               *rxptr;      /* pointer to next byte to read */
    int                rxcnt;       /* bytes left in current read packet */
    word               ack_pending; /* true if outgoing ack is deferred while
				       awaiting incoming ack */
    dword              ack;
    dword              seq;
    char               pktbuf[ETH_MAX_PKTLEN];
} tcp_socket_t;

/*
 * Our address.
 */
extern enet_addr_t __local_enet_addr;
extern ip_addr_t   __local_ip_addr;


/*
 * Set a timer. Caller is responsible for providing the timer_t struct.
 */
extern void __timer_set(timer_t *t, unsigned long delay,
			tmr_handler_t handler, void *user_data);

/*
 * Cancel the given timer.
 */
extern void __timer_cancel(timer_t *t);

/*
 * Poll timer list for timer expirations.
 */
extern void __timer_poll(void);

/*
 * Initialize the free list.
 */
extern void __pktbuf_init(void);

/*
 * simple pktbuf allocation.
 * allocates at least nbytes for packet data including ethernet header.
 */
extern pktbuf_t *__pktbuf_alloc(int nbytes);

/*
 * return a pktbuf for reuse.
 */
extern void __pktbuf_free(pktbuf_t *pkt);

/*
 * Dump packet structures (debug, in case of running out)
 */
extern void __pktbuf_dump(void);


/*
 * Install handlers for ethernet packets.
 * Returns old handler.
 */
extern pkt_handler_t __eth_install_handler(int eth_type,
					   pkt_handler_t handler);
/*
 * Set tcp port number for debugging. If not called, a default
 * will be selected.
 */
extern void _bsp_net_set_debug_port(int portnum);

/*
 * Non-blocking poll of ethernet link. Processes at most
 * one packet.
 */
extern void __enet_poll(void);

/*
 * Send an ethernet packet.
 */
extern void __enet_send(pktbuf_t *pkt, enet_addr_t *dest, int eth_type);


/*
 * Handle incoming ARP packets.
 */
extern void __arp_handler(pktbuf_t *pkt);

/* 
 * Find the ethernet address of the machine with the given
 * ip address.
 * Return true and fills in 'eth_addr' if successful, false
 * if unsuccessful.
 */
extern int __arp_request(ip_addr_t *ip_addr, enet_addr_t *eth_addr);

/*
 * Lookup an address from the local ARP cache.  If not found,
 * then call 'arp_request' to find it.  [Basically just a cached
 * version of 'arp_request']
 */
extern int __arp_lookup(ip_addr_t *host, ip_route_t *rt);

/*
 * Do a one's complement checksum.
 * The data being checksum'd is in network byte order.
 * The returned checksum is in network byte order.
 */
extern unsigned short __sum(word *w, int len, int init_sum);

/*
 * Compute a partial checksum for the UDP/TCP pseudo header.
 */
extern int __pseudo_sum(ip_header_t *ip);

/*
 * Handle IP packets coming from the polled ethernet interface.
 */
extern void __ip_handler(pktbuf_t *pkt, enet_addr_t *src_enet_addr);

/*
 * Send an IP packet.
 *
 * The IP data field should contain pkt->pkt_bytes of data.
 * pkt->[udp|tcp|icmp]_hdr points to the IP data field. Any
 * IP options are assumed to be already in place in the IP
 * options field.
 */
extern void __ip_send(pktbuf_t *pkt, int protocol, ip_route_t *dest);


/*
 * Handle incoming ICMP packets.
 */
extern void __icmp_handler(pktbuf_t *pkt, ip_route_t *r);

/*
 * Handle incoming UDP packets.
 */
extern void __udp_handler(pktbuf_t *pkt, ip_route_t *r);

/*
 * Install a handler for incoming udp packets.
 * Caller provides the udp_socket_t structure.
 * Returns zero if successful, -1 if socket is already used.
 */
extern int __udp_install_listener(udp_socket_t *s, word port,
				  udp_handler_t handler);

/*
 * Remove the handler for the given socket.
 */
extern void __udp_remove_listener(word port);

/*
 * Send a UDP packet.
 */
extern void __udp_send(char *buf, int len, ip_route_t *dest_ip,
		       word dest_port, word src_port);

// Send a UDP packet
extern int __udp_sendto(char *buf, int len, 
                        struct sockaddr_in *server, struct sockaddr_in *local);

// Receive a UDP packet
extern int __udp_recvfrom(char *buf, int len, 
                          struct sockaddr_in *from, struct sockaddr_in *local,
                          struct timeval *timeout);

/*
 * TCP poll function. Should be called as often as possible.
 */
extern void __tcp_poll(void);

/*
 * Set up a listening socket on the given port.
 * Does not block.
 */
extern int __tcp_listen(tcp_socket_t *s, word port);


/*
 * Block while waiting for all outstanding socket data to
 * be transmitted.
 */
extern void __tcp_drain(tcp_socket_t *s);


/*
 * Initiate connection close.
 */
extern void __tcp_close(tcp_socket_t *s);

/*
 * Wait until connection has fully closed.
 */
extern void __tcp_close_wait(tcp_socket_t *s);

/*
 * Read up to 'len' bytes without blocking.
 * Returns number of bytes read.
 * If connection is closed, returns -1.
 */
extern int __tcp_read(tcp_socket_t *s, char *buf, int len);

/*
 * Write up to 'len' bytes without blocking.
 * Returns number of bytes written.
 * If connection is closed, returns -1.
 */
extern int __tcp_write(tcp_socket_t *s, char *buf, int len);


/*
 * The following are a higher-level tcp socket interface.
 */

/*
 * Initialize a socket for given port.
 */
extern void __skt_init(tcp_socket_t *s, unsigned short port);

/*
 * Return true if socket connection is closed.
 */
#define __skt_is_closed(s) (((tcp_socket_t *)(s))->state == _CLOSED)

/*
 * Block while listening for an incoming connection.
 */
extern void __skt_wait_for_connect(tcp_socket_t *s);

/*
 * Read up to 'len' bytes from the given socket.
 * Returns number of bytes read.
 * Doesn't block.
 */
extern int __skt_read(tcp_socket_t *s, char *buf, int len);

/*
 * Write 'len' bytes to the given socket.
 * Returns number of bytes written.
 * May not write all data if connection closes.
 */
extern int __skt_write(tcp_socket_t *s, char *buf, int len);

// Initialize the network stack - logical driver layer, etc.
extern void net_init(void);

// Test for new network I/O connections
extern void net_io_test(void);

// Conversion between IP addresses and printable strings
extern bool  inet_aton(char *, in_addr_t *);
extern char *inet_ntoa(in_addr_t *);

// FIXME
/* #define NET_SUPPORT_RARP  1 */
#define NET_SUPPORT_ICMP 1
#define NET_SUPPORT_UDP  1
#define NET_SUPPORT_TCP  1

#ifdef BSP_LOG
#define BSPLOG(x) { int old_console = start_console(); x; end_console(old_console); }
#define bsp_log printf
#else
#define BSPLOG(x)
#endif

#endif // _NET_H_
