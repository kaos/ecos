#ifndef CYGONCE_PKGCONF_SERIAL_RS232_COMMON_H
#define CYGONCE_PKGCONF_SERIAL_RS232_COMMON_H
// ====================================================================
//
//	pkgconf/serial_rs232_common.h
//
//	Configuration file for shared serial line code.
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
// Date:	        1998-09-03
// Purpose:	        To allow the user to edit configuration
//                      options common to all serial lines.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES_SERIAL_RS232_COMMON {
       display "Serial RS232 devices"
       type bool
       # parent CYGPKG_DEVICES_SERIAL
       parent CYGPKG_DEVICES
       requires CYGPKG_KERNEL
       description "
           Drivers for various RS232 devices."
	   doc ref/ecos-ref/part-iii.html
   }

   }}CFG_DATA
*/   

#endif  /* CYGONCE_PKGCONF_SERIAL_RS232_COMMON_H */
/* EOF serial_rs232_common.h */
