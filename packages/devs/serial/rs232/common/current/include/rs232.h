#ifndef CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_H
#define CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_H

//==========================================================================
//
//	rs232.h
//
//	Header file for the generic serial device 
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	proven
// Contributors:	proven
// Date:	1998-04-20
// Purpose:	Define Cyg_Device_Serial data structure
// Description:	This file contains the baud rates and line modes for the
//              RS232 serial devices.
// Usage:	#include <cyg/serial/rs232/common/rs232.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Various baud rates
// Macro names are the same as the POSIX ones.
#define B0 	 0
#define B50 	 1
#define B75 	 2
#define B110 	 3
#define B134 	 4
#define B150 	 5
#define B200 	 6
#define B300 	 7
#define B600 	 8
#define B1200 	 9
#define B1800 	10
#define B2400 	11
#define B4800 	12
#define B9600 	13
#define B19200 	14
#define B38400 	15
// Non POSIX FAST baud rates.
#define B57600	16
#define B115200	17
#define B230400	18
#define B460800	19

// Various line modes
// Macro names are the same as the POSIX ones.
#define CSIZE	0x03	// Mask for number of bits per byte
#define	CS5	0x00
#define	CS6	0x01	
#define	CS7	0x02
#define	CS8	0x04
#define CSTOPB	0x04	// If set then two stop bits otherwise one
#define PARENB	0x10	// If set then parity is enabled
#define PARODD	0x20	// If set then odd parity otherwise even

// Read mode bits
#define CYG_DEVICE_SERIAL_RS232_READ_MODE_IGN	0x01	// Ignore
#define CYG_DEVICE_SERIAL_RS232_READ_MODE_TRN 	0x02	// Translate
#define CYG_DEVICE_SERIAL_RS232_READ_MODE_EOB	0x04	// End of Buffer

// Possible future read mode options
//#define CYG_DEVICE_SERIAL_RS232_READ_MODE_ESCAPE 	0x10
//#define CYG_DEVICE_SERIAL_RS232_READ_MODE_XOFF	0x20
//#define CYG_DEVICE_SERIAL_RS232_READ_MODE_XON	 	0x40

#endif  // CYGONCE_DEVS_SERIAL_RS232_COMMON_RS232_H
// End of rs232.h

