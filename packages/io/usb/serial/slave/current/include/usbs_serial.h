#ifndef CYGONCE_USBS_SERIAL_H
#define CYGONCE_USBS_SERIAL_H
//==========================================================================
//
//      include/usbs_serial.h
//
//      Description of the USB slave-side serial device support
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Contributors: 
// Date:         2008-06-02
// Purpose:
// Description:  USB slave-side serial support
//
//
//####DESCRIPTIONEND####
//==========================================================================

#ifdef __cplusplus
extern "C" {
#endif
    
//
// The primary purpose of the USB slave-side serial code is to provide a 
// simple USB connection to the host, especially for embedded systems that
// are upgrading from RS-232 serial connections. The host would see the 
// device as if through a serial port, and thus the host software would
// remain unchanged. It would also eliminate the need for a new device
// driver on the host.
// 
// On this side (the eCos USB slave side), the application sees the host
// through a normal USB slave connection with two Bulk endpoints - one in 
// the IN direction and one in the OUT direction. This module provides the
// necessary USB descriptors to enumerate the device for a single serial
// port, but then the application is free to communicate with the host
// using any desired API:
//  - The standard eCos USB slave API
//  - The low-level File I/O layer (if USB devtab entries configured)
//  - The C stdio functions (again, if USB devtab entries configured)
//  - The USB serial API defined here.
// 
// The USB serial API is a thin layer over the standard eCos USB functions
// to provide common synchronous and asynchronous transfers over the assigned
// Bulk endpoints.
//

#include <cyg/infra/cyg_type.h>
#include <cyg/io/usb/usbs.h>

// ----------------------------------------------------------------------------
// The ACM class requests
// 

#define USBS_SERIAL_SEND_ENCAPSULATED_COMMAND   0x00
#define USBS_SERIAL_GET_ENCAPSULATED_RESPONSE   0x01
#define USBS_SERIAL_SET_COMM_FEATURE            0x02
#define USBS_SERIAL_GET_COMM_FEATURE            0x03
#define USBS_SERIAL_CLEAR_COMM_FEATURE          0x04

#define USBS_SERIAL_SET_LINE_CODING             0x20
#define USBS_SERIAL_GET_LINE_CODING             0x21
#define USBS_SERIAL_SET_CONTROL_LINE_STATE      0x22
#define USBS_SERIAL_SEND_BREAK                  0x23

// ----------------------------------------------------------------------------
// Data structure to manage the pair of USB endpoints that comprise a single
// serial port connection. Each "port" requires one Bulk IN endpoint and one
// Bulk OUT endpoint.

typedef struct usbs_serial {
    // The communication endpoints. For the first (default) channel, these
    // are normally set by the configuration, but can be changed by the
    // application, if desired.
    usbs_tx_endpoint*   tx_ep;
    usbs_rx_endpoint*   rx_ep;

    // The signal that a transmit operation is complete, and it's result.
    cyg_sem_t   tx_ready;
    int         tx_result;

    // The signal that a receive operation is complete, and it's result.
    cyg_sem_t   rx_ready;
    int         rx_result;

} usbs_serial;

// The package contains one USB serial device.
extern usbs_serial usbs_ser0;

// It's assumed that there's a single USB slave chip in the system, with a
// single control endpoint 0. The actual variable is contained in the device
// driver, but the USB serial code keeps a pointer to it for driver 
// independence. The application might find it useful for overriding low-level
// code or callbacks.
extern usbs_control_endpoint* usbs_serial_ep0;

// ----------------------------------------------------------------------------
// A C interface to the serial USB code.
// The application can use this interface, the standard (low-level) USB slave
// API, the standard Unix-like I/O API, or C stdio API.
    
// Initialize support for a particular USB serial "port"
// This associates a usbs_serial structure with specific endpoints and 
// initializes the structure for communications.
void usbs_serial_init(usbs_serial*, usbs_tx_endpoint*, usbs_rx_endpoint*);

// Block the calling thread until the host configures the USB device.
void usbs_serial_wait_until_configured(void);

// Determines if the USB subsystem is configured
cyg_bool usbs_serial_is_configured(void);

// Start an asynchronous transmit of a single buffer.
void usbs_serial_start_tx(usbs_serial*, const void* buf, int n);

// Block the calling thread until the transmit completes.
// Returns the result code for the transfer
int usbs_serial_wait_for_tx(usbs_serial*);

// Blocking, synchronous transmit of a single buffer.
int usbs_serial_tx(usbs_serial*, const void* buf, int n);

// Start an asynchronous receive of a buffer.
void usbs_serial_start_rx(usbs_serial*, void* buf, int n);

// Block the calling thread until the receive completes.
// Returns the result code for the transfer
int usbs_serial_wait_for_rx(usbs_serial*);

// Blocking, synchronous receive of a single buffer.
int usbs_serial_rx(usbs_serial*, void* buf, int n);

// The default USB-serial state change handler paces the functions
// usbs_serial_wait_until_configured() and usbs_serial_is_configured().
// The application can override the state chain handler, but chain to 
// this function to keep the full USB-serial system working.
void usbs_serial_state_change_handler(usbs_control_endpoint*, void*, 
                                      usbs_state_change, int);

// Starts the USB subsystem
void usbs_serial_start(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_USBS_SERIAL_H

