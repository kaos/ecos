#ifndef CYGONCE_PKGCONF_WALLCLOCK_H
#define CYGONCE_PKGCONF_WALLCLOCK_H
// ====================================================================
//
//	pkgconf/wallclock.h
//
//	Wallclock configuration file
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-07-29	
// Purpose:	To allow the user to edit wallclock config options.
// Description:
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/kernel.h>             // kernel configuration file

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES_WALLCLOCK {
       display  "Wallclock device"
       parent   CYGPKG_DEVICES
       requires CYGVAR_KERNEL_COUNTERS_CLOCK
       description "
           The wallclock device provides real time stamps, as opposed
           to the eCos kernel timers which typically just count the
           number of clock ticks since the hardware was powered up.
           Depending on the target platform this device may involve
           interacting with a suitable clock chip, or it may be
           emulated by using the kernel timers.
       "
   }

   }}CFG_DATA */

/* ---------------------------------------------------------------------
 * Decide whether we are using the emulated wallclock or a hardware
 * supplied device.
 */

#define CYGIMP_WALLCLOCK_EMULATE

/* -------------------------------------------------------------------- */
#endif  /* CYGONCE_PKGCONF_WALLCLOCK_H */
/* EOF wallclock.h */
