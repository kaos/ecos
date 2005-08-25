#ifndef CYGONCE_CANIO_H
#define CYGONCE_CANIO_H
// ====================================================================
//
//      canio.h
//
//      Device I/O 
//
// ====================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
// ====================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: gthomas
// Date:         2005-05-12
// Purpose:      Special support for CAN I/O devices
// Description:
//
//####DESCRIPTIONEND####
//
// ====================================================================


//===========================================================================
//                               INCLUDES
//===========================================================================
#include <pkgconf/system.h>
#include <pkgconf/io_can.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/config_keys.h>


#ifdef __cplusplus
extern "C" {
#endif


//===========================================================================
//                                DATA TYPES
//===========================================================================

//
// Supported baud rates
//
typedef enum {
    CYGNUM_CAN_KBAUD_10 = 1,
    CYGNUM_CAN_KBAUD_20,
    CYGNUM_CAN_KBAUD_50,
    CYGNUM_CAN_KBAUD_100,
    CYGNUM_CAN_KBAUD_125,
    CYGNUM_CAN_KBAUD_250,
    CYGNUM_CAN_KBAUD_500,
    CYGNUM_CAN_KBAUD_800,
    CYGNUM_CAN_KBAUD_1000,
} cyg_can_baud_rate_t;
#define CYGNUM_CAN_KBAUD_MIN CYGNUM_CAN_KBAUD_10 
#define CYGNUM_CAN_KBAUD_MAX CYGNUM_CAN_KBAUD_1000


// Note: two levels of macro are required to get proper expansion.
#define _CYG_CAN_BAUD_RATE(n) CYGNUM_CAN_KBAUD_##n
#define CYG_CAN_BAUD_RATE(n) _CYG_CAN_BAUD_RATE(n)

//
// Event types for received events. Not all event types are supported by each CAN
// hardware but normally these events should cover the most common CAN events
// that may occur. A combination of the event type values is allowed.
//
typedef enum 
{
    CYGNUM_CAN_EVENT_RX                  = 0x0001, // message received
    CYGNUM_CAN_EVENT_TX                  = 0x0002, // mesage transmitted
    CYGNUM_CAN_EVENT_WARNING_RX          = 0x0004, // tx error counter (TEC) reached warning level (>96)
    CYGNUM_CAN_EVENT_WARNING_TX          = 0x0008, // rx error counter (REC) reached warning level (>96)
    CYGNUM_CAN_EVENT_ERR_PASSIVE         = 0x0010, // CAN "error passive" occured
    CYGNUM_CAN_EVENT_BUS_OFF             = 0x0020, // CAN "bus off" error occured
    CYGNUM_CAN_EVENT_OVERRUN_RX          = 0x0040, // overrun in RX queue or hardware occured
    CYGNUM_CAN_EVENT_OVERRUN_TX          = 0x0080, // overrun in TX queue occured
    CYGNUM_CAN_EVENT_CAN_ERR             = 0x0100, // a CAN bit or frame error occured
    CYGNUM_CAN_EVENT_LEAVING_STANDBY     = 0x0200, // CAN hardware leaves standby / power don mode or is waked up
    CYGNUM_CAN_EVENT_ENTERING_STANDBY    = 0x0400, // CAN hardware enters standby / power down mode
    CYGNUM_CAN_EVENT_ARBITRATION_LOST    = 0x0800, // arbitration lost
    CYGNUM_CAN_EVENT_DEVICE_CHANGED      = 0x1000, // device changed event
} cyg_can_event_flags;

//
// Type of CAN identifier. 
//
typedef enum
{
    CYGNUM_CAN_ID_STD = 0x00, // standard ID 11 Bit
    CYGNUM_CAN_ID_EXT = 0x01  // extended ID 29 Bit
} cyg_can_id_type;

//
// Type of CAN frame
//
typedef enum
{
    CYGNUM_CAN_FRAME_DATA = 0x00, // CAN data frame
    CYGNUM_CAN_FRAME_RTR  = 0x01  // CAN remote transmission request
} cyg_can_frame_type;


//
// CAN message type for transport or transmit of CAN messages 
//
typedef struct can_message
{
    cyg_uint32          id;     // 11 Bit or 29 Bit CAN identifier - cyg_can_id_type 
    cyg_uint8           data[8];// 8 data bytes
    cyg_can_id_type     ext;    // CYGNUM_CAN_ID_STD = 11 Bit CAN id, CYGNUM_CAN_ID_EXT = 29 Bit CAN id
    cyg_can_frame_type  rtr;    // CYGNUM_CAN_FRAME_DATA = data frame, CYGNUM_CAN_FRAME_RTR = remote transmission request
    cyg_uint8           dlc;    // data length code (number of bytes (0 - 8) containing valid data
} cyg_can_message;

//
// CAN event type for reception of CAN events from driver. CAN events may be
// a received CAN message or any other status information like tx msg or
// arbitration lost
//
typedef struct cyg_can_event_st
{
    cyg_uint32      timestamp;
    cyg_can_message msg;
    cyg_uint16      flags;     
} cyg_can_event;

//
// CAN configuration - at the moment there is only one data member but we are
// prepared for future enhancements
//
typedef struct cyg_can_info_st {
    cyg_can_baud_rate_t   baud;
} cyg_can_info_t;

//
// buffer configuration - bufsize and count for tx are the number of messages
// and for rx the number of events
//
typedef struct cyg_can_buf_info_st
{  
    cyg_int32 rx_bufsize; 
    cyg_int32 rx_count;
    cyg_int32 tx_bufsize;
    cyg_int32 tx_count;
} cyg_can_buf_info_t;

//
// Timeout configuration
//
typedef struct cyg_can_timeout_info_st
{
    cyg_uint32 rx_timeout; 
    cyg_uint32 tx_timeout;
} cyg_can_timeout_info_t;

//
// this data type defines a remote transmission request buffer
//
typedef struct cyg_can_rtr_buf_st
{
    cyg_int8        handle;
    cyg_can_message msg;
} cyg_can_rtr_buf_t;

//
// Values for the handle field of the cyg_can_rtr_buf_t data structure
//
#define CYGNUM_CAN_RTR_BUF_NA    -0x01 // no free message buffer available
#define CYGNUM_CAN_RTR_BUF_INIT  -0x02 // initialize the remote message buffer


#define CYG_CAN_INFO_INIT(_baud) \
  { _baud}

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------
#endif // CYGONCE_CANIO_H
