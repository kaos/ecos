// ====================================================================
//
//      eeprom.c
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005 eCosCentric LTD
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
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:                10.03.2005
// Purpose:             EEPROM I/O
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#include <cyg/hal/hal_netarm.h>
#include <sys/types.h>

static void wait(int time)
{
  int i;
  
  for(i=0; i < time; i++);
}

static void i2cStop(void)
{
  *PORTC |= 0x00c00000;
  *PORTC &= ~0x000000c0;  // SDA = 0, SLK = 0
  wait(5);
  *PORTC |= 0x00000040;   // SLK = 1
  wait(5);
  *PORTC |= 0x00000080;   // SDA = 1
}

static void i2cStart(void)
{
  *PORTC |= 0x00c000c0;   // SDA = 1, SLK = 1
  wait(5);
  *PORTC &= ~0x00000080;  // SDA = 0
  wait(5);
  *PORTC &= ~0x00000040;  // SLK = 0
}

static void i2cPutByte(char byte)
{
  int i, bit;
  
  *PORTC |= 0x00c00000;
  
  for(i=7; i >= 0; i--)
    {
      bit = (byte >> i) & 1;
      *PORTC = (*PORTC & ~0x80) | (bit << 7); // SDA = data
      *PORTC |= 0x00000040;   // SLK = 1
      wait(5);
      *PORTC &= ~0x00000040;  // SLK = 0
      wait(5);
    }
  *PORTC |= 0x00000080;           // SDA = 1
}

static char i2cGetByte(void)
{
  int i;
  char byte = 0;
  
  *PORTC &= ~0x00800000;
  for(i=7; i >= 0; i--)
    {
      *PORTC |= 0x00000040;   // SLK = 1
      byte |= ((*PORTC & 0x80) >> 7) << i;    // data = SDA
      wait(5);
      *PORTC &= ~0x00000040;  // SLK = 0
      wait(5);
    }
  *PORTC |= 0x00800080;           // SDA = 1
  return byte;
}

static void i2cGiveAck(void)
{
  *PORTC |= 0x00c00000;
  *PORTC &= ~0x00000080;  // SDA = 0
  wait(5);
  *PORTC |= 0x00000040;   // SLK = 1
  wait(5);
  *PORTC &= ~0x00000040;  // SLK = 0
  wait(5);
  *PORTC |= 0x00000080;   // SDA = 1
  wait(5);
}

static void i2cGetAck(void)
{
  *PORTC &= ~0x00800000;  // SDA in
  *PORTC |= 0x00400040;   // SLK = 1
  while(*PORTC & 0x80);   // wait for SDA = 1
  *PORTC &= ~0x00000040;  // SLK = 0
}

void initI2C(void)
{
  *PORTC &= ~0xc0000000;  // mode GPIO
  i2cStop();
}

void eepromPollAck(int deviceAddr)
{
  deviceAddr <<= 1;
  
  *PORTC |= 0x00400040;   // SLK = 1
  while(1)
    {
      i2cStart();
      i2cPutByte(deviceAddr);
      *PORTC &= ~0x00800000;          // SDA in
      *PORTC |= 0x00400040;           // SLK = 1
      if((*PORTC & 0x80) == 0)
        {
          *PORTC &= ~0x00000040;  // SLK = 0
          break;
        }
      *PORTC &= ~0x00000040;          // SLK = 0
    }
}

void eepromRead(int deviceAddr, int readAddr, char *buf, int numBytes)
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
  i2cPutByte(deviceAddr | 1);             // set read flag
  i2cGetAck();
  
  for(i=0;i<numBytes;i++)
    {
      buf[i] = i2cGetByte();
      if(i < numBytes - 1)
        i2cGiveAck();
    }
  
  i2cStop();
}

void eepromWrite(int deviceAddr, int writeAddr, char *buf, int numBytes)
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
