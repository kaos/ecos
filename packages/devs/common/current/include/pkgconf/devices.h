#ifndef CYGONCE_PKGCONF_DEVICES_H
#define CYGONCE_PKGCONF_DEVICES_H
// ====================================================================
//
//	pkgconf/devices.h
//
//	Device configuration file
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
// Author(s): 	proven
// Contributors:proven
// Date:	1998-04-24	
// Purpose:	Contain #include for all the possible devices for the
//		current configuration. All applications should include
//		this file if they want to use the raw C++ device
//              Eventually this file should be generated automatically.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ---------------------------------------------------------------------
 * Get the device configurations.
 */

#include <pkgconf/devs.h>

/* ---------------------------------------------------------------------
 * Now #include the specific driver include file.
 */

#ifdef __cplusplus

#ifdef CYG_HAL_MN10300
# ifdef CYGPKG_DEVICES_SERIAL_RS232_MN10300_1
#  include <cyg/devs/serial/rs232/mn10300/serial_mn10300_1.hxx>
# endif
# ifdef CYGPKG_DEVICES_SERIAL_RS232_MN10300_2
#  include <cyg/devs/serial/rs232/mn10300/serial_mn10300_2.hxx>
# endif
#endif

#ifdef CYG_HAL_TX39
# ifdef CYGPKG_DEVICES_SERIAL_RS232_TX39
#  include <cyg/devs/serial/rs232/tx39/serial_tx39.hxx>
# endif
#endif

#ifdef CYGPKG_DEVICES_GDB
# include <cyg/devs/gdb/gdb_1.hxx>
#endif

#ifdef CYGPKG_DEVICES_WALLCLOCK
# include <cyg/devs/wallclock.hxx>
#endif

#ifdef CYGPKG_DEVICES_WATCHDOG
# include <cyg/devs/watchdog.hxx>
#endif

#endif  /* __cplusplus */

#endif  /* CYGONCE_PKGCONF_DEVICES_H */
/* EOF devices.h */
