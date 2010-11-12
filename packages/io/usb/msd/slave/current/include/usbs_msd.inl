//==========================================================================
//
//      usbs_msd.inl
//
//      Support for slave-side USB mass storage devices
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
// Author(s):    ccoutand <ccoutand@stmi.com>
//              
// Contributors: jld
// Date:         2010-06-06
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_IO_USB_SLAVE_MSD_INL
#define CYGONCE_IO_USB_SLAVE_MSD_INL

#define MFG_STR_INDEX           '\x01'
#define PRODUCT_STR_INDEX       '\x02'
#define SERIAL_STR_INDEX        '\x03'

#define USB_MAX_STR_LEN         256

// Declare Mass Storage Device 0
#ifdef CYGPKG_IO_USB_SLAVE_MSD0

#ifdef CYGDAT_IO_USB_SLAVE_MSD0_SUB_CLASS_TYPE_SCSI
#define USBS_MSD0_DATA_IFACE_SUB_CLASS USBS_MSD_SUB_CLASS_SCSI
#else
# error "Only SCSI Sub Class is implemented"
#endif

// ----- USB external definition
extern usbs_control_endpoint    CYGDAT_IO_USB_SLAVE_MSD0_EP0;
#if defined(CYGPKG_IO_USB_SLAVE_MSD0_STATIC_EP)
extern usbs_tx_endpoint         CYGDAT_IO_USB_SLAVE_MSD0_TX_EP;
extern usbs_rx_endpoint         CYGDAT_IO_USB_SLAVE_MSD0_RX_EP;
#endif


// ----- Configuration Descriptor -----

static const usb_configuration_descriptor usbs_msd0_configuration = {
    length            : sizeof(usb_configuration_descriptor),
    type              : USB_CONFIGURATION_DESCRIPTOR_TYPE,
    total_length_lo   :    
        USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_LO(USBS_MSD_NUM_IFACE, 
                                                     USBS_MSD_NUM_ENDP),
    total_length_hi   :    
        USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH_HI(USBS_MSD_NUM_IFACE, 
                                                     USBS_MSD_NUM_ENDP),
    number_interfaces : USBS_MSD_NUM_IFACE,
    configuration_id  : 1,
    configuration_str : 0,
#ifdef CYGOPT_IO_USB_SLAVE_MSD0_BUSPOWERED
    attributes        : (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED),
#else
    attributes        : (USB_CONFIGURATION_DESCRIPTOR_ATTR_REQUIRED |
                         USB_CONFIGURATION_DESCRIPTOR_ATTR_SELF_POWERED),
#endif
    max_power         : ((CYGNUM_IO_USB_SLAVE_MSD0_CURRENTDRAW+1) >> 1)
};

// ----- Interface Descriptor -----

static const usb_interface_descriptor usbs_msd0_interface[] = {
    {
        length:             sizeof(usb_interface_descriptor),
        type:               USB_INTERFACE_DESCRIPTOR_TYPE,
        interface_id:       0,
        alternate_setting:  0,
        number_endpoints:   2,
        interface_class:    USBS_MSD_DATA_IFACE_CLASS,
        interface_subclass: USBS_MSD0_DATA_IFACE_SUB_CLASS,
        interface_protocol: USBS_MSD_INTERFACE_PROTO,
        interface_str:      0x00
    }
};

// ----- Endpoint Descriptors -----

static const usb_endpoint_descriptor usbs_msd0_endpoints[] =
{ 
    // Tx (Bulk IN) Endpoint Descriptor
    {
        sizeof(usb_endpoint_descriptor),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_IN  | \
                CYGNUM_IO_USB_SLAVE_MSD0_TX_EP_NUM,
        USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        LO_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_TX_EP_MAX_PACKET_SIZE),
        HI_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_TX_EP_MAX_PACKET_SIZE),
        0
    },

    // Rx (Bulk OUT) Endpoint Descriptor
    {
        sizeof(usb_endpoint_descriptor),
        USB_ENDPOINT_DESCRIPTOR_TYPE,
        USB_ENDPOINT_DESCRIPTOR_ENDPOINT_OUT | \
               CYGNUM_IO_USB_SLAVE_MSD0_RX_EP_NUM,
        USB_ENDPOINT_DESCRIPTOR_ATTR_BULK,
        LO_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_RX_EP_MAX_PACKET_SIZE),
        HI_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_RX_EP_MAX_PACKET_SIZE),
        0
    }
};

// ----- String Descriptors -----

static char usbs_msd0_mfg_str_descr[USB_MAX_STR_LEN],
            usbs_msd0_product_str_descr[USB_MAX_STR_LEN],
            usbs_msd0_serial_str_descr[USB_MAX_STR_LEN];

static const char* usbs_msd0_strings[] = {
    "\x04\x03\x09\x04",
    usbs_msd0_mfg_str_descr,
    usbs_msd0_product_str_descr,
    usbs_msd0_serial_str_descr
};     

// ----- Enumeration Data w/ Device Descriptor -----

static usbs_enumeration_data usbs_msd0_enum_data = {
    {
        length:                 sizeof(usb_device_descriptor),
        type:                   USB_DEVICE_DESCRIPTOR_TYPE,
        usb_spec_lo:            0x00, 
        usb_spec_hi:            0x02,
        device_class:           USBS_MSD_DEVICE_CLASS,
        device_subclass:        0,
        device_protocol:        0,
        max_packet_size:        CYGNUM_IO_USB_SLAVE_MSD0_EP0_MAX_PACKET_SIZE,
        vendor_lo:              LO_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_VENDOR_ID),
        vendor_hi:              HI_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_VENDOR_ID),
        product_lo:             LO_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_PRODUCT_ID),
        product_hi:             HI_BYTE_16(CYGNUM_IO_USB_SLAVE_MSD0_PRODUCT_ID),
        device_lo:              0x00,
        device_hi:              0x00,
        manufacturer_str:       MFG_STR_INDEX,
        product_str:            PRODUCT_STR_INDEX,
        serial_number_str:      SERIAL_STR_INDEX, // must be > 0 for mass-storage
        number_configurations:  1
    },

    total_number_interfaces:    USBS_MSD_NUM_IFACE,
    total_number_endpoints:     USBS_MSD_NUM_ENDP,
    total_number_strings:       3,
    configurations:             &usbs_msd0_configuration,
    interfaces:                 usbs_msd0_interface,
    endpoints:                  usbs_msd0_endpoints,
    strings:                    (const unsigned char **) usbs_msd0_strings
};

// Logical Unit
usbs_msd_lun msd0_lun = {
#ifdef CYGDAT_IO_USB_SLAVE_MSD0_LUN0_NAME
  .name[0] = CYGDAT_IO_USB_SLAVE_MSD0_LUN0_NAME,
#else
  .name[0] = NULL,
#endif  
#ifdef CYGDAT_IO_USB_SLAVE_MSD0_LUN1_NAME
  .name[1] = CYGDAT_IO_USB_SLAVE_MSD0_LUN1_NAME
#else
  .name[1] = NULL
#endif 
};


// USBS Mass Storage Device 0
usbs_msd msd0 = {
#ifdef CYGDAT_IO_USB_SLAVE_MSD0_SUB_CLASS_TYPE_SCSI
 .handler_cmd      = usbs_msd_scsi_handle_cmd,
 .handler_init     = usbs_msd_scsi_init,
#else
 .handler_cmd      = NULL,
 .handler_init     = NULL,
#endif
 .ctrl_ep          = &CYGDAT_IO_USB_SLAVE_MSD0_EP0,
#if defined(CYGPKG_IO_USB_SLAVE_MSD0_STATIC_EP)
 .tx_ep            = &CYGDAT_IO_USB_SLAVE_MSD0_TX_EP,
 .rx_ep            = &CYGDAT_IO_USB_SLAVE_MSD0_RX_EP,
 .static_ep        = true,
#else
 .static_ep        = false,
#endif
 .tx_ep_num        = CYGNUM_IO_USB_SLAVE_MSD0_TX_EP_NUM,
 .rx_ep_num        = CYGNUM_IO_USB_SLAVE_MSD0_RX_EP_NUM,
 .lun              = &msd0_lun,
 .handler_data     = NULL,
 .serv_name        = "msd0_service",
 .enum_data        = &usbs_msd0_enum_data,
 .enum_mfg_str     = &usbs_msd0_mfg_str_descr[0],
 .enum_product_str = &usbs_msd0_product_str_descr[0],
 .enum_serial_str  = &usbs_msd0_serial_str_descr[0],
 .mfg_str          = CYGDAT_IO_USB_SLAVE_MSD0_MFG_STR,
 .product_str      = CYGDAT_IO_USB_SLAVE_MSD0_PRODUCT_STR,
 .serial_str       = CYGDAT_IO_USB_SLAVE_MSD0_SERIAL_STR
};

#endif // CYGPKG_IO_USB_SLAVE_MSD0

#endif // CYGONCE_IO_USB_SLAVE_MSD_INL
