//=============================================================================
//
//      sntp.c
//
//      Simple Network Time Protocol
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003 Andrew Lunn
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
// Contributors:
// Date:        2003-02-12
// Description: Provides a Simple Network Time Protocol Client
//####DESCRIPTIONEND####
//
//=============================================================================

#include <network.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/sntp/sntp.h>
#include <time.h>

#define VERSION_MASK   0x38000000
#define VERSION_SHIFT  27
#define MODE_MASK      0x07000000
#define MODE_SHIFT     24
#define STRATUM_MASK   0x00ff0000
#define STRATUM_SHIFT  16

#define MODE_BROADCAST 5

struct sntp_srv_s {
  struct sockaddr addr;
  int stratum;
  int version;
  cyg_uint32 timestamp;
};

#define SECONDSPERMINUTE (cyg_uint32)60
#define SECONDSPERHOUR   (cyg_uint32)(SECONDSPERMINUTE * 60)
#define SECONDSPERDAY    (cyg_uint32)(SECONDSPERHOUR * 24)
#define SECONDSPERYEAR   (cyg_uint32)(SECONDSPERDAY * 365)

/* Convert a SNTP timestamp to a time_t. timestamps are seconds from
   1/1/1900. time_t is 1/1/1970.  That equates to 70 years, or which
   17 were leap years.*/
static time_t timestamp2time(cyg_uint32 timestamp) {
  
  return timestamp - 
    (SECONDSPERYEAR *  (cyg_uint32)(70)) -
    (SECONDSPERDAY * (cyg_uint32)(17));
}

/* Is the new server better than the current one? If its the same as
   the current, its always better. If the stratum is lower its better.
   If we have not heard from the old server for more than 10 minutes,
   the new server is better. */

static int is_better(struct sntp_srv_s *newer, struct sntp_srv_s *old) {
   
  time_t last_time, diff;
  
  if (!memcmp(&newer->addr, &old->addr, newer->addr.sa_len)) return 1;
  if (newer->stratum < old->stratum) return 1;

  if (old->timestamp != 0xffffffff) {
    last_time = timestamp2time(old->timestamp);
  
    diff = time(NULL) - last_time;
    if (diff > 600) return 1;
    
    return 0;
  }
  return 1;
}

const struct in6_addr in6addr_ntp_multicast = IN6ADDR_NTP_MULTICAST;

static void sntp_fn(cyg_addrword_t data) {
  int fd;
  int ret;
  struct sockaddr_in local;
  struct servent *serv;
  cyg_uint32 buf[12];
  struct sntp_srv_s new_srv;
  struct sntp_srv_s best_srv;
  int mode;
  int len;
  time_t new_time, current_time, diff;
  fd_set readfds;
  int n;
#ifdef CYGPKG_NET_INET6
  int fd6 = -1;
  struct ipv6_mreq mreq;
  struct sockaddr_in6 local6;
#endif

  memset(&best_srv,0xff,sizeof(best_srv));

  fd = socket(AF_INET,SOCK_DGRAM,0);
  CYG_ASSERT(-1 != fd,"Failed to open socket");

  serv = getservbyname("ntp","udp");
  CYG_ASSERT(serv,"getservbyname(sntp,udp)");

  memset(&local,0,sizeof(local));
  local.sin_family = AF_INET;
  local.sin_len = sizeof(local);
  local.sin_port = serv->s_port;
  local.sin_addr.s_addr = INADDR_ANY;

  ret=bind(fd,(struct sockaddr *)&local,sizeof(local));
  CYG_ASSERT(0 == ret, "Bind failed");

  n = fd;

#ifdef CYGPKG_NET_INET6
  fd6 = socket(AF_INET6, SOCK_DGRAM,0);
  CYG_ASSERT(-1 != fd,"Failed to open socket");
  mreq.ipv6mr_multiaddr = in6addr_ntp_multicast;
  mreq.ipv6mr_interface = 0;

  // Node-Local
  ret = setsockopt(fd6, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
  CYG_ASSERT(0 == ret, "setsockopt(IPV6_JOIN_GROUP) Node-Local");

#ifdef CYGHWR_NET_DRIVER_ETH0
  // Link-Local
  mreq.ipv6mr_multiaddr.s6_addr[1]=0x02;
  mreq.ipv6mr_interface = if_nametoindex("eth0");
  if (mreq.ipv6mr_interface != 0 ) {
    ret = setsockopt(fd6, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
    CYG_ASSERT(0 == ret, "setsockopt(IPV6_JOIN_GROUP) Link-Local eth0");
  }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
  // Link-Local
  mreq.ipv6mr_multiaddr.s6_addr[1]=0x02;
  mreq.ipv6mr_interface = if_nametoindex("eth1");
  if (mreq.ipv6mr_interface != 0 ) {
    ret = setsockopt(fd6, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
    CYG_ASSERT(0 == ret, "setsockopt(IPV6_JOIN_GROUP) Link-Local eth1");
  }
#endif

  // Site-Local
  mreq.ipv6mr_multiaddr.s6_addr[1]=0x05;
  ret = setsockopt(fd6, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
  CYG_ASSERT(0 == ret, "setsockopt(IPV6_JOIN_GROUP) Site-Local");

  // Global
  mreq.ipv6mr_multiaddr.s6_addr[1]=0x0e;
  ret = setsockopt(fd6, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
  CYG_ASSERT(0 == ret, "setsockopt(IPV6_JOIN_GROUP) Global");
  
  memset(&local6,0,sizeof(local6));
  local6.sin6_family = AF_INET6;
  local6.sin6_len = sizeof(local6);
  local6.sin6_port = serv->s_port;
  local6.sin6_addr = in6addr_any;
  
  ret = bind(fd6, (struct sockaddr *)&local6,sizeof(local6));
  CYG_ASSERT(0 == ret, "Bind6 failed");
  
  n = (n > fd6 ? n : fd6);
#endif

  while (1) {
    FD_ZERO(&readfds);
    FD_SET(fd,&readfds);
#ifdef CYGPKG_NET_INET6
    FD_SET(fd6,&readfds);
#endif
    
    ret = select(n+1, &readfds,NULL,NULL,NULL);
    CYG_ASSERT(-1 != ret, "Select");

    len = sizeof(new_srv.addr);
    if (FD_ISSET(fd,&readfds)) {
      ret=recvfrom(fd,buf,sizeof(buf),0,(struct sockaddr *)&new_srv.addr,&len);
    }
#ifdef CYGPKG_NET_INET6
    if (FD_ISSET(fd6,&readfds)) {
      ret=recvfrom(fd6,buf,sizeof(buf),0,(struct sockaddr *)&new_srv.addr,&len);
    }
#endif
    CYG_ASSERT(0 < ret,"recvfrom");
      
    /* We expect at least enough bytes to fill the buffer. There could
       be more if there is a digest, but we ignore that. */
    if (ret < sizeof(buf))
      continue;
    
    new_srv.version = (htonl(buf[0]) & VERSION_MASK) >> VERSION_SHIFT;
    new_srv.stratum = (htonl(buf[0]) & STRATUM_MASK) >> STRATUM_SHIFT;
    new_srv.timestamp = htonl(buf[10]);
    mode = (htonl(buf[0]) & MODE_MASK) >> MODE_SHIFT;
    
    /* Only support protocol versions 3 or 4 */
    if (new_srv.version < 3 || new_srv.version > 4) {
      CYG_TRACE1(1, "Unsupported version of NTP. Version %d",new_srv.version);
      continue;
    }
    
    /* Only process broadcast packet */
    if (mode != MODE_BROADCAST) 
      continue;
    
    /* Is the packet from a better server than our current one */
    if (is_better(&new_srv,&best_srv)) {
      best_srv = new_srv;

      /* Work out the difference between server and our time */
      new_time = timestamp2time(best_srv.timestamp);
      current_time = time(NULL);
      diff = current_time - new_time;
      
      if (diff < 0) 
	diff = -diff;
      
      if (diff > 2) 
	cyg_libc_time_settime(new_time);
    }
  }
}

/* Start the SNTP server */
void cyg_sntp_start(void) {
  
  static char sntp_stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
  static cyg_thread sntp_thread_data;
  static cyg_handle_t sntp_handle;

  cyg_thread_create(CYGPKG_NET_THREAD_PRIORITY+1, 
		    sntp_fn,               // entry
		    0,                     // entry parameter
		    "sntp",                // Name
		    &sntp_stack,           // stack
		    sizeof(sntp_stack),    // stack size
		    &sntp_handle,          // Handle
		    &sntp_thread_data);    // Thread data structure

  cyg_thread_resume(sntp_handle);
}
