#ifndef CYGONCE_PKGCONF_HAL_POWERPC_PPC60X_H
#define CYGONCE_PKGCONF_HAL_POWERPC_PPC60X_H
// ====================================================================
//
//      pkgconf/hal_powerpc_ppc60x.h
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

   cdl_package CYGPKG_HAL_POWERPC_PPC60X {
       display "PowerPC variant"
       type    radio
       parent  CYGFUN_HAL_POWERPC_VARIANTS
       target  powerpc_ppc60x
       description "
           The PPC60x architecture HAL package provides generic support
           for this processor architecture. It is also necessary to
           select a specific target platform HAL package."
   }
 
   cdl_option CYGPKG_HAL_POWERPC_PPC603 {
       display "PPC603 microprocessor"
       parent  CYGPKG_HAL_POWERPC_PPC60X
       type    radio
       description "
           The PPC603 microprocessor. "
   }

 }}CFG_DATA */

#define CYGPKG_HAL_POWERPC_PPC603

// -------------------------------------------------------------------
// Characterize the CPU so that the generic architecture part can
// configure itself.

#undef  CYGHWR_HAL_POWERPC_FPU          // We have no FPU
#define CYGPKG_HAL_POWERPC_MSBFIRST     // Big-endian CPU
#undef  CYGSEM_HAL_USE_ROM_MONITOR

//----------------------------------------------------------------------
#endif  // CYGONCE_PKGCONF_HAL_POWERPC_PPC60X_H
// EOF hal_powerpc_ppc60x.h
