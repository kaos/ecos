#ifndef CYGONCE_PKGCONF_HAL_I386_LINUX_H
#define CYGONCE_PKGCONF_HAL_I386_LINUX_H
// ====================================================================
//
//      pkgconf/hal_i386_linux.h
//
//      HAL configuration file
//
// ====================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           jskov
// Contributors:        jskov
// Date:                1999-01-12      
// Purpose:             To allow the user to edit HAL configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* ---------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_HAL_I386_LINUX {
       display  "Linux i386 synthetic target"
       type     radio
       parent   CYGPKG_HAL_I386
       platform linux
       description "
           The Linux i386 Synthetic Target HAL package provides the 
           support needed to run eCos binaries on top of the i386
           Linux kernel."
   }

   cdl_option CYGSEM_HAL_I386_LINUX_REAL_TIME {
       display          "Use Linux real-time timer"
       parent           CYGPKG_HAL_I386_LINUX
       description      "
           Using the real-time timer to drive the eCos RTC allows the
           eCos idle loop to call select(2) and thus not put any load
           on the host machine.
           Deselecting this option means the virtual timer will be used
           instead. This may prevent (to some degree) time skew in the
           eCos kernel, especially if the host machine has a high load."
   }

   }}CFG_DATA */

#define CYGHWR_HAL_I386_LINUX_STARTUP       ram
#define CYGSEM_HAL_I386_LINUX_REAL_TIME

// Real-time clock/counter specifics

#define CYGNUM_HAL_RTC_NUMERATOR     1000000000
#define CYGNUM_HAL_RTC_DENOMINATOR   100
#define CYGNUM_HAL_RTC_PERIOD        10000

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_I386_LINUX_H */
/* EOF hal_i386_linux.h */
