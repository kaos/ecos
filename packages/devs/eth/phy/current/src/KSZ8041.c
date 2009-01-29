//==========================================================================
//
//      dev/KSZ8041.c
//
//      Ethernet transceiver (PHY) support for Micrel KSZ8041
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler <uwe_kindler@web.de>
// Contributors: oli@snr.ch
// Date:         2008-09-18
// Purpose:
// Description:  Support for ethernet PHY Micrel KSZ8041
//
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                                INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/devs_eth_phy.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>

#include <cyg/io/eth_phy.h>
#include <cyg/io/eth_phy_dev.h>


//==========================================================================
//                                DEFINES
//==========================================================================
// 100BASE-TX PHY Control Register
#define PHY_100BASE_CTRL                0x1f	

#define PHY_100BASE_CTRL_OP_MODE_MASK   (0x07 << 2)
#define PHY_100BASE_CTRL_AN_MODE        (0x00 << 2)
#define PHY_100BASE_CTRL_10T_HDX        (0x01 << 2)
#define PHY_100BASE_CTRL_100T_HDX       (0x02 << 2)
#define PHY_100BASE_CTRL_DEFAULT        (0x03 << 2)
#define PHY_100BASE_CTRL_10T_FDX        (0x05 << 2)
#define PHY_100BASE_CTRL_100T_FDX       (0x06 << 2)

//==========================================================================
// Query the 100BASE-TX PHY Control Register and return a status bitmap
// indicating the state of the physical connection
//==========================================================================

#ifdef  CYGDBG_DEVS_ETH_PHY
void
ksz8041_diag (eth_phy_access_t * f)
{

  cyg_uint32 i;
  cyg_uint16 reg;

  eth_phy_printf ("KSZ8041 MIIM Register setings:\n");

  for (i = 0; i < 0x20; i++) {
    if (i % 2 == 0) {
      _eth_phy_read (f, i, f->phy_addr, &reg);
      eth_phy_printf ("r%02x: %04x ", i, reg);
    } else {
      _eth_phy_read (f, i, f->phy_addr, &reg);
      eth_phy_printf ("%04x\n", reg);
    }
  }
}
#endif

static bool
ksz8041_stat (eth_phy_access_t * f, int *state)
{

  cyg_uint16 phy_state;
  cyg_uint16 phy_100ctrl_reg;
  cyg_uint32 tries;
  cyg_uint32 ms;

#ifdef  CYGDBG_DEVS_ETH_PHY
  ksz8041_diag (f);
#endif

  if (_eth_phy_read (f, PHY_BMSR, f->phy_addr, &phy_state)) {
    if ((phy_state & PHY_BMSR_AUTO_NEG) == 0) {

      eth_phy_printf ("... waiting for auto-negotiation");

      for (tries = 0; tries < CYGINT_DEVS_ETH_PHY_AUTO_NEGOTIATION_TIME;
           tries++) {
        
        if (_eth_phy_read (f, PHY_BMSR, f->phy_addr, &phy_state)) {
          if ((phy_state & PHY_BMSR_AUTO_NEG) != 0) {
            break;
          }
        } else {
          eth_phy_printf ("error: _eth_phy_read()\n");
        }
        
        //
        // Wait for 1 second
        //
        for (ms = 0; ms < 1000; ++ms) {
          CYGACC_CALL_IF_DELAY_US (1000);	// 1 ms
        }
        eth_phy_printf (".");
      }
      eth_phy_printf ("\n");
    }
    
    if ((phy_state & PHY_BMSR_AUTO_NEG) != 0) {
      *state = 0;
      if ((phy_state & PHY_BMSR_LINK) != 0) {
        *state |= ETH_PHY_STAT_LINK;
      }
      
      _eth_phy_read (f, PHY_100BASE_CTRL, f->phy_addr, &phy_100ctrl_reg);
      phy_100ctrl_reg &= PHY_100BASE_CTRL_OP_MODE_MASK;
      switch (phy_100ctrl_reg) {
        case PHY_100BASE_CTRL_10T_HDX:
          break;
          
        case PHY_100BASE_CTRL_100T_HDX:
          *state |= ETH_PHY_STAT_100MB;
          break;
          
        case PHY_100BASE_CTRL_10T_FDX:
          *state |= ETH_PHY_STAT_FDX;
          break;
          
        case PHY_100BASE_CTRL_100T_FDX:
          *state |= ETH_PHY_STAT_100MB | ETH_PHY_STAT_FDX;
          break;
          
        default:
          // force to set default 100 Full Duplex
          *state |= ETH_PHY_STAT_100MB | ETH_PHY_STAT_FDX;
      }			// switch (phy_100ctrl_reg)
      
      return true;
    }
  }
  return false;
}

_eth_phy_dev ("Micrel KSZ8041", 0x00221512, ksz8041_stat)
