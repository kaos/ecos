#ifndef CYGONCE_LWIPOPTS_H
#define CYGONCE_LWIPOPTS_H
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2009, 2010 Free Software Foundation
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

#include <pkgconf/net_lwip.h>

#include <cyg/hal/hal_arch.h>

//------------------------------------------------------------------------------
// Platform specific locking
//------------------------------------------------------------------------------

#define SYS_LIGHTWEIGHT_PROT        1
#define NO_SYS                      defined(CYGFUN_LWIP_MODE_SIMPLE)

//------------------------------------------------------------------------------
// Memory options
//------------------------------------------------------------------------------

#define MEM_LIBC_MALLOC             defined(CYGIMP_LWIP_MEM_LIBC_MALLOC)
#define MEMP_MEM_MALLOC             defined(CYGIMP_LWIP_MEMP_MEM_MALLOC)
#define MEM_ALIGNMENT               CYGNUM_LWIP_MEM_ALIGNMENT
#define MEM_SIZE                    CYGNUM_LWIP_MEM_SIZE

#if defined(CYGSEM_LWIP_MEM_SECTION) && CYGSEM_LWIP_MEM_SECTION
# include <cyg/infra/cyg_type.h>
# define MEM_SECTION CYGBLD_ATTRIB_SECTION(CYGDAT_LWIP_MEM_SECTION_NAME)
#else
# define MEM_SECTION
#endif

#define MEMP_OVERFLOW_CHECK         defined(CYGDBG_LWIP_MEMP_OVERFLOW_CHECK)
#define MEMP_SANITY_CHECK           defined(CYGDBG_LWIP_MEMP_SANITY_CHECK)

//#define MEM_USE_POOLS                             // not configurable
//#define MEM_USE_POOLS_TRY_BIGGER_POOL             // not configurable
//#define MEMP_USE_CUSTOM_POOLS                     // not configurable
//#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT    // not configurable

//------------------------------------------------------------------------------
// Internal memory pool sizes
//------------------------------------------------------------------------------

#define MEMP_NUM_PBUF               CYGNUM_LWIP_MEMP_NUM_PBUF
#ifdef CYGPKG_LWIP_TCP
# define MEMP_NUM_TCP_PCB           CYGNUM_LWIP_MEMP_NUM_TCP_PCB
# define MEMP_NUM_TCP_PCB_LISTEN    CYGNUM_LWIP_MEMP_NUM_TCP_PCB_LISTEN
# define MEMP_NUM_TCP_SEG           CYGNUM_LWIP_MEMP_NUM_TCP_SEG
#endif
#define MEMP_NUM_REASSDATA          CYGNUM_LWIP_MEMP_NUM_REASSDATA
#ifdef CYGFUN_LWIP_ARP_QUEUEING
# define MEMP_NUM_ARP_QUEUE         CYGNUM_LWIP_MEMP_NUM_ARP_QUEUE
#endif
#ifdef CYGPKG_LWIP_IGMP
# define MEMP_NUM_IGMP_GROUP        CYGNUM_LWIP_MEMP_NUM_IGMP_GROUP
#endif
#define MEMP_NUM_SYS_TIMEOUT        (CYGNUM_LWIP_MEMP_NUM_CORE_SYS_TIMEOUT + \
                                     CYGNUM_LWIP_MEMP_NUM_USER_SYS_TIMEOUT)
#define MEMP_NUM_NETBUF             CYGNUM_LWIP_MEMP_NUM_NETBUF
#define MEMP_NUM_NETCONN            CYGNUM_LWIP_MEMP_NUM_NETCONN
#define MEMP_NUM_TCPIP_MSG_API      CYGNUM_LWIP_MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_INPKT    CYGNUM_LWIP_MEMP_NUM_TCPIP_MSG_INPKT
#define PBUF_POOL_SIZE              CYGNUM_LWIP_PBUF_POOL_SIZE

//------------------------------------------------------------------------------
// ARP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_ARP
# define LWIP_ARP                   1
# define ARP_TABLE_SIZE             CYGNUM_LWIP_ARP_TABLE_SIZE
# define ARP_QUEUEING               defined(CYGFUN_LWIP_ARP_QUEUEING)
# define ETHARP_TRUST_IP_MAC        defined(CYGFUN_LWIP_ETHARP_TRUST_IP_MAC)
#endif

//------------------------------------------------------------------------------
// IP options
//------------------------------------------------------------------------------

#ifdef CYGFUN_LWIP_IP_FORWARD
# define IP_FORWARD                 1
#endif
#define IP_OPTIONS_ALLOWED          defined(CYGFUN_LWIP_IP_OPTIONS_ALLOWED)
#define IP_REASSEMBLY               defined(CYGFUN_LWIP_IP_REASSEMBLY)
#define IP_FRAG                     defined(CYGFUN_LWIP_IP_FRAG)
#define IP_REASS_MAXAGE             CYGNUM_LWIP_IP_REASS_MAXAGE
#define IP_REASS_MAX_PBUFS          CYGNUM_LWIP_IP_REASS_MAX_PBUFS
#define IP_FRAG_USES_STATIC_BUF     defined(CYGFUN_LWIP_IP_FRAG_USES_STATIC_BUF)
#ifdef CYGFUN_LWIP_IP_FRAG_USES_STATIC_BUF
# define IP_FRAG_MAX_MTU            CYGNUM_LWIP_IP_FRAG_MAX_MTU
#endif
#define IP_DEFAULT_TTL              CYGNUM_LWIP_IP_DEFAULT_TTL
#define IP_SOF_BROADCAST            defined(CYGFUN_LWIP_IP_SOF_BROADCAST)
#define IP_SOF_BROADCAST_RECV       defined(CYGFUN_LWIP_IP_SOF_BROADCAST_RECV)

//------------------------------------------------------------------------------
// ICMP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_ICMP
# define LWIP_ICMP                  1
# define ICMP_TTL                   CYGNUM_LWIP_ICMP_TTL
# define LWIP_BROADCAST_PING        defined(CYGFUN_LWIP_BROADCAST_PING)
# define LWIP_MULTICAST_PING        defined(CYGFUN_LWIP_MULTICAST_PING)
#endif

//------------------------------------------------------------------------------
// RAW options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_RAW
# define LWIP_RAW                   1
# define RAW_TTL                    CYGNUM_LWIP_RAW_TTL
#endif

//------------------------------------------------------------------------------
// DHCP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_DHCP
# define LWIP_DHCP                  1
# ifdef CYGPKG_LWIP_ARP
#  define DHCP_DOES_ARP_CHECK       defined(CYGFUN_LWIP_DHCP_DOES_ARP_CHECK)
# endif
#endif

//------------------------------------------------------------------------------
// AUTOIP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_AUTOIP
# define LWIP_AUTOIP                1
# ifdef CYGPKG_LWIP_DHCP
#  define LWIP_DHCP_AUTOIP_COOP     defined(CYGFUN_LWIP_DHCP_AUTOIP_COOP)
# endif
#endif

//------------------------------------------------------------------------------
// SNMP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_SNMP
# define LWIP_SNMP                  1
# define SNMP_CONCURRENT_REQUESTS   CYGNUM_LWIP_SNMP_CONCURRENT_REQUESTS
# define SNMP_TRAP_DESTINATIONS     CYGNUM_LWIP_SNMP_TRAP_DESTINATIONS
# define SNMP_PRIVATE_MIB           defined(CYGFUN_LWIP_SNMP_PRIVATE_MIB)
# define SNMP_SAFE_REQUESTS         defined(CYGFUN_LWIP_SNMP_SAFE_REQUESTS)
#endif

//------------------------------------------------------------------------------
// IGMP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_IGMP
# define LWIP_IGMP                  1
#endif

//------------------------------------------------------------------------------
// DNS options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_DNS
# define LWIP_DNS                   1
# define DNS_TABLE_SIZE             CYGNUM_LWIP_DNS_TABLE_SIZE
# define DNS_MAX_NAME_LENGTH        CYGNUM_LWIP_DNS_MAX_NAME_LENGTH
# define DNS_MAX_SERVERS            CYGNUM_LWIP_DNS_MAX_SERVERS
# define DNS_DOES_NAME_CHECK        defined(CYGFUN_LWIP_DNS_DOES_NAME_CHECK)
# define DNS_USES_STATIC_BUF        CYGNUM_LWIP_DNS_USES_STATIC_BUF
# define DNS_MSG_SIZE               CYGNUM_LWIP_DNS_MSG_SIZE
# ifdef CYGPKG_LWIP_DNS_LOCAL_HOSTLIST
#  define DNS_LOCAL_HOSTLIST        1
#  define DNS_LOCAL_HOSTLIST_INIT   CYGDAT_LWIP_DNS_LOCAL_HOSTLIST_INIT
#  define DNS_LOCAL_HOSTLIST_IS_DYNAMIC defined(CYGFUN_LWIP_DNS_LOCAL_HOSTLIST_IS_DYNAMIC)
# endif
#endif

//------------------------------------------------------------------------------
// UDP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_UDP
# define LWIP_UDP                   1
# define LWIP_UDPLITE               defined(CYGFUN_LWIP_UDPLITE)
# define UDP_TTL                    CYGNUM_LWIP_UDP_TTL
#endif

//------------------------------------------------------------------------------
// TCP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_TCP
# define LWIP_TCP                   1
# define TCP_TTL                    CYGNUM_LWIP_TCP_TTL
# define TCP_WND                    CYGNUM_LWIP_TCP_WND
# define TCP_MAXRTX                 CYGNUM_LWIP_TCP_MAXRTX
# define TCP_SYNMAXRTX              CYGNUM_LWIP_TCP_SYNMAXRTX
# define TCP_QUEUE_OOSEQ            defined(CYGFUN_LWIP_TCP_QUEUE_OOSEQ)
# define TCP_MSS                    CYGNUM_LWIP_TCP_MSS
# define TCP_CALCULATE_EFF_SEND_MSS defined(CYGFUN_LWIP_TCP_CALCULATE_EFF_SEND_MSS)
# define TCP_SND_BUF                CYGNUM_LWIP_TCP_SND_BUF
# define TCP_SND_QUEUELEN           CYGNUM_LWIP_TCP_SND_QUEUELEN
# define TCP_SNDLOWAT               CYGNUM_LWIP_TCP_SNDLOWAT
# ifdef CYGFUN_LWIP_TCP_LISTEN_BACKLOG
#  define TCP_LISTEN_BACKLOG        1
#  define TCP_DEFAULT_LISTEN_BACKLOG CYGNUM_LWIP_TCP_DEFAULT_LISTEN_BACKLOG
# endif
# define LWIP_TCP_TIMESTAMPS        defined(CYGFUN_LWIP_TCP_TIMESTAMPS)
# define TCP_WND_UPDATE_THRESHOLD   CYGNUM_LWIP_TCP_WND_UPDATE_THRESHOLD
# ifdef CYGFUN_LWIP_EVENT_API
#  define LWIP_EVENT_API            1
# endif
# ifdef CYGFUN_LWIP_CALLBACK_API
#  define LWIP_CALLBACK_API         1
# endif
#endif

//------------------------------------------------------------------------------
// Pbuf options
//------------------------------------------------------------------------------

#define PBUF_LINK_HLEN              CYGNUM_LWIP_PBUF_LINK_HLEN
#define PBUF_POOL_BUFSIZE           CYGNUM_LWIP_PBUF_POOL_BUFSIZE

//------------------------------------------------------------------------------
// Network interfaces options
//------------------------------------------------------------------------------

#define LWIP_NETIF_API              defined(CYGPKG_LWIP_NETIF_API)
#define LWIP_NETIF_HOSTNAME         defined(CYGIMP_LWIP_NETIF_HOSTNAME)
#define LWIP_NETIF_STATUS_CALLBACK  defined(CYGFUN_LWIP_NETIF_STATUS_CALLBACK)
#define LWIP_NETIF_LINK_CALLBACK    defined(CYGFUN_LWIP_NETIF_LINK_CALLBACK)
#define LWIP_NETIF_HWADDRHINT       defined(CYGIMP_LWIP_NETIF_HWADDRHINT)
#define LWIP_NETIF_LOOPBACK         defined(CYGIMP_LWIP_NETIF_LOOPBACK)
#define LWIP_LOOPBACK_MAX_PBUFS     CYGIMP_LWIP_NETIF_LOOPBACK_MAX_PBUFS
//#define LWIP_NETIF_LOOPBACK_MULTITHREADING // not configurable
//#define LWIP_NETIF_TX_SINGLE_PBUF // not configurable

//------------------------------------------------------------------------------
// LOOPIF options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_LOOPIF
# define LWIP_HAVE_LOOPIF           1
#endif

//------------------------------------------------------------------------------
// SLIPIF options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_SLIP
# define LWIP_HAVE_SLIPIF           1
#endif

//------------------------------------------------------------------------------
// Thread options
//------------------------------------------------------------------------------

#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
# define TCPIP_THREAD_NAME          "tcpip"
# define TCPIP_THREAD_PRIO          CYGNUM_LWIP_THREAD_TCPIP_PRIORITY
//# define TCPIP_MBOX_SIZE          // not configurable
# if CYGNUM_LWIP_THREAD_TCPIP_STACKSIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM
#  define TCPIP_THREAD_STACKSIZE    CYGNUM_HAL_STACK_SIZE_MINIMUM
# else
#  define TCPIP_THREAD_STACKSIZE    CYGNUM_LWIP_THREAD_TCPIP_STACKSIZE
# endif
#else
# define TCPIP_THREAD_STACKSIZE     0
#endif

#if defined(CYGFUN_LWIP_MODE_SEQUENTIAL) && defined(CYGPKG_LWIP_SLIP)
# define SLIPIF_THREAD_NAME         "slipif"
# if CYGNUM_LWIP_THREAD_SLIP_STACKSIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM
#  define SLIPIF_THREAD_STACKSIZE   CYGNUM_HAL_STACK_SIZE_MINIMUM
# else
#  define SLIPIF_THREAD_STACKSIZE   CYGNUM_LWIP_THREAD_SLIP_STACKSIZE
# endif
# define SLIPIF_THREAD_PRIO         CYGNUM_LWIP_THREAD_SLIP_PRIORITY
#else
# define SLIPIF_THREAD_STACKSIZE    0
#endif

#if defined(CYGFUN_LWIP_MODE_SEQUENTIAL) && defined(CYGFUN_LWIP_PPPOS_SUPPORT)
# define PPP_THREAD_NAME            "ppp"
# if CYGNUM_LWIP_THREAD_PPPOS_STACKSIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM
#  define PPP_THREAD_STACKSIZE      CYGNUM_HAL_STACK_SIZE_MINIMUM
# else
#  define PPP_THREAD_STACKSIZE      CYGNUM_LWIP_THREAD_PPPOS_STACKSIZE
# endif
# define PPP_THREAD_PRIO            CYGNUM_LWIP_THREAD_PPPOS_PRIORITY
#else
# define PPP_THREAD_STACKSIZE       0
#endif

#if defined(CYGFUN_LWIP_MODE_SEQUENTIAL) && defined(CYGPKG_LWIP_ETH)
# define ETH_THREAD_NAME            "eth"
# if CYGNUM_LWIP_THREAD_ETH_STACKSIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM
#  define ETH_THREAD_STACKSIZE      CYGNUM_HAL_STACK_SIZE_MINIMUM
# else
#  define ETH_THREAD_STACKSIZE      CYGNUM_LWIP_THREAD_ETH_STACKSIZE
# endif
# define ETH_THREAD_PRIO            CYGNUM_LWIP_THREAD_ETH_PRIORITY
#else
# define ETH_THREAD_STACKSIZE       0
#endif

//#define DEFAULT_THREAD_NAME       // not configurable
//#define DEFAULT_THREAD_STACKSIZE  // not configurable
//#define DEFAULT_THREAD_PRIO       // not configurable
//#define DEFAULT_RAW_RECVMBOX_SIZE // not configurable
//#define DEFAULT_UDP_RECVMBOX_SIZE // not configurable
//#define DEFAULT_TCP_RECVMBOX_SIZE // not configurable
//#define DEFAULT_ACCEPTMBOX_SIZE   // not configurable

//------------------------------------------------------------------------------
// Sequential layer options
//------------------------------------------------------------------------------

//#define LWIP_TCPIP_CORE_LOCKING   // not configurable
#define LWIP_NETCONN                defined(CYGPKG_LWIP_NETCONN_API)

//------------------------------------------------------------------------------
// Socket options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_SOCKET_API
# define LWIP_SOCKET                1
# define LWIP_COMPAT_SOCKETS        defined(CYGFUN_LWIP_COMPAT_SOCKETS)
# define LWIP_POSIX_SOCKETS_IO_NAMES defined(CYGFUN_LWIP_POSIX_SOCKETS_IO_NAMES)
# define LWIP_TCP_KEEPALIVE         defined(CYGFUN_LWIP_TCP_KEEPALIVE)
# define LWIP_SO_RCVTIMEO           defined(CYGFUN_LWIP_SO_RCVTIMEO)
# define LWIP_SO_RCVBUF             defined(CYGFUN_LWIP_SO_RCVBUF)
//# define RECV_BUFSIZE_DEFAULT     // not configurable
//# define SO_REUSE                 // not configurable
#else
# define LWIP_SOCKET                0
#endif

//------------------------------------------------------------------------------
// Statistics options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_STATS
# define LWIP_STATS                 1
# define LWIP_STATS_DISPLAY         1
# define LINK_STATS                 defined(CYGVAR_LWIP_LINK_STATS)
# ifdef CYGPKG_LWIP_ARP
#  define ETHARP_STATS              defined(CYGVAR_LWIP_ETHARP_STATS)
# endif
# define IP_STATS                   defined(CYGVAR_LWIP_IP_STATS)
# if defined(CYGFUN_LWIP_IP_REASSEMBLY) || defined(CYGFUN_LWIP_IP_FRAG)
#  define IPFRAG_STATS              defined(CYGVAR_LWIP_IPFRAG_STATS)
# endif
# ifdef CYGPKG_LWIP_ICMP
#  define ICMP_STATS                defined(CYGVAR_LWIP_ICMP_STATS)
# endif
# ifdef CYGPKG_LWIP_IGMP
#  define IGMP_STATS                defined(CYGVAR_LWIP_IGMP_STATS)
# endif
# ifdef CYGPKG_LWIP_UDP
#  define UDP_STATS                 defined(CYGVAR_LWIP_UDP_STATS)
# endif
# ifdef CYGPKG_LWIP_TCP
#  define TCP_STATS                 defined(CYGVAR_LWIP_TCP_STATS)
# endif
# define MEM_STATS                  defined(CYGVAR_LWIP_MEM_STATS)
# define MEMP_STATS                 defined(CYGVAR_LWIP_MEMP_STATS)
# define SYS_STATS                  defined(CYGVAR_LWIP_SYS_STATS)
#endif

//------------------------------------------------------------------------------
// PPP options
//------------------------------------------------------------------------------

#ifdef CYGPKG_LWIP_PPP
# define PPP_SUPPORT                1
# define PPPOE_SUPPORT              defined(CYGFUN_LWIP_PPPOE_SUPPORT)
# define PPPOS_SUPPORT              defined(CYGFUN_LWIP_PPPOS_SUPPORT)
# define NUM_PPP                    CYGNUM_LWIP_NUM_PPP
# ifdef CYGFUN_LWIP_PAP_SUPPORT
#  define PAP_SUPPORT               1
# else
#  define PAP_SUPPORT               0
# endif
# ifdef CYGFUN_LWIP_CHAP_SUPPORT
#  define CHAP_SUPPORT              1
# else
#  define CHAP_SUPPORT              0
# endif
//# define MSCHAP_SUPPORT           // not supported yet
//# define CBCP_SUPPORT             // not supported yet
//# define CCP_SUPPORT              // not supported yet
# define VJ_SUPPORT                 defined(CYGFUN_LWIP_VJ_SUPPORT)
# define MD5_SUPPORT                defined(CYGFUN_LWIP_MD5_SUPPORT)
# define RECORD_SUPPORT             defined(CYGFUN_LWIP_RECORD_SUPPORT)

//# define FSM_DEFTIMEOUT           // not configurable
//# define FSM_DEFMAXTERMREQS       // not configurable
//# define FSM_DEFMAXCONFREQS       // not configurable
//# define FSM_DEFMAXNAKLOOPS       // not configurable
//# define UPAP_DEFTIMEOUT          // not configurable
//# define UPAP_DEFREQTIME          // not configurable
//# define CHAP_DEFTIMEOUT          // not configurable
//# define CHAP_DEFTRANSMITS        // not configurable
//# define LCP_ECHOINTERVAL         // not configurable
//# define LCP_MAXECHOFAILS         // not configurable
//# define PPP_MAXIDLEFLAG          // not configurable
//# define PPP_MAXMTU               // not configurable
//# define PPP_DEFMRU               // not configurable
#endif

//------------------------------------------------------------------------------
// Checksum options
//------------------------------------------------------------------------------

#define CHECKSUM_GEN_IP             defined(CYGIMP_LWIP_CHECKSUM_GEN_IP)
#define CHECKSUM_GEN_UDP            defined(CYGIMP_LWIP_CHECKSUM_GEN_UDP)
#define CHECKSUM_GEN_TCP            defined(CYGIMP_LWIP_CHECKSUM_GEN_TCP)
#define CHECKSUM_CHECK_IP           defined(CYGIMP_LWIP_CHECKSUM_CHECK_IP)
#define CHECKSUM_CHECK_UDP          defined(CYGIMP_LWIP_CHECKSUM_CHECK_UDP)
#define CHECKSUM_CHECK_TCP          defined(CYGIMP_LWIP_CHECKSUM_CHECK_TCP)

//------------------------------------------------------------------------------
// Debugging options
//------------------------------------------------------------------------------

#ifdef CYGDBG_LWIP_DEBUG
# define LWIP_DEBUG                 1
// Modules
# ifdef CYGDBG_LWIP_DEBUG_ETHARP
#  define ETHARP_DEBUG              LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_NETIF
#  define NETIF_DEBUG               LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_PBUF
#  define PBUF_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_API
#  define API_LIB_DEBUG             LWIP_DBG_ON
#  define API_MSG_DEBUG             LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_SOCKETS
#  define SOCKETS_DEBUG             LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_ICMP
#  define ICMP_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_IGMP
#  define IGMP_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_IP
#  define INET_DEBUG                LWIP_DBG_ON
#  define IP_DEBUG                  LWIP_DBG_ON
#  define IP_REASS_DEBUG            LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_RAW
#  define RAW_DEBUG                 LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_MEM
#  define MEM_DEBUG                 LWIP_DBG_ON
#  define MEMP_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_SYS
#  define SYS_DEBUG                 LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_TCP
#  define TCP_DEBUG                 LWIP_DBG_ON
#  define TCP_INPUT_DEBUG           LWIP_DBG_ON
#  define TCP_FR_DEBUG              LWIP_DBG_ON
#  define TCP_RTO_DEBUG             LWIP_DBG_ON
#  define TCP_CWND_DEBUG            LWIP_DBG_ON
#  define TCP_WND_DEBUG             LWIP_DBG_ON
#  define TCP_OUTPUT_DEBUG          LWIP_DBG_ON
#  define TCP_RST_DEBUG             LWIP_DBG_ON
#  define TCP_QLEN_DEBUG            LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_UDP
#  define UDP_DEBUG                 LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_TCPIP
#  define TCPIP_DEBUG               LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_PPP
#  define PPP_DEBUG                 LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_SLIP
#  define SLIP_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_DHCP
#  define DHCP_DEBUG                LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_AUTOIP
#  define AUTOIP_DEBUG              LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_SNMP
#  define SNMP_MSG_DEBUG            LWIP_DBG_ON
#  define SNMP_MIB_DEBUG            LWIP_DBG_ON
# endif
# ifdef CYGDBG_LWIP_DEBUG_DNS
#  define DNS_DEBUG                 LWIP_DBG_ON
# endif
// Log level
# define LWIP_DBG_MIN_LEVEL         CYGDBG_LWIP_DEBUG_LEVEL
// Log types
# ifdef CYGDBG_LWIP_DEBUG_TRACE
#  define __LWIP_DBG_TRACE          LWIP_DBG_TRACE
# else
#  define __LWIP_DBG_TRACE          0
# endif
# ifdef CYGDBG_LWIP_DEBUG_STATE
#  define __LWIP_DBG_STATE          LWIP_DBG_STATE
# else
#  define __LWIP_DBG_STATE          0
# endif
# ifdef CYGDBG_LWIP_DEBUG_FRESH
#  define __LWIP_DBG_FRESH          LWIP_DBG_FRESH
# else
#  define __LWIP_DBG_FRESH          0
# endif
# define LWIP_DBG_TYPES_ON          (LWIP_DBG_ON | __LWIP_DBG_TRACE | \
                                     __LWIP_DBG_STATE | __LWIP_DBG_FRESH)
#endif

#endif // CYGONCE_LWIPOPTS_H
