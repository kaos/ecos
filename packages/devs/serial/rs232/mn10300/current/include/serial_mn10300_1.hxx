#ifndef CYGONCE_DEVS_SERIAL_RS232_MN10300_SERIAL_MN10300_1_HXX
#define CYGONCE_DEVS_SERIAL_RS232_MN10300_SERIAL_MN10300_1_HXX

//==========================================================================
//
//	serial_mn10300_1.hxx
//
//	Header file for the mn10300 serial port #1
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
// Date:	1998-04-22
// Purpose:	Define Cyg_Device_Serial_mn10300_1 data structure
// Description:	This file contains the C++ class declaration of 
//		Cyg_Device_Serial_mn10300_1. There can be only one.
// Usage:	#include <cyg/devs/serial/rs232/mn10300/serial_mn10300_1.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs.h>		// Device configureation file

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1
// Include Configureation options specific to the device
#include CYG_DEVICE_SERIAL_RS232_MN10300_1	

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         	// assertion macros
#include <cyg/infra/cyg_trac.h>        	// tracing macros

#include <cyg/kernel/intr.hxx>		// Cyg_ISR, Cyg_DSR
#include <cyg/kernel/sched.hxx>		// intr.hxx
#include <cyg/kernel/sched.inl>		// intr.hxx
#include <cyg/hal/hal_intr.h>		// vectors
#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
#include <cyg/kernel/mutex.hxx>
#endif
#include <cyg/devs/serial/rs232/common/rs232.hxx>

// -------------------------------------------------------------------------
// The mn10300 serial 1 device is CURRENTLY unique.

#ifdef CYG_DEVICE_INTERNAL
// Define the serial registers.
#define CYG_DEVICE_SERIAL_RS232_CR     ((volatile cyg_uint16 *)0x34000810)
#define CYG_DEVICE_SERIAL_RS232_ICR    ((volatile cyg_uint8 *) 0x34000814)
#define CYG_DEVICE_SERIAL_RS232_TXR    ((volatile cyg_uint8 *) 0x34000818)
#define CYG_DEVICE_SERIAL_RS232_RXR    ((volatile cyg_uint8 *) 0x34000819)

#define CYG_DEVICE_SERIAL_RS232_SR_ADDR     0x3400081c
#define CYG_DEVICE_SERIAL_RS232_SR_SIZE	    cyg_uint16
#define CYG_DEVICE_SERIAL_RS232_SR          \
  ((volatile CYG_DEVICE_SERIAL_RS232_SR_SIZE *) CYG_DEVICE_SERIAL_RS232_SR_ADDR)

// Timer 2 provides the baud rate divisor for serial 2 exclusivly
#define TIMER1_MD       ((volatile cyg_uint8 *)0x34001001)
#define TIMER1_BR       ((volatile cyg_uint8 *)0x34001011)
#define TIMER2_CR       ((volatile cyg_uint8 *)0x34001021)

#define PORT3_MD        ((volatile cyg_uint8 *)0x36008025)


#define CYG_DEVICE_SERIAL_RS232_TVEC   CYG_VECTOR_SERIAL_1_TX
#define CYG_DEVICE_SERIAL_RS232_RVEC   CYG_VECTOR_SERIAL_1_RX

// Here are the values for the timers to set the various baud rates
// Timer 1 is the 8 bit timer 1 register
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0		  0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B600		  0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B1200		  0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B2400		  0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B4800		  0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B9600		195
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B19200		 98
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400		 48
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B57600		 32
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B115200	 16
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B230400	  8
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B460800	  4

// Timer 2 doesn't exist and is always 0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B600		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B1200		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B2400		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B4800		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B9600		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B19200		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B57600		  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B115200	  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B230400	  0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B460800	  0

// Now set the default baudrate
#ifndef CYG_DEVICE_SERIAL_RS232_MN10300_1_DEFAULT_BAUD_RATE
#error "Must specify a default baud rate"
#else
#if ((CYG_DEVICE_SERIAL_RS232_MN10300_1_DEFAULT_BAUD_RATE > 0) && \
    (CYG_DEVICE_SERIAL_RS232_MN10300_1_DEFAULT_BAUD_RATE < 9600))
#error "Baud rates less than 9600 are not supported for the mn10300 serial port 1 at this time"
#endif
#define CYG_DEVICE_SERIAL_RS232_DEFAULT_BAUD_RATE \
	CYG_DEVICE_SERIAL_RS232_MN10300_1_DEFAULT_BAUD_RATE
#endif // CYG_DEVICE_SERIAL_RS232_MN10300_1_DEFAULT_BAUD_RATE

#include <cyg/devs/serial/rs232/common/baud.h>

#endif // CYG_DEVICE_INTERNAL

// -------------------------------------------------------------------------
// Cyg_Device_Serial_RS232_mn10300_1

#define CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_1	Cyg_Device_Serial_RS232_mn10300_1

class CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_1
    : public Cyg_Device_Serial_RS232
{
#ifdef CYG_DEVICE_INTERNAL
#include <cyg/devs/serial/rs232/common/rs232.ht>
#endif

private:
    /* Internal routines */
    cyg_int32 	io_read(Cyg_IORB * iorb);
    cyg_int32 	io_write(Cyg_IORB * iorb);
    void 	io_write_flush(void);

public:

    /* Constructor */
    CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_1();

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_KMODE_ASSERT
    cyg_int32 io_read_assert(Cyg_IORB * iorb);
    cyg_int32 io_write_assert(Cyg_IORB * iorb);
#endif 
 
    cyg_int32 io_read_cancel(Cyg_IORB * iorb);
    cyg_int32 io_write_cancel(Cyg_IORB * iorb);
    cyg_int32 io_read_blocking(Cyg_IORB * iorb);
    cyg_int32 io_write_blocking(Cyg_IORB * iorb);
    cyg_int32 io_read_asynchronous(Cyg_IORB * iorb);
    cyg_int32 io_write_asynchronous(Cyg_IORB * iorb);

    cyg_int32 set_kmode(cyg_uint32);
    cyg_int32 get_kmode(void);

    cyg_int32 set_baud_rate(cyg_uint32);
    cyg_int32 get_baud_rate(void);

    cyg_int32 set_line_mode(cyg_uint32);
    cyg_int32 get_line_mode(void);

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_READ_MODES
    cyg_int32 set_read_mode(cyg_uint32);
    cyg_int32 set_read_data(cyg_uint32 /* mode */, const char * /* data */, cyg_uint32 /* count */);
#endif
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_WRITE_MODES
    cyg_int32 set_write_mode(cyg_uint32);
#endif
};

// Declare exported variables
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME
#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_1_DECLARE
extern CYG_CLASS_DEVICE_SERIAL_RS232_MN10300_1 CYG_DEVICE_SERIAL_RS232_MN10300_1_NAME;
#endif
#endif

#endif  // CYG_DEVICE_SERIAL_RS232_MN10300_1

#endif  // CYGONCE_DEVS_SERIAL_RS232_MN10300_SERIAL_MN10300_1_HXX
// End of serial_mn10300_1.hxx

