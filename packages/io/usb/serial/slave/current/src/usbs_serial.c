//==========================================================================
//
//      usbs_serial.c
//
//      Support for slave-side USB serial devices.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Date:         2008-06-02
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/kernel/kapi.h>

#include <pkgconf/io_usb_slave_serial.h>
#include <cyg/io/usb/usbs_serial.h>
#include <string.h>

#if defined(CYGBLD_IO_USB_SLAVE_SERIAL_DEBUG)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

#define EP0_MAX_PACKET_SIZE     CYGNUM_IO_USB_SLAVE_SERIAL_EP0_MAX_PACKET_SIZE

extern usbs_control_endpoint    CYGDAT_IO_USB_SLAVE_SERIAL_EP0;
extern usbs_tx_endpoint         CYGDAT_IO_USB_SLAVE_SERIAL_TX_EP;
extern usbs_rx_endpoint         CYGDAT_IO_USB_SLAVE_SERIAL_RX_EP;

#define TX_EP_NUM               CYGNUM_IO_USB_SLAVE_SERIAL_TX_EP_NUM
#define RX_EP_NUM               CYGNUM_IO_USB_SLAVE_SERIAL_RX_EP_NUM
#define INTR_EP_NUM             CYGNUM_IO_USB_SLAVE_SERIAL_INTR_EP_NUM
#define EP0                     (&CYGDAT_IO_USB_SLAVE_SERIAL_EP0)
#define TX_EP                   (&CYGDAT_IO_USB_SLAVE_SERIAL_TX_EP)
#define RX_EP                   (&CYGDAT_IO_USB_SLAVE_SERIAL_RX_EP)
#define INTR_EP                 (&CYGDAT_IO_USB_SLAVE_SERIAL_INTR_EP)


#define VENDOR_ID               CYGNUM_IO_USB_SLAVE_SERIAL_VENDOR_ID
#define PRODUCT_ID              CYGNUM_IO_USB_SLAVE_SERIAL_PRODUCT_ID

#define USB_MAX_STR_LEN         256

#define LO_BYTE_16(word16)      ((cyg_uint8) ((word16) & 0xFF))
#define HI_BYTE_16(word16)      ((cyg_uint8) (((word16) >> 8) & 0xFF))

#define BYTE0_32(word32)        ((cyg_uint8) ((word32) & 0xFF))
#define BYTE1_32(word32)        ((cyg_uint8) (((word32) >>  8) & 0xFF))
#define BYTE2_32(word32)        ((cyg_uint8) (((word32) >> 16) & 0xFF))
#define BYTE3_32(word32)        ((cyg_uint8) (((word32) >> 24) & 0xFF))


#define MFG_STR_INDEX           '\x01'
#define PRODUCT_STR_INDEX       '\x02'

#define USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH(interfaces, endpoints) \
            (USB_CONFIGURATION_DESCRIPTOR_LENGTH +            \
            ((interfaces) * USB_INTERFACE_DESCRIPTOR_LENGTH) +  \
            ((endpoints)  * USB_ENDPOINT_DESCRIPTOR_LENGTH))


#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM
    #define USBS_SERIAL_DEVICE_CLASS        2
    #define USBS_SERIAL_NUM_IFACE           2
    #define USBS_SERIAL_NUM_ENDP            3
    #define USBS_SERIAL_DATA_IFACE_CLASS    0x0A    // Data
#else
    #define USBS_SERIAL_DEVICE_CLASS        0
    #define USBS_SERIAL_NUM_IFACE           1
    #define USBS_SERIAL_NUM_ENDP            2
    #define USBS_SERIAL_DATA_IFACE_CLASS    0xFF    // Vendor
#endif

// ----- Configuration Descriptor -----

static const usb_configuration_descriptor usb_configuration = {
    length:             sizeof(usb_configuration_descriptor),
    type:               USB_CONFIGURATION_DESCRIPTOR_TYPE,
    total_length_lo:    
        USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_LO(USBS_SERIAL_NUM_IFACE, 
                                                     USBS_SERIAL_NUM_ENDP),
    total_length_hi:    
        USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_HI(USBS_SERIAL_NUM_IFACE, 
                                                     USBS_SERIAL_NUM_ENDP),
    number_interfaces:  USBS_SERIAL_NUM_IFACE,
    configuration_id:   1,
    configuration_str:  0,
#ifdef CYGOPT_IO_USB_SLAVE_SERIAL_BUSPOWERED
    attributes:         (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED),
#else
    attributes:         (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
                         USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED),
#endif
    max_power:          (CYGNUM_IO_USB_SLAVE_SERIAL_CURRENTDRAW+1)/2	
};

// ----- Interface Descriptor -----

static const usb_interface_descriptor usb_interface[] = {

#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM
    {
        length:             sizeof(usb_interface_descriptor),
        type:               USB_INTERFACE_DESCRIPTOR_TYPE,
        interface_id:       0,
        alternate_setting:  0,
        number_endpoints:   1,
        interface_class:    0x02,   // Comm class
        interface_subclass: 0x02,
        interface_protocol: 0x01,
        interface_str:      0x00
    },
    {
        length:             sizeof(usb_interface_descriptor),
        type:               USB_INTERFACE_DESCRIPTOR_TYPE,
        interface_id:       1,
        alternate_setting:  0,
        number_endpoints:   2,
        interface_class:    USBS_SERIAL_DATA_IFACE_CLASS,
        interface_subclass: 0x00,
        interface_protocol: 0x00,
        interface_str:      0x00
    }
#else
    {
        length:             sizeof(usb_interface_descriptor),
        type:               USB_INTERFACE_DESCRIPTOR_TYPE,
        interface_id:       0,
        alternate_setting:  0,
        number_endpoints:   2,
        interface_class:    USBS_SERIAL_DATA_IFACE_CLASS,
        interface_subclass: 0x00,
        interface_protocol: 0x00,
        interface_str:      0x00
    }
#endif
};

// ----- Endpoint Descriptors -----

static const usb_endpoint_descriptor usb_endpoints[] =
{ 
#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM
    // Interrupt in endpoint
    {
        sizeof(usb_endpoint_descriptor),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | INTR_EP_NUM,
        USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT,
        0x40,
        0,
        255
    },
#endif

    // Tx (Bulk IN) Endpoint Descriptor
    {
        sizeof(usb_endpoint_descriptor),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN | TX_EP_NUM,
        USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        0x40,
        0,
        0
    },

    // Rx (Bulk OUT) Endpoint Descriptor
    {
        sizeof(usb_endpoint_descriptor),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT | RX_EP_NUM,
        USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        0x40,
        0,
        0
    }
};

// ----- String Descriptors -----

static char mfg_str_descr[USB_MAX_STR_LEN],
            product_str_descr[USB_MAX_STR_LEN];


static const char* usb_strings[] = {
    "\x04\x03\x09\x04",
    mfg_str_descr,
    product_str_descr
};

// ----- Enumeration Data w/ Device Descriptor -----

static usbs_enumeration_data usb_enum_data = {
    {
        length:                 sizeof(usb_device_descriptor),
        type:                   USB_DEVICE_DESCRIPTOR_TYPE,
        usb_spec_lo:            0x00, 
        usb_spec_hi:            0x02,
        device_class:           USBS_SERIAL_DEVICE_CLASS,
        device_subclass:        0,
        device_protocol:        0,
        max_packet_size:        EP0_MAX_PACKET_SIZE,
        vendor_lo:              LO_BYTE_16(VENDOR_ID),
        vendor_hi:              HI_BYTE_16(VENDOR_ID),
        product_lo:             LO_BYTE_16(PRODUCT_ID),
        product_hi:             HI_BYTE_16(PRODUCT_ID),
        device_lo:              0x00,
        device_hi:              0x00,
        manufacturer_str:       MFG_STR_INDEX,
        product_str:            PRODUCT_STR_INDEX,
        serial_number_str:      0,
        number_configurations:  1
    },

    total_number_interfaces:    USBS_SERIAL_NUM_IFACE,
    total_number_endpoints:     USBS_SERIAL_NUM_ENDP,
    total_number_strings:       3,
    configurations:             &usb_configuration,
    interfaces:                 usb_interface,
    endpoints:                  usb_endpoints,
    strings:                    (const unsigned char **) usb_strings
};

// --------------------------------------------------------------------------
// USBS Serial Data
// --------------------------------------------------------------------------

usbs_control_endpoint* usbs_serial_ep0 = EP0;

// Lock for the state.
cyg_mutex_t usbs_serial_lock;   

// Condition variable for state changes
cyg_cond_t  usbs_serial_state_cond;

int usbs_serial_state;

usbs_serial usbs_ser0 = {
    tx_ep:      TX_EP,
    rx_ep:      RX_EP,
    tx_result:  0,    
    rx_result:  0,    
};

static void (*usbs_serial_app_state_change_fn)(struct usbs_control_endpoint*, 
                                               void*, usbs_state_change, int) 
= 0;

// --------------------------------------------------------------------------
// Create a USB String Descriptor from a C string.

void
usbs_serial_create_str_descriptor(char descr[], const char *str)
{
    int i, n = strlen(str);

    if (n > (USB_MAX_STR_LEN/2 - 2))
        n = USB_MAX_STR_LEN/2 - 2;

    descr[0] = (cyg_uint8) (2*n + 2);
    descr[1] = USB_DEVREQ_DESCRIPTOR_TYPE_STRING;

    for (i=0; i<n; i++) {
        descr[i*2+2] = str[i];
        descr[i*2+3] = '\x00';
    }
}

// --------------------------------------------------------------------------
// ACM Class Handler
//
// For a Windows host, the device must, at least, respond to a SetLineCoding
// request (0x20), otherwise Windows will report that it's unable to open the 
// port. This request normally sets the standard serial parameters:
//          baud rate, # stop bits, parity, and # data bits
// If we're just making believe that we're a serial port to communicate with
// the host via USB, then these values don't matter. So we ACK the request,
// but ignore the parameters.
// If we were actually creating a USB-serial converter, then we would need to
// read these values and configure the serial port accordingly.
// 
// Similarly, the host can request the current settings through a 
// GetLineCoding request (0x21). Since we're just faking it, we return some
// arbitrary values: 38400,1,N,8

#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM

static usbs_control_return 
usbs_serial_acm_class_handler(usbs_control_endpoint* ep0, void* data)
{
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
  usb_devreq      *req = (usb_devreq *) ep0->control_buffer;
  static cyg_uint8 rsp_buf[32];
  cyg_uint32 baud = 38400;  // Arbitrary, fake value to return to the host.

  DBG("USB Serial ACM Class Handler: ");
  
  switch (req->request) {
    
    case USBS_SERIAL_SET_LINE_CODING :
      DBG("ACM Request: Set Line Coding\n");
      result = USBS_CONTROL_RETURN_HANDLED;
      break;
      
    case USBS_SERIAL_GET_LINE_CODING :
      DBG("ACM Request: Get Line Coding\n");
      rsp_buf[0] = BYTE0_32(baud);
      rsp_buf[1] = BYTE1_32(baud);
      rsp_buf[2] = BYTE2_32(baud);
      rsp_buf[3] = BYTE3_32(baud);
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
// Callback for a USB state change

void
usbs_serial_state_change_handler(usbs_control_endpoint* ep, void* data,
                                 usbs_state_change change, int prev_state)
{
#if defined(CYGBLD_IO_USB_SLAVE_SERIAL_DEBUG)
  const char *STATE_CHG_STR[] = { "Detached", "Attached", "Powered", "Reset",
                                  "Addressed", "Configured", "Deconfigured",
                                  "Suspended", "Resumed" };
  
  if (change > 0) {
    DBG("### %d:%s ###\n", change, STATE_CHG_STR[(int) change-1]);
  }
#endif
  
  // Called from DSR, cond broadcast should be ok without mutex lock
  usbs_serial_state = usbs_serial_ep0->state;
  cyg_cond_broadcast(&usbs_serial_state_cond);
  
  if (usbs_serial_app_state_change_fn)
    (*usbs_serial_app_state_change_fn)(ep, data, change, prev_state);
}

// --------------------------------------------------------------------------
// Block the calling thread until the USB is configured.
 
void
usbs_serial_wait_until_configured(void)
{
  cyg_mutex_lock(&usbs_serial_lock);
  while (usbs_serial_state != USBS_STATE_CONFIGURED)
    cyg_cond_wait(&usbs_serial_state_cond);
  cyg_mutex_unlock(&usbs_serial_lock);
}

// --------------------------------------------------------------------------
// Determine if the device is currently configured.

cyg_bool
usbs_serial_is_configured(void)
{
  return usbs_serial_state == USBS_STATE_CONFIGURED;
}

// --------------------------------------------------------------------------
// Callback for when a transmit is complete

static void 
usbs_serial_tx_complete(void *p, int result)
{
  usbs_serial* ser = (usbs_serial*) p;
  ser->tx_result = result;
  cyg_semaphore_post(&ser->tx_ready);
}

// --------------------------------------------------------------------------
// Callback for when a receive is complete

static void 
usbs_serial_rx_complete(void *p, int result)
{
  usbs_serial* ser = (usbs_serial*) p;
  ser->rx_result = result;
  cyg_semaphore_post(&ser->rx_ready);
}

// --------------------------------------------------------------------------
// Start an asynchronous transmit of a buffer.
// 
 
void
usbs_serial_start_tx(usbs_serial* ser, const void* buf, int n)
{
  usbs_start_tx_buffer(ser->tx_ep, (unsigned char*) buf, n,
                       usbs_serial_tx_complete, ser);
}

// --------------------------------------------------------------------------
// Block the caller until the transmit is complete

int
usbs_serial_wait_for_tx(usbs_serial* ser)
{
  cyg_semaphore_wait(&ser->tx_ready);
  return ser->tx_result;
}

// --------------------------------------------------------------------------
// Perform a synchronous transmit and wait for it to complete.

int
usbs_serial_tx(usbs_serial* ser, const void* buf, int n)
{
  usbs_serial_start_tx(ser, buf, n);
  return usbs_serial_wait_for_tx(ser);
}

// --------------------------------------------------------------------------
// Start an asynchronous receive of a buffer.

void
usbs_serial_start_rx(usbs_serial* ser, void* buf, int n)
{
  usbs_start_rx_buffer(ser->rx_ep, (unsigned char*) buf, n,
                       usbs_serial_rx_complete, ser);
}

// --------------------------------------------------------------------------
// Block the caller until the receive is complete

int
usbs_serial_wait_for_rx(usbs_serial* ser)
{
  cyg_semaphore_wait(&ser->rx_ready);
  return ser->rx_result;
}

// --------------------------------------------------------------------------
// Perform a synchronous receive and wait for it to complete.

int
usbs_serial_rx(usbs_serial* ser, void* buf, int n)
{
  usbs_serial_start_rx(ser, buf, n);
  return usbs_serial_wait_for_rx(ser);
}

// --------------------------------------------------------------------------
// Initialize a serial port structure.

void
usbs_serial_init(usbs_serial* ser, usbs_tx_endpoint* tx_ep, 
                 usbs_rx_endpoint* rx_ep)
{
  ser->tx_ep = tx_ep;
  ser->rx_ep = rx_ep;
  
  cyg_semaphore_init(&ser->tx_ready, 0);
  cyg_semaphore_init(&ser->rx_ready, 0);
}

// --------------------------------------------------------------------------
// Start the USB subsystem

void
usbs_serial_start(void)
{
  usbs_serial_init(&usbs_ser0, TX_EP, RX_EP);
  
  cyg_mutex_init(&usbs_serial_lock);
  cyg_cond_init(&usbs_serial_state_cond, &usbs_serial_lock);
  
  // Make the mfg & product names into USB string descriptors
  
  usbs_serial_create_str_descriptor(mfg_str_descr, 
                                    CYGDAT_IO_USB_SLAVE_SERIAL_MFG_STR);
  usbs_serial_create_str_descriptor(product_str_descr, 
                                    CYGDAT_IO_USB_SLAVE_SERIAL_PRODUCT_STR);
  
  // ----- Set up enumeration & USB callbacks -----
  
  usbs_serial_state = usbs_serial_ep0->state;
  
  usbs_serial_ep0->enumeration_data   = &usb_enum_data;
  
  if (usbs_serial_ep0->state_change_fn)
    usbs_serial_app_state_change_fn = usbs_serial_ep0->state_change_fn;
  
  usbs_serial_ep0->state_change_fn = usbs_serial_state_change_handler;
  
#ifdef CYGDAT_IO_USB_SLAVE_CLASS_TYPE_ACM
  if (!usbs_serial_ep0->class_control_fn)
    usbs_serial_ep0->class_control_fn = usbs_serial_acm_class_handler;
#endif
  
  // ----- Start USB subsystem -----
  
  usbs_start(usbs_serial_ep0);
}
