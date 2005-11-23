#ifndef CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_H
#define CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_H

// ====================================================================
//
//      netarm_eth_drv.h
//
//      Device I/O - Description of NET+ARM ethernet hardware functions
//		and data structures
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
// Date:        		01.08.2004
// Purpose:     		Internal interfaces and data structures
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================

#define MaxKeys 20

typedef struct
{
	unsigned char *RxBuffer;
	unsigned short key_head, key_tail;
	unsigned KeyBuffer[MaxKeys];
}private_data_t;

typedef struct
{
	unsigned lo;
	unsigned hi;
}BDP_t;


static bool netarm_init(struct cyg_netdevtab_entry *tab);

static void netarm_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len);
static void netarm_deliver(struct eth_drv_sc *sc);
static int netarm_can_send(struct eth_drv_sc *sc);
static void netarm_send(
	struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, int total_len, unsigned long key);
static void netarm_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags);
static void netarm_stop(struct eth_drv_sc *sc);
static int netarm_control(struct eth_drv_sc *sc, unsigned long key, void *data, int len);
static void netarm_poll(struct eth_drv_sc *sc);
static int netarm_int_vector(struct eth_drv_sc *sc);

static void setMAC(unsigned char *esa);

#endif	// CYGONCE_DEVS_ETH_ARM_NETARM_ETH_DR_H
