#ifndef CYGONCE_DEVS_SERIAL_RS232_TX39_SERIAL_TX39_HXX
#define CYGONCE_DEVS_SERIAL_RS232_TX39_SERIAL_TX39_HXX

//==========================================================================
//
//	serial_tx39.hxx
//
//	Header file for the tx39 serial device
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
// Purpose:	Define Cyg_Device_Serial_tx39 data structure
// Description:	This file contains the C++ class declaration of 
//		Cyg_Device_Serial_tx39. There can be only one.
// Usage:	#include <cyg/devs/serial/rs232/tx39/serial_tx39.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs.h>		// Device configureation file

#ifdef CYG_DEVICE_SERIAL_RS232_TX39
// Include Configureation options specific to the device
#include CYG_DEVICE_SERIAL_RS232_TX39	

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         	// assertion macros
#include <cyg/infra/cyg_trac.h>        	// tracing macros

#include <cyg/kernel/intr.hxx>		// Cyg_ISR, Cyg_DSR
#include <cyg/kernel/sched.hxx>		// intr.hxx
#include <cyg/kernel/sched.inl>		// intr.hxx

#ifdef CYG_DEVICE_SERIAL_RS232_MUTEX
#include <cyg/kernel/mutex.hxx>
#endif
#include <cyg/devs/serial/rs232/common/rs232.hxx>

// -------------------------------------------------------------------------
// The tx39 serial device registers.

#ifdef CYG_DEVICE_INTERNAL
// Define the serial registers.
#define CYG_DEVICE_SERIAL_RS232_TX39_SLCR	((volatile cyg_uint16 *)0xfffff300)
#define CYG_DEVICE_SERIAL_RS232_TX39_SDICR	((volatile cyg_uint16 *)0xfffff308)
#define CYG_DEVICE_SERIAL_RS232_TX39_SDISR	((volatile cyg_uint16 *)0xfffff30c)
#define CYG_DEVICE_SERIAL_RS232_TX39_SFCR	((volatile cyg_uint16 *)0xfffff310)
#define CYG_DEVICE_SERIAL_RS232_TX39_SBRG	((volatile cyg_uint16 *)0xfffff314)
#define CYG_DEVICE_SERIAL_RS232_TX39_TFIFO	((volatile cyg_uint8  *)0xfffff320)
#define CYG_DEVICE_SERIAL_RS232_TX39_SFIFO	((volatile cyg_uint8  *)0xfffff330)

#define CYG_DEVICE_SERIAL_RS232_TX39_VEC 	CYG_VECTOR_SIO_0

// Here are the values for the timers to set the various baud rates
// Timer 1 is the serial 2 timer register
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B0		   0
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B600		0x02
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B1200		0x02
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B2400		0x02
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B4800		0x02
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B9600		0x01
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B19200		0x01
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400		0x01

// Timer 2 is the 8 bit timer 2 register
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B0		   0
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B600		  81
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B1200		  41
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B2400		  20
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B4800		  41
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B9600		  20
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B19200		  10
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400		  05

// Now set the default baudrate
#ifndef CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE
#error "Must specify a default baud rate"
#else
#if ((CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE > 0) && \
    (CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE < 600))
#error "Baud rates less than 600 are not supported for the tx39 at this time"
#endif
#define CYG_DEVICE_SERIAL_RS232_DEFAULT_BAUD_RATE \
	CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE
#endif // CYG_DEVICE_SERIAL_RS232_TX39_DEFAULT_BAUD_RATE

#include <cyg/devs/serial/rs232/common/baud.h>

#endif // CYG_DEVICE_INTERNAL

// -------------------------------------------------------------------------
// Cyg_Device_Serial_RS232_tx39

#define CYG_CLASS_DEVICE_SERIAL_RS232_TX39	Cyg_Device_Serial_RS232_tx39

class CYG_CLASS_DEVICE_SERIAL_RS232_TX39
    : public Cyg_Device_Serial_RS232
{
#ifdef CYG_DEVICE_INTERNAL
#define CYG_DEVICE_SERIAL_RS232_ONE_VECTOR
#include <cyg/devs/serial/rs232/common/rs232.ht>
#endif

private:
    /* Internal routines */
    cyg_int32 	io_read(Cyg_IORB * iorb);
    cyg_int32 	io_write(Cyg_IORB * iorb);
    void 	io_write_flush(void);

public:

    /* Constructor */
    CYG_CLASS_DEVICE_SERIAL_RS232_TX39();

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_KMODE_ASSERT
    cyg_int32 io_read_assert(Cyg_IORB * iorb);
    cyg_int32 io_write_assert(Cyg_IORB * iorb);
#endif 
 
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

#ifdef CYG_DEVICE_SERIAL_RS232_TX39_READ_MODES
    cyg_int32 set_read_mode(cyg_uint32);
#endif
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_WRITE_MODES
    cyg_int32 set_write_mode(cyg_uint32);
#endif
};

// Declare exported variables
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_NAME
#ifdef CYG_DEVICE_SERIAL_RS232_TX39_DECLARE
extern CYG_CLASS_DEVICE_SERIAL_RS232_TX39 CYG_DEVICE_SERIAL_RS232_TX39_NAME;
#endif
#endif

#endif  // CYG_DEVICE_SERIAL_RS232_TX39

#endif  // CYGONCE_DEVS_SERIAL_RS232_TX39_SERIAL_TX39_HXX
// End of serial_tx39.hxx

