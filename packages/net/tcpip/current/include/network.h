//==========================================================================
//
//      include/network.h
//
//      Misc network support
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

#ifndef _NETWORK_H_
#define _NETWORK_H_

#define NO_LIBKERN_INLINE  // Avoid kernel inline functions

#include <pkgconf/net.h>
#undef _KERNEL
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/route.h>

#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>

#include <netdb.h>
#include <bootp.h>

#ifdef CYGHWR_NET_DRIVER_ETH0
extern struct bootp eth0_bootp_data;
extern cyg_bool_t   eth0_up;
extern const char  *eth0_name;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
extern struct bootp eth1_bootp_data;
extern cyg_bool_t   eth1_up;
extern const char  *eth1_name;
#endif

extern void init_all_network_interfaces(void);

extern void  route_reinit(void);
extern void  perror(const char *);
extern int   close(int);
extern int   read(int, void *, int);
extern int   write(int, void *, int);
extern char *inet_ntoa(struct in_addr);
extern int   select(int, fd_set *, fd_set *, fd_set *, struct timeval *tv);

#endif // _NETWORK_H_
