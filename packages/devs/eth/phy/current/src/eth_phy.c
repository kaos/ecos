//==========================================================================
//
//      dev/eth_phy.c
//
//      Ethernet transciever (PHY) support 
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
// Contributors: 
// Date:         2003-08-01
// Purpose:      
// Description:  API support for ethernet PHY
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>

#include <cyg/io/eth_phy.h>

// MII interface
#define MII_Start            0x40000000
#define MII_Read             0x20000000
#define MII_Write            0x10000000
#define MII_Cmd              0x30000000
#define MII_Phy(phy)         (phy << 23)
#define MII_Reg(reg)         (reg << 18)
#define MII_TA               0x00020000

//
// PHY unit access (via MII channel)
//

static cyg_uint32
phy_cmd(eth_phy_access_t *f, cyg_uint32 cmd)
{
    cyg_uint32  retval;
    int         i, off;
    bool        is_read = ((cmd & MII_Cmd) == MII_Read);

    // Set both bits as output
    (f->set_dir)(1);

    // Preamble
    for (i = 0; i < 32; i++) {
        (f->set_clock)(0);
        (f->set_data)(1);
        CYGACC_CALL_IF_DELAY_US(1);
        (f->set_clock)(1);
        CYGACC_CALL_IF_DELAY_US(1);
    }

    // Command/data
    for (i = 0, off = 31; i < (is_read ? 14 : 32); i++, --off) {
        (f->set_clock)(0);
        (f->set_data)((cmd >> off) & 0x00000001);
        CYGACC_CALL_IF_DELAY_US(1);
        (f->set_clock)(1);
        CYGACC_CALL_IF_DELAY_US(1);
    }

    retval = cmd;

    // If read, fetch data register
    if (is_read) {
        retval >>= 16;

        (f->set_clock)(0);
        (f->set_dir)(0);
        CYGACC_CALL_IF_DELAY_US(1);
        (f->set_clock)(1);
        CYGACC_CALL_IF_DELAY_US(1);
        (f->set_clock)(0);
        CYGACC_CALL_IF_DELAY_US(1);

        for (i = 0, off = 15; i < 16; i++, off--) {
            (f->set_clock)(1);
            retval <<= 1;
            retval |= (f->get_data)();
            CYGACC_CALL_IF_DELAY_US(1);
            (f->set_clock)(0);
            CYGACC_CALL_IF_DELAY_US(1);
        }
    }

    // Set both bits as output
    (f->set_dir)(1);

    // Postamble
    for (i = 0; i < 32; i++) {
        (f->set_clock)(0);
        (f->set_data)(1);
        CYGACC_CALL_IF_DELAY_US(1);
        (f->set_clock)(1);
        CYGACC_CALL_IF_DELAY_US(1);
    }

    return retval;
}

externC void
_eth_phy_init(eth_phy_access_t *f)
{
    (f->init)();
}

externC void
_eth_phy_write(eth_phy_access_t *f, int reg, int addr, unsigned short data)
{
    phy_cmd(f, MII_Start | MII_Write | MII_Phy(addr) | MII_Reg(reg) | MII_TA | data);
}

externC bool
_eth_phy_read(eth_phy_access_t *f, int reg, int addr, unsigned short *val)
{
    cyg_uint32 ret;

    ret = phy_cmd(f, MII_Start | MII_Read | MII_Phy(addr) | MII_Reg(reg) | MII_TA);
    *val = ret;
    return true;
}

externC void _eth_phy_init(eth_phy_access_t *f);
externC void _eth_phy_write(eth_phy_access_t *f, int reg, int unit, unsigned short data);
externC bool _eth_phy_read(eth_phy_access_t *f, int reg, int unit, unsigned short *val);
