#ifndef CYGONCE_DEVS_ETH_PHY_H_
#define CYGONCE_DEVS_ETH_PHY_H_
//==========================================================================
//
//      eth_phy.h
//
//      API for ethernet transciever (PHY) support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003 Gary Thomas
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2003-08-01
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


// Transceiver mode
#define PHY_BMCR             0x00    // Register number
#define PHY_BMCR_RESET       0x8000
#define PHY_BMCR_LOOPBACK    0x4000
#define PHY_BMCR_100MB       0x2000
#define PHY_BMCR_AUTO_NEG    0x1000
#define PHY_BMCR_POWER_DOWN  0x0800
#define PHY_BMCR_ISOLATE     0x0400
#define PHY_BMCR_RESTART     0x0200
#define PHY_BMCR_FULL_DUPLEX 0x0100
#define PHY_BMCR_COLL_TEST   0x0080

#define PHY_BMSR             0x01    // Status register
#define PHY_BMSR_AUTO_NEG    0x0020  
#define PHY_BMSR_LINK        0x0004

// Physical device access - defined by hardware instance
typedef struct {
    void (*init)(void);
    void (*set_data)(int);
    int  (*get_data)(void);
    void (*set_clock)(int);
    void (*set_dir)(int);
} eth_phy_access_t;

#define ETH_PHY_ACCESS_FUNS(_l,_init,_set_data,_get_data,_set_clock,_set_dir) \
static eth_phy_access_t _l = {_init, _set_data, _get_data, _set_clock, _set_dir}

externC void _eth_phy_init(eth_phy_access_t *f);
externC void _eth_phy_write(eth_phy_access_t *f, int reg, int unit, unsigned short data);
externC bool _eth_phy_read(eth_phy_access_t *f, int reg, int unit, unsigned short *val);

#endif  // CYGONCE_DEVS_ETH_PHY_H_
// ------------------------------------------------------------------------
