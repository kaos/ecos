#ifndef CYGONCE_PKGCONF_GDB_1_H
#define CYGONCE_PKGCONF_GDB_1_H
// ====================================================================
//
//	pkgconf/gdb_1.h
//
//	Device configuration file for gdb 1
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
// Purpose:	To allow the user to edit device specific configuration
//		for the gdb 1 pseudo device.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES_GDB {
       display  "GDB pseudo device"
       parent   CYGPKG_DEVICES
       requires CYGPKG_DEVICES_SERIAL_RS232_MN10300_2
       description "
           If the gdb source level debugger is going to be used to
           debug the application then on some target systems this will
           involve the exclusive use of some hardware, for example a
           serial line. Hence this device cannot be used for other
           purposes, for example C library standard I/O. If the gdb
           pseudo device is enabled then this provides a means of
           performing output operations encapsulated within the gdb
           protocol, thus allowing a single serial line or similar
           device to be used for both debugging and for output. Note
           that currently GDB does not support input operations. This
           package is only supported on the MN10300."
   }

   }}CFG_DATA */

/* ==================================================================== 
 * First we define all the options that are exportable to the user. 
 * These options will show/hide various methods for the class. 
 * These options are always device specific.
 */

#ifdef CYG_DEVICE_INTERNAL
/* ==================================================================== 
 * Now define internal only options. These options dictate the struture
 * of a class but have no effect on the methods declared for a class.
 */

/* ---------------------------------------------------------------------
 * Define the number of bytes to convert and output in to the base driver.
 * This number will result in a buffer which is SIZE * 2 + 5 bytes.
 */
# define CYGNUM_DEVICE_GDB_BUFFER_SIZE			100

/* ====================================================================
 * Duplicate the device specific options to device generic options. 
 * DO NOT modify these options. Modify the corresponding options above.
 * This is so the internal source files can be compiled from the same
 * common sources. These defines can never be exported to the user.
 */

#endif // CYG_DEVICE_INTERNAL

#endif  /* CYGONCE_PKGCONF_GDB_1_H */
/* EOF gdb_1.h */
