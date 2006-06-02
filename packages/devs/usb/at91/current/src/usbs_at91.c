//==========================================================================
//
//      usbs_at91.c
//
//      Driver for the AT91 USB device
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric
// Copyright (C) 2006 Andrew Lunn
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Oliver Munz,
// Contributors: Andrew Lunn, bartv
// Date:         2006-02-22
//
// This code implements support for the on-chip USB port on the AT91
// family of processors. The code has been developed on the AT91SAM7S
// and may or may not work on other members of the AT91 family.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/devs_usb_at91.h>
#include <cyg/io/usb/usb.h>
#include <cyg/io/usb/usbs.h>
#include <cyg/io/usb/usbs_at91.h>

#include <cyg/hal/hal_io.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_platform_ints.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include "bitops.h"

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define AT91_UDP_CSR0 (AT91_UDP_CSR)
#define AT91_UDP_FDR0 (AT91_UDP_FDR)

#define pIER (AT91_UDP + AT91_UDP_IER)
#define pIDR (AT91_UDP + AT91_UDP_IDR)
#define pISR (AT91_UDP + AT91_UDP_ISR)
#define pIMR (AT91_UDP + AT91_UDP_IMR)
#define pICR (AT91_UDP + AT91_UDP_ICR)

#define pCSR0 (AT91_UDP + AT91_UDP_CSR0)
#define pFDR0 (AT91_UDP + AT91_UDP_FDR0)

#define pCSRn(N) (pCSR0 + (N * 4))
#define pFDRn(N) (pFDR0 + (N * 4))

#define AT91_UDP_ALLOWED_IRQs \
    (AT91_UDP_WAKEUP | AT91_UDP_ENDBUSRES | AT91_UDP_EXTRSM | \
     AT91_UDP_RXRSM  | AT91_UDP_RXSUSP    | AT91_UDP_EPINT0 | \
     AT91_UDP_EPINT1 | AT91_UDP_EPINT2    | AT91_UDP_EPINT3)

#define THERE_IS_A_NEW_PACKET_IN_THE_UDP 0xffff

// Fifo size for each end point.
static const cyg_uint16 usbs_at91_endpoint_fifo_size[AT91_USB_ENDPOINTS] = {
  8,
  64,
  64,
  64,
};

// Does an endpoint support ping pong buffering?
static const bool usbs_at91_endpoint_pingpong[AT91_USB_ENDPOINTS] = {
  false,
  true,
  true,
  false
};

static cyg_uint8 *usbs_at91_endpoint_pbegin[AT91_USB_ENDPOINTS] = 
  { 0, 0, 0, 0 };
static cyg_uint8 *usbs_at91_endpoint_pend[AT91_USB_ENDPOINTS] = 
  { 0, 0 ,0, 0 };
static bool usbs_at91_endpoint_bank1[AT91_USB_ENDPOINTS] = 
  { false, false, false, false };
static cyg_uint16 usbs_at91_endpoint_bytes_in_fifo[AT91_USB_ENDPOINTS] =
  { 0, 0, 0, 0 };
static cyg_uint16 usbs_at91_endpoint_bytes_received[AT91_USB_ENDPOINTS] =
  { THERE_IS_A_NEW_PACKET_IN_THE_UDP, THERE_IS_A_NEW_PACKET_IN_THE_UDP,
    THERE_IS_A_NEW_PACKET_IN_THE_UDP, THERE_IS_A_NEW_PACKET_IN_THE_UDP};

static cyg_interrupt usbs_at91_intr_data;
static cyg_handle_t usbs_at91_intr_handle;

static void usbs_at91_ep0_start(usbs_control_endpoint *);
static void usbs_at91_poll(usbs_control_endpoint *);

static void usbs_at91_endpoint_start(usbs_rx_endpoint * pep);
static void usbs_at91_endpoint_set_halted(usbs_rx_endpoint * pep, 
                                          cyg_bool new_value);
void usbs_at91_endpoint_init(usbs_rx_endpoint * pep, 
                             cyg_uint8 endpoint_type,
                             cyg_bool enable);

// Endpoint 0, the control endpoint, structure. 
usbs_control_endpoint usbs_at91_ep0 = {
  // The hardware does not distinguish  between detached, attached and powered.
  state:                  USBS_STATE_POWERED,   
  enumeration_data:       (usbs_enumeration_data *) 0,
  start_fn:               usbs_at91_ep0_start,
  poll_fn:                usbs_at91_poll,
  interrupt_vector:       CYGNUM_HAL_INTERRUPT_UDP,
  control_buffer:         {0, 0, 0, 0, 0, 0, 0, 0},
  state_change_fn:        (void (*) (usbs_control_endpoint *, 
                                     void *, usbs_state_change, int)) 0,
  state_change_data:      (void *) 0,
  standard_control_fn:    (usbs_control_return (*)
                          (usbs_control_endpoint *, void *)) 0,
  standard_control_data:  (void *) 0,
  class_control_fn:       (usbs_control_return (*)
                           (usbs_control_endpoint *, void *)) 0,
  class_control_data:     (void *) 0,
  vendor_control_fn:      (usbs_control_return (*) 
                           (usbs_control_endpoint *, void *)) 0,
  vendor_control_data:    (void *) 0,
  reserved_control_fn:    (usbs_control_return (*) 
                           (usbs_control_endpoint *, void *)) 0,
  reserved_control_data:  (void *) 0,
  buffer:                 (unsigned char *) 0,
  buffer_size:            0,
  fill_buffer_fn:         (void (*)(usbs_control_endpoint *)) 0,
  fill_data:              (void *) 0,
  fill_index:             0,
  complete_fn:            (usbs_control_return (*)(usbs_control_endpoint *, 
                                                   int)) 0
};

// Endpoint 1 receive control structure
usbs_rx_endpoint usbs_at91_ep1 = {
  start_rx_fn:    usbs_at91_endpoint_start,
  set_halted_fn:  usbs_at91_endpoint_set_halted,
  complete_fn:    (void (*)(void *, int)) 0,
  complete_data:  (void *) 0,
  buffer:         (unsigned char *) 0,
  buffer_size:    0,
  halted:         0,
}; 

// Endpoint 2 Receive control structure
usbs_rx_endpoint usbs_at91_ep2 = {
  start_rx_fn:    usbs_at91_endpoint_start,
  set_halted_fn:  usbs_at91_endpoint_set_halted,
  complete_fn:    (void (*)(void *, int)) 0,
  complete_data:  (void *) 0,
  buffer:         (unsigned char *) 0,
  buffer_size:    0,
  halted:         0,
};

// Endpoint 3 Receive control structure
usbs_rx_endpoint usbs_at91_ep3 = {
  start_rx_fn:    usbs_at91_endpoint_start,
  set_halted_fn:  usbs_at91_endpoint_set_halted,
  complete_fn:    (void (*)(void *, int)) 0,
  complete_data:  (void *) 0,
  buffer:         (unsigned char *) 0,
  buffer_size:    0,
  halted:         0,
};

// Array of end points. Used for translating end point pointer to an
// end point number
static const void *usbs_at91_endpoints[AT91_USB_ENDPOINTS] = {
  (void *) &usbs_at91_ep0,
  (void *) &usbs_at91_ep1,
  (void *) &usbs_at91_ep2,
  (void *) &usbs_at91_ep3
};

// Convert an endpoint pointer to an endpoint number, using the array
// of endpoint structures
static int
usbs_at91_pep_to_number(const usbs_rx_endpoint * pep)
{
  int epn;
  
  for(epn=0; epn < AT91_USB_ENDPOINTS; epn++) {
    if (pep == usbs_at91_endpoints[epn])
      return epn;
  }
  CYG_FAIL("Unknown endpoint");
  return 0;
}

typedef enum ep0_low_level_status_t {
  EP0_LL_IDLE = 0,
  EP0_LL_REQUEST,
  EP0_LL_SEND_READY,
  EP0_LL_ACK,
  EP0_LL_RECEIVE_READY,
  EP0_LL_ISOERROR,
  EP0_LL_STALL,
  EP0_LL_SET_ADDRESS,
} ep0_low_level_status_t;

// Enable/Disable interrupts for a specific endpoint.
static void
usbs_at91_endpoint_interrupt_enable (cyg_uint8 epn, bool enable)
{
  CYG_ASSERT (epn < AT91_USB_ENDPOINTS, "Invalid endpoint");
  
  if (enable) {
    HAL_WRITE_UINT32 (pIER, 1 << epn);
  } else {
    HAL_WRITE_UINT32 (pIDR, 1 << epn);
  }
}

static cyg_uint8 *
read_fifo_uint8 (cyg_uint8 * pdest, cyg_addrword_t psource, cyg_uint32 size)
{
  cyg_uint8 *preqbyte = pdest;
  cyg_uint8 reqbyte;
  
  while (size--) {
    HAL_READ_UINT8 (psource, reqbyte);
    *preqbyte = reqbyte;
    preqbyte++;
  }

  return preqbyte;
}

static cyg_uint8 *
write_fifo_uint8 (cyg_addrword_t pdest, cyg_uint8 * psource,
                      cyg_uint8 * psource_end)
{
  cyg_uint8 *preqbyte;

  for (preqbyte = psource; preqbyte < psource_end; preqbyte++) {
    HAL_WRITE_UINT8 (pdest, (*preqbyte));
  }
  
  return preqbyte;
}

/* Tell the host that the device is ready to start communication */
static void
usbs_at91_set_pullup (bool set)
{                

#ifndef CYGDAT_DEVS_USB_AT91_GPIO_SET_PULLUP_PIN_NONE
  if (
#ifdef CYGNUM_DEVS_USB_AT91_GPIO_SET_PULLUP_INVERTED
      !set
#else
      set
#endif
      ) {
    HAL_ARM_AT91_GPIO_RESET(CYGDAT_DEVS_USB_AT91_GPIO_SET_PULLUP_PIN);
  } else {
    HAL_ARM_AT91_GPIO_SET(CYGDAT_DEVS_USB_AT91_GPIO_SET_PULLUP_PIN);
  }
#endif
}
  
/* Is the USB powered? */
bool
usbs_at91_read_power (void)
{
#ifndef CYGDAT_DEVS_USB_AT91_GPIO_READ_POWER_PIN_NONE
  cyg_bool state;
  
  HAL_ARM_AT91_GPIO_GET(CYGDAT_DEVS_USB_AT91_GPIO_READ_POWER_PIN, state);
#ifdef CYGNUM_DEVS_USB_AT91_GPIO_READ_POWER_INVERTED
  return !state;
#else
  return state;
#endif
#endif
  return true;
}

// Stop all transfers that are currently active.
static void
usbs_end_all_transfers (usbs_control_return returncode)
{
  cyg_uint32 epn;
  usbs_rx_endpoint *pep;
  
  for (epn = 1; epn < AT91_USB_ENDPOINTS; epn++) {
    if (BITS_ARE_SET (pIMR, 1 << epn)) {
      // If the end point is transmitting, call the complete function
      // to terminate to transfer
      pep = (usbs_rx_endpoint *) usbs_at91_endpoints[epn];

      if (pep->complete_fn) {
        (*pep->complete_fn) (pep->complete_data, returncode);
      }
      // Disable interrupts from the endpoint
      usbs_at91_endpoint_interrupt_enable (epn, false);
    }
  }
}

// There has been a change in state. Update the end point.
static void
usbs_state_notify (usbs_control_endpoint * pcep)
{
  static int old_state = USBS_STATE_CHANGE_POWERED;
  int state = pcep->state & USBS_STATE_MASK;
  
  if (pcep->state != old_state) {
    usbs_end_all_transfers (-EPIPE);
    switch (state) {
      case USBS_STATE_DETACHED:
      case USBS_STATE_ATTACHED:
      case USBS_STATE_POWERED:
        // Nothing to do
        break;
      case USBS_STATE_DEFAULT:
        HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_GLB_STATE, 0);
        break;
      case USBS_STATE_ADDRESSED:
        HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_GLB_STATE, AT91_UDP_GLB_FADDEN);
        break;
      case USBS_STATE_CONFIGURED:
        HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_GLB_STATE, AT91_UDP_GLB_CONFG);
        break;
      default:
        CYG_FAIL("Unknown endpoint state");
    }
    
    if (pcep->state_change_fn) {
      (*pcep->state_change_fn) (pcep, 0, pcep->state, old_state);
    }
    
    old_state = pcep->state;
  }
}

static usbs_control_return
usbs_parse_host_get_command (usbs_control_endpoint * pcep)
{
  usbs_control_return retcode;
  cyg_uint8 dev_req_type =
    (((usb_devreq *) pcep->control_buffer)->type) & USB_DEVREQ_TYPE_MASK;
  
  switch (dev_req_type) {
    case USB_DEVREQ_TYPE_STANDARD: 
      if (!pcep->standard_control_fn) {
        return usbs_handle_standard_control (pcep);
      }

      retcode =
        (*pcep->standard_control_fn) (pcep, pcep->standard_control_data);
      
      if (retcode == USBS_CONTROL_RETURN_UNKNOWN) {
        return usbs_handle_standard_control (pcep);
      }
      return retcode;
    
    case USB_DEVREQ_TYPE_CLASS:
      if (!pcep->class_control_fn) {
        return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->class_control_fn) (pcep, pcep->class_control_data);

    case USB_DEVREQ_TYPE_VENDOR: 
      if (!pcep->class_control_fn) {
        return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->class_control_fn) (pcep, pcep->vendor_control_data);

    case USB_DEVREQ_TYPE_RESERVED:
      if (!pcep->reserved_control_fn) {
        return USBS_CONTROL_RETURN_STALL;
      }
      return (*pcep->reserved_control_fn) (pcep, pcep->reserved_control_data);
    default:
      return USBS_CONTROL_RETURN_STALL;
  }
}

static void
usbs_at91_endpoint_set_halted (usbs_rx_endpoint * pep, cyg_bool new_value)
{
  int epn = usbs_at91_pep_to_number(pep);
  cyg_addrword_t pCSR = pCSRn(epn);
  
  cyg_drv_dsr_lock ();
  
  if (pep->halted != new_value) {       
    /* There is something is to do */
    pep->halted = new_value;
    
    if (new_value && BITS_ARE_SET (pIMR, 1 << epn)) {   
      /* Ready to transmit */
      if (pep->complete_fn) {
        (*pep->complete_fn) (pep->complete_data, -EAGAIN);
      }
      usbs_at91_endpoint_interrupt_enable (epn, false);
      SET_BITS (pCSR, AT91_UDP_CSR_FORCESTALL);
    } else {
      CLEAR_BITS (pCSR, AT91_UDP_CSR_FORCESTALL);
    }
  }
  cyg_drv_dsr_unlock ();
}

void
usbs_at91_endpoint_init (usbs_rx_endpoint * pep, cyg_uint8 endpoint_type,
                         cyg_bool enable)
{  
  int epn = usbs_at91_pep_to_number(pep);
  cyg_addrword_t pCSR = pCSRn(epn);
  
  CYG_ASSERT (AT91_USB_ENDPOINTS > epn, "Invalid end point");
  
  usbs_at91_endpoint_interrupt_enable (epn, false);
  /* Reset endpoint */
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_RST_EP, 1 << epn);      
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_RST_EP, 0);
  
  pep->halted = false;

  /* Type | In */
  HAL_WRITE_UINT32 (pCSR, (((((cyg_uint32) endpoint_type) & 0x03) << 8) | 
                           ((((cyg_uint32) endpoint_type) & 0x80) << 3)));

  usbs_at91_endpoint_bytes_in_fifo[epn] = 0;
  usbs_at91_endpoint_bytes_received[epn] = THERE_IS_A_NEW_PACKET_IN_THE_UDP;
  usbs_at91_endpoint_bank1[epn] = false;

  if (enable) {
    SET_BITS (pCSR, AT91_UDP_CSR_EPEDS);
  }
}

static void
usbs_at91_handle_reset (void)
{
  int epn;
  const usb_endpoint_descriptor *usb_endpoints;
  cyg_uint8 endpoint_type;
  
  usbs_end_all_transfers (-EPIPE);
  
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_IDR, 0xffffffff);
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_ICR, 0xffffffff);
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_RST_EP, 0xffffffff);
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_RST_EP, 0x00000000);
  
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_FADDR, AT91_UDP_FADDR_FEN);
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_CSR0,
                    AT91_UDP_CSR_EPEDS | AT91_UDP_CSR_EPTYPE_CTRL);
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_IER, AT91_UDP_ALLOWED_IRQs);
  
  for (epn=1; epn < AT91_USB_ENDPOINTS; epn++) {
    usbs_at91_endpoint_init ((usbs_rx_endpoint *)usbs_at91_endpoints[epn], 
                             0, false);
  }

  // Now walk the endpoints configuring them correctly. This only
  // works if there is one interface.
  usb_endpoints = usbs_at91_ep0.enumeration_data->endpoints;
  
  for (epn = 1; 
       epn <= usbs_at91_ep0.enumeration_data->total_number_endpoints; 
       epn++) {
    
    endpoint_type = (usb_endpoints[epn-1].attributes |
                     (usb_endpoints[epn-1].endpoint & 
                      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN ?
                      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN : 
                      USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT));
    usbs_at91_endpoint_init((usbs_rx_endpoint *)usbs_at91_endpoints[epn],
                            endpoint_type,
                            true);
  }
}

static void
usbs_at91_ep0_start (usbs_control_endpoint * endpoint)
{
  usbs_at91_handle_reset ();

  // If there is additional platform-specific initialization to
  // perform, do it now. This macro can come from the platform HAL,
  // but may not be available on all platforms.
#ifdef AT91_USB_PLATFORM_INIT
  AT91_USB_PLATFORM_INIT ();
#endif
  
  usbs_at91_set_pullup (true);
}

static void
usbs_at91_endpoint_start (usbs_rx_endpoint * pep)
{  
  int epn = usbs_at91_pep_to_number(pep);
  cyg_addrword_t pCSR = pCSRn(epn);
  cyg_addrword_t pFDR = pFDRn(epn);
  cyg_uint16 space = 0;
  cyg_uint16 endpoint_size = usbs_at91_endpoint_fifo_size[epn];
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[epn];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[epn];
  
  CYG_ASSERT (pep->complete_fn, "No complete_fn()");

  cyg_drv_dsr_lock ();
  if (usbs_at91_ep0.state != USBS_STATE_CONFIGURED) {   
    /* If not configured it means there is nothing to do */
    cyg_drv_dsr_unlock ();
    
    if (pep->complete_fn) {
      (*pep->complete_fn) (pep->complete_data, -EPIPE);
    }
    return;
  }
  
  if (pep->halted) {            
    /* Halted means nothing to do */
    cyg_drv_dsr_unlock ();

    if (pep->complete_fn) {
      (*pep->complete_fn) (pep->complete_data, -EAGAIN);
    }
    return;
  }

  if (BITS_ARE_SET (pIMR, 1 << epn)) {
    cyg_drv_dsr_unlock ();

    if (pep->complete_fn) {
      (*pep->complete_fn) (pep->complete_data, -EIO);
    }
    
    return;
  }
  
  CYG_ASSERT (BITS_ARE_SET (pCSR, 1 << 9), "Wrong endpoint type");
  
  *ppbegin = pep->buffer;       /* Set the working pointers */
  *ppend = (cyg_uint8 *) ((cyg_uint32) pep->buffer + pep->buffer_size);
  
  if (BITS_ARE_SET (pCSR, 0x400)) {     /* IN: tx_endpoint */
    space = (cyg_uint32) * ppend - (cyg_uint32) * ppbegin;
    if (space == endpoint_size) {
      *ppend = *ppbegin;        /* Send zero-packet */
    }
    
    *ppbegin =
      write_fifo_uint8 (pFDR, *ppbegin,
                        (cyg_uint8 *) ((cyg_uint32) * ppbegin +
                                       MIN (space, endpoint_size)));
    SET_BITS (pCSR, AT91_UDP_CSR_TXPKTRDY);
    
    if (*ppend == *ppbegin) {   /* Last packet ? */
      *ppend = *ppbegin - 1;    /* The packet hasn't been sent yet */
    }
  }
  
  usbs_at91_endpoint_interrupt_enable (epn, true);
  
  cyg_drv_dsr_unlock ();
}

// Perform transmit handling on an endpoint
static bool 
usbs_at91_endpoint_isr_tx(cyg_uint8 epn)
{
  cyg_addrword_t pCSR = pCSRn(epn);
  cyg_addrword_t pFDR = pFDRn(epn);
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[epn];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[epn];
  cyg_uint32 space = 0;
  cyg_uint16 endpoint_size = usbs_at91_endpoint_fifo_size[epn];

  CLEAR_BITS (pCSR, AT91_UDP_CSR_TXCOMP);

  if (BITS_ARE_CLEARED (pCSR, AT91_UDP_CSR_TXPKTRDY)) {       
    /* Ready to transmit ? */
    if (*ppend > *ppbegin) {  
      /* Something to send */
      
      space = (cyg_uint32) * ppend - (cyg_uint32) * ppbegin;
      if (space == endpoint_size) {
        *ppend = *ppbegin;            /* Send zero-packet */
      }
      
      *ppbegin =
        write_fifo_uint8 (pFDR, *ppbegin,
                          (cyg_uint8 *) ((cyg_uint32) * ppbegin +
                                         MIN (space, endpoint_size)));
      SET_BITS (pCSR, AT91_UDP_CSR_TXPKTRDY);
      
      if (*ppend == *ppbegin) {       /* Last packet ? */
        *ppend = *ppbegin - 1;        /* The packet isn't sent yet */
      }
    } else {
      if (*ppend + 1 == *ppbegin) {
        *ppend = *ppbegin;    /* Flag for DSR */
        return true;
      } else {
        *ppend = *ppbegin - 1;        /* Flag for zero-packet */
        SET_BITS (pCSR, AT91_UDP_CSR_TXPKTRDY);       /* Send no data */
      }
    }
  }
  
  CLEAR_BITS (pCSR,
              AT91_UDP_CSR_RX_DATA_BK0 | AT91_UDP_CSR_RX_DATA_BK1 |
              AT91_UDP_CSR_RXSETUP | AT91_UDP_CSR_ISOERROR);
  return false;
}

static bool
usbs_at91_endpoint_isr_rx(cyg_uint8 epn)
{
  cyg_addrword_t pCSR = pCSRn(epn);
  cyg_addrword_t pFDR = pFDRn(epn);
  cyg_uint16 *pinfifo = &usbs_at91_endpoint_bytes_in_fifo[epn];
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[epn];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[epn];
  cyg_uint16 *preceived = &usbs_at91_endpoint_bytes_received[epn];
  cyg_uint16 endpoint_size = usbs_at91_endpoint_fifo_size[epn];

  if (*preceived == THERE_IS_A_NEW_PACKET_IN_THE_UDP) {      
    /* There is a new packet */
    *preceived = ((*(cyg_uint32 *)pCSR) >> 16) & 0x7ff;    
    *pinfifo = *preceived;           
  }

  while ((*ppbegin < *ppend) && *pinfifo) { 
    /* If we have buffer-space AND data in the FIFO */
    HAL_READ_UINT8(pFDR, **ppbegin);
    (*ppbegin)++;
    (*pinfifo)--;
  }
  
  if (*ppbegin == *ppend) { 
    /* The buffer is full... call the DSR */
    return true;
  }
  
  if (*pinfifo == 0) {      
    /* If the FIFO is empty, then we can release it */
    if (usbs_at91_endpoint_pingpong[epn]) { 
      /* Time to clear the interrupt flag */
      
      if (usbs_at91_endpoint_bank1[epn]) {
        CLEAR_BITS (pCSR, AT91_UDP_CSR_RX_DATA_BK1);
      } else {
        CLEAR_BITS (pCSR, AT91_UDP_CSR_RX_DATA_BK0);
      }
      usbs_at91_endpoint_bank1[epn] = !usbs_at91_endpoint_bank1[epn];
    } else {
      CLEAR_BITS (pCSR, AT91_UDP_CSR_RX_DATA_BK0);
    }
    
    if (*preceived < endpoint_size) {
      /* If the last packet was smaller then the endpoint-size... */
      *ppend = *ppbegin;
      *preceived = THERE_IS_A_NEW_PACKET_IN_THE_UDP; /* Set flag */
      
      return true;     /* We can call the completion-function in the DSR */
    }
    
    *preceived = THERE_IS_A_NEW_PACKET_IN_THE_UDP;   /* Set flag */
  }
  return false;
}
    
// ISR for an endpoint. Handle receive and transmit interrupts.
static bool
usbs_at91_endpoint_isr (cyg_uint8 epn)
{                               
  cyg_addrword_t pCSR = pCSRn(epn);
  
  CYG_ASSERT (AT91_USB_ENDPOINTS > epn && epn, "Invalid end point");
  
  if (BITS_ARE_SET (pCSR, 0x400)) {      /* IN: tx_endpoint */
    if (usbs_at91_endpoint_isr_tx(epn))
      return true;  // Call the DSR
  } else {                               /* OUT: rx_endpoint */
    if (!BITS_ARE_CLEARED (pCSR, AT91_UDP_CSR_RX_DATA_BK0) || 
        !BITS_ARE_CLEARED (pCSR, AT91_UDP_CSR_RX_DATA_BK1)) {   
      /* Sometime something received ? */
      if(usbs_at91_endpoint_isr_rx(epn))
        return true; // Call the DSR;
    }
        
        CLEAR_BITS (pCSR,
                AT91_UDP_CSR_TXCOMP | AT91_UDP_CSR_RXSETUP |
                AT91_UDP_CSR_ISOERROR);
  }

  return false;
}

// Handle a DSR for an endpoint
static void
usbs_at91_endpoint_dsr (cyg_uint8 epn)
{
  usbs_rx_endpoint *pep = (usbs_rx_endpoint *) usbs_at91_endpoints[epn];
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[epn];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[epn];

  CYG_ASSERT (AT91_USB_ENDPOINTS > epn && epn, "Invalid end point");
  CYG_ASSERT (pep->complete_fn, "No complete_fn()");

  if (*ppend == *ppbegin) {     /* Transmitted/Received ? */
    
    pep->buffer_size = (cyg_uint32) * ppbegin - (cyg_uint32) pep->buffer;
    if (pep->buffer_size && pep->complete_fn) {
      if (!pep->halted) {
        (*pep->complete_fn) (pep->complete_data, pep->buffer_size);
      } else {
        (*pep->complete_fn) (pep->complete_data, -EAGAIN);
      }
    }
    usbs_at91_endpoint_interrupt_enable (epn, false);
  }
}

// Handle an error condition on the control endpoint
static ep0_low_level_status_t
usbs_at91_control_error(ep0_low_level_status_t status)
{
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];

  usbs_at91_ep0.buffer_size = 0;
  usbs_at91_ep0.fill_buffer_fn = 0;
  usbs_at91_ep0.complete_fn = 0;
  
  *ppbegin = usbs_at91_ep0.buffer;
  *ppend = *ppbegin;
  
  if (status == EP0_LL_IDLE) {
    if (usbs_at91_ep0.complete_fn) {
      (*usbs_at91_ep0.complete_fn) (&usbs_at91_ep0,
                                    USBS_CONTROL_RETURN_STALL);
    }
  }
  
  status = EP0_LL_IDLE;
  
  CLEAR_BITS (pCSR0, AT91_UDP_CSR_ISOERROR | AT91_UDP_CSR_FORCESTALL);
  
  return status;
}

// Handle a get status setup message on the control end point
static ep0_low_level_status_t 
usbs_at91_control_setup_get_status(void)
{
  ep0_low_level_status_t status;
  usb_devreq *req = (usb_devreq *)usbs_at91_ep0.control_buffer;
  cyg_uint8 recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];
  cyg_uint16 word = 0;
  
  status = EP0_LL_SEND_READY;
  
  switch (recipient) {
    case USB_DEVREQ_RECIPIENT_DEVICE:
    case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;
    case USB_DEVREQ_RECIPIENT_ENDPOINT: 
      if ((usbs_at91_ep0.state == USBS_STATE_CONFIGURED) && 
          (req->index_lo > 0) && 
          (req->index_lo < AT91_USB_ENDPOINTS)) {
        cyg_uint32 CSR;
        
        HAL_READ_UINT32(pCSRn(req->index_lo), CSR);
        if (CSR & AT91_UDP_CSR_EPEDS) {
          word = 1;
        }
      } else {
        status = EP0_LL_STALL;
      }
      break;
    default:
      status = EP0_LL_STALL;
  }
  
  *ppbegin = (cyg_uint8 *)&word;
  *ppend = *ppbegin + sizeof (word);
  return status;
}

// Setup the begin and end pointers such that an ACK is sent
static void
usbs_at91_control_setup_send_ack(void)
{
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];
  
  *ppbegin = usbs_at91_ep0.buffer;
  *ppend = *ppbegin;
}

// Handle a get status set feature message on the control endpoint
static ep0_low_level_status_t 
usbs_at91_control_setup_set_feature(void)
{
  ep0_low_level_status_t status;
  usb_devreq *req = (usb_devreq *)usbs_at91_ep0.control_buffer;
  cyg_uint8 recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;
  
  usbs_at91_control_setup_send_ack();
  status = EP0_LL_SEND_READY;
  
  switch(recipient) {
    case USB_DEVREQ_RECIPIENT_DEVICE:
      status = EP0_LL_STALL;
      break;
    case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;
    case USB_DEVREQ_RECIPIENT_ENDPOINT: 
      if ((usbs_at91_ep0.state == USBS_STATE_CONFIGURED) && 
          (req->index_lo > 0) && 
          (req->index_lo < AT91_USB_ENDPOINTS)) {
        cyg_uint32 CSR;
        
        HAL_READ_UINT32(pCSRn(req->index_lo), CSR);
        if (CSR & AT91_UDP_CSR_EPEDS) {
          /* TODO */
        }
      } else {
        status = EP0_LL_STALL;
      }
    default:
      status = EP0_LL_STALL;
  }
  return status;
}

// Handle a get status clear feature message on the control endpoint
static ep0_low_level_status_t 
usbs_at91_control_setup_clear_feature(void)
{
  ep0_low_level_status_t status;
  usb_devreq *req = (usb_devreq *)usbs_at91_ep0.control_buffer;
  cyg_uint8 recipient = req->type & USB_DEVREQ_RECIPIENT_MASK;
  
  usbs_at91_control_setup_send_ack();
  status = EP0_LL_SEND_READY;

  switch (recipient) {
    case USB_DEVREQ_RECIPIENT_DEVICE:
      status = EP0_LL_STALL;
      break;
    case USB_DEVREQ_RECIPIENT_INTERFACE:
      // Nothing to do
      break;
    case USB_DEVREQ_RECIPIENT_ENDPOINT: 
      if ((usbs_at91_ep0.state == USBS_STATE_CONFIGURED) && 
          (req->index_lo > 0) && 
          (req->index_lo < AT91_USB_ENDPOINTS)) {
        cyg_uint32 CSR;
        
        HAL_READ_UINT32(pCSRn(req->index_lo), CSR);
        if (CSR & AT91_UDP_CSR_EPEDS) {
          /* TODO */
        }
      } else {
        status = EP0_LL_STALL;
      }
    default:
      status = EP0_LL_STALL;
  }
  return status;
}

// Handle a setup message from the host
static ep0_low_level_status_t
usbs_at91_control_setup(ep0_low_level_status_t status)
{
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];
  usb_devreq *req = (usb_devreq *) usbs_at91_ep0.control_buffer;
  cyg_uint8   protocol;
  cyg_uint16 length;
  bool dev_to_host;
  usbs_control_return usbcode;
  bool handled = false;
  
  usbs_at91_ep0.buffer_size = 0;
  usbs_at91_ep0.fill_buffer_fn = 0;
  usbs_at91_ep0.complete_fn = 0;
  
  read_fifo_uint8 ((cyg_uint8 *)req, pFDR0, sizeof (usb_devreq));

  length = (req->length_hi << 8) | req->length_lo;;
  dev_to_host = req->type & USB_DEVREQ_DIRECTION_IN;
  
  CLEAR_BITS (pCSR0, AT91_UDP_CSR_DTGLE);

  status = EP0_LL_REQUEST;

  protocol = req->type & (USB_DEVREQ_TYPE_MASK);
  
  // Set the next transfer direction
  if (dev_to_host) {        
    SET_BITS (pCSR0, AT91_UDP_CSR_DIR);     /* Set IN direction */
  } else {
    CLEAR_BITS (pCSR0, AT91_UDP_CSR_DIR);   /* Set OUT direction */
  }

  if (protocol == USB_DEVREQ_TYPE_STANDARD) {
    handled = true;
    switch (req->request) {
      case USB_DEVREQ_GET_STATUS:
        status = usbs_at91_control_setup_get_status();
        break;
      case USB_DEVREQ_SET_ADDRESS:
        // Most of the hard work is done by the hardware. We just need
        // to send an ACK.
        usbs_at91_control_setup_send_ack();
        status = EP0_LL_SEND_READY;
        break;
      case USB_DEVREQ_SET_FEATURE:     
        status = usbs_at91_control_setup_set_feature();
        break;
      case USB_DEVREQ_CLEAR_FEATURE:
        status = usbs_at91_control_setup_clear_feature();
        break;
      default:
        handled = false;
    }
  }
  if ((protocol != USB_DEVREQ_TYPE_STANDARD) || !handled) {
    // Ask the layer above to process the message
    usbcode = usbs_parse_host_get_command (&usbs_at91_ep0);
    usbs_at91_ep0.buffer_size = MIN (usbs_at91_ep0.buffer_size, length);
      
    *ppbegin = usbs_at91_ep0.buffer;
    *ppend = *ppbegin + usbs_at91_ep0.buffer_size; /* Ready to send... */
    
    if (usbcode == USBS_CONTROL_RETURN_HANDLED) { /* OK */
      if (dev_to_host) {  
        status = EP0_LL_SEND_READY;
      } else {
        status = EP0_LL_RECEIVE_READY;
      }
    } else {
      status = EP0_LL_STALL;
    }
  }
  // Clear the setup bit so indicating we have processed the message
  CLEAR_BITS (pCSR0, AT91_UDP_CSR_RXSETUP);   
  
  return status;
}

static ep0_low_level_status_t
usbs_at91_control_data_recv(ep0_low_level_status_t status)
{
  cyg_uint32 received = 0;
  cyg_uint32 length;
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];
  usbs_control_return usbcode;
  
  if (status == EP0_LL_RECEIVE_READY) {
    received = ((*(cyg_uint32 *) pCSR0) >> 16) & 0x7ff;
    length = MIN (received, (cyg_uint32) *ppend - (cyg_uint32) *ppbegin);
    *ppbegin = read_fifo_uint8 (*ppbegin, pFDR0, length);
    
    if (received < usbs_at91_endpoint_fifo_size[0]) {        /* Last packet ? */
      *ppend = *ppbegin;
    }
    
    if (*ppbegin == *ppend) {   /* All received ? */
      usbs_at91_ep0.buffer_size =
        (cyg_uint32) *ppend - (cyg_uint32) usbs_at91_ep0.buffer;
      usbcode = USBS_CONTROL_RETURN_STALL;
      
      if (usbs_at91_ep0.complete_fn) {
        usbcode = (*usbs_at91_ep0.complete_fn) (&usbs_at91_ep0, 0);
      }
      
      if (usbcode == USBS_CONTROL_RETURN_HANDLED) {
        status = EP0_LL_SEND_READY;
      } else {
        status = EP0_LL_STALL;
      }
    }
  }
  
  CLEAR_BITS (pCSR0, AT91_UDP_CSR_RX_DATA_BK0);
  
  return status;
}

static ep0_low_level_status_t
usbs_at91_control_data_sent(ep0_low_level_status_t status)
{
  cyg_uint8 **ppbegin = &usbs_at91_endpoint_pbegin[0];
  cyg_uint8 **ppend = &usbs_at91_endpoint_pend[0];
  cyg_uint32 bytes_to_write = 0;
  usb_devreq *req = (usb_devreq *)usbs_at91_ep0.control_buffer;
  cyg_uint16 value;

  switch (status) {
    case EP0_LL_SEND_READY: 
      if (*ppbegin == *ppend &&
          usbs_at91_ep0.fill_buffer_fn == NULL) {   
        // All bytes are sent, send ACK
        status = EP0_LL_ACK;
        SET_BITS (pCSR0, AT91_UDP_CSR_TXPKTRDY);      // Signal FIFO loaded 
      } else {
        // We have more bytes to send
        bytes_to_write =
          MIN (*ppend - *ppbegin, usbs_at91_endpoint_fifo_size[0]);
        *ppbegin = write_fifo_uint8 (pFDR0, *ppbegin, (cyg_uint8 *) 
                                     ((cyg_uint32) *ppbegin + bytes_to_write));
        // Send next few bytes 
        if (*ppbegin == *ppend) { /* Control-Endoints don't need ACK's */
          if (usbs_at91_ep0.fill_buffer_fn) { // More Records ?
            (*usbs_at91_ep0.fill_buffer_fn) (&usbs_at91_ep0);
            
            *ppbegin = usbs_at91_ep0.buffer;
            *ppend = *ppbegin + usbs_at91_ep0.buffer_size;        
            
            /* Ready to send... */
            bytes_to_write =
              MIN (*ppend - *ppbegin,
                   usbs_at91_endpoint_fifo_size[0] - bytes_to_write);
            
            *ppbegin = write_fifo_uint8 (pFDR0, *ppbegin, (cyg_uint8 *) 
                                         ((cyg_uint32) *ppbegin + bytes_to_write));
            // Send next few bytes 
          } else {
            if (bytes_to_write == usbs_at91_endpoint_fifo_size[0]) {
              // Last packet is full, so we need to send a zero bytes
              // packet next time
              status = EP0_LL_SEND_READY;
            } else {
              status = EP0_LL_IDLE;
            }
            
          }
        }
        SET_BITS (pCSR0, AT91_UDP_CSR_TXPKTRDY);      // Signal FIFO loaded 
      }
      break;
    case EP0_LL_RECEIVE_READY:       
      /* Maybe we have to send an ACK */
      if (*ppbegin == *ppend) {   // All bytes are received, send ACK
        status = EP0_LL_ACK;
        SET_BITS (pCSR0, AT91_UDP_CSR_TXPKTRDY);      // Signal FIFO loaded 
      }
      break;
    case EP0_LL_ACK:
      if (req->request == USB_DEVREQ_SET_ADDRESS) {   // Special-processing 
        HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_FADDR,
                          req->value_lo | AT91_UDP_FADDR_FEN);
        value = (req->value_hi << 8) | req->value_lo;
        if (value) {
          usbs_at91_ep0.state = USBS_STATE_ADDRESSED;
        }
      }
      
      if (usbs_at91_ep0.complete_fn) {
        (*usbs_at91_ep0.complete_fn) (&usbs_at91_ep0,
                                      USBS_CONTROL_RETURN_HANDLED);
      }
      status = EP0_LL_IDLE;
      usbs_state_notify (&usbs_at91_ep0);
      break;
    default:
      break;
  }
  return status;
}
  
static void
usbs_at91_control_dsr (void)
{
  static ep0_low_level_status_t status = EP0_LL_IDLE;
  
  while (!BITS_ARE_CLEARED(pCSR0,
                           AT91_UDP_CSR_TXCOMP | AT91_UDP_CSR_RX_DATA_BK0 |
                           AT91_UDP_CSR_RXSETUP | AT91_UDP_CSR_ISOERROR |
                           AT91_UDP_CSR_RX_DATA_BK1)) {
    
    // Check and handle any error conditions
    if (BITS_ARE_SET (pCSR0, AT91_UDP_CSR_ISOERROR)) {
      status = usbs_at91_control_error(status);
    }
    
    // Check for a setup message and handle it
    if (BITS_ARE_SET (pCSR0, AT91_UDP_CSR_RXSETUP)) {
      status = usbs_at91_control_setup(status);
    }
    
    // Check for received data on the control endpoint
    if (BITS_ARE_SET (pCSR0, AT91_UDP_CSR_RX_DATA_BK0)) {
      status = usbs_at91_control_data_recv(status);
    }
    
    // Check if the last packet has been sent
    if (BITS_ARE_CLEARED (pCSR0, AT91_UDP_CSR_TXPKTRDY)) {
      status = usbs_at91_control_data_sent(status);
    }

    // Received an ACK packet
    if (BITS_ARE_SET (pCSR0, AT91_UDP_CSR_TXCOMP)) {
      CLEAR_BITS (pCSR0, AT91_UDP_CSR_TXCOMP);
    }
    
    if (status == EP0_LL_STALL) {
      CLEAR_BITS (pCSR0, 0x7f);
      SET_BITS (pCSR0, AT91_UDP_CSR_FORCESTALL);
    }
  }
}

static void
usbs_at91_dsr (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  cyg_uint8 n;
  
  CYG_ASSERT (CYGNUM_HAL_INTERRUPT_UDP == vector, "Wrong interrupts");
  CYG_ASSERT (0 == data, "DSR needs no data");
  
  CLEAR_BITS (AT91_UDP + AT91_UDP_GLB_STATE, 0x10);

  if (BITS_ARE_SET (pISR, AT91_UDP_WAKEUP)) {
    usbs_at91_ep0.state = USBS_STATE_DEFAULT;
    usbs_state_notify (&usbs_at91_ep0);

    HAL_WRITE_UINT32 (pICR, AT91_UDP_WAKEUP);
  }
  
  if (BITS_ARE_SET (pISR, AT91_UDP_ENDBUSRES)) {        // RESET UDP 
    usbs_at91_ep0.state = USBS_STATE_POWERED;
    usbs_state_notify (&usbs_at91_ep0);
    usbs_at91_handle_reset ();
    
    HAL_WRITE_UINT32 (pCSR0, AT91_UDP_CSR_EPEDS | AT91_UDP_CSR_EPTYPE_CTRL);
    HAL_WRITE_UINT32 (pIER, AT91_UDP_EPINT0);
    
    usbs_at91_ep0.state = USBS_STATE_DEFAULT;
    usbs_state_notify (&usbs_at91_ep0);
    
    HAL_WRITE_UINT32 (pICR, AT91_UDP_ENDBUSRES);
  }

  if (BITS_ARE_SET (pISR, AT91_UDP_SOFINT)) {
    HAL_WRITE_UINT32 (pICR, AT91_UDP_SOFINT);
  }

  if (BITS_ARE_SET (pISR, AT91_UDP_EXTRSM)) {
    usbs_at91_ep0.state = usbs_at91_ep0.state & ~USBS_STATE_SUSPENDED;
    usbs_state_notify (&usbs_at91_ep0);
    HAL_WRITE_UINT32 (pICR, AT91_UDP_EXTRSM);
  }
  
  if (BITS_ARE_SET (pISR, AT91_UDP_RXRSM)) {
    usbs_at91_ep0.state = usbs_at91_ep0.state & ~USBS_STATE_SUSPENDED;
    usbs_state_notify (&usbs_at91_ep0);
    HAL_WRITE_UINT32 (pICR, AT91_UDP_RXRSM);
  }

  if (BITS_ARE_SET (pISR, AT91_UDP_RXSUSP)) {
    usbs_at91_ep0.state = usbs_at91_ep0.state | USBS_STATE_SUSPENDED;
    usbs_state_notify (&usbs_at91_ep0);
    HAL_WRITE_UINT32 (pICR, AT91_UDP_RXSUSP);
  }

  if (BITS_ARE_SET (pISR, AT91_UDP_EPINT0)) {
    usbs_at91_control_dsr ();
  }

  for (n = 1; n < AT91_USB_ENDPOINTS; n++) {
    if (*(cyg_uint32 *) pIMR & (1 << n)) {
      usbs_at91_endpoint_dsr (n);
    }
  }
  
  cyg_drv_interrupt_unmask (vector);
}

static cyg_uint32
usbs_at91_isr (cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_uint8 n;
  bool need_dsr = false;
  cyg_uint32 IMR;
  cyg_uint32 ISR;
  
  CYG_ASSERT (CYGNUM_HAL_INTERRUPT_UDP == vector, "Wrong interrupts");
  CYG_ASSERT (0 == data, "ISR needs no data");

  HAL_READ_UINT32(pIMR, IMR);
  HAL_READ_UINT32(pISR, ISR);
  
  for (n = 1; n < AT91_USB_ENDPOINTS; n++) {    
    /* Do any data endpoint need a data transfer ? */
    if (IMR & ISR & (1 << n)) {
      need_dsr = usbs_at91_endpoint_isr (n) || need_dsr;
    }
  }
  /* If we don't need any DSR re-enable interrupts and finish */
  if (BITS_ARE_CLEARED (pISR, AT91_UDP_ALLOWED_IRQs & 0xffffff01)
      && !need_dsr) {           
    cyg_drv_interrupt_acknowledge (vector);
    return CYG_ISR_HANDLED;
  }

  /* Call the DSR */
  cyg_drv_interrupt_mask (vector);      
  cyg_drv_interrupt_acknowledge (vector);
  
  return CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
}



// ----------------------------------------------------------------------------
// Polling support. It is not clear that this is going to work particularly
// well since according to the documentation the hardware does not generate
// NAKs automatically - instead the ISR has to set the appropriate bits
// sufficiently quickly to avoid confusing the host.
//
// Calling the isr directly avoids duplicating code, but means that
// cyg_drv_interrupt_acknowledge() will get called when not inside a
// real interrupt handler. This should be harmless.

static void
usbs_at91_poll (usbs_control_endpoint * endpoint)
{
  CYG_ASSERT (endpoint == &usbs_at91_ep0, "Wrong endpoint");
  if (CYG_ISR_CALL_DSR == usbs_at91_isr (CYGNUM_HAL_INTERRUPT_UDP, 0)) {
    usbs_at91_dsr (CYGNUM_HAL_INTERRUPT_UDP, 0, 0);
  }
}

// ----------------------------------------------------------------------------
// Initialization
//
// This routine gets called from a prioritized static constructor during
// eCos startup.

void
usbs_at91_init (void)
{

  cyg_uint32 reg;

  HAL_READ_UINT32 (AT91_PMC + AT91_PMC_PLLR, reg);      
  
  /* Set USB divider so we have a 48MHz clock */
#if   ((CYGNUM_HAL_ARM_AT91_CLOCK_SPEED <  48120000) && \
       (CYGNUM_HAL_ARM_AT91_CLOCK_SPEED >  47880000))
  
  // 48MHz clock, divider set to 1
  HAL_WRITE_UINT32 (AT91_PMC + AT91_PMC_PLLR,
                    (reg & 0x0fffffff) | AT91_PMC_PLLR_USBDIV_1);
  
#elif ((CYGNUM_HAL_ARM_AT91_CLOCK_SPEED <  96240000) && \
       (CYGNUM_HAL_ARM_AT91_CLOCK_SPEED >  95760000))

  // 96MHz clock, divider set to 2
  HAL_WRITE_UINT32 (AT91_PMC + AT91_PMC_PLLR,
                    (reg & 0x0fffffff) | AT91_PMC_PLLR_USBDIV_2);
#else
#error CYGNUM_HAL_ARM_AT91_CLOCK_SPEED is not 48, 96 or 192MHz plusminus 0.25% ...
#endif

  /* Enable USB clock */
  HAL_WRITE_UINT32 (AT91_PMC + AT91_PMC_SCER, AT91_PMC_SCER_UDP);       
  HAL_WRITE_UINT32 (AT91_PMC + AT91_PMC_PCER, AT91_PMC_PCER_UDP); 

  usbs_at91_set_pullup (false);
#ifndef CYGDAT_DEVS_USB_AT91_GPIO_SET_PULLUP_PIN_NONE
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(CYGDAT_DEVS_USB_AT91_GPIO_SET_PULLUP_PIN,
                                  AT91_PIN_OUT);
#endif
#ifndef CYGDAT_DEVS_USB_AT91_GPIO_READ_POWER_PIN_NONE
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(CYGDAT_DEVS_USB_AT91_GPIO_READ_POWER_PIN,
                                  AT91_PIN_IN);
#endif
  usbs_at91_handle_reset ();
  
  cyg_drv_interrupt_create (CYGNUM_HAL_INTERRUPT_UDP, 
                            6,  // priority
                            0,  // data
                            &usbs_at91_isr,
                            &usbs_at91_dsr,
                            &usbs_at91_intr_handle, &usbs_at91_intr_data);
  
  cyg_drv_interrupt_attach (usbs_at91_intr_handle);
  cyg_drv_interrupt_unmask (CYGNUM_HAL_INTERRUPT_UDP);
  
  HAL_WRITE_UINT32 (AT91_UDP + AT91_UDP_TXVC, 0);
  
  usbs_at91_ep0.state = USBS_STATE_POWERED;
  usbs_state_notify (&usbs_at91_ep0);
}

// ----------------------------------------------------------------------------
// Testing support.
usbs_testing_endpoint usbs_testing_endpoints[] = {
    {
        endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL, 
        endpoint_number     : 0,
        endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
        endpoint            : (void*) &usbs_at91_ep0,
#ifdef CYGVAR_DEVS_USB_AT91_EP0_DEVTAB_ENTRY
        devtab_entry        : CYGDAT_DEVS_USB_AT91_DEVTAB_BASENAME "0c",
#else        
        devtab_entry        : (const char*) 0,
#endif        
        min_size            : 1,            // zero-byte control transfers are meaningless
        max_size            : 0x0FFFF,      // limit imposed by protocol
        max_in_padding      : 0,
        alignment           : 0
    },
    {
        endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        endpoint_number     : 1,
        endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT,
        endpoint            : (void*) &usbs_at91_ep1,
#ifdef CYGVAR_DEVS_USB_AT91_EP1_DEVTAB_ENTRY
        devtab_entry        : CYGDAT_DEVS_USB_AT91_DEVTAB_BASENAME "1r",
#else        
        devtab_entry        : (const char*) 0,
#endif        
        min_size            : 1,
        max_size            : -1,           // No hardware or driver limitation
        max_in_padding      : 0,
        alignment           : 0
    },
    {
        endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        endpoint_number     : 2,
        endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
        endpoint            : (void*) &usbs_at91_ep2,
#ifdef CYGVAR_DEVS_USB_AT91_EP2_DEVTAB_ENTRY
        devtab_entry        : CYGDAT_DEVS_USB_AT91_DEVTAB_BASENAME "2w",
#else        
        devtab_entry        : (const char*) 0,
#endif        
        min_size            : 1,
        max_size            : -1,           // No hardware or driver limitation
        max_in_padding      : 1,            // hardware limitation
        alignment           : 0
    },
    {
        endpoint_type       : USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        endpoint_number     : 3,
        endpoint_direction  : USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN,
        endpoint            : (void*) &usbs_at91_ep3,
#ifdef CYGVAR_DEVS_USB_AT91_EP3_DEVTAB_ENTRY
        devtab_entry        : CYGDAT_DEVS_USB_AT91_DEVTAB_BASENAME "3w",
#else        
        devtab_entry        : (const char*) 0,
#endif        
        min_size            : 1,
        max_size            : -1,           // No hardware or driver limitation
        max_in_padding      : 1,            // hardware limitation
        alignment           : 0
    },
    USBS_TESTING_ENDPOINTS_TERMINATOR
};
