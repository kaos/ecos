#ifndef CYGONCE_USBS_MSD_OPCODE_H
#define CYGONCE_USBS_MSD_OPCODE_H
//==========================================================================
//
//      include/usbs_msd_opcode.h
//
//      SCSI support for the USB slave-side MSD device
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2010 Free Software Foundation, Inc.
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
// Author(s):    ccoutand <ccoutand@stmi.com>
// Contributors:
// Date:         2010-06-02
// Purpose:
// Description:  USB slave-side MSD support
//
//
//####DESCRIPTIONEND####
//==========================================================================

#ifdef __cplusplus
extern "C" {
#endif

// SCSI commands from SBC-2 or SBC-3
#define USBS_SCSI_FORMAT_UNIT            0x04
#define USBS_SCSI_INQUIRY                0x12
#define USBS_SCSI_MODE_SELECT_6          0x15
#define USBS_SCSI_MODE_SELECT_10         0x55
#define USBS_SCSI_SENSE_6                0x1A
#define USBS_SCSI_SENSE_10               0x5A
#define USBS_SCSI_READ_6                 0x08
#define USBS_SCSI_READ_10                0x28
#define USBS_SCSI_READ_12                0xA8
#define USBS_SCSI_READ_CAPACITY          0x25
#define USBS_SCSI_READ_FORMAT_CAPACITIES 0x23
#define USBS_SCSI_READ_TOC               0x43
#define USBS_SCSI_REPORT_LUNS            0xA0
#define USBS_SCSI_REQUEST_SENSE          0x03
#define USBS_SCSI_SEND_DIAGNOSTIC        0x1D
#define USBS_SCSI_START_STOP_UNIT        0x1B
#define USBS_SCSI_SYNC_CACHE_10          0x35
#define USBS_SCSI_TEST_UNIT_READY        0x00
#define USBS_SCSI_VERIFY_10              0x2F
#define USBS_SCSI_WRITE_6                0x0A
#define USBS_SCSI_WRITE_10               0x2A
#define USBS_SCSI_WRITE_12               0xAA
#define USBS_SCSI_PREVENT_ALLOW_REMOVAL  0x1E

// Sense result
#define USBS_MSD_SCSI_SENSE_NOT_READY                            0x2
#define USBS_MSD_SCSI_SENSE_MEDIUM_ERROR                         0x3
#define USBS_MSD_SCSI_SENSE_ILLEGAL_REQUEST                      0x5
#define USBS_MSD_SCSI_SENSE_UNIT_ATTENTION                       0x6
#define USBS_MSD_SCSI_SENSE_ASC_LOGICAL_BLOCK_ADDR_OUT_OF_RANGE  0x21
#define USBS_MSD_SCSI_SENSE_ASCQ_LOGICAL_BLOCK_ADDR_OUT_OF_RANGE 0x0
#define USBS_MSD_SCSI_SENSE_ASC_INVALID_OPCODE                   0x20
#define USBS_MSD_SCSI_SENSE_ASCQ_INVALID_OPCODE                  0x0
#define USBS_MSD_SCSI_SENSE_ASC_MEDIUM_NOT_PRESENT               0x3a
#define USBS_MSD_SCSI_SENSE_ASCQ_MEDIUM_NOT_PRESENT              0x00
#define USBS_MSD_SCSI_SENSE_ASC_PERIPHERAL_DEVICE_WRITE_FAULT    0x03
#define USBS_MSD_SCSI_SENSE_ASCQ_PERIPHERAL_DEVICE_WRITE_FAULT   0x00
#define USBS_MSD_SCSI_SENSE_ASC_UNRECOVERED_READ_ERROR           0x11
#define USBS_MSD_SCSI_SENSE_ASCQ_UNRECOVERED_READ_ERROR          0x00
#define USBS_MSD_SCSI_SENSE_ASC_WRITE_ERROR                      0x0C
#define USBS_MSD_SCSI_SENSE_ASCQ_WRITE_ERROR                     0x00
#define USBS_MSD_SCSI_SENSE_ASC_WRITE_PROTECTED                  0x27
#define USBS_MSD_SCSI_SENSE_ASCQ_WRITE_PROTECTED                 0x00
#define USBS_MSD_SCSI_SENSE_ASC_INVALID_FIELD_IN_CBD             0x24

// Mode Page
#define USBS_MSD_SCSI_MODE_PAGE_CODE_ALL     0x3F
#define USBS_MSD_SCSI_MODE_PAGE_CODE_IEC     0x1C
#define USBS_MSD_SCSI_MODE_PAGE_CODE_CACHING 0x08
#define USBS_MSD_SCSI_MODE_PAGE_CODE_FDM     0x05

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_USBS_MSD_OPCODE_H
