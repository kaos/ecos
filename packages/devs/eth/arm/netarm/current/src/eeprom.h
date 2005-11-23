#ifndef CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_EEPROM_H
#define CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_EEPROM_H

// ====================================================================
//
//      eeprom.h
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
// Purpose:     		EEPROM interface
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


/* a block of max. 32 bytes can be written at once */
void cyg_netarm_eepromWrite(int deviceAddr, int writeAddr, char *buf, int numBytes);
/* reads out as many bytes as desired */
void cyg_netarm_eepromRead(int deviceAddr, int readAddr, char *buf, int numBytes);
/* blocks until the internal write cycle, initiated after eepromWrite, completes */
void cyg_netarm_eepromPollAck(int deviceAddr);
void cyg_netarm_initI2C(void);

#endif	// CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_EEPROM_H
