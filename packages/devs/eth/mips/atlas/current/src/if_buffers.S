// #========================================================================
// #
// #    if_buffers.S
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
// # Description:   This file defines a chunk of 2Kbyte aligned memory for use
// #                by bus-mastering PCI ethernet device.
// #
// #####DESCRIPTIONEND####
// #
// #========================================================================

#include "saa9730.h"

	.bss
	.p2align(11)
	.globl cyg_io_atlas_2kbuffers
cyg_io_atlas_2kbuffers:
	.rept (SAA9730_RXPKTS_PER_BUFFER + SAA9730_TXPKTS_PER_BUFFER) \
		  * SAA9730_BUFFERS * SAA9730_PACKET_SIZE
	.byte 0
	.endr
