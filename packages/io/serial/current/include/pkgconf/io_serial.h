#ifndef CYGONCE_PKGCONF_IO_SERIAL_H
#define CYGONCE_PKGCONF_IO_SERIAL_H
// ====================================================================
//
//      pkgconf/io_serial.h
//
//      Device configuration file
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
// Author(s):   gthomas
// Contributors:        gthomas
// Date:        1999-02-04
// Purpose:     To allow the user to edit device configuration options.
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <pkgconf/system.h>

/* ----------------------------------------------------------------------------
   {{CFG_DATA

   cdl_package CYGPKG_IO_SERIAL {
       display "Serial device drivers"
       type    bool
       requires CYGPKG_IO
       parent CYGPKG_IO
       description "
           This option enables drivers for basic I/O services on
           serial devices."
           doc ref/ecos-ref/ecos-device-drivers.html
   }
   }}CFG_DATA */

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_TTY {
       display "TTY-mode serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
       description "
           This option enables the terminal-like device driver 
           used for serial devices that interact with humans,
           such as a system console."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TTY_CONSOLE {
       display "Console device name"
       type    string
       requires CYGPKG_IO_SERIAL_TTY
       description "
           This option selects the TTY device to use for the console."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

#define CYGPKG_IO_SERIAL_TTY
#define CYGDAT_IO_SERIAL_TTY_CONSOLE "/dev/ttydiag"

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_HALDIAG {
       display "HAL/diag serial device driver"
       type    bool
       requires CYGPKG_IO_SERIAL_TTY
       parent CYGPKG_IO_SERIAL_TTY
       description "
           This option enables the use of the HAL diagnostic channel
           via the standard I/O drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

#define CYGPKG_IO_SERIAL_HALDIAG


/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_TTY_TTYDIAG {
       display "TTY mode HAL/diag channel"
       type    bool
       requires CYGPKG_IO_SERIAL_TTY
       parent CYGPKG_IO_SERIAL_TTY
       description "
           This option causes '/dev/ttydiag' to be included in the standard drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_TTY_TTY0 {
       display "TTY mode channel #0"
       type    bool
       requires CYGPKG_IO_SERIAL_TTY
       parent CYGPKG_IO_SERIAL_TTY
       description "
           This option causes '/dev/tty0' to be included in the standard drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TTY_TTY0_DEV {
       display "TTY mode channel #0 device"
       type    string
       requires CYGPKG_IO_SERIAL_TTY_TTY0
       description "
           This option selects the physical device to use for '/dev/tty0'."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

  cdl_component CYGPKG_IO_SERIAL_TTY_TTY1 {
       display "TTY mode channel #1"
       type    bool
       requires CYGPKG_IO_SERIAL_TTY
       parent CYGPKG_IO_SERIAL_TTY
       description "
           This option causes '/dev/tty1' to be included in the standard drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TTY_TTY1_DEV {
       display "TTY mode channel #1 device"
       type    string
       requires CYGPKG_IO_SERIAL_TTY_TTY1
       description "
           This option selects the physical device to use for '/dev/tty1'."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_TTY_TTY2 {
       display "TTY mode channel #2"
       type    bool
       requires CYGPKG_IO_SERIAL_TTY
       parent CYGPKG_IO_SERIAL_TTY
       description "
           This option causes '/dev/tty2' to be included in the standard drivers."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TTY_TTY2_DEV {
       display "TTY mode channel #2 device"
       type    string
       requires CYGPKG_IO_SERIAL_TTY_TTY2
       description "
           This option selects the physical device to use for '/dev/tty2'."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

#define CYGPKG_IO_SERIAL_TTY_TTYDIAG
#undef  CYGPKG_IO_SERIAL_TTY_TTY0
#define CYGDAT_IO_SERIAL_TTY_TTY0_DEV "/dev/ser0"
#undef  CYGPKG_IO_SERIAL_TTY_TTY1
#define CYGDAT_IO_SERIAL_TTY_TTY1_DEV "/dev/ser1"
#undef  CYGPKG_IO_SERIAL_TTY_TTY2
#define CYGDAT_IO_SERIAL_TTY_TTY2_DEV "/dev/ser2"

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_ARM_PID {
       display "ARM PID serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_ARM_PID
       description "
           This option enables the serial device drivers for the ARM PID."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_ARM_PID_SERIAL0 {
       display "ARM PID serial port 0 driver"
       type    bool
       parent CYGPKG_IO_SERIAL_ARM_PID
       description "
           This option includes the serial device driver for the ARM PID port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_ARM_PID_SERIAL0_NAME {
       display "Device name for ARM PID serial port 0 driver"
       type    string
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL0
       description "
           This option specifies the name of the serial device for the ARM PID port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BAUD {
       display "Baud rate for the ARM PID serial port 0 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL0
       description "
           This option specifies the default baud rate (speed) for the ARM PID port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BUFSIZE {
       display "Buffer size for the ARM PID serial port 0 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL0
       description "
           This option specifies the size of the internal buffers used for the ARM PID port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_ARM_PID_SERIAL1 {
       display "ARM PID serial port 1 driver"
       type    bool
       parent CYGPKG_IO_SERIAL_ARM_PID
       description "
           This option includes the serial device driver for the ARM PID port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_ARM_PID_SERIAL1_NAME {
       display "Device name for ARM PID serial port 1 driver"
       type    string
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL1
       description "
           This option specifies the name of the serial device for the ARM PID port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BAUD {
       display "Baud rate for the ARM PID serial port 1 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL1
       description "
           This option specifies the default baud rate (speed) for the ARM PID port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BUFSIZE {
       display "Buffer size for the ARM PID serial port 1 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_ARM_PID_SERIAL1
       description "
           This option specifies the size of the internal buffers used for the ARM PID port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */

#undef  CYGPKG_IO_SERIAL_ARM_PID
#undef  CYGPKG_IO_SERIAL_ARM_PID_SERIAL0
#define CYGDAT_IO_SERIAL_ARM_PID_SERIAL0_NAME "/dev/ser0"
#define CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BAUD 38400
#define CYGNUM_IO_SERIAL_ARM_PID_SERIAL0_BUFSIZE 128
#define CYGPKG_IO_SERIAL_ARM_PID_SERIAL1
#define CYGDAT_IO_SERIAL_ARM_PID_SERIAL1_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BAUD 38400
#define CYGNUM_IO_SERIAL_ARM_PID_SERIAL1_BUFSIZE 128

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_ARM_AEB {
       display "ARM AEB-1 serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_ARM_AEB
       description "
           This option enables the serial device drivers for the ARM AEB-1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_ARM_AEB_SERIAL0 {
       display "ARM AEB-1 serial port 0 driver"
       type    bool
       parent CYGPKG_IO_SERIAL_ARM_AEB
       description "
           This option includes the serial device driver for the ARM AEB-1 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_ARM_AEB_SERIAL0_NAME {
       display "Device name for the ARM AEB-1 serial port 0 driver"
       type    string
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL0
       description "
           This option sets the name of the serial device for the ARM AEB-1 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_AEB_SERIAL0_BAUD {
       display "Baud rate for the ARM AEB-1 serial port 0 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL0
       description "
           This option specifies the default baud rate (speed) for the ARM AEB-1 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_AEB_SERIAL0_BUFSIZE {
       display "Buffer size for the ARM AEB-1 serial port 0 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL0
       description "
           This option specifies the size of the internal buffers used for the ARM AEB-1 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1 {
       display "ARM AEB-1 serial port 1 driver"
       type    bool
       parent CYGPKG_IO_SERIAL_ARM_AEB
       description "
           This option includes the serial device driver for the ARM AEB-1 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_ARM_AEB_SERIAL1_NAME {
       display "Device name for the ARM AEB-1 serial port 1 driver"
       type    string
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1
       description "
           This option specifies the name of serial device for the ARM AEB-1 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_AEB_SERIAL1_BAUD {
       display "Baud rate for the ARM AEB-1 serial port 1 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1
       description "
           This option specifies the default baud rate (speed) for the ARM AEB-1 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_ARM_AEB_SERIAL1_BUFSIZE {
       display "Buffer size for the ARM AEB-1 serial port 1 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1
       description "
           This option specifies the size of the internal buffers used for the ARM AEB-1 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */
#undef  CYGPKG_IO_SERIAL_ARM_AEB
#undef  CYGPKG_IO_SERIAL_ARM_AEB_SERIAL0
#define CYGDAT_IO_SERIAL_ARM_AEB_SERIAL0_NAME "/dev/ser0"
#define CYGNUM_IO_SERIAL_ARM_AEB_SERIAL0_BAUD 38400
#define CYGNUM_IO_SERIAL_ARM_AEB_SERIAL0_BUFSIZE 128
#undef  CYGPKG_IO_SERIAL_ARM_AEB_SERIAL1
#define CYGDAT_IO_SERIAL_ARM_AEB_SERIAL1_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_ARM_AEB_SERIAL1_BAUD 38400
#define CYGNUM_IO_SERIAL_ARM_AEB_SERIAL1_BUFSIZE 128


/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_POWERPC_COGENT {
       display "Cogent PowerPC serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_POWERPC_COGENT
       description "
           This option enables the serial device drivers for the Cogent PowerPC."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A {
       display "Cogent PowerPC serial port A driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT
       description "
           This option includes the serial device driver for the Cogent PowerPC port A."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_POWERPC_COGENT_SERIAL_A_NAME {
       display "Device name for Cogent PowerPC serial port A"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A
       description "
           This option specifies the device name for the Cogent PowerPC port A."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_A_BAUD {
       display "Baud rate for the Cogent PowerPC serial port A driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A
       description "
           This option specifies the default baud rate (speed) for the Cogent PowerPC port A."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_A_BUFSIZE {
       display "Buffer size for the Cogent PowerPC serial port A driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A
       description "
           This option specifies the size of the internal buffers used for the Cogent PowerPC port A."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B {
       display "Cogent PowerPC serial port B driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT
       description "
           This option includes the serial device driver for the Cogent PowerPC port B."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_POWERPC_COGENT_SERIAL_B_NAME {
       display "Device name for Cogent PowerPC serial port B"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B
       description "
           This option specifies the device name for the Cogent PowerPC port B."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_B_BAUD {
       display "Baud rate for the Cogent PowerPC serial port B driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B
       description "
           This option specifies the default baud rate (speed) for the Cogent PowerPC port B."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_B_BUFSIZE {
       display "Buffer size for the Cogent PowerPC serial port B driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B
       description "
           This option specifies the size of the internal buffers used for the Cogent PowerPC port B."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */
#undef  CYGPKG_IO_SERIAL_POWERPC_COGENT
// Only enable both ports if interrupt chaining is enabled.
#undef  CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A
#define CYGDAT_IO_SERIAL_POWERPC_COGENT_SERIAL_A_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_A_BAUD 38400
#define CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_A_BUFSIZE 128
#define CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B
#define CYGDAT_IO_SERIAL_POWERPC_COGENT_SERIAL_B_NAME "/dev/ser2"
#define CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_B_BAUD 38400
#define CYGNUM_IO_SERIAL_POWERPC_COGENT_SERIAL_B_BUFSIZE 128

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_SPARCLITE_SLEB {
       display "SPARClite SLEB serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_SPARCLITE_SLEB
       description "
           This option enables the serial device drivers for the SPARClite SLEB."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1 {
       display "SPARClite SLEB serial CON1 port driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB
       description "
           This option includes the serial device driver for the SPARClite SLEB CON1 port."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_SPARCLITE_SLEB_CON1_NAME {
       display "Device name for SPARClite SLEB serial CON1 port"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1
       description "
           This option specifies the device name for the SPARClite SLEB CON1 port."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON1_BAUD {
       display "Baud rate for the SPARClite SLEB serial CON1 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1
       description "
           This option specifies the default baud rate (speed) for the SPARClite SLEB CON1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON1_BUFSIZE {
       display "Buffer size for the SPARClite SLEB serial CON1 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1
       description "
           This option specifies the size of the internal buffers used for the SPARClite SLEB CON1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON2 {
       display "SPARClite SLEB serial CON2 port driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB
       description "
           This option includes the serial device driver for the SPARClite SLEB CON2 port."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_SPARCLITE_SLEB_CON2_NAME {
       display "Device name for SPARClite SLEB serial CON2 port"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON2
       description "
           This option specifies the device name for the SPARClite SLEB CON2 port."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON2_BAUD {
       display "Baud rate for the SPARClite SLEB serial CON2 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON2
       description "
           This option specifies the default baud rate (speed) for the SPARClite SLEB CON2."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON2_BUFSIZE {
       display "Buffer size for the SPARClite SLEB serial CON2 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON2
       description "
           This option specifies the size of the internal buffers used for the SPARClite SLEB CON2."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */
#undef  CYGPKG_IO_SERIAL_SPARCLITE_SLEB
#undef  CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON1
#define CYGDAT_IO_SERIAL_SPARCLITE_SLEB_CON1_NAME "/dev/ser0"
#define CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON1_BAUD 19200
#define CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON1_BUFSIZE 128
#undef  CYGPKG_IO_SERIAL_SPARCLITE_SLEB_CON2
#define CYGDAT_IO_SERIAL_SPARCLITE_SLEB_CON2_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON2_BAUD 19200
#define CYGNUM_IO_SERIAL_SPARCLITE_SLEB_CON2_BUFSIZE 128

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_TX39_JMR3904 {
       display "TX39 JMR3904 serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_TX39_JMR3904
       description "
           This option enables the serial device drivers for the TX39 JMR3904."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_TX39_JMR3904_POLLED_MODE {
       display "TX39 JMR3904 polled mode serial drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_TX39_JMR3904
       description "
           If asserted, this option specifies that the serial device drivers for 
           the TX39 JMR3904 should be polled-mode instead of interrupt driven."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0 {
       display "TX39 JMR3904 serial port 0 driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_TX39_JMR3904
       description "
           This option includes the serial device driver for port 0 on the TX39 JMR3904."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TX39_JMR3904_SERIAL0_NAME {
       display "Device name for TX39 JMR3904 serial port 0"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0
       description "
           This option specifies the device name port 0 on the TX39 JMR3904."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL0_BAUD {
       display "Baud rate for the TX39 JMR3904 serial port 0 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0
       description "
           This option specifies the default baud rate (speed) for the TX39 JMR3904 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL0_BUFSIZE {
       display "Buffer size for the TX39 JMR3904 serial port 0 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0
       description "
           This option specifies the size of the internal buffers used for the TX39 JMR3904 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL1 {
       display "TX39 JMR3904 serial port 1 driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_TX39_JMR3904
       description "
           This option includes the serial device driver for port 1 on the TX39 JMR3904."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_TX39_JMR3904_SERIAL1_NAME {
       display "Device name for TX39 JMR3904 serial port 1"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL1
       description "
           This option specifies the device name port 1 on the TX39 JMR3904."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL1_BAUD {
       display "Baud rate for the TX39 JMR3904 serial port 1 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL1
       description "
           This option specifies the default baud rate (speed) for the TX39 JMR3904 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL1_BUFSIZE {
       display "Buffer size for the TX39 JMR3904 serial port 1 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL1
       description "
           This option specifies the size of the internal buffers used for the TX39 JMR3904 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */
#undef  CYGPKG_IO_SERIAL_TX39_JMR3904
#undef  CYGPKG_IO_SERIAL_TX39_JMR3904_POLLED_MODE
#undef  CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL0
#define CYGDAT_IO_SERIAL_TX39_JMR3904_SERIAL0_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL0_BAUD 38400
#define CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL0_BUFSIZE 128
#undef  CYGPKG_IO_SERIAL_TX39_JMR3904_SERIAL1
#define CYGDAT_IO_SERIAL_TX39_JMR3904_SERIAL1_NAME "/dev/ser2"
#define CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL1_BAUD 38400
#define CYGNUM_IO_SERIAL_TX39_JMR3904_SERIAL1_BUFSIZE 128

// Note: this is not currently tied to a specific board since the ports are "on chip"

/* ----------------------------------------------------------------------------
   {{CFG_DATA
   cdl_component CYGPKG_IO_SERIAL_MN10300 {
       display "MN10300 serial device drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
#       parent CYGPKG_IO_SERIAL
       parent CYGPKG_HAL_MN10300
       description "
           This option enables the serial device drivers for the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_MN10300_POLLED_MODE {
       display "MN10300 polled mode serial drivers"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300
       description "
           If asserted, this option specifies that the serial device drivers for 
           the MN10300 should be polled-mode instead of interrupt driven."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_MN10300_SERIAL0 {
       display "MN10300 serial port 0 driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300
       description "
           This option includes the serial device driver for port 0 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_MN10300_SERIAL0_NAME {
       display "Device name for MN10300 serial port 0"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL0
       description "
           This option specifies the device name port 0 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL0_BAUD {
       display "Baud rate for the MN10300 serial port 0 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL0
       description "
           This option specifies the default baud rate (speed) for the MN10300 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE {
       display "Buffer size for the MN10300 serial port 0 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL0
       description "
           This option specifies the size of the internal buffers used for the MN10300 port 0."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_MN10300_SERIAL1 {
       display "MN10300 serial port 1 driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300
       description "
           This option includes the serial device driver for port 1 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_MN10300_SERIAL1_NAME {
       display "Device name for MN10300 serial port 1"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL1
       description "
           This option specifies the device name port 1 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL1_BAUD {
       display "Baud rate for the MN10300 serial port 1 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL1
       description "
           This option specifies the default baud rate (speed) for the MN10300 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE {
       display "Buffer size for the MN10300 serial port 1 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL1
       description "
           This option specifies the size of the internal buffers used for the MN10300 port 1."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_component CYGPKG_IO_SERIAL_MN10300_SERIAL2 {
       display "MN10300 serial port 2 driver"
       type    bool
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300
       description "
           This option includes the serial device driver for port 2 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGDAT_IO_SERIAL_MN10300_SERIAL2_NAME {
       display "Device name for MN10300 serial port 2"
       type    string
       requires CYGPKG_IO_SERIAL
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL2
       description "
           This option specifies the device name port 2 on the MN10300."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL2_BAUD {
       display "Baud rate for the MN10300 serial port 2 driver"
       type    enum
       legal_values 50 75 110 134_5 150 200 300 600 1200 1800 2400 3600 4800 7200 9600 14400 19200 38400 57600 115200 234000
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL2
       description "
           This option specifies the default baud rate (speed) for the MN10300 port 2."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   cdl_option CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE {
       display "Buffer size for the MN10300 serial port 2 driver"
       type    count
       legal_values 0 to 8192
       parent CYGPKG_IO_SERIAL_MN10300_SERIAL2
       description "
           This option specifies the size of the internal buffers used for the MN10300 port 2."
           doc ref/ecos-ref/ecos-device-drivers.html
   }

   }}CFG_DATA */
#undef  CYGPKG_IO_SERIAL_MN10300
#undef  CYGPKG_IO_SERIAL_MN10300_POLLED_MODE
#undef  CYGPKG_IO_SERIAL_MN10300_SERIAL0
#define CYGDAT_IO_SERIAL_MN10300_SERIAL0_NAME "/dev/ser0"
#define CYGNUM_IO_SERIAL_MN10300_SERIAL0_BAUD 38400
#define CYGNUM_IO_SERIAL_MN10300_SERIAL0_BUFSIZE 128
#undef  CYGPKG_IO_SERIAL_MN10300_SERIAL1
#define CYGDAT_IO_SERIAL_MN10300_SERIAL1_NAME "/dev/ser1"
#define CYGNUM_IO_SERIAL_MN10300_SERIAL1_BAUD 38400
#define CYGNUM_IO_SERIAL_MN10300_SERIAL1_BUFSIZE 128
#undef  CYGPKG_IO_SERIAL_MN10300_SERIAL2
#define CYGDAT_IO_SERIAL_MN10300_SERIAL2_NAME "/dev/ser2"
#define CYGNUM_IO_SERIAL_MN10300_SERIAL2_BAUD 38400
#define CYGNUM_IO_SERIAL_MN10300_SERIAL2_BUFSIZE 128

#endif  /* CYGONCE_PKGCONF_IO_SERIAL_H */
/* EOF io_serial.h */
