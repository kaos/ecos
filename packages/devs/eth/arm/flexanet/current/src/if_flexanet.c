//==========================================================================
//
//      devs/eth/arm/flexanet/if_flexanet.c
//
//      Ethernet device driver for Flexanet using SMSC LAN91C96
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
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
// Author(s):    Jordi Colomer <jco@ict.es>
// Contributors: jco, 
// Date:         2001-07-01
// Purpose:      
// Description:  hardware driver for Flexanet/SMSC LAN91CXX ethernet
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_flexanet.h>
#if defined(CYGPKG_REDBOOT)
#include <pkgconf/redboot.h>
#endif

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>


// ESA (Ethernet Station Address), when constant
#ifndef CYGSEM_DEVS_ETH_ARM_FLEXANET_REDBOOT_ESA
static unsigned char static_esa[] = CYGDAT_DEVS_ETH_ARM_FLEXANET_ESA;
#endif



