//==========================================================================
//
//      usbs_msd_scsi.c
//
//      Support for slave-side USB mass storage SCSI devices.
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
// Author(s):    ccoutand <ccoutand@stmi.com>
//
// Date:         2010-06-02
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <pkgconf/io_usb_slave_msd.h>
#include <cyg/io/usb/usbs_msd.h>

#include <cyg/io/usb/usbs_msd_opcode.h>
#include <cyg/io/usb/usbs_msd_scsi.h>

#include <cyg/io/disk.h>

#include <stdio.h> // for memcpy

#if defined(CYGBLD_IO_USB_SLAVE_MSD_DEBUG)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

#if defined(CYGBLD_IO_USB_SLAVE_MSD_TRACE)
#define TRACE diag_printf
#else
#define TRACE (1) ? (void)0 : diag_printf
#endif


const msd_scsi_inq_resp inq_resp = {
   USBS_SCSI_DIRECT_ACCESS_BLOCK_DEVICE, // Direct Access Block device
   USBS_SCSI_REMOVABLE_DEVICE,           // Device is removable
   0x04,                                 // Comply with SPC-2
   0x02,                                 // Standard format
   0x20,                                 // Response is 0x20 + 4 bytes
   0x00,
   0x00,
   0x00,
   {'e','C','o','s',' ',' ',' ',' '},
   {'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' '},
   {'0','.','0','1'},
};


// Handle Mode Sense Command
//
static inline void
usbs_msd_scsi_update_sense(msd_scsi_req_sense_resp *sense, \
                                cyg_uint8 asc, cyg_uint8 ascq, cyg_uint8 key )
{
   USBS_MSD_SCSI_SET_SENSE_ASC( sense->byte , asc );
   USBS_MSD_SCSI_SET_SENSE_ASCQ( sense->byte , ascq );
   USBS_MSD_SCSI_SET_SENSE_KEY( sense->byte , key );
}


// Handle Inquiry Request Command
//
static inline cyg_int32
usbs_msd_scsi_handle_inquiry( usbs_msd * msd )
{
   cyg_int32 tx_bytes = sizeof(msd_scsi_inq_resp);

   TRACE("USB Slave MSD: SCSI inquiry\n\r");

   // Copy over to message buffer
   memcpy( msd->buffer, (cyg_uint8 *) &inq_resp, tx_bytes );

   tx_bytes = msd->send( msd, msd->buffer, tx_bytes );

   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   return tx_bytes;
}


// Handle Read Capacity Command
//
static inline cyg_int32
usbs_msd_scsi_handle_capacity( usbs_msd * msd )
{
   cyg_disk_info_t disk_info;
   cyg_int32 tx_bytes = sizeof(cyg_uint32) << 1;
   cyg_uint32 len     = sizeof(cyg_disk_info_t);
   cyg_uint32 key     = CYG_IO_GET_CONFIG_DISK_INFO;
   cyg_io_handle_t lun = msd->lun->handle[msd->cbw.lun];

   TRACE("USB Slave MSD: SCSI capacity\n\r");

   // Get the configuration of a device
   cyg_io_get_config( lun, key, &disk_info, &len );

   msd->buffer[0] = BYTE3_32( disk_info.blocks_num );
   msd->buffer[1] = BYTE2_32( disk_info.blocks_num );
   msd->buffer[2] = BYTE1_32( disk_info.blocks_num );
   msd->buffer[3] = BYTE0_32( disk_info.blocks_num );

   msd->buffer[4] = BYTE3_32( disk_info.block_size );
   msd->buffer[5] = BYTE2_32( disk_info.block_size );
   msd->buffer[6] = BYTE1_32( disk_info.block_size );
   msd->buffer[7] = BYTE0_32( disk_info.block_size );

   tx_bytes = msd->send( msd, msd->buffer, tx_bytes );

   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   return tx_bytes;
}


// Handle Read Command
//
static inline cyg_int32
usbs_msd_scsi_handle_read( usbs_msd * msd, msd_scsi_req_sense_resp *sense )
{
   Cyg_ErrNo ret;
   cyg_uint32 transfert_len = 0, pos = 0, len = 1;
   cyg_int32 ret_len = 0;
   cyg_int32 tx_byte = 0;
   cyg_io_handle_t lun = msd->lun->handle[msd->cbw.lun];

   TRACE("USB Slave MSD: SCSI read\n\r");

   pos = ( msd->cbw.cb.data[2] << 24 ) | \
         ( msd->cbw.cb.data[3] << 16 ) | \
         ( msd->cbw.cb.data[4] <<  8 ) | \
           msd->cbw.cb.data[5];

   transfert_len = ( msd->cbw.cb.data[7] << 8 ) | msd->cbw.cb.data[8];

   // Read data from a block device
   while ( transfert_len > 0 ) {
     if( ( ret = cyg_io_bread( lun, msd->buffer, &len, pos)) != ENOERR )
       break;
     tx_byte = msd->send( msd, msd->buffer, (USBS_MSD_DISK_SECTOR_SIZE) );
     TRACE("USB Slave MSD: Read %d block, %d bytes\n\r", transfert_len, tx_byte);
     ret_len += tx_byte;
     // Substract 1 sector
     transfert_len -= 1;
     pos += 1;
   }

   if ( ret == ENOERR )
   {
      msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;
   }
   else
   {
     msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;
     usbs_msd_scsi_update_sense( sense, \
             USBS_MSD_SCSI_SENSE_ASC_UNRECOVERED_READ_ERROR, \
             USBS_MSD_SCSI_SENSE_ASCQ_UNRECOVERED_READ_ERROR, \
             USBS_MSD_SCSI_SENSE_MEDIUM_ERROR );
   }

   return ret_len;
}


// Handle Write Command
//
static inline cyg_int32
usbs_msd_scsi_handle_write( usbs_msd * msd , msd_scsi_req_sense_resp *sense )
{
   Cyg_ErrNo ret;
   cyg_uint32 transfert_len = 0, pos = 0, len = 1;
   cyg_int32 ret_len = 0;
   cyg_int32 rx_byte = 0;
   cyg_io_handle_t lun = msd->lun->handle[msd->cbw.lun];

   DBG("USB Slave MSD: SCSI Write\n\r");

   pos = ( msd->cbw.cb.data[2] << 24 ) | \
             ( msd->cbw.cb.data[3] << 16 ) | \
             ( msd->cbw.cb.data[4] <<  8 ) | \
               msd->cbw.cb.data[5];

   transfert_len = ( msd->cbw.cb.data[7] << 8 ) | msd->cbw.cb.data[8];

   // Read data from a block device
   while ( transfert_len > 0 ) {
     rx_byte = msd->receive( msd, msd->buffer, (USBS_MSD_DISK_SECTOR_SIZE) );
     if( ( ret = cyg_io_bwrite( lun, msd->buffer, &len, pos)) != ENOERR )
       break;
     TRACE("USB Slave MSD: Write %d block, %d bytes\n\r", transfert_len, rx_byte);
     ret_len += ( rx_byte );
     // Substract 1 sector
     transfert_len -= 1;
     pos += 1;
   }

   if ( ret == ENOERR )
   {
     msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;
   }
   else
   {
     msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;
     usbs_msd_scsi_update_sense( sense, \
             USBS_MSD_SCSI_SENSE_ASC_WRITE_ERROR, \
             USBS_MSD_SCSI_SENSE_ASCQ_WRITE_ERROR, \
             USBS_MSD_SCSI_SENSE_MEDIUM_ERROR );
   }

   return ret_len;
}


// Handle Request Sense Command
//
static inline cyg_int32
usbs_msd_scsi_handle_req_sense( usbs_msd * msd,  \
                                      msd_scsi_req_sense_resp *sense )
{
   cyg_uint8 i = 0;
   cyg_int32 tx_bytes = sizeof(msd_scsi_req_sense_resp);

   TRACE("USB Slave MSD: SCSI request sense\n\r");

   // Copy over to message buffer
   for(i = 0 ; i < tx_bytes ; i++)
       msd->buffer[i] = sense->byte[i];

   tx_bytes = msd->send( msd, msd->buffer, tx_bytes );

   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   return tx_bytes;
}


// Handle Mode Sense Command
//
static inline cyg_int32
usbs_msd_scsi_handle_sense( usbs_msd * msd,  \
                           msd_scsi_req_sense_resp *sense )
{
   cyg_int32 tx_bytes = 0x0;
   DBG("USB Slave MSD: SCSI sense\n\r");

   msd->buffer[0] = 0x03; // Number of byte in the following message
   msd->buffer[1] = 0x00; // Media type
   msd->buffer[2] = 0x00; // Not Write protected, no cache-control-bit support
   msd->buffer[3] = 0x00; // No Mode parameter

   // FIXME : handle different option correctly. For the time being,
   // just respond with minimum data
   if( ( msd->cbw.cb.data[2] & 0x3F ) == \
        USBS_MSD_SCSI_MODE_PAGE_CODE_ALL )
   {
     DBG("USB Slave MSD: SCSI sense all\n\r");

     tx_bytes = msd->send( msd, msd->buffer, 4 );

     msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;
   }
   else
   {
     DBG("USB Slave MSD: SCSI sense page: %x\n\r", \
             msd->cbw.cb.data[2]);

     tx_bytes = msd->send( msd, msd->buffer, 4 );

     msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;
   }

   return tx_bytes;
}


// Handle Test Unit Ready Command
//
static inline cyg_int32
usbs_msd_scsi_handle_test_unit_ready( usbs_msd * msd , \
                                         msd_scsi_req_sense_resp *sense )
{
   cyg_disk_info_t disk_info;
   Cyg_ErrNo ret;
   cyg_uint32 len = sizeof(cyg_disk_info_t);
   cyg_uint32 key = CYG_IO_GET_CONFIG_DISK_INFO;
   cyg_io_handle_t lun = msd->lun->handle[msd->cbw.lun];

   TRACE("USB Slave MSD: SCSI test unit ready\n\r");

   // Get the configuration of a device
   ret = cyg_io_get_config( lun, key, &disk_info, &len);

   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   USBS_MSD_SCSI_INIT_SENSE_DATA( sense->byte );

   // check if disk is connected / mounted here
   if( disk_info.connected == false )
   {
      DBG("USB Slave MSD: Disk not mounted \n\r");
      usbs_msd_scsi_update_sense( sense, \
             USBS_MSD_SCSI_SENSE_ASC_MEDIUM_NOT_PRESENT, \
             USBS_MSD_SCSI_SENSE_ASCQ_MEDIUM_NOT_PRESENT, \
             USBS_MSD_SCSI_SENSE_UNIT_ATTENTION);

      msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;
   }

   // No data to transfer
   return 0;
}


// Windows host operating system uses this command. Usually if
// a command is not supported, the CSW should return failure.
// Since no data is sent in the data transfer phase, the TX
// endpoint shall be stalled. Since some USB driver in eCos
// do not support endpoint stall, is it required to respond
// with some data, otherwise the driver would freeze. In the
// following case, we fill the data structure but report
// failure in the CSW.
#if !defined(CYGSEM_IO_USB_SLAVE_MSD_STALL_ENABLE)
// Handle Read Format Capacities
//
static inline cyg_int32
usbs_msd_scsi_read_format_capacities( usbs_msd * msd )
{
   cyg_int32 i = 0;
   DBG("USB Slave MSD: SCSI read format capacities\n\r");

   for(i = 0 ; i < msd->cbw.data_transfert_len; i++)
   {
     msd->buffer[i] = 0;
   }

   i = msd->send( msd, msd->buffer, i );

   msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;

   return i;
}
#endif


// Handle Verify
//
static inline cyg_int32
usbs_msd_scsi_handle_verify( usbs_msd * msd )
{
   DBG("USB Slave MSD: SCSI test unit verify\n\r");

   // FIXME: Should do something here?
   // Return success, command is optional
   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   // No data to transfer
   return 0;
}


// Handle Medium Removal
//
static inline cyg_int32
usbs_msd_scsi_handle_removal( usbs_msd * msd )
{
   DBG("USB Slave MSD: SCSI Prevent / Allow device removal, LUN %d, flag %d %d\n\r",
          ( msd->cbw.cb.data[1] & 0xf0 >> 4 ) , ( msd->cbw.cb.data[4] & 0x01 ),
            msd->cbw.data_transfert_len );

   // Support for this command is not mandatory.
   // No specific handling of this command is implemented.
   // Maybe mount / umount the disc?
   msd->csw.status = USBS_MSD_CSW_STATUS_PASSED;

   // No data to transfer
   return 0;
}


// Handle SCSI command
//
cyg_int32
usbs_msd_scsi_handle_cmd( usbs_msd * msd )
{
   bool ret;
   cyg_uint8 cmd = msd->cbw.cb.data[0];
   msd_scsi_req_sense_resp *sense = (msd_scsi_req_sense_resp *) msd->handler_data;

   switch( cmd )
   {
     case USBS_SCSI_INQUIRY:
        ret = usbs_msd_scsi_handle_inquiry( msd );
        break;

     case USBS_SCSI_READ_CAPACITY:
        ret = usbs_msd_scsi_handle_capacity( msd );
        break;

     case USBS_SCSI_READ_10:
        ret = usbs_msd_scsi_handle_read(msd , sense );
        break;

     case USBS_SCSI_WRITE_10:
        ret = usbs_msd_scsi_handle_write( msd , sense );
        break;

     case USBS_SCSI_REQUEST_SENSE:
        ret = usbs_msd_scsi_handle_req_sense( msd , sense );
        break;

     case USBS_SCSI_SENSE_6:
        ret = usbs_msd_scsi_handle_sense( msd , sense);
        break;

     case USBS_SCSI_TEST_UNIT_READY:
        ret = usbs_msd_scsi_handle_test_unit_ready( msd , sense );
        break;

     case USBS_SCSI_VERIFY_10:
        ret = usbs_msd_scsi_handle_verify( msd );
        break;

#if !defined(CYGSEM_IO_USB_SLAVE_MSD_STALL_ENABLE)
     case USBS_SCSI_READ_FORMAT_CAPACITIES:
        ret = usbs_msd_scsi_read_format_capacities( msd );
        break;
#endif

     case USBS_SCSI_PREVENT_ALLOW_REMOVAL:
        ret = usbs_msd_scsi_handle_removal( msd );
        break;

     default:
        // Use for all commands not implemented, not
        // supported
        DBG("USB Slave MSD: SCSI illegal request %x \n\r", cmd );
        USBS_MSD_SCSI_INIT_SENSE_DATA( sense->byte );
        usbs_msd_scsi_update_sense( sense, \
                USBS_MSD_SCSI_SENSE_ASC_INVALID_OPCODE, \
                USBS_MSD_SCSI_SENSE_ASCQ_INVALID_OPCODE, \
                USBS_MSD_SCSI_SENSE_ILLEGAL_REQUEST);
        msd->csw.status = USBS_MSD_CSW_STATUS_FAILED;
        ret = 0;
        break;

   }

   return ret;

}

// SCSI initialization
//
bool
usbs_msd_scsi_init( void ** ctxt )
{
  static msd_scsi_req_sense_resp sense;

  DBG("USB Slave MSD: SCSI init %p\n", &sense);

  USBS_MSD_SCSI_INIT_SENSE_DATA( sense.byte );

  *ctxt = &sense;

  return true;
}
