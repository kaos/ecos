#ifndef CYGONCE_PKGCONF_SERIAL_MN10300_H
#define CYGONCE_PKGCONF_SERIAL_MN10300_H
// ====================================================================
//
//	pkgconf/serial_mn10300.h
//
//	Device configuration file for mn10300 serial devices
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
// Author(s): 	        bartv
// Contributors:	bartv
// Date:	        1998-04-24	
// Purpose:	        To allow the user to edit device configuration
//		        options for the mn10300 serial devices
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/system.h>

/* Only define options if the driver is to be present. */
#if defined( CYGPKG_HAL_MN10300 ) && \
   !defined( CYGPKG_HAL_MN10300_SIM ) && \
    defined( CYGPKG_DEVICES_SERIAL_RS232_MN10300 )

/* ----------------------------------------------------------------------------
  {{CFG_DATA

  cdl_package CYGPKG_DEVICES_SERIAL_RS232_MN10300 {
      display  "MN10300 serial devices"
      parent   CYGPKG_DEVICES_SERIAL_RS232_COMMON
#      requires CYGPKG_HAL_MN10300_STDEVAL1
      requires CYGPKG_KERNEL
      description "
          Drivers for the on-chip RS232 devices."
  }

  cdl_component CYGPKG_DEVICES_SERIAL_RS232_MN10300_1 {
      display  "On-chip serial device 1"
      parent   CYGPKG_DEVICES_SERIAL_RS232_MN10300
      requires CYGPKG_KERNEL
      description "
          Driver for the first on-chip RS232 device."
  }

  cdl_component CYGPKG_DEVICES_SERIAL_RS232_MN10300_2 {
      display  "On-chip serial device 2"
      parent   CYGPKG_DEVICES_SERIAL_RS232_MN10300
      requires CYGPKG_KERNEL
      description "
          Driver for the second on-chip RS232 device."
  }

  }}CFG_DATA
*/

#define CYGPKG_DEVICES_SERIAL_RS232_MN10300_1
#define CYGPKG_DEVICES_SERIAL_RS232_MN10300_2

#endif /* ifdef CYGPKG_DEVICES_SERIAL_RS232_MN10300 et al */

#endif  /* CYGONCE_PKGCONF_SERIAL_MN10300_H */
/* EOF serial_mn10300.h */
