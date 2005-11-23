//==========================================================================
//
//      MII.c
//
//      NetSilion NET+ARM PHY chip configuration
//
//==========================================================================
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   	Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:  	Harald Brandl
// Date:        	01.08.2004
// Purpose:     	PHY chip configuration
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include "eth_regs.h"


#define PHYS(_i_) (0x800 | _i_)

#define SysReg (unsigned *)0xffb00004	// System Status Register

/*  Function: void cyg_netarm_mii_poll_busy (void)
 *
 *  Description:
 *     This routine is responsible for waiting for the current PHY
 *     operation to complete.
 *
 *  Parameters:
 *     none
 */

static void
mii_poll_busy(void)
{
	unsigned reg;
     /* check to see if PHY is busy with read or write */
    do
    {
		HAL_READ_UINT32(MIIIR, reg);
	}while (reg & 1);
}

/*  Function: void cyg_netarm_mii_reset (void)
 *
 *  Description:
 *
 *       This routine resets the PHY.
 *
 *  Return Values:
 *      none
 */

void
cyg_netarm_mii_reset(void)
{
     HAL_WRITE_UINT32(MIIAR, PHYS(0));	// select command register
     HAL_WRITE_UINT32(MIIWDR, 0x8000);	// reset
     mii_poll_busy();
}

/*  Function: cyg_bool cyg_netarm_mii_negotiate (void)
 *
 *  Description:
 *     This routine is responsible for causing the external Ethernet PHY
 *     to begin the negotatiation process.
 *
 *  Parameters:
 *     none
 *
 *  Return Value:
 *     0: SUCCESS
 *     1: ERROR
 */

cyg_bool
cyg_netarm_mii_negotiate(void)
{
     unsigned timeout = 100000, reg;

     HAL_WRITE_UINT32(MIIAR, PHYS(4));

     mii_poll_busy();

     HAL_WRITE_UINT32(MIIAR, PHYS(0));
     HAL_OR_UINT32(MIIWDR, 0x1200);

     mii_poll_busy();

     while(timeout)
     {
          HAL_WRITE_UINT32(MIIAR, PHYS(1));
          HAL_WRITE_UINT32(MIICR, 1);

          mii_poll_busy();

          HAL_READ_UINT32(MIIRDR, reg);

          if(0x24 == (reg & 0x24))
               return 0;
          else
               timeout--;
     }

     return 1;
}


/*  Function: void cyg_netarm_mii_set_speed (cyg_bool speed, cyg_bool duplex)
 *
 *  Description:
 *
 *       This routine will set the speed and duplex of the external PHY.
 *
 *  Parameters:
 *      Speed
 *        0: 10Mbit
 *        1: 100Mbit
 *      Duplex
 *        0: Half
 *        1: Full
 *
 *  Return Values:
 *      none
 */

void
cyg_netarm_mii_set_speed(cyg_uint32 speed, cyg_bool duplex)
{
     unsigned timeout = 1000000, reg;

     HAL_WRITE_UINT32(MIIAR, PHYS(0));	// select command register
     HAL_WRITE_UINT32(MIIWDR, (speed << 13) | (duplex << 8));	// set speed and duplex
     mii_poll_busy();

     while(timeout)
     {
          HAL_WRITE_UINT32(MIIAR, PHYS(1));	// select status register
          HAL_WRITE_UINT32(MIICR, 1);
          mii_poll_busy();
          HAL_READ_UINT32(MIIRDR, reg);
          if(reg & 0x4)
               break;
          timeout--;
     }
}

/*  Function: cyg_bool cyg_netarm_mii_check_speed
 *
 *  Description:
 *
 *       This routine will check the operating speed of the ethernet
 *       interface.
 *
 *  Parameters:
 *      none
 *
 *  Return Values:
 *      0: 10Mbit Speed
 *      1: 100Mbit Speed
 */

cyg_uint32
cyg_netarm_mii_check_speed(void)
{
	unsigned reg;

     HAL_WRITE_UINT32(MIIAR, PHYS(17));
     HAL_WRITE_UINT32(MIICR, 1);
     mii_poll_busy();
     HAL_READ_UINT32(MIIRDR, reg);
     return (reg >> 14) & 1;
}

/*  Function: void cyg_netarm_mii_check_duplex
 *
 *  Description:
 *
 *       This routine will check the operating duplex of the ethernet
 *       interface.
 *
 *  Parameters:
 *      none
 *
 *  Return Values:
 *      0: Half Duplex
 *      1: Full Duplex
 */

cyg_bool
cyg_netarm_mii_check_duplex(void)
{
	unsigned reg;

     HAL_WRITE_UINT32(MIIAR, PHYS(17));
     HAL_WRITE_UINT32(MIICR, 1);
     mii_poll_busy();
     HAL_READ_UINT32(MIIRDR, reg);
     return (reg >> 9) & 1;
}
