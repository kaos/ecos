#ifndef CYGONCE_IO_ETH_ETH_DRV_STATS_H
#define CYGONCE_IO_ETH_ETH_DRV_STATS_H
//==========================================================================
//
//      include/eth_drv_stats.h
//
//      High level networking driver interfaces - statistics gathering
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
// Author(s):    hmt
// Contributors: hmt
// Date:         2000-08-23
// Purpose:      
// Description:  High level networking driver interfaces - stats gathering
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#ifdef CYGPKG_NET
#include <sys/param.h>
#include <sys/socket.h>

#include <net/if.h>

// This information is oriented towards SNMP's needs.

#define DESC_LEN (48)
#define SNMP_CHIPSET_LEN (20)

struct ether_drv_stats {
    struct ifreq ifreq;                 // tell ioctl() which interface.

    char description[ DESC_LEN ];       // Textual description of hardware
    unsigned char snmp_chipset[ SNMP_CHIPSET_LEN ];
                                        // SNMP ID of chipset
    unsigned char duplex;               // 1 = UNKNOWN, 2 = SIMPLEX, 3 = DUPLEX
    unsigned char operational;          // 1 = UNKNOWN, 2 = DOWN, 3 = UP
    // These are general status information:
    unsigned int speed;                 // 10,000,000 or 100,000,000
                                        //     to infinity and beyond?

    // These are typically kept by device hardware - and there may be some
    // cost for getting up to date values:

    unsigned int supports_dot3;  /* Boolean value if the device supports dot3*/
    unsigned int tx_good;
    unsigned int tx_max_collisions;
    unsigned int tx_late_collisions;
    unsigned int tx_underrun;
    unsigned int tx_carrier_loss;
    unsigned int tx_deferred;
    unsigned int tx_sqetesterrors;
    unsigned int tx_single_collisions;
    unsigned int tx_mult_collisions;
    unsigned int tx_total_collisions;
    unsigned int rx_good;
    unsigned int rx_crc_errors;
    unsigned int rx_align_errors;
    unsigned int rx_resource_errors;
    unsigned int rx_overrun_errors;
    unsigned int rx_collisions;
    unsigned int rx_short_frames;
    unsigned int rx_too_long_frames;
    unsigned int rx_symbol_errors;

    // These are typically kept by driver software:
    unsigned int interrupts;
    unsigned int rx_count;
    unsigned int rx_deliver;
    unsigned int rx_resource;
    unsigned int rx_restart;
    unsigned int tx_queue_len;
    unsigned int tx_count;
    unsigned int tx_complete;
    unsigned int tx_dropped;

    // Add any others here...

};
#endif // CYGPKG_NET
#endif // CYGONCE_IO_ETH_ETH_DRV_STATS_H

// EOF include/eth_drv_stats.h
