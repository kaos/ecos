#ifndef CYGONCE_PKGCONF_WATCHDOG_H
#define CYGONCE_PKGCONF_WATCHDOG_H
// ====================================================================
//
//      pkgconf/watchdog.h
//
//      Watchdog configuration file
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-07-29      
// Purpose:     To allow the user to edit watchdog config options.
// Description:
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/kernel.h>             // kernel configuration file

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES_WATCHDOG {
       display  "Watchdog device"
       requires CYGVAR_KERNEL_COUNTERS_CLOCK
       description "
           The watchdog device allows applications to make use of a
           timer facility. First the application should register an
           action routine with the watchdog device, and start the
           watchdog running. After this the application must call a
           watchdog reset function at regular intervals, or else the
           device will cause the installed action routine to be
           invoked. The assumption is that the watchdog timer should
           never trigger unless there has been a serious fault in
           either the hardware or the software, and the application's
           action routine should perform an appropriate reset
           operation."
       doc ref/ecos-ref/p-cygpkg-devices-watchdog.html
   }

   }}CFG_DATA */
   
/* ---------------------------------------------------------------------
 * Decide whether we are using the emulated watchdog or a hardware
 * supplied device.
 */


#define CYGIMP_WATCHDOG_EMULATE

#if defined(CYGPKG_HAL_MN10300) && !defined(CYGPKG_HAL_MN10300_AM31_SIM)
#undef CYGIMP_WATCHDOG_EMULATE
#endif


#if defined(CYGPKG_HAL_ARM_AEB)
#undef CYGIMP_WATCHDOG_EMULATE
#endif



/* -------------------------------------------------------------------- */
#endif  /* CYGONCE_PKGCONF_WATCHDOG_H */
/* EOF watchdog.h */
