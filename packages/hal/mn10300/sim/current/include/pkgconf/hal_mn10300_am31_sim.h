#ifndef CYGONCE_PKGCONF_HAL_MN10300_AM31_SIM_H
#define CYGONCE_PKGCONF_HAL_MN10300_AM31_SIM_H
// ====================================================================
//
//      pkgconf/hal_mn10300_am31_sim.h
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
// Author(s):           bartv
// Contributors:        bartv
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_MN10300_AM31_SIM {
       display  "AM31 minimal simulator"
       type     radio
       parent   CYGPKG_HAL_MN10300
       requires CYGPKG_HAL_MN10300_AM31
       platform sim
       description "
           The minimal simulator HAL package is provided for use when
           only a simple simulation of the processor architecture is
           desired, as opposed to detailed simulation of any specific
           board. In particular it is not possible to simulate any of
           the I/O devices, so device drivers cannot be used."
       }
   
   }}CFG_DATA */

/* -------------------------------------------------------------------*/


// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        9999

#endif  /* CYGONCE_PKGCONF_HAL_MN10300_AM31_SIM_H */
/* EOF hal_mn10300_am31_sim.h */
