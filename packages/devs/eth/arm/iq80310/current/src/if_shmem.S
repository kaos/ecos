// #========================================================================
// #
// #    if_shmem.S
// #
// #    Declare a chunk of 4Kbyte aligned memory for use by bus-mastering
// #    PCI ethernet device.
// #
// #========================================================================
// ####COPYRIGHTBEGIN####
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
// ####COPYRIGHTEND####
// #========================================================================
// ######DESCRIPTIONBEGIN####
// #
// # Author(s):     msalter
// # Contributors:  msalter
// # Date:          2000-11-03
// # Purpose:       
// # Description:   This file defines a chunk of aligned memory for use
// #                by bus-mastering PCI ethernet device.
// #
// #####DESCRIPTIONEND####
// #
// #========================================================================

#include <pkgconf/devs_eth_arm_iq80310.h>
        
#define MAX_PACKET_SIZE   1536
#define SIZEOF_DESCRIPTOR 16
	
	.bss
	.p2align(12)
	.globl cyg_io_iq80310_i82559_shmem
cyg_io_iq80310_i82559_shmem:
	.rept ((MAX_PACKET_SIZE + SIZEOF_DESCRIPTOR) * \
               (CYGNUM_DEVS_ETH_ARM_IQ80310_MAX_TX_DESCRIPTORS + \
                CYGNUM_DEVS_ETH_ARM_IQ80310_MAX_RX_DESCRIPTORS))
	.byte 0
	.endr
