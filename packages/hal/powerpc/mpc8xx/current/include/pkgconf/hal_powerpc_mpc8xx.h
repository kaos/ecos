#ifndef CYGONCE_PKGCONF_HAL_POWERPC_MPC8XX_H
#define CYGONCE_PKGCONF_HAL_POWERPC_MPC8XX_H
// ====================================================================
//
//      pkgconf/hal_powerpc_mpc8xx.h
//
//      HAL configuration file
//
// ====================================================================
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           jskov
// Contributors:        jskov
// Date:                2000-02-04      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/hal_powerpc.h>
#include CYGBLD_HAL_PLATFORM_H

/* -------------------------------------------------------------------
 * Decide which variant of the PowerPC family we have.

 {{CFG_DATA

   cdl_package CYGPKG_HAL_POWERPC_MPC8xx {
       display "PowerPC variant"
       type    radio
       parent  CYGFUN_HAL_POWERPC_VARIANTS
       target  powerpc_mpc8xx
       description "
           The MPC8xx architecture HAL package provides generic support
           for this processor architecture. It is also necessary to
           select a specific target platform HAL package."
   }
 
   cdl_option CYGPKG_HAL_POWERPC_MPC860 {
       display "MPC860 microprocessor"
       parent  CYGPKG_HAL_POWERPC_MPC8xx
       type    radio
       description "
           The MPC860 microprocessor. This is an embedded part that in
           addition to the PowerPC processor core has built in peripherals
           such as memory controllers, DMA controllers, serial ports and
           timers/counters."               
  }

  cdl_component CYGSEM_HAL_POWERPC_MPC860_CPM_ENABLE {
       display       "Enable CPM interrupts"
       parent        CYGPKG_HAL_POWERPC_MPC860
       type          boolean
       description   "
           This option causes the CPM interrupt arbiter to be attached
           at startup, and CPM interrupts are enabled. Enabling CPM
           level interrupt arbitration and handling must still be
           done by the application code. See intr0.c test for an
           example."
  }

  cdl_option CYGHWR_HAL_POWERPC_MPC860_CPM_LVL {
       display       "CPM interrupt level on the SIU"
       type          count
       legal_values  0 to 7
       parent        CYGSEM_HAL_POWERPC_MPC860_CPM_ENABLE
       description   "
           This option selects which SIU level the CPM interrupts
           should be routed to."
   }

 }}CFG_DATA */

#define CYGPKG_HAL_POWERPC_MPC860

// -------------------------------------------------------------------
// Characterize the CPU so that the generic architecture part can
// configure itself.

#undef  CYGHWR_HAL_POWERPC_FPU          // We have no FPU
#define CYGPKG_HAL_POWERPC_MSBFIRST     // Big-endian CPU
#define CYGSEM_HAL_USE_ROM_MONITOR

// CPM options
#define CYGSEM_HAL_POWERPC_MPC860_CPM_ENABLE
#define CYGHWR_HAL_POWERPC_MPC860_CPM_LVL 7

//----------------------------------------------------------------------
#endif  // CYGONCE_PKGCONF_HAL_POWERPC_MPC8XX_H
// EOF hal_powerpc_mpc8xx.h
