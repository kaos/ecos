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
       display  "Linux i386 Synthetic Target"
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
           By default the eCos timer will driven by a virtual timer,
           meaning that it will only tick when the Linux task is
           executing. This prevents (to some degree) time skew in
           the eCos kernel due to heavy load on Linux. By checking
           this option, the eCos timer can be driven by the Linux
           real-time timer instead."
   }

   }}CFG_DATA */

#define CYGHWR_HAL_I386_LINUX_STARTUP       ram
#undef  CYGSEM_HAL_I386_LINUX_REAL_TIME

/* -------------------------------------------------------------------*/
#endif  /* CYGONCE_PKGCONF_HAL_I386_LINUX_H */
/* EOF hal_i386_linux.h */
