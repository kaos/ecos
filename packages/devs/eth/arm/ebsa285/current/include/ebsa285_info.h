#ifndef CYGONCE_DEVS_ETH_ARM_EBSA285_EBSA285_INFO_H
#define CYGONCE_DEVS_ETH_ARM_EBSA285_EBSA285_INFO_H
/*==========================================================================
//
//        ebsa285_info.h
//
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
// Author(s):     hmt
// Contributors:  hmt
// Date:          2000-05-03
// Description:
//
//####DESCRIPTIONEND####
*/

#include <pkgconf/devs_eth_arm_ebsa285.h>

#ifdef CYGDBG_DEVS_ETH_ARM_EBSA285_KEEP_STATISTICS
# define KEEP_STATISTICS
# define nDISPLAY_STATISTICS
# define nDISPLAY_82559_STATISTICS
#else
# define nKEEP_STATISTICS
# define nDISPLAY_STATISTICS
# define nDISPLAY_82559_STATISTICS
#endif


// ------------------------------------------------------------------------
//
//                       STATISTICAL COUNTER STRUCTURE
//
// ------------------------------------------------------------------------
#ifdef KEEP_STATISTICS
typedef struct {
/*  0 */    cyg_uint32 tx_good;
/*  4 */    cyg_uint32 tx_max_collisions;
/*  8 */    cyg_uint32 tx_late_collisions;
/* 12 */    cyg_uint32 tx_underrun;
/* 16 */    cyg_uint32 tx_carrier_loss;
/* 20 */    cyg_uint32 tx_deferred;
/* 24 */    cyg_uint32 tx_single_collisions;
/* 28 */    cyg_uint32 tx_mult_collisions;
/* 32 */    cyg_uint32 tx_total_collisions;
/* 36 */    cyg_uint32 rx_good;
/* 40 */    cyg_uint32 rx_crc_errors;
/* 44 */    cyg_uint32 rx_align_errors;
/* 48 */    cyg_uint32 rx_resource_errors;
/* 52 */    cyg_uint32 rx_overrun_errors;
/* 56 */    cyg_uint32 rx_collisions; // Always 0
/* 60 */    cyg_uint32 rx_short_frames;
// In this setup; can also be flow-control counts after.
// If these are to be used, a config command (as in set promiscuous mode)
// must be issued at start, to let those stats escape.  Params are in
// comments around the config command setup...
/* 64 */    cyg_uint32 done;
} I82559_COUNTERS;


typedef struct {
    cyg_uint32 interrupts;
    cyg_uint32 rx_count;
    cyg_uint32 rx_deliver;
    cyg_uint32 rx_resource;
    cyg_uint32 rx_restart;
    cyg_uint32 tx_count;
    cyg_uint32 tx_complete;
    cyg_uint32 tx_dropped;
} STATISTICS;


extern STATISTICS statistics[2];
#ifdef CYGDBG_DEVS_ETH_ARM_EBSA285_KEEP_82559_STATISTICS
extern I82559_COUNTERS i82559_counters[2];
#endif

#endif // KEEP_STATISTICS

// ------------------------------------------------------------------------
//
//                      DEVICES AND PACKET QUEUES
//
// ------------------------------------------------------------------------
// The system seems to work OK with as few as 8 of RX and TX descriptors.
// It limps very painfully with only 4.
// Performance is better with more than 8.
// But the size of non-cached (so useless for anything else)
// memory window is 1Mb, so we might as well use it all.
//
// 128 for these uses the whole 1Mb, near enough.

#ifndef MAX_RX_DESCRIPTORS
#define MAX_RX_DESCRIPTORS	128     // number of Rx descriptors
#endif
#ifndef MAX_TX_DESCRIPTORS
#define MAX_TX_DESCRIPTORS	128     // number of Tx descriptors
#endif


typedef struct i82559 {
    cyg_uint8                           // (split up for atomic byte access)
        found:1,                        // was hardware discovered?
        mac_addr_ok:1,                  // can we bring up?
        active:1,                       // has this if been brung up?
        spare1:5; 
    cyg_uint8
        spare2:8; 
    cyg_uint8
        tx_in_progress:1,               // transmit in progress flag
        tx_queue_full:1,                // all Tx descriptors used flag
        spare3:6; 
    cyg_uint8  index;                   // 0 or 1 or whatever
    cyg_uint32 devid;                   // PCI device id
    cyg_uint32 memory_address;          // PCI memory address
    cyg_uint32 io_address;              // memory mapped I/O address
    cyg_uint8  mac_address[6];          // mac (hardware) address
    void *ndp;                          // Network Device Pointer

    int next_rx_descriptor;             // descriptor index for RFDs
    struct rfd *rx_ring[MAX_RX_DESCRIPTORS]; // location of Rx descriptors

    int tx_descriptor_add;              // descriptor index for additions
    int tx_descriptor_active;           // descriptor index for active tx
    int tx_descriptor_remove;           // descriptor index for remove

    struct txcb *tx_ring[MAX_TX_DESCRIPTORS];  // location of Tx descriptors
    unsigned long tx_keys[MAX_TX_DESCRIPTORS];
                                        // keys for tx q management

    // Interrupt handling stuff
    cyg_vector_t    vector;             // interrupt vector
    cyg_handle_t    interrupt_handle;   // handle for int.handler
    cyg_interrupt   interrupt_object;

#ifdef KEEP_STATISTICS
    void *p_statistics;                 // pointer to statistical counters
#endif

} I82559;



// ------------------------------------------------------------------------
//
//                   82559 GENERAL STATUS REGISTER
//
// ------------------------------------------------------------------------
#define GEN_STATUS_FDX          0x04    // 1 = full duplex, 0 = half
#define GEN_STATUS_100MBPS      0x02    // 1 = 100 Mbps, 0 = 10 Mbps
#define GEN_STATUS_LINK         0x01    // 1 = link up, 0 = link down

extern int i82559_status( struct eth_drv_sc *sc );

// ------------------------------------------------------------------------

#ifdef KEEP_STATISTICS
void update_statistics(struct i82559* p_i82559);
#endif


#ifdef CYGDBG_DEVS_ETH_ARM_EBSA285_KEEP_82559_STATISTICS
#define ETH_STATS_INIT( p ) \
        update_statistics( (struct i82559 *)((p)->driver_private) )
#else
#define ETH_STATS_INIT( p ) // otherwise do nothing
#endif

#define CYGDAT_DEVS_ETH_DESCRIPTION "Intel EtherPRO 10/100+ (i82559)"

#define ETH_DEV_DOT3STATSETHERCHIPSET 1,3,6,1,2,1,10,7,8,2,5

#endif /* ifndef CYGONCE_DEVS_ETH_ARM_EBSA285_EBSA285_INFO_H */

/* EOF ebsa285_info.h */

