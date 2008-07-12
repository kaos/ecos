//==========================================================================
//
//      ecos_usbserial.c
//
//      Kernel driver for the eCos USB serial driver
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), SoRo Systems, Inc.
// Contributors: 
// Date:         2008-06-02
// Description:  Kernel driver for the eCos USB serial driver
//
//####DESCRIPTIONEND####
//===========================================================================

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#define VENDOR_ID	0xFFFF
#define PRODUCT_ID	1

static struct usb_device_id id_table[] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ }
};

MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver ecos_usbserial_driver = {
	.name	 	= "ecos_usbserial",
	.probe		= usb_serial_probe,
	.disconnect	= usb_serial_disconnect,
	.id_table	= id_table
};

static struct usb_serial_driver ecos_usbserial_device = {
	.driver = {
		.owner			= THIS_MODULE,
		.name			= "ecos_usbserial",
	},
	.id_table			= id_table, 
	.num_interrupt_in	= NUM_DONT_CARE,
	.num_bulk_in		= NUM_DONT_CARE,
	.num_bulk_out		= NUM_DONT_CARE,
	.num_ports			= 1
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

