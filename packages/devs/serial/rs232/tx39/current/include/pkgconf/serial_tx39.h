#ifndef CYGONCE_PKGCONF_SERIAL_TX39_H
#define CYGONCE_PKGCONF_SERIAL_TX39_H
// ====================================================================
//
//	pkgconf/serial_tx39.h
//
//	Device configuration file for tx39 serial ports
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
// Author(s): 	        proven
// Contributors:	proven
// Date:	        1998-05-25	
// Purpose:	        To allow the user to edit device specific
//		        configuration options for the tx39 2 serial device.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/system.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

/* Only define options if the driver is to be present. */
#if defined( CYGPKG_HAL_TX39 ) && \
   !defined( CYGPKG_HAL_TX39_SIM ) && \
    defined( CYGPKG_DEVICES_SERIAL_RS232_TX39 )

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_DEVICES_SERIAL_RS232_TX39 {
       display  "TX39 serial device code"
       parent   CYGPKG_DEVICES_SERIAL_RS232_COMMON
#       requires CYGPKG_HAL_TX39_JMR3904
       requires CYGPKG_KERNEL
       description "
           Driver for the on-chip RS232 device."
   }
   
   }}CFG_DATA
*/

/* ---------------------------------------------------------------------
 * First we define all the options that are exportable to the user. These
 * options will show/hide various methods for the class. 
 */

/* ---------------------------------------------------------------------
 * Define what kernel mode the driver can support. One must be defined.
 * Both can be defined. 
 */
#define CYG_DEVICE_SERIAL_RS232_TX39_KMODE_POLLED
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS
#define CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT
#endif

/* ---------------------------------------------------------------------
 * Define if we do diag output. This could possible be another kmode.
 * --proven 19980512
 */
#define CYG_DEVICE_SERIAL_RS232_TX39_KMODE_ASSERT	

/* ---------------------------------------------------------------------
 * Define the default baudrate and line mode.
 */
#define CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE	38400

/* ---------------------------------------------------------------------
 * Define if non binary terminal protocol modes are needed.
 */
#define CYG_DEVICE_SERIAL_RS232_TX39_READ_MODES
#define CYG_DEVICE_SERIAL_RS232_TX39_WRITE_MODES


#ifdef CYG_DEVICE_INTERNAL
/* ==================================================================== 
 * Now define internal only options. These options dictate the struture
 * of a class but have no effect on the methods declared for a class.
 */

/* ---------------------------------------------------------------------
 * The need for queues is only necessary in interrupt mode.
 */
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT

/* ---------------------------------------------------------------------
 * Define the number of iorb read buffers the serial read_isr device can 
 * handle at one time. Also define if the device needs to queue more iorbs
 * than what the read_isr can handle.
 */
# define CYG_DEVICE_SERIAL_RS232_READ_BUFFERS	4
# define CYG_DEVICE_SERIAL_RS232_READ_BUFFERS_LL	 

/* ---------------------------------------------------------------------
 * Define the number of iorb write buffers the serial write_isr device can 
 * handle at one time. Also define if the device needs to queue more iorbs
 * than what the write_isr can handle.
 */
# define CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS	4
# define CYG_DEVICE_SERIAL_RS232_WRITE_BUFFERS_LL	

#endif //  CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT

/* ==================================================================== 
 * Duplicate the above options. DO NOT modify them. Modify the ones above.
 * This is so the internal source files can be compiled from the same 
 * common sources. These defines can never be exported to the user.
 */

/* ---------------------------------------------------------------------
 * Define what kernel mode the driver can support. 
 */
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_INTERRUPT
#define CYG_DEVICE_SERIAL_RS232_KMODE_INTERRUPT	1
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_POLLED
#define CYG_DEVICE_SERIAL_RS232_KMODE_POLLED	0
#endif

/* ---------------------------------------------------------------------
 * Define what line modes the driver can support. 
 */
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_LINE_MODES
#define CYG_DEVICE_SERIAL_RS232_LINE_MODES
#endif

/* ---------------------------------------------------------------------
 * Define any terminal protocols need to be supported.
 */
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_READ_MODES
#define CYG_DEVICE_SERIAL_RS232_READ_MODES
#endif

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_WRITE_MODES
#define CYG_DEVICE_SERIAL_RS232_WRITE_MODES
#endif

#ifdef CYGPKG_KERNEL
/* ---------------------------------------------------------------------
 * Define if a mutex is needed to lock out multiple threads accessing
 * the serial driver. If not defined then it is assumes only one
 * thread will access the driver at a given time.
 */
#define CYG_DEVICE_SERIAL_RS232_MUTEX
#endif /* CYGPKG_KERNEL */

#endif // CYG_DEVICE_INTERNAL

#endif /* ifdef CYGPKG_DEVICES_SERIAL_RS232_TX39 et al */

#endif  /* CYGONCE_PKGCONF_SERIAL_TX39_H */
/* EOF serial_tx39.h */
