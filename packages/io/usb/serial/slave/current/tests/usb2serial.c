//==========================================================================
//
//      usb2serial.c
//
//      Example application for the USB serial layer in eCos.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2010 Free Software Foundation, Inc.                        
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Contributors: 
// Date:         2008-06-02
// Description:  USB serial example application.
//
//####DESCRIPTIONEND####
//===========================================================================

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <pkgconf/kernel.h>
#include <cyg/io/serialio.h>

#include <cyg/io/usb/usbs_serial.h>
#include <pkgconf/io_usb_slave_serial.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// This application creates a USB-serial converter. To the host it will appear
// as a single serial USB port, such as /dev/ttyUSB0 for a Linux host.
// Any characters received from the USB host will be sent out the serial port
// and visa-verse. It creates a separate, dedicated thread for each direction.
// 
// It uses the eCos USB-serial layer to enumerate with the USB host and monitor
// the connection, but then uses standard C I/O functions to perform the 
// communications.
// 
// The USB serial module can be configured as a generic adapter or an an ACM
// communications class device. For the latter, the application handles the
// USB communications class requests which allows it to receive requests from
// the host to set serial parameters, like the baud rate. This actually turns
// this example into a more realistic USB-serial adapter that can be configured
// dynamically by the host.
// 
// The eCos library must be configured with the packages for USB slave, USB 
// serial, and File I/O. It also requires the proper serial port driver for the
// target platform.
// 
// This example was tested with the AT91SAM7S-EK board, but should work with any
// board that has a USB slave and serial port, and the necessary drivers.


// Comment this line out to remove debug output.
#define DEBUG_OUTPUT

#if defined(DEBUG_OUTPUT)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

// Set these to the USB devtab entries for the Tx and Rx Bulk endpoints 
// selected in the configuration of the USB serial subsystem.
#define USB_TX_DEV  "/dev/usbs1"
#define USB_RX_DEV  "/dev/usbs2"

// Set this for any available serial port on the target.
#define SER_DEV     "/dev/ser0"

// Buffer for incoming USB bulk data. The local USB driver can probably split
// packets, but just in case, making this the page size of the host might be
// helpful.
#define BUF_SIZE 4096
static char usb2ser_buf[BUF_SIZE];

// The threads
cyg_thread thread[2];

// Space for two 4K stacks
#define THREAD_STACK_SIZE 4096
char stack[2][THREAD_STACK_SIZE];

// The handles for the threads
cyg_handle_t    usb2ser_thread, 
                ser2usb_thread;

// --------------------------------------------------------------------------
// For an ACM serial device we can handle the USB class messages to deal with
// requests from the host like setting the serial parameters (baud rate, 
// etc).

#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM

static cyg_uint8    acm_buf[32];
static cyg_uint32   baud = 34800;

// --------------------------------------------------------------------------
// Handler for the completion of a SetLineCoding request from the host.
// The 'acm_buf' should contain the 7-byte request OUT packet from the
// host. This contains a request to change the serial parameters.
// In this example function, we will accept a few different baud rates.
// To keep the example relatively simple, though, we keep the other serial
// parameters to 1 stop bit, no parity, 8 data bits.

static usbs_control_return
acm_set_line_coding(usbs_control_endpoint* ep0, int n)
{
  int err;
  cyg_uint32 req_baud;
  cyg_io_handle_t handle;
  
  // Get the requested baud rate from the received ctrl OUT packet
  req_baud = ((acm_buf[3] << 24) | (acm_buf[2] << 16) | 
              (acm_buf[1] << 8) | acm_buf[0]);
  
  DBG("Set Baud: %u\n", (unsigned) baud);
  
  // Look up the serial handle and attempt to set the baud rate.
  if (cyg_io_lookup(SER_DEV, &handle) == 0) {
    cyg_serial_info_t ser_info;
    cyg_uint32 len = sizeof(ser_info);
    
    switch (baud) {
      case   9600 : ser_info.baud = CYGNUM_SERIAL_BAUD_9600;      break;
      case  38400 : ser_info.baud = CYGNUM_SERIAL_BAUD_38400;     break;
      case 115200 : ser_info.baud = CYGNUM_SERIAL_BAUD_115200;    break;
      default:
        DBG("Unsupported baud rate\n");
        return USBS_CONTROL_RETURN_HANDLED;
    }
    ser_info.stop = CYGNUM_SERIAL_STOP_1;
    ser_info.parity = CYGNUM_SERIAL_PARITY_NONE;
    ser_info.word_length = CYGNUM_SERIAL_WORD_LENGTH_8;
    ser_info.flags = 0;
    
    err = cyg_io_set_config(handle, CYG_IO_SET_CONFIG_SERIAL_INFO, 
                            &ser_info, &len);
    if (err == 0)
      baud = req_baud;
    else {
      DBG("Error setting serial params\n");
    }
  }
  else {
    DBG("Error looking up serial device: %s\n", SER_DEV);
  }
  return USBS_CONTROL_RETURN_HANDLED;
}

// --------------------------------------------------------------------------
// Handler for the ACM class messages.
// 
static usbs_control_return 
acm_class_handler(usbs_control_endpoint* ep0, void* data)
{
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
  
  usb_devreq  *req = (usb_devreq *) ep0->control_buffer;
  
  static cyg_uint8 rsp_buf[32];
  
  DBG("ACM Class Handler\n");
  
  switch (req->request) {
    
    case USBS_SERIAL_SET_LINE_CODING :
      DBG("Set Line Coding\n");
      memset(acm_buf, 0, 32);
      ep0->buffer = acm_buf;
      ep0->buffer_size = 7;
      ep0->complete_fn = acm_set_line_coding;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;
      
    case USBS_SERIAL_GET_LINE_CODING :
      DBG("Get Line Coding\n");
      rsp_buf[0] = baud & 0xFF;
      rsp_buf[1] = (baud >>  8) & 0xFF;
      rsp_buf[2] = (baud >> 16) & 0xFF;
      rsp_buf[3] = (baud >> 24) & 0xFF;
      rsp_buf[4] = 0; // One stop bit
      rsp_buf[5] = 0; // No parity
      rsp_buf[6] = 8; // 8 data bits
      ep0->buffer = rsp_buf;
      ep0->buffer_size = 7;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;
      
    default :
      DBG("*** Unhandled ACM Request: 0x%02X ***\n",
          (unsigned) req->request);
  }
  
  return result;
}

#endif      // CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM

// --------------------------------------------------------------------------
// Thread receives packets from the USB and sends them out the serial port
// It uses a buffered stdio input, an un-buffered low-level file output.
// This isn't terribly efficient, but rather an example of both methods.

void usb2ser_func(cyg_addrword_t data)
{
  int  c;
  FILE *rxf = fopen(USB_RX_DEV, "r");
  int   txh = open(SER_DEV, O_WRONLY, 0);
  
  DBG("Usb2Ser: Thread starting\n");
  
  if (!rxf) {
    DBG("Error opening USB rx port\n");
    return;
  }
  
  if (txh < 0) {
    DBG("Error opening serial tx port\n");
    return;
  }
  
  // Give the USB receiver an adequate buffer.
  setvbuf(rxf, usb2ser_buf, _IOFBF, BUF_SIZE);
  
  while (1) {
    
    // ----- Wait for the host to configure -----
    
    DBG("Usb2Ser: Waiting for USB configuration\n");
    usbs_serial_wait_until_configured();
    cyg_thread_delay((cyg_tick_count_t) 10);
    
    // ----- While configured read data & send out serial port -----
    
    DBG("Usb2Ser: USB configured\n");
    while (usbs_serial_is_configured()) {
      if ((c = getc(rxf)) < 0) {
        DBG("*** USB Read Error: %d ***\n", c);
      }
      else {
        char ch = (char) c;
        write(txh, &ch, 1);
      }
    }
  }
}

// --------------------------------------------------------------------------
// Thread receives packets from the serial port and sends them out the USB
// It uses a buffered stdio input, an un-buffered low-level file output.
// This isn't terribly efficient, but rather an example of both methods.

void ser2usb_func(cyg_addrword_t data)
{
  int  c;
  FILE *rxf = fopen(SER_DEV, "r");
  int  txh = open(USB_TX_DEV, O_WRONLY, 0);
  
  DBG("Ser2Usb: Thread starting\n");
  
  if (!rxf) {
    DBG("Error opening serial rx port\n");
    return;
  }
  
  if (txh < 0) {
    DBG("Error opening USB tx port\n");
    return;
  }
  
  while (1) {
    
    // ----- Wait for the host to configure -----
    
    DBG("Ser2Usb: Waiting for USB configuration\n");
    usbs_serial_wait_until_configured();
    cyg_thread_delay((cyg_tick_count_t) 10);
    
    // ----- While configured read data & send out serial port -----
    
    DBG("Ser2Usb: USB configured\n");
    while (usbs_serial_is_configured()) {
      if ((c = getc(rxf)) < 0) {
        DBG("*** Console Read Error: %d ***\n", c);
      }
      else {
        char ch = (char) c;
        write(txh, &ch, 1);
      }
    }
  }
}

// --------------------------------------------------------------------------
//  Application Startup
// --------------------------------------------------------------------------

void cyg_user_start(void)
{
  DBG("Entering cyg_user_start() function\n");
  
#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM
  // Override the class handler to use ours.
  usbs_serial_ep0->class_control_fn = acm_class_handler;
#endif
  
  cyg_thread_create(4, usb2ser_func, (cyg_addrword_t) 0,
                    "Usb2Serial", (void *) stack[0], THREAD_STACK_SIZE,
                    &usb2ser_thread, &thread[0]);
  
  cyg_thread_create(4, ser2usb_func, (cyg_addrword_t) 1,
                    "Serial2Usb", (void *) stack[1], THREAD_STACK_SIZE,
                    &ser2usb_thread, &thread[1]);
  
  // Start USB subsystem
  usbs_serial_start();
  
  // Start the threads running.
  cyg_thread_resume(usb2ser_thread);
  cyg_thread_resume(ser2usb_thread);
}

