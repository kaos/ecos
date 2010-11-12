//==========================================================================
//
//      usbs_msd_handler.c
//
//      Support for slave-side USB mass storage devices.
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
// Author(s):    ccoutand
//
// Date:         2010-06-02
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_endian.h>
#include <cyg/hal/drv_api.h>
#include <cyg/kernel/kapi.h>

#include <pkgconf/io_usb_slave_msd.h>
#include <cyg/io/usb/usbs_msd.h>

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

#define CONSTRUCT_CSW( _csw_, _tag_ ) \
{ \
   _csw_.tag = _tag_; \
   _csw_.signature = USBS_MSD_CSW_SIGNATURE; \
};


// Validate a command block wrapper
// CBW is sent from host in little endian format, hence it
// must be converted to the CPU endianness
//
static inline bool
usbs_msd_validate_cbw(usbs_msd_cbw *cbw, cyg_int32 len)
{

  // Convert to CPU endianness
  cbw->signature = CYG_LE32_TO_CPU( cbw->signature );
  cbw->tag       = CYG_LE32_TO_CPU( cbw->tag );
  cbw->data_transfert_len = CYG_LE32_TO_CPU( cbw->data_transfert_len );

  if( len != sizeof( usbs_msd_cbw ) ||
     cbw->signature != USBS_MSD_CBW_SIGNATURE )
  {
    DBG("USB Slave MSD: CBW signature %x, len %d\n\r", \
      cbw->signature, len);
    return false;
  }
  else
    return true;
}


// Validate a command block wrapper content
//
static inline bool
usbs_msd_validate_cbw_data(usbs_msd_cbw *cbw)
{
  if( ( cbw->lun < USBS_MSD_CBW_MAX_LUN ) && \
      ( cbw->cb.len <= USBS_MSD_CBW_MAX_SCSI_CMD_LEN ) && \
      ( cbw->cb.len >= USBS_MSD_CBW_MIN_SCSI_CMD_LEN ) && \
      ( ( cbw->flags == USBS_MSD_CBW_HOST2DEVICE ) || \
        ( cbw->flags == USBS_MSD_CBW_DEVICE2HOST ) ) )
  {
    return true;
  }
  else
  {
    DBG("USB Slave MSD: CBW Invalid data, LUN %d, len %d, flag %x\n\r", \
        cbw->lun, cbw->cb.len, cbw->flags);
  }
  return false;
}


// Send Command Status Wrapper
//
static inline cyg_int32
usbs_msd_send_csw( usbs_msd* msd )
{
  // Convert to host endianness
  msd->csw.signature = CYG_CPU_TO_LE32( msd->csw.signature );
  msd->csw.tag       = CYG_CPU_TO_LE32( msd->csw.tag );
  msd->csw.data_residue = CYG_CPU_TO_LE32( msd->csw.data_residue );

  return msd->send( msd, &msd->csw, sizeof( usbs_msd_csw ));
}


// Receive Command Status Wrapper
//
static cyg_int32
usbs_msd_wait_for_cbw( usbs_msd *msd )
{
  cyg_uint8 cbw[sizeof( usbs_msd_cbw ) + 1];
  cyg_int32 n;

  TRACE("USB Slave MSD: Wait for CBW\n\r");
  n = msd->receive( msd, cbw, sizeof( cbw ) );

  TRACE("USB Slave MSD: CBW received, len %d\n\r", n);

  if ( n < 0)
    return n;

  memcpy(&msd->cbw, cbw, sizeof( usbs_msd_cbw ));

  if ( ( true == usbs_msd_validate_cbw( &msd->cbw, n ) ) && \
          ( true == usbs_msd_validate_cbw_data( &msd->cbw ) ) )
  {
    CONSTRUCT_CSW( msd->csw , msd->cbw.tag );

    if( msd->cbw.flags == USBS_MSD_CBW_DEVICE2HOST )
    {
      msd->state = CYG_USBS_MSD_DATA_OUT;
    }
    else if ( msd->cbw.flags == USBS_MSD_CBW_HOST2DEVICE )
    {
      msd->state = CYG_USBS_MSD_DATA_IN;
    }
  }

  return n;
}

// USB slave side Mass-Storage Handler
//
void
usbs_msd_handler( cyg_addrword_t data )
{
  usbs_msd* msd = (usbs_msd*) data;
  cyg_int32 transfered = 0;
  cyg_int32 received   = 0;

  while(1)
  {

    // Wait for device to be configured
    DBG("USB Slave MSD: Wait for device configuration\n\r");
    usbs_msd_wait_until_configured( msd );
    cyg_thread_delay( (cyg_tick_count_t) 10 );

    while(1)
    {

      switch( msd->state )
      {
         case CYG_USBS_MSD_WAIT:
            // FIXME, check for return values
            // For the time being, -EPIPE and -EAGAIN triggers the
            // driver to wait for the device to be configured again.
            received = usbs_msd_wait_for_cbw( msd );
            if ( received < 0 )
              break;
            // Fall through

         case CYG_USBS_MSD_DATA_IN:
         case CYG_USBS_MSD_DATA_OUT:
            // Handle data transfer IN and OUT
            transfered = msd->handler_cmd( msd );

            // Data transfer failed because of -EPIPE or -EAGAIN. For
            // the time being just wait for the device to be re-configured
            if(transfered < 0)
            {
              DBG("USB Slave MSD: Data transfer error\n\r");
              msd->state = CYG_USBS_MSD_WAIT;
              break;
            }

            // Compute the data residue value to be recorded in the
            // Command Status Wrapper
            msd->csw.data_residue = msd->cbw.data_transfert_len - transfered;

            // Check if all data has been transfered / received to / from
            // the host
            if( msd->csw.data_residue != 0 )
            {
              DBG("USB Slave MSD: Transfer not complete %d / %d\n\r", \
                        msd->cbw.data_transfert_len, transfered);

              // Stall endpoint, this is a waiting call until the host has
              // cleared the stall. AT91 USB driver does not seem to handle
              // it!
              msd->stall( msd, CYG_USBS_EP_TX );
#if defined(CYGSEM_IO_USB_SLAVE_MSD_STALL_ENABLE)
              TRACE("USB Slave MSD: Stall completed\n\r");
#endif
            }

            // Complete the transaction by sending the command status
            usbs_msd_send_csw( msd );

            TRACE("USB Slave MSD: CSW sent\n\r");

            // Wait for next command
            msd->state = CYG_USBS_MSD_WAIT;
            break;

         default:
            DBG("USB Slave MSD: Invalid state\n\r");
            break;
      }

      if( ( transfered < 0 ) || ( received < 0 ) )
        break;

    }
  }
}


