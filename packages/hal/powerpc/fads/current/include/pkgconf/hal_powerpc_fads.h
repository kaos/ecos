#ifndef CYGONCE_PKGCONF_HAL_POWERPC_FADS_H
#define CYGONCE_PKGCONF_HAL_POWERPC_FADS_H
//=============================================================================
//
//      pkgconf/hal_powerpc_fads.h
//
//      HAL configuration file
//
//=============================================================================
// ####UNSUPPORTEDBEGIN####
//
// -------------------------------------------
// This source file has been contributed to eCos/Red Hat. It may have been
// changed slightly to provide an interface consistent with those of other 
// files.
//
// The functionality and contents of this file is supplied "AS IS"
// without any form of support and will not necessarily be kept up
// to date by Red Hat.
//
// All inquiries about this file, or the functionality provided by it,
// should be directed to the 'ecos-discuss' mailing list (see
// http://sourceware.cygnus.com/ecos/intouch.html for details).
//
// Maintained by:  <Unmaintained>
// -------------------------------------------
//
// ####UNSUPPORTEDEND####
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           bartv
// Contributors:        bartv, khester
// Date:                1998-09-02      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
//=============================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_POWERPC_FADS {
       display  "Motorola FADS PowerPC evaluation board"
       type     radio
       parent   CYGPKG_HAL_POWERPC
       platform fads
       description "
           The FADS HAL package provides the support needed to run
           eCos on a Motorola FADS board equipped with a PowerPC processor."
   }

   cdl_option CYGHWR_HAL_POWERPC_FADS_STARTUP {
       display          "Startup type"
       parent           CYGPKG_HAL_POWERPC_FADS
       #type             count
       type             enum
       legal_values     ram rom
       startup
       description      "
           When targetting the FADS board for RAM bootstrap it is expected 
           that the image will be downloaded via the Motorola MPC8BUG tool."
   }

   }}CFG_DATA */

#define CYGHWR_HAL_POWERPC_FADS_STARTUP       ram

// Real-time clock/counter specifics

// Period is busclock/16/100.
#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        20833

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_POWERPC_FADS_H */
/* EOF hal_powerpc_FADS.h */
