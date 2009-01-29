//==========================================================================
//
//	ecos_usbserial.c
//
//	Kernel driver for the eCos USB serial driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):	 Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Contributors: 
// Date:	 2008-06-02
// Description:	 Kernel driver for the eCos USB serial driver
//
//####DESCRIPTIONEND####
//===========================================================================

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#define VENDOR_ID	0xFFFF
#define PRODUCT_ID	1

static int debug;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) 
static inline int 
usb_endpoint_xfer_bulk(const struct usb_endpoint_descriptor *endpoint) {

	return (endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK;
}

static inline int 
usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd) {

	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN;
}
#endif

/* Our probe function will detect if the interface has sufficient bulk
 * in and out endpoints to be useful. The ACM interface only has an
 * interrupt endpoint, so we don't want a serial device bound to it.
 */

static int ecos_usbserial_probe(struct usb_serial *serial,
				const struct usb_device_id *id)
{
	struct usb_interface *interface = serial->interface;
	struct usb_host_interface *iface_desc; 
	struct usb_endpoint_descriptor *endpoint;
	int num_bulk_in = 0;
	int num_bulk_out = 0;
	int i;
	
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		
		if (usb_endpoint_xfer_bulk(endpoint)) {
			if (usb_endpoint_dir_in(endpoint)) {
				/* we found a bulk in endpoint */
				dbg("found bulk in on endpoint %d", i);
				++num_bulk_in;
			} else {
				/* we found a bulk out endpoint */
				dbg("found bulk out on endpoint %d", i);
				++num_bulk_out;
			}
		}
	}
	
	if (!num_bulk_in || !num_bulk_out) {
		info("Ignoring interface, insufficient endpoints");
		return -ENODEV;
	}
	return 0;
}

static struct usb_device_id id_table[] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ }
};

MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver ecos_usbserial_driver = {
	.name		= "ecos_usbserial",
	.probe		= usb_serial_probe,
	.disconnect	= usb_serial_disconnect,
	.id_table	= id_table
};

static struct usb_serial_driver ecos_usbserial_device = {
	.driver = {
		.owner			= THIS_MODULE,
		.name			= "ecos_usbserial",
	},
	.id_table		= id_table,
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,25) 
	.num_interrupt_in	= NUM_DONT_CARE,
	.num_bulk_in		= NUM_DONT_CARE,
	.num_bulk_out		= NUM_DONT_CARE,
#else
	.usb_driver		= &ecos_usbserial_driver,
#endif
	.num_ports		= 1,
	.probe			= ecos_usbserial_probe,

};

static int __init ecos_usbserial_init(void)
{
	int retval;

	retval = usb_serial_register(&ecos_usbserial_device);
	if (retval)
		return retval;

	retval = usb_register(&ecos_usbserial_driver);
	if (retval) {
		usb_serial_deregister(&ecos_usbserial_device);
		return retval;
	}

	return 0;
}

static void __exit ecos_usbserial_exit(void)
{
	usb_deregister(&ecos_usbserial_driver);
	usb_serial_deregister(&ecos_usbserial_device);
}

module_init(ecos_usbserial_init);
module_exit(ecos_usbserial_exit);

MODULE_LICENSE("GPL");
module_param(debug, bool, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug enabled or not");

