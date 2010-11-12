//==========================================================================
//
//      usbs_msd.c
//
//      Support for slave-side USB mass storage devices.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2010 Free Software Foundation, Inc.
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
// Contributors: jld
// Date:         2010-05-27
//
//####DESCRIPTIONEND####
//
//==========================================================================
#include <pkgconf/system.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/kernel/kapi.h>

#include <pkgconf/io_usb_slave_msd.h>
#include <cyg/io/usb/usbs_msd.h>

#ifdef CYGDAT_IO_USB_SLAVE_MSD0_SUB_CLASS_TYPE_SCSI
#include <cyg/io/usb/usbs_msd_scsi.h>
#endif

#include <string.h>

#if defined(CYGBLD_IO_USB_SLAVE_MSD_DEBUG)
#define DBG diag_printf
#else
#define DBG (1) ? (void)0 : diag_printf
#endif

#include CYGDAT_IO_USB_SLAVE_MSD_INL // Declaration of the Mass Storage devices

// --------------------------------------------------------------------------
// Create a USB String Descriptor from a C string.
//
void
usbs_msd_create_str_descriptor(char descr[], const char *str)
{
    cyg_uint32 i, n = strlen(str);

    if (n > (USB_MAX_STR_LEN/2 - 2))
        n = USB_MAX_STR_LEN/2 - 2;

    descr[0] = (cyg_uint8) (2*n + 2);
    descr[1] = USB_DEVREQ_DESCRIPTOR_TYPE_STRING;

    for (i=0; i<n; i++) {
        descr[i*2+2] = str[i];
        descr[i*2+3] = '\x00';
    }
}

// --------------------------------------------------------------------------
// Mass Storage Device Class Handler
//
static usbs_control_return
usbs_msd_class_handler(usbs_control_endpoint* ep0, void* callback_data)
{
  usbs_control_return result = USBS_CONTROL_RETURN_UNKNOWN;
  usb_devreq *req = (usb_devreq *) ep0->control_buffer;
  usbs_msd   *msd = (usbs_msd *) callback_data;
  static cyg_uint8 rsp_buf[32];

  DBG("USB MSD Class Handler: ");

  switch (req->request) {

    case USBS_MSD_CLASS_REQ_GET_MAX_LUN :
      DBG("USB MSD Request: Get MAX LUN %d\n", \
                                 msd->lun->max_lun);
      rsp_buf[0]  = msd->lun->max_lun - 1;
      ep0->buffer = rsp_buf;
      ep0->buffer_size = 1;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

    case USBS_MSD_CLASS_REQ_BOMSR :
      DBG("USB MSD Request: Reset\n");
      // FIXME : Handle Reset
      ep0->buffer_size = 0;
      result = USBS_CONTROL_RETURN_HANDLED;
      break;

    default :
      DBG("*** USB MSD Request: 0x%02X not implemented !***\n",
          (unsigned) req->request);
  }

  return result;
}


// --------------------------------------------------------------------------
// Callback for a USB state change
//
void
usbs_msd_state_change_handler(usbs_control_endpoint* ep, void* callback_data,
                                 usbs_state_change change, int prev_state)
{
  usbs_msd *msd = (usbs_msd *) callback_data;

#if defined(CYGBLD_IO_USB_SLAVE_MSD_DEBUG)
  const char *STATE_CHG_STR[] = { "Detached", "Attached", "Powered", "Reset",
                                  "Addressed", "Configured", "Deconfigured",
                                  "Suspended", "Resumed" };

  if (change > 0 && change < 10) {
    DBG("### %d:%s ###\n", change, STATE_CHG_STR[(int) change-1]);
  }
  else {
    DBG("### %d ###\n", change);
  }
#endif

  // Called from DSR, cond broadcast should be ok without mutex lock
  msd->usb_state = msd->ctrl_ep->state;

  cyg_cond_broadcast( &msd->state_cond );

  if (msd->app_state_change_fn)
    (*msd->app_state_change_fn)(ep, callback_data, change, prev_state);

}


// --------------------------------------------------------------------------
// Block the calling thread until the USB is configured.
 //
void
usbs_msd_wait_until_configured( usbs_msd* msd )
{
  cyg_mutex_lock( &msd->lock );
  while ( msd->usb_state != USBS_STATE_CONFIGURED )
    cyg_cond_wait( &msd->state_cond );

  // Dynamic end-points configuration
  if ( msd->static_ep == false )
  {
    msd->tx_ep = usbs_get_tx_endpoint( msd->ctrl_ep, msd->tx_ep_num );
    msd->rx_ep = usbs_get_rx_endpoint( msd->ctrl_ep, msd->rx_ep_num );
    DBG("tx_ep = %p\n", msd->tx_ep);
    DBG("rx_ep = %p\n", msd->rx_ep);
  }

  cyg_mutex_unlock( &msd->lock );
}


// --------------------------------------------------------------------------
// Determine if the device is currently configured.
//
cyg_bool
usbs_msd_is_configured( usbs_msd* msd )
{
  return msd->usb_state == USBS_STATE_CONFIGURED;
}


// --------------------------------------------------------------------------
// Callback for when a transmit / stall is complete
//
static void
usbs_msd_tx_complete( void *p, cyg_int32 result )
{
  usbs_msd* msd  = (usbs_msd*) p;
  msd->tx.result = result;
  cyg_semaphore_post( &msd->tx.ready );
}


// --------------------------------------------------------------------------
// Callback for when a receive / stall is complete
//
static void
usbs_msd_rx_complete( void *p, cyg_int32 result )
{
  usbs_msd* msd  = (usbs_msd*) p;
  msd->rx.result = result;
  cyg_semaphore_post( &msd->rx.ready );
}


// --------------------------------------------------------------------------
// Start an asynchronous transmit of a buffer.
//
static void
usbs_msd_start_tx(usbs_msd* msd, const void* buf, cyg_int32 n)
{
  usbs_start_tx_buffer( msd->tx_ep, (unsigned char*) buf, n,
                       usbs_msd_tx_complete, msd );
}


// --------------------------------------------------------------------------
// Block the caller until the transmit / stall is complete
//
static int
usbs_msd_wait_for_tx(usbs_msd* msd)
{
  cyg_semaphore_wait( &msd->tx.ready );
  return msd->tx.result;
}


// --------------------------------------------------------------------------
// Perform a synchronous transmit and wait for it to complete.
//
static cyg_int32
usbs_msd_tx(usbs_msd* msd, const void* buf, cyg_int32 n)
{
  usbs_msd_start_tx( msd, buf, n );
  return usbs_msd_wait_for_tx( msd );
}


// --------------------------------------------------------------------------
// Start an asynchronous receive of a buffer.
//
static void
usbs_msd_start_rx(usbs_msd* msd, void* buf, cyg_int32 n)
{
  usbs_start_rx_buffer( msd->rx_ep, (unsigned char*) buf, n,
                       usbs_msd_rx_complete, msd );
}


// --------------------------------------------------------------------------
// Block the caller until the receive / stall is complete
//
static cyg_int32
usbs_msd_wait_for_rx(usbs_msd* msd)
{
  cyg_semaphore_wait( &msd->rx.ready );
  return msd->rx.result;
}


// --------------------------------------------------------------------------
// Perform a synchronous receive and wait for it to complete
//
static cyg_int32
usbs_msd_rx(usbs_msd* msd, void* buf, cyg_int32 n)
{
  usbs_msd_start_rx( msd, buf, n );
  return usbs_msd_wait_for_rx( msd );
}


// --------------------------------------------------------------------------
// Halt end-point
//
static void
usbs_msd_set_ep_halted( usbs_msd* msd , usbs_msd_ep_t ep, cyg_bool state )
{
  if( ep == CYG_USBS_EP_RX )
    usbs_set_rx_endpoint_halted( msd->rx_ep , state );
  else if( ep == CYG_USBS_EP_TX )
    usbs_set_tx_endpoint_halted( msd->tx_ep , state );
}


// --------------------------------------------------------------------------
// Perform a synchronous end-point halt and wait for host to restart it
//
static cyg_int32
usbs_msd_ep_stall(usbs_msd* msd, usbs_msd_ep_t ep )
{
  cyg_sem_t *ready;
  cyg_int32 *result;

#if !defined(CYGSEM_IO_USB_SLAVE_MSD_STALL_ENABLE)
   return 1;
#endif

  if( ep == CYG_USBS_EP_RX )
  {
    ready    = &msd->rx.ready;
    result   = &msd->rx.result;
  }
  else if( ep == CYG_USBS_EP_TX )
  {
    ready    = &msd->tx.ready;
    result   = &msd->tx.result;
  }
  else
    return 0;

  // Halt End-point
  usbs_msd_set_ep_halted( msd, ep , 1 );

  // Wait
  if( ep == CYG_USBS_EP_RX )
    usbs_start_rx_endpoint_wait( msd->rx_ep, usbs_msd_rx_complete, msd );
  else if( ep == CYG_USBS_EP_TX )
    usbs_start_tx_endpoint_wait( msd->tx_ep, usbs_msd_tx_complete, msd );

  cyg_semaphore_wait( ready );

  return ( *result );

}


// --------------------------------------------------------------------------
// Initialize a Mass Storage Device structure
//
static bool
usbs_msd_init( usbs_msd* msd )
{
  cyg_io_handle_t handle;
  cyg_uint8 i = 0;

  if( msd->ctrl_ep == NULL )
    return false;

  // Lookup storage devices
  msd->lun->max_lun = 0;
  for (i = 0; i < USBS_MSD_CBW_MAX_LUN; i++)
  {
    if ( msd->lun->name[i] != NULL )
    {
      DBG("USB Slave MSD: Lookup %s \n", msd->lun->name[i] );
      if( cyg_io_lookup( msd->lun->name[i] , &handle ) != ENOERR )
        return false;
      msd->lun->handle[i] = handle;
      msd->lun->max_lun++;
    }
  }

  msd->tx.result =  0;
  msd->rx.result =  0;

  // Initialize TX and RX semaphore
  cyg_semaphore_init( &msd->tx.ready, 0 );
  cyg_semaphore_init( &msd->rx.ready, 0 );

  cyg_mutex_init( &msd->lock );

  cyg_cond_init( &msd->state_cond, &msd->lock );

  // Make USB string descriptors
  usbs_msd_create_str_descriptor( \
                     msd->enum_mfg_str,\
                     msd->mfg_str );
  usbs_msd_create_str_descriptor( \
                     msd->enum_product_str,\
                     msd->product_str );
  usbs_msd_create_str_descriptor( \
                     msd->enum_serial_str,\
                     msd->serial_str );

  // Set up enumeration & USB call-backs
  msd->usb_state = msd->ctrl_ep->state;

  msd->ctrl_ep->enumeration_data = msd->enum_data;

  if ( msd->ctrl_ep->state_change_fn )
    msd->app_state_change_fn = msd->ctrl_ep->state_change_fn;

  msd->ctrl_ep->state_change_fn    = usbs_msd_state_change_handler;
  msd->ctrl_ep->state_change_data  = (void*) msd;
  msd->ctrl_ep->class_control_fn   = usbs_msd_class_handler;
  msd->ctrl_ep->class_control_data = (void*) msd;

  if ( msd->handler_init )
     msd->handler_init( &msd->handler_data );

  msd->send    = usbs_msd_tx;
  msd->receive = usbs_msd_rx;
  msd->stall   = usbs_msd_ep_stall;

  // Setup initial state
  msd->state = CYG_USBS_MSD_WAIT;

  return true;
}


// --------------------------------------------------------------------------
// Initialize a Mass Storage Device structure
//
static void
usbs_msd_start_service(usbs_msd* msd)
{

  cyg_thread_create( USBS_MSD_THREAD_STACK_PRIORITY , &usbs_msd_handler, \
                     (cyg_addrword_t) msd, msd->serv_name, msd->serv_stack,  \
                     USBS_MSD_THREAD_STACK_SIZE, &msd->serv_handle, \
                     &msd->serv_thread);

  cyg_thread_resume( msd->serv_handle );

}


// --------------------------------------------------------------------------
// Start the USB subsystem
//
bool
usbs_msd_start(void)
{

#ifdef CYGPKG_IO_USB_SLAVE_MSD0

  DBG("USB Slave Mass-Storage 0 starts\n");

  if ( usbs_msd_init( &msd0 ) == false )
  {
    DBG("USB Slave Mass-Storage 0 startup failed\n");
    return false;
  }

  // ----- Start USB subsystem -----
  //
  usbs_start( msd0.ctrl_ep );

  usbs_msd_start_service( &msd0 );

#endif

  return true;
}




