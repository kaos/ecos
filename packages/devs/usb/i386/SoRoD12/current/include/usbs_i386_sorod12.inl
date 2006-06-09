#ifndef USBS_I386_SORO12_INL
#define USBS_I386_SORO12_INL
//==========================================================================
//
//      usbS_i386_sorod12.inl
//
//      Hardware specific parts for the SoRo D12 PC 104 USB card.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006, eCosCentric
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Frank M. Pagliughi (fmp), ASL
// Date:         2004-05-22
//
// This code is a hardware specific device driver for the SoRo Systems
// USB-D12-104, a PC/104 (ISA) Full-Speed USB slave board, which turns
// a PC/104 stack into a USB slave device. The board contains a
// Philips PDIUSBD12 Peripheral Controller Chip mapped into the PC's
// I/O space, with jumper-selectable I/O base address, IRQ, and DMA
// settings. The eCos config tool is used to adjust settings for this
// driver to match the physical jumper settings. The chip could run in
// polled mode without an IRQ, but this wouldn't be a great idea other
// than maybe a debug environment.

// ------------------------------------------------------------------------
//								Data-Only I/O                              
// ------------------------------------------------------------------------
//

#include <pkgconf/devs_usb_i386_sorod12.h>

// These routines read or write 8 bit values to the data area.

static inline byte d12_read_data_byte(d12_addr_type base_addr)
{
	#if defined(CYGSEM_DEVS_USB_I386_SOROD12_IO_MAPPED)
		byte val;
		HAL_READ_UINT8((unsigned) base_addr, val);
		return val;
	#else
		return *base_addr;
	#endif
}

static inline void d12_write_data_byte(d12_addr_type base_addr, byte val)
{
	#if defined(CYGSEM_DEVS_USB_I386_SOROD12_IO_MAPPED)
		HAL_WRITE_UINT8((unsigned) base_addr, val);
	#else
		*base_addr = val;
	#endif
}

// This routine writes a command to the device.

static inline void d12_write_cmd(d12_addr_type base_addr, byte cmd)
{
	#if defined(CYGSEM_DEVS_USB_I386_SOROD12_IO_MAPPED)
		HAL_WRITE_UINT8((unsigned) base_addr+1, cmd);
	#else
		*(base_addr+1) = cmd;
	#endif
}

// ------------------------------------------------------------------------
//	Reads 'n' bytes from the data address of the D12 and places them into
//	the buf[] array. Buf can be NULL, in which case the specified number of
//	bytes are read and discarded.

static uint8 d12_read_data(d12_addr_type base_addr, byte *buf, uint8 n)
{
	uint8 i;

	if (buf) {
		for (i=0; i<n; ++i)
			buf[i] = d12_read_data_byte(base_addr);
	}
	else {
		for (i=0; i<n; ++i)
			d12_read_data_byte(base_addr);
		n = 0;
	}

	return n;
}

// ------------------------------------------------------------------------
// Writes 'n' bytes out the data reg of the chip

static void d12_write_data(d12_addr_type base_addr, const byte *buf, uint8 n)
{
	uint8 i;

	for (i=0; i<n; ++i)
		d12_write_data_byte(base_addr, buf[i]);
}

#endif // USBS_I386_SORO12_INL
