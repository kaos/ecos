//==========================================================================
//
//      ide.c
//
//      RedBoot IDE support
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    msalter
// Contributors: msalter
// Date:         2001-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <cyg/hal/hal_io.h>
#include <fs/disk.h>
#include <fs/ide.h>

static int ide_read(struct disk *d,
		    cyg_uint32 start_sector,
		    cyg_uint32 *buf,
		    cyg_uint8  nr_sectors);

static disk_funs_t ide_funs = { ide_read };

static struct ide_priv ide_privs[HAL_IDE_NUM_CONTROLLERS * 2];

static inline void
__wait_for_ready(int ctlr)
{
    cyg_uint8 status;
    do {
	HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
    } while (status & (IDE_STAT_BSY | IDE_STAT_DRQ));
}

static inline int
__wait_for_drq(int ctlr)
{
    cyg_uint8 status;

    CYGACC_CALL_IF_DELAY_US(10);
    do {
	HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
	if (status & IDE_STAT_DRQ)
	    return 1;
    } while (status & IDE_STAT_BSY);

    return 0;
}

static void
ide_reset(int ctlr)
{
    HAL_IDE_WRITE_CONTROL(ctlr, 6);	// polled mode, reset asserted
    CYGACC_CALL_IF_DELAY_US(5000);
    HAL_IDE_WRITE_CONTROL(ctlr, 2);	// polled mode, reset cleared
    CYGACC_CALL_IF_DELAY_US((cyg_uint32)50000);
    __wait_for_ready(ctlr);
}

static int
ide_ident(int ctlr, int dev, int is_packet_dev, cyg_uint16 *buf)
{
    int i;

    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE, dev << 4);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND, is_packet_dev ? 0xA1 : 0xEC);

    if (!__wait_for_drq(ctlr)) {
	cyg_uint8 status;

	HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);

	printf("%s: NO DRQ for ide%d, device %d. status[%02x]\n",
	       __FUNCTION__, ctlr, dev, status);
	return 0;
    }

    for (i = 0; i < (SECTOR_SIZE / sizeof(cyg_uint16)); i++, buf++)
	HAL_IDE_READ_UINT16(ctlr, IDE_REG_DATA, *buf);

    return 1;
}

static int
ide_read_sectors(int ctlr, int dev, cyg_uint32 start, cyg_uint8 count, cyg_uint16 *buf)
{
    int  i, j;
    cyg_uint16 *p;

    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COUNT, count);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBALOW, start & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAMID, (start >>  8) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAHI,  (start >> 16) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE,
			((start >> 24) & 0xf) | (dev << 4) | 0x40);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND, 0x20);

    for(p = buf, i = 0; i < count; i++) {

	if (!__wait_for_drq(ctlr)) {
	    printf("%s: NO DRQ for ide%d, device %d.\n",
		   __FUNCTION__, ctlr, dev);
	    return 0;
	}

	for (j = 0; j < (SECTOR_SIZE / sizeof(cyg_uint16)); j++, p++)
	    HAL_IDE_READ_UINT16(ctlr, IDE_REG_DATA, *p);
    }
    return 1;
}

static int
ide_read(struct disk *d,
	 cyg_uint32 start_sec, cyg_uint32 *buf, cyg_uint8 nr_secs)
{
    struct ide_priv *p = (struct ide_priv *)(d->private);

    return ide_read_sectors(p->controller, p->drive,
			    start_sec, nr_secs, (cyg_uint16 *)buf);
}


static void
ide_init(void)
{
    cyg_uint32 buf[SECTOR_SIZE/sizeof(cyg_uint32)];
    cyg_uint32 u32;
    cyg_uint16 u16;
    cyg_uint8  sig[2][4];
    int i, j;
    disk_t disk;
    struct ide_priv *priv;

    HAL_IDE_INIT();

    CYGACC_CALL_IF_DELAY_US(5);

    priv = ide_privs;
    for (i = 0; i < HAL_IDE_NUM_CONTROLLERS; i++) {

	// soft reset the devices on this controller
	ide_reset(i);

	// save off signature values for later
	HAL_IDE_WRITE_UINT8(i, IDE_REG_DEVICE, (0 << 4));
	HAL_IDE_READ_UINT8(i, IDE_REG_COUNT,  sig[0][0]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBALOW, sig[0][1]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBAMID, sig[0][2]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBAHI,  sig[0][3]);
	HAL_IDE_WRITE_UINT8(i, IDE_REG_DEVICE, (1 << 4));
	HAL_IDE_READ_UINT8(i, IDE_REG_COUNT,  sig[1][0]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBALOW, sig[1][1]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBAMID, sig[1][2]);
	HAL_IDE_READ_UINT8(i, IDE_REG_LBAHI,  sig[1][3]);

	// 2 devices per controller
	for (j = 0; j < 2; j++, priv++) {

	    priv->controller = i;
	    priv->drive = j;
	    priv->flags = 0;

	    if (sig[j][0] != 0x01 || sig[j][1] != 0x01)
		continue;

	    if (!(sig[j][2] == 0x00 && sig[j][3] == 0x00) &&
		!(sig[j][2] == 0x14 && sig[j][3] == 0xeb))
		continue;

	    // device present

	    priv->flags = IDE_DEV_PRESENT;

	    if (sig[j][2] == 0x14 && sig[j][3] == 0xeb)
		priv->flags |= IDE_DEV_PACKET;

	    if (ide_ident(i, j, priv->flags & IDE_DEV_PACKET, (cyg_uint16 *)buf) <= 0)
		continue;
    
	    memset(&disk, 0, sizeof(disk));
	    disk.funs = &ide_funs;
	    disk.private = priv;

	    disk.kind = DISK_IDE_HD;  // until proven otherwise

	    if (priv->flags & IDE_DEV_PACKET) {
		u16 = *(cyg_uint16 *)((char *)buf + IDE_DEVID_GENCONFIG);
		if (((u16 >> 8) & 0x1f) == 5)
		    disk.kind = DISK_IDE_CDROM;
	    } else {
		u32 = *(cyg_uint32 *)((char *)buf + IDE_DEVID_LBA_CAPACITY);
		disk.nr_sectors = u32;
	    }

	    if (!disk_register(&disk))
		return;
	}
    }
}

RedBoot_init(ide_init, RedBoot_INIT_FIRST);

