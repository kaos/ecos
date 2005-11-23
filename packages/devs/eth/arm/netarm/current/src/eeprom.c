// ====================================================================
//
//      eeprom.c
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric Ltd.
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
//####ECOSGPLCOPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:        		10.03.2005
// Purpose:     		EEPROM I/O
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <cyg/hal/hal_modnet50.h>
#include "eth_regs.h"

static void wait(int time)
{
	int i;

	for(i=0; i < time; i++);
}

/* send i2c stop condition */
static void i2cStop(void)
{
	HAL_OR_UINT32(PORTC, 0x00c00000);
	HAL_AND_UINT32(PORTC, ~0x000000c0);	// SDA = 0, SLK = 0
	wait(5);
	HAL_OR_UINT32(PORTC, 0x00000040);	// SLK = 1
	wait(5);
	HAL_OR_UINT32(PORTC, 0x00000080);	// SDA = 1
}

/* send i2c start condition */
static void i2cStart(void)
{
	HAL_OR_UINT32(PORTC, 0x00c000c0);	// SDA = 1, SLK = 1
	wait(5);
	HAL_AND_UINT32(PORTC, ~0x00000080);	// SDA = 0
	wait(5);
	HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
}

static void i2cPutByte(char byte)
{
	int i, bit, reg;

	HAL_OR_UINT32(PORTC, 0x00c00000);

	for(i=7; i >= 0; i--)
	{
		bit = (byte >> i) & 1;
		HAL_READ_UINT32(PORTC, reg);
		HAL_WRITE_UINT32(PORTC, (reg & ~0x80) | (bit << 7));	// SDA = data
		HAL_OR_UINT32(PORTC, 0x00000040);	// SLK = 1
		wait(5);
		HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
		wait(5);
	}
	HAL_OR_UINT32(PORTC, 0x00000080);		// SDA = 1
}

static char i2cGetByte(void)
{
	int i, reg;
	char byte = 0;

	HAL_AND_UINT32(PORTC, ~0x00800000);
	for(i=7; i >= 0; i--)
	{
		HAL_OR_UINT32(PORTC, 0x00000040);	// SLK = 1
		HAL_READ_UINT32(PORTC, reg);
		byte |= ((reg & 0x80) >> 7) << i;	// data = SDA
		wait(5);
		HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
		wait(5);
	}
	HAL_OR_UINT32(PORTC, 0x00800080);		// SDA = 1
	return byte;
}

/* acknowledge received bytes */
static void i2cGiveAck(void)
{
	HAL_OR_UINT32(PORTC, 0x00c00000);
	HAL_AND_UINT32(PORTC, ~0x00000080);	// SDA = 0
	wait(5);
	HAL_OR_UINT32(PORTC, 0x00000040);	// SLK = 1
	wait(5);
	HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
	wait(5);
	HAL_OR_UINT32(PORTC, 0x00000080);	// SDA = 1
	wait(5);
}

/* wait for acknowledge from slaves */
static void i2cGetAck(void)
{
	unsigned reg;

	HAL_AND_UINT32(PORTC, ~0x00800000);	// SDA in
	HAL_OR_UINT32(PORTC, 0x00400040);	// SLK = 1
	do
	{
		HAL_READ_UINT32(PORTC, reg);
	}while(reg & 0x80);					// wait for SDA = 1
	HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
}

void cyg_netarm_initI2C(void)
{
	HAL_AND_UINT32(PORTC, ~0xc0000000);	// mode GPIO
	i2cStop();
}

/* wait until eeprom's internal write cycle has finished */
void cyg_netarm_eepromPollAck(int deviceAddr)
{
	unsigned reg;

	deviceAddr <<= 1;

	HAL_OR_UINT32(PORTC, 0x00400040);	// SLK = 1
	while(1)
	{
		i2cStart();
		i2cPutByte(deviceAddr);
		HAL_AND_UINT32(PORTC, ~0x00800000);		// SDA in
		HAL_OR_UINT32(PORTC, 0x00400040);		// SLK = 1
		HAL_READ_UINT32(PORTC, reg);
		if((reg & 0x80) == 0)
		{
			HAL_AND_UINT32(PORTC, ~0x00000040);	// SLK = 0
			break;
		}
		HAL_AND_UINT32(PORTC, ~0x00000040);		// SLK = 0
	}
}

/* reads numBytes from eeprom starting at readAddr into buf */
void cyg_netarm_eepromRead(int deviceAddr, int readAddr, char *buf, int numBytes)
{
	int i;

	deviceAddr <<= 1;
	i2cStart();
	i2cPutByte(deviceAddr);
	i2cGetAck();
	i2cPutByte(readAddr >> 8);
	i2cGetAck();
	i2cPutByte(readAddr & 0xff);
	i2cGetAck();
	i2cStart();
	i2cPutByte(deviceAddr | 1);		// set read flag
	i2cGetAck();

	for(i=0;i<numBytes;i++)
	{
		buf[i] = i2cGetByte();
		if(i < numBytes - 1)
			i2cGiveAck();
	}

	i2cStop();
}

/* writes up to a page of 32 bytes from buf into eeprom;
max. number of bytes depends on the offset within a page, indexed by the
lower 5 bits of writeAddr and equals 32 - offset */
void cyg_netarm_eepromWrite(int deviceAddr, int writeAddr, char *buf, int numBytes)
{
	int i;

	deviceAddr <<= 1;
	i2cStart();
	i2cPutByte(deviceAddr);
	i2cGetAck();
	i2cPutByte(writeAddr >> 8);
	i2cGetAck();
	i2cPutByte(writeAddr & 0xff);
	i2cGetAck();

	for(i=0; i<numBytes; i++)
	{
		i2cPutByte(buf[i]);
		i2cGetAck();
	}

	i2cStop();
}
