#ifndef CYGONCE_PKGCONF_DEVS_H
#define CYGONCE_PKGCONF_DEVS_H
// ====================================================================
//
//	pkgconf/devs.h
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
// Contributors:	proven
// Date:	1998-04-24	
// Purpose:	To allow the user to edit device configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES {
       display "Device drivers"
       type    bool
       requires CYGPKG_ERROR
       description "
           The eCos system is supplied with a number of different
           device drivers."
	   doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

/* ---------------------------------------------------------------------
 * This file needs to know which HAL and device packages are in the
 * current configuration.
 */

#include <pkgconf/system.h>

/*
 * This #include is spurious but avoids some problems in the current
 * sources.
 */
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>

/* ---------------------------------------------------------------------
 * Define all the potential devices that the kernel can have. Then define
 * the configuration file that contains the specific configuration for 
 * that device. Eventually all these configuration files will be generated
 * by a hardware configuration tool.
 */

#if defined(CYG_HAL_MN10300) && defined(CYGPKG_DEVICES_SERIAL_RS232_MN10300)

# include <pkgconf/serial_mn10300.h>

# ifdef CYGPKG_DEVICES_SERIAL_RS232_MN10300_1
#  define CYG_DEVICE_SERIAL_RS232_MN10300_1	 "pkgconf/serial_mn10300_1.h"
#  define CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME CYG_DEVICE_NAME_1
#  define CYG_DEVICE_SERIAL_RS232_MN10300_1_DECLARE
# endif

# ifdef CYGPKG_DEVICES_SERIAL_RS232_MN10300_2
#  define CYG_DEVICE_SERIAL_RS232_MN10300_2	"pkgconf/serial_mn10300_2.h"
#  define CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME	CYG_DEVICE_NAME_2
// If we are the simulator then we want native output,
// otherwise we want GDB output by default. These
// all depend on the HAL --proven 19980906
#  if defined(CYG_HAL_MN10300_SIM) || !defined(CYGPKG_DEVICES_GDB)
#   define CYG_DEVICE_SERIAL_RS232_MN10300_2_DECLARE
#  endif
# endif

/* 
 * GDB is a pseudo device that wraps a real device. If this package
 * is required default it to the second serial port.
 */
#if defined(CYGPKG_DEVICES_GDB) \
    && defined(CYGPKG_DEVICES_SERIAL_RS232_MN10300_2)
#define CYG_DEVICE_GDB_1		"pkgconf/gdb_1.h"
//#define CYG_DEVICE_GDB_1_NAME		CYG_DEVICE_NAME_2
#ifndef CYG_HAL_MN10300_SIM
#define CYG_DEVICE_GDB_1_NAME		CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME
#endif

// These last two fields I really don't like.
// The should probably go in "pkgconf/gdb_1.h" but I'd rather they
// went away completely --proven 19980630
// Note CYG_DEVICE_GDB_1_CLASS is different than CYG_CLASS_DEVICE_GDB_1
#define CYG_DEVICE_GDB_1_CLASS		CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_2
#define CYG_DEVICE_GDB_1_INCLUDE	"cyg/devs/serial/rs232/mn10300/serial_mn10300_2.hxx"
#endif

#endif  /* CYG_HAL_MN10300 */

#ifdef CYG_HAL_TX39

/* No serial drivers for simulator yet. */
#if defined(CYG_HAL_TX39_JMR3904) && defined(CYGPKG_DEVICES_SERIAL_RS232_TX39)
#define CYG_DEVICE_SERIAL_RS232_TX39		"pkgconf/serial_tx39.h"
#define CYG_DEVICE_SERIAL_RS232_TX39_NAME	CYG_DEVICE_NAME_1
#define CYG_DEVICE_SERIAL_RS232_TX39_DECLARE
#endif
#endif

/* ---------------------------------------------------------------------
 * Define the device that is to be used for outputing diag data. One is
 * required only if CYGDBG_INFRA_DIAG_USE_DEVICE is defined. Define the
 * name in terms of an above device name.
 */
#ifdef CYG_HAL_MN10300
// #ifdef on rom monitor
// #define CYG_DEVICE_DIAG_CLASS		CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_1
// #define CYG_DEVICE_DIAG_NAME			CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME
#define CYG_DEVICE_DIAG_CLASS			CYG_CLASS_DEVICE_GDB_1
#define CYG_DEVICE_DIAG_NAME			CYG_DEVICE_GDB_1_NAME

#endif

#ifdef CYG_HAL_TX39
#define CYG_DEVICE_DIAG_CLASS			CYG_CLASS_DEVICE_SERIAL_RS232_TX39
#define CYG_DEVICE_DIAG_NAME			CYG_DEVICE_SERIAL_RS232_TX39_NAME
#endif

/* ---------------------------------------------------------------------
 * Define the name of the device you wish to test. The test 
 * infrastructure can only handle one.
 */
#define CYGVAR_DEVICE_TEST_NAME			CYG_DEVICE_NAME_2

/* ---------------------------------------------------------------------
 * Define the names of the devices. These must be defined after the 
 * device to device name mappings.
 */
#define CYG_DEVICE_NAME_1			cyg_serial1
#define CYG_DEVICE_NAME_2			cyg_serial2

#endif // ifdef CYGPKG_KERNEL

/* ---------------------------------------------------------------------
 * Define for a device table. Necessary for the C library to access devices.
 */
#define CYG_DEVICE_TABLE

#endif  /* CYGONCE_PKGCONF_DEVS_H */
/* EOF devs.h */
