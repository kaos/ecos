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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           Harald Brandl (harald.brandl@fh-joanneum.at)
// Contributors:        Harald Brandl
// Date:                01.08.2004
// Purpose:             PHY chip configuration
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <stdio.h>
#include <pkgconf/devs_eth_arm_netarm.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_io.h>
#include "eth_regs.h"

#define PHYS(_i_) (0x800 | _i_)

#define SysReg (unsigned int*)0xffb00004        // System Status Register

/*  Function: void mii_poll_busy (void)
 *
 *  Description:
 *     This routine is responsible for waiting for the current PHY
 *     operation to complete.
 *
 *  Parameters:
 *     none
 */

void mii_poll_busy(void)
{
  /* check to see if PHY is busy with read or write */
  while (MIIIR & 1)
    HAL_DELAY_US(1);
}

/*  Function: void mii_reset (void)
 *
 *  Description:
 *
 *       This routine resets the PHY.
 *
 *  Return Values:
 *      none
 */

void mii_reset(void)
{
  MIIAR = PHYS(0);      // select command register
  MIIWDR = 0x8000;      // reset
  mii_poll_busy();
}

/*  Function: cyg_bool mii_negotiate (void)
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

cyg_bool mii_negotiate(void)
{
  int timeout = 100000;
  
  MIIAR = PHYS(4);
  
  mii_poll_busy();
  
  MIIAR = PHYS(0);
  MIIWDR |= 0x1200;
  
  mii_poll_busy();
  
  while(timeout)
    {
      MIIAR = PHYS(1);
      MIICR = 1;
      
      mii_poll_busy();
      
      if(0x24 == (MIIRDR & 0x24))
        return 0;
      else
        timeout--;
    }
  
  return 1;
}


/*  Function: void mii_set_speed (cyg_bool speed, cyg_bool duplex)
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

void mii_set_speed(cyg_bool speed, cyg_bool duplex)
{
  unsigned long int timeout = 1000000;
  
  MIIAR = PHYS(0);      // select command register
  MIIWDR = (speed << 13) | (duplex << 8);       // set speed and duplex
  mii_poll_busy();
  
  
  while(timeout)
    {
      MIIAR = PHYS(1);  // select status register
      MIICR = 1;
      mii_poll_busy();
      if((MIIRDR) & 0x4)
        break;
      timeout--;
    }
}

/*  Function: cyg_bool mii_check_speed
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

cyg_bool mii_check_speed(void)
{
  MIIAR = PHYS(17);
  MIICR = 1;
  mii_poll_busy();
  return (MIIRDR >> 14) & 1;
}

/*  Function: void mii_check_duplex
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

cyg_bool mii_check_duplex(void)
{
  MIIAR = PHYS(17);
  MIICR = 1;
  mii_poll_busy();
  return (MIIRDR >> 9) & 1;
}
