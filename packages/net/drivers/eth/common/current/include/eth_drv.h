//==========================================================================
//
//      include/eth_drv.h
//
//      High level networking driver interfaces
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

// Ethernet driver structure

#ifndef _ETH_DRV_H_
#define _ETH_DRV_H_

#include <sys/param.h>
#include <sys/socket.h>

#include <net/if.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#endif

#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

struct eth_drv_sg {
    CYG_ADDRESS  buf;
    CYG_ADDRWORD len;
};
#define MAX_ETH_DRV_SG 8

struct eth_drv_sc;

struct eth_drv_funs {
    // Initialize hardware (including startup)
    void (*start)(struct eth_drv_sc *sc,
                  unsigned char *enaddr,
                  int flags);
    // Shut down hardware
    void (*stop)(struct eth_drv_sc *sc);
    // Device control (ioctl pass-thru)
    int  (*control)(struct eth_drv_sc *sc,
                    unsigned long key,
                    void *data,
                    int   data_length);
    // Query - can a packet be sent?
    int  (*can_send)(struct eth_drv_sc *sc);
    // Send a packet of data
    void (*send)(struct eth_drv_sc *sc,
                 struct eth_drv_sg *sg_list,
                 int sg_len,
                 int total_len,
                 unsigned long key);
    // Receive [unload] a packet of data
    void (*recv)(struct eth_drv_sc *sc,
                 struct eth_drv_sg *sg_list,
                 int sg_len);
};

struct eth_drv_sc {
    struct eth_drv_funs *funs;
    void                *driver_private;
    const char          *dev_name;
    struct arpcom       sc_arpcom; /* ethernet common */
};

#define ETH_DRV_SC(sc,priv,name,start,stop,control,can_send,send,recv) \
static void start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags); \
static void stop(struct eth_drv_sc *sc); \
static int  control(struct eth_drv_sc *sc, unsigned long key, void *data, int data_length); \
static int  can_send(struct eth_drv_sc *sc); \
static void send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, int total, unsigned long key); \
static void recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len); \
static struct eth_drv_funs sc##_funs = {start, stop, control, can_send, send, recv}; \
struct eth_drv_sc sc = {&sc##_funs, priv, name};

void eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr);
void eth_drv_recv(struct eth_drv_sc *sc, int total_len);
void eth_drv_tx_done(struct eth_drv_sc *sc, unsigned long key, int status);

// Control 'key's
#define ETH_DRV_SET_MAC_ADDRESS 0x0100

#endif // _ETH_DRV_H_
