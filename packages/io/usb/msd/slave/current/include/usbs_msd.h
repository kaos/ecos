#ifndef CYGONCE_USBS_MSD_H
#define CYGONCE_USBS_MSD_H
//==========================================================================
//
//      include/usbs_msd.h
//
//      Description of the USB slave-side MSD
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
// Contributors:
// Date:         2010-06-03
// Purpose:
// Description:  USB slave-side MSD support
//
//
//####DESCRIPTIONEND####
//==========================================================================

#ifdef __cplusplus
extern "C" {
#endif

#include <cyg/infra/cyg_type.h>
#include <cyg/io/usb/usbs.h>

// ----------------------------------------------------------------------------
// The Mass Storage class requests
//
#define USBS_MSD_CLASS_REQ_ADSC          0x00
#define USBS_MSD_CLASS_REQ_GET_REQUEST   0xFC
#define USBS_MSD_CLASS_REQ_PUT_REQUEST   0xFD
#define USBS_MSD_CLASS_REQ_GET_MAX_LUN   0xFE
#define USBS_MSD_CLASS_REQ_BOMSR         0xFF

// ----------------------------------------------------------------------------
// Subclass values
//
#define USBS_MSD_SUB_CLASS_RBC           0x01 // Reduced Block Commands, typically for flash device
#define USBS_MSD_SUB_CLASS_SFF8020I      0x02 // For CDROM devices
#define USBS_MSD_SUB_CLASS_QIC_157       0x03 // QIC-157, Tape devices
#define USBS_MSD_SUB_CLASS_UFI           0x04 // For Floppy Drive Device
#define USBS_MSD_SUB_CLASS_SFF8070I      0x05 // For Floppy Drive Device
#define USBS_MSD_SUB_CLASS_SCSI          0x06 // SCSI transparent

#define USBS_MSD_DATA_IFACE_CLASS        0x8  // mass-storage

// ----------------------------------------------------------------------------
// Command Wrapper values
//
#define USBS_MSD_CSW_STATUS_PASSED       0x0
#define USBS_MSD_CSW_STATUS_FAILED       0x1

#define USBS_MSD_CBW_SIGNATURE           0x43425355
#define USBS_MSD_CSW_SIGNATURE           0x53425355

#define USBS_MSD_CBW_HOST2DEVICE         0x00
#define USBS_MSD_CBW_DEVICE2HOST         0x80

#define USBS_MSD_DEVICE_CLASS            0
#define USBS_MSD_NUM_IFACE               1
#define USBS_MSD_NUM_ENDP                2

// ----------------------------------------------------------------------------
// Helper macros
//
#define LO_BYTE_16(word16)          ((cyg_uint8) ((word16) & 0xFF))
#define HI_BYTE_16(word16)          ((cyg_uint8) (((word16) >> 8) & 0xFF))

#define BYTE0_32(word32)            ((cyg_uint8) ((word32) & 0xFF))
#define BYTE1_32(word32)            ((cyg_uint8) (((word32) >>  8) & 0xFF))
#define BYTE2_32(word32)            ((cyg_uint8) (((word32) >> 16) & 0xFF))
#define BYTE3_32(word32)            ((cyg_uint8) (((word32) >> 24) & 0xFF))

#define USB_CONFIGURATION_DESCRIPTOR_TOTAL_LENGTH(interfaces, endpoints) \
            (USB_CONFIGURATION_DESCRIPTOR_LENGTH +            \
            ((interfaces) * USB_INTERFACE_DESCRIPTOR_LENGTH) +  \
            ((endpoints)  * USB_ENDPOINT_DESCRIPTOR_LENGTH))

// ----------------------------------------------------------------------------
// Interface values
//
#define USBS_MSD_INTERFACE_PROTO          0x50 // Bulk mode

#define USBS_MSD_CBW_MAX_LUN               2
#define USBS_MSD_CBW_MIN_SCSI_CMD_LEN      1
#define USBS_MSD_CBW_MAX_SCSI_CMD_LEN     16

#define USBS_MSD_DISK_SECTOR_SIZE        512

// ----------------------------------------------------------------------------
// Mass Storage task constant
//
#define USBS_MSD_THREAD_STACK_SIZE       2048
#define USBS_MSD_THREAD_STACK_PRIORITY   15

// command block wrapper
typedef struct usbs_msd_cbw {
    cyg_uint32 signature;
    cyg_uint32 tag;
    cyg_uint32 data_transfert_len;
    cyg_uint8 flags;
    cyg_uint8 lun;
    struct cb {
      cyg_uint8 len;
      cyg_uint8 data[16];
    } cb;
} __attribute__((packed)) usbs_msd_cbw;

// command status wrapper
typedef struct usbs_msd_csw {
    cyg_uint32 signature;
    cyg_uint32 tag;
    cyg_uint32 data_residue;
    cyg_uint8  status;
} __attribute__((packed)) usbs_msd_csw;

// command block wrapper state
typedef enum {
    CYG_USBS_MSD_WAIT,
    CYG_USBS_MSD_DATA_IN,
    CYG_USBS_MSD_DATA_OUT
} usbs_msd_state_t;

// End-point type
typedef enum {
    CYG_USBS_EP0,
    CYG_USBS_EP_RX,
    CYG_USBS_EP_TX
} usbs_msd_ep_t;

// End-point status / control
typedef struct usbs_msd_ep_sc_t {
    cyg_sem_t   ready;
    cyg_int32   result;
} usbs_msd_ep_sc_t;

// Storage devices
typedef struct usbs_msd_lun {
    cyg_io_handle_t handle[USBS_MSD_CBW_MAX_LUN];
    const char    * name[USBS_MSD_CBW_MAX_LUN];
    cyg_uint8       max_lun;
} usbs_msd_lun;

typedef struct usbs_msd usbs_msd;

// Sub Class Handler
typedef cyg_int32 (*handler_cmd_fn)( usbs_msd * );
typedef bool (*handler_init_fn)(void **);

// End-point functions
typedef cyg_int32 (*send_fn)   ( usbs_msd *, const void * , cyg_int32 );
typedef cyg_int32 (*receive_fn)( usbs_msd *, const void * , cyg_int32 );
typedef cyg_int32 (*stall_fn)  ( usbs_msd* msd, usbs_msd_ep_t ep );

// Mass-storage device structure
struct usbs_msd {
    // Specify end-points to be used
    usbs_control_endpoint* ctrl_ep;
    usbs_tx_endpoint*   tx_ep;
    usbs_rx_endpoint*   rx_ep;

    cyg_uint8           tx_ep_num;
    cyg_uint8           rx_ep_num;

    cyg_bool            static_ep;

    // Enumeration data for this device
    usbs_enumeration_data* enum_data;

    // Enumeration string
    const char*         mfg_str;
    const char*         product_str;
    const char*         serial_str;
    char*               enum_mfg_str;
    char*               enum_product_str;
    char*               enum_serial_str;

    // End-point internal status/control
    usbs_msd_ep_sc_t    rx;
    usbs_msd_ep_sc_t    tx;

    // Lock for the state
    cyg_mutex_t         lock;

    // Condition variable for state changes
    cyg_cond_t          state_cond;

    // State of the USB interface
    cyg_uint32          usb_state;

    // Data transport-phase buffer
    cyg_uint8           buffer[USBS_MSD_DISK_SECTOR_SIZE];

    // State of the MSD
    usbs_msd_state_t    state;

    void (*app_state_change_fn)(struct usbs_control_endpoint*,
                                               void*, usbs_state_change, int);

    // Pointer to handler data
    void*               handler_data;

    // Commands Handler
    handler_cmd_fn      handler_cmd;

    // Initialize Command Handler
    handler_init_fn     handler_init;

    send_fn             send;
    receive_fn          receive;
    stall_fn            stall;

    // Mass-Storage Service
    char                serv_stack[USBS_MSD_THREAD_STACK_SIZE];
    cyg_handle_t        serv_handle;
    cyg_thread          serv_thread;
    char*               serv_name;

    // Storage Device
    usbs_msd_lun*       lun;

    // Command Block Wrapper
    usbs_msd_cbw        cbw;

    // Command Status Wrapper
    usbs_msd_csw        csw;
};

// ----------------------------------------------------------------------------
// A C interface to the MSD USB code.
// This is the interface for internal code

// Block the calling thread until the host configures the USB device.
void usbs_msd_wait_until_configured( usbs_msd* );

// Determines if the USB subsystem is configured
cyg_bool usbs_msd_is_configured( usbs_msd* );

// MSD main thread
void usbs_msd_handler( cyg_addrword_t );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_USBS_MSD_H

