#ifndef CYGONCE_DEVS_DISK_MMC_PROTOCOL_H
#define CYGONCE_DEVS_DISK_MMC_PROTOCOL_H
//==========================================================================
//
//      mmc_protocol.h
//
//      Define the protocol used for interacting with MMC cards
//
//===========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2005 eCosCentric Limited
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       bartv
// Contributors: jlarmour
// Date:         2004-04-25
// Usage:        Only this package and implementing device drivers should
//               include this header file.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/cyg_type.h>       /* Common types */

// The MMC command set. A request is a six-byte sequence. First a
// single command byte, one of the following with bit 6 (0x40) or'd in
// and bit 7 clear as a start bit. Then a four-byte argument, usually
// a 32-bit integer most significant byte first. Finally a 7-bit CRC
// and a stop bit. In SPI mode the CRC is usually optional, except for
// the GO_IDLE_STATE command.
//
// Commands ALL_SEND_CID, SEND_RELATIVE_ADDR, SET_DSR,
// SELECT_DESELECT, READ_DATA_UNTIL_STOP, STOP_TRANSMISSION,
// GO_INACTIVE, READ_MULTIPLE_BLOCK, WRITE_DATA_UNTIL_STOP,
// WRITE_MULTIPLE_BLOCK, and PROGRAM_CID are MMC only, and not
// available in SPI mode.
//
// Device drivers may use values with bit 7 set to indicate
// driver-specific commands.

#define MMC_REQUEST_GO_IDLE_STATE           0x00
#define MMC_REQUEST_SEND_OP_COND            0x01
#define MMC_REQUEST_ALL_SEND_CID            0x02
#define MMC_REQUEST_SEND_RELATIVE_ADDR      0x03
#define MMC_REQUEST_SET_DSR                 0x04
#define MMC_REQUEST_SELECT_DESELECT         0x07
#define MMC_REQUEST_SEND_CSD                0x09
#define MMC_REQUEST_SEND_CID                0x0A
#define MMC_REQUEST_READ_DATA_UNTIL_STOP    0x0B
#define MMC_REQUEST_STOP_TRANSMISSION       0x0C
#define MMC_REQUEST_SEND_STATUS             0x0D
#define MMC_REQUEST_GO_INACTIVE             0x0F
#define MMC_REQUEST_SET_BLOCKLEN            0x10
#define MMC_REQUEST_READ_SINGLE_BLOCK       0x11
#define MMC_REQUEST_READ_MULTIPLE_BLOCK     0x12
#define MMC_REQUEST_WRITE_DATA_UNTIL_STOP   0x14
#define MMC_REQUEST_WRITE_BLOCK             0x18
#define MMC_REQUEST_WRITE_MULTIPLE_BLOCK    0x19
#define MMC_REQUEST_PROGRAM_CID             0x1A
#define MMC_REQUEST_PROGRAM_CSD             0x1B
#define MMC_REQUEST_SET_WRITE_PROT          0x1C
#define MMC_REQUEST_CLR_WRITE_PROT          0x1D
#define MMC_REQUEST_SEND_WRITE_PROT         0x1E
#define MMC_REQUEST_TAG_SECTOR_START        0x20
#define MMC_REQUEST_TAG_SECTOR_END          0x21
#define MMC_REQUEST_UNTAG_SECTOR            0x22
#define MMC_REQUEST_TAG_ERASE_GROUP_START   0x23
#define MMC_REQUEST_TAG_ERASE_GROUP_END     0x24
#define MMC_REQUEST_UNTAG_ERASE_GROUP       0x25
#define MMC_REQUEST_ERASE                   0x26
#define MMC_REQUEST_LOCK_UNLOCK             0x2A
#define MMC_REQUEST_READ_OCR                0x3A
#define MMC_REQUEST_CRC_ON_OFF              0x3B

// Response formats are different for MMC vs. SPI mode, so are defined
// in the appropriate source files.

// The CID register is generally treated as an opaque data structure
// used only for unique identification of cards.
typedef struct mmc_cid_register {
    cyg_uint8   cid_data[16];
} mmc_cid_register;

#define MMC_CID_REGISTER_MID(_data_)                    ((_data_)->cid_data[0])
#define MMC_CID_REGISTER_OID(_data_)                    (((_data_)->cid_data[1] << 8) | \
                                                         ((_data_)->cid_data[2]))
#define MMC_CID_REGISTER_PNM(_data_)                    ((const char*)&((_data_)->cid_data[3]))
#define MMC_CID_REGISTER_PRV(_data_)                    ((_data_)->cid_data[9])
#define MMC_CID_REGISTER_PSN(_data_)                    (((_data_)->cid_data[10] << 24) | \
                                                         ((_data_)->cid_data[11] << 16) | \
                                                         ((_data_)->cid_data[12] <<  8) | \
                                                         ((_data_)->cid_data[13]))
#define MMC_CID_REGISTER_MDT(_data_)                    ((_data_)->cid_data[14])
#define MMC_CID_REGISTER_CRC(_data_)                    ((_data_)->cid_data[15] >> 1)
                                                         

// The CSD register is just lots of small bitfields. For now keep it
// as an array of 16 bytes and provide macros to read the fields.
typedef struct mmc_csd_register {
    cyg_uint8   csd_data[16];
} mmc_csd_register;

#define MMC_CSD_REGISTER_CSD_STRUCTURE(_data_)          (((_data_)->csd_data[0] & 0x00C0) >> 6)
#define MMC_CSD_REGISTER_SPEC_VERS(_data_)              (((_data_)->csd_data[0] & 0x003C) >> 2)
#define MMC_CSD_REGISTER_TAAC_MANTISSA(_data_)          (((_data_)->csd_data[1] & 0x0078) >> 3)
#define MMC_CSD_REGISTER_TAAC_EXPONENT(_data_)          ((_data_)->csd_data[1] & 0x0007)
#define MMC_CSD_REGISTER_NSAC(_data_)                   ((_data_)->csd_data[2])
#define MMC_CSD_REGISTER_TRAN_SPEED_MANTISSA(_data_)    (((_data_)->csd_data[3] & 0x0078) >> 3)
#define MMC_CSD_REGISTER_TRAN_SPEED_EXPONENT(_data_)    ((_data_)->csd_data[3] & 0x0007)
#define MMC_CSD_REGISTER_CCC(_data_)                    (((_data_)->csd_data[4] << 4) | (((_data_)->csd_data[5] & 0x00F0) >> 4))
#define MMC_CSD_REGISTER_READ_BL_LEN(_data_)            ((_data_)->csd_data[5] & 0x000F)
#define MMC_CSD_REGISTER_READ_BL_PARTIAL(_data_)        (((_data_)->csd_data[6] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_WRITE_BLK_MISALIGN(_data_)     (((_data_)->csd_data[6] & 0x0040) >> 6)
#define MMC_CSD_REGISTER_READ_BLK_MISALIGN(_data_)      (((_data_)->csd_data[6] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_DSR_IMP(_data_)                (((_data_)->csd_data[6] & 0x0010) >> 4)
#define MMC_CSD_REGISTER_C_SIZE(_data_)                 ((((_data_)->csd_data[6] & 0x0003) << 10) |     \
                                                          ((_data_)->csd_data[7] << 2)            |     \
                                                          (((_data_)->csd_data[8] & 0x00C0) >> 6))
#define MMC_CSD_REGISTER_VDD_R_CURR_MIN(_data_)         (((_data_)->csd_data[8] & 0x0038)  >> 3)
#define MMC_CSD_REGISTER_VDD_R_CURR_MAX(_data_)         ((_data_)->csd_data[8] & 0x0007)
#define MMC_CSD_REGISTER_VDD_W_CURR_MIN(_data_)         (((_data_)->csd_data[9] & 0x00E0) >> 5)
#define MMC_CSD_REGISTER_VDD_W_CURR_MAX(_data_)         (((_data_)->csd_data[9] & 0x001C) >> 2)
#define MMC_CSD_REGISTER_C_SIZE_MULT(_data_)            ((((_data_)->csd_data[9] & 0x0003) << 1) |      \
                                                         (((_data_)->csd_data[10] & 0x0080) >> 7))
#define MMC_CSD_REGISTER_SECTOR_SIZE(_data_)            (((_data_)->csd_data[10] & 0x007C) >> 2)
#define MMC_CSD_REGISTER_ERASE_GRP_SIZE(_data_)         ((((_data_)->csd_data[10] & 0x0003) << 3) |     \
                                                         (((_data_)->csd_data[11] & 0x00E0) >> 5))
#define MMC_CSD_REGISTER_WR_GRP_SIZE(_data_)            ((_data_)->csd_data[11] & 0x001F)
#define MMC_CSD_REGISTER_WR_GRP_ENABLE(_data_)          (((_data_)->csd_data[12] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_DEFAULT_ECC(_data_)            (((_data_)->csd_data[12] & 0x0060) >> 5)
#define MMC_CSD_REGISTER_R2W_FACTOR(_data_)             (((_data_)->csd_data[12] & 0x001C) >> 2)
#define MMC_CSD_REGISTER_WRITE_BL_LEN(_data_)           ((((_data_)->csd_data[12] & 0x0003) << 2) |     \
                                                         (((_data_)->csd_data[13] & 0x00C0) >> 6))
#define MMC_CSD_REGISTER_WR_BL_PARTIAL(_data_)          (((_data_)->csd_data[13] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_FILE_FORMAT_GROUP(_data_)      (((_data_)->csd_data[14] & 0x0080) >> 7)
#define MMC_CSD_REGISTER_COPY(_data_)                   (((_data_)->csd_data[14] & 0x0040) >> 6)
#define MMC_CSD_REGISTER_PERM_WRITE_PROTECT(_data_)     (((_data_)->csd_data[14] & 0x0020) >> 5)
#define MMC_CSD_REGISTER_TMP_WRITE_PROTECT(_data_)      (((_data_)->csd_data[14] & 0x0010) >> 4)
#define MMC_CSD_REGISTER_FILE_FORMAT(_data_)            (((_data_)->csd_data[14] & 0x000C) >> 2)
#define MMC_CSD_REGISTER_ECC(_data_)                    ((_data_)->csd_data[14] & 0x0003)
#define MMC_CSD_REGISTER_CRC(_data_)                    (((_data_)->csd_data[15] & 0xFE) >> 1)


#endif //  ifdef CYGONCE_DEVS_DISK_MMC_PROTOCOL_H

/* EOF mmc_protocol.h */
