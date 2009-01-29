#ifndef CYGONCE_CAN_AT91SAM7_H
#define CYGONCE_CAN_AT91SAM7_H
//==========================================================================
//
//      devs/can/arm/at91sam7x/current/include/can_at91sam7.inl
//
//      CAN message macros for Atmel AT91SAM7X CAN driver
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler
// Contributors: Uwe Kindler
// Date:         2007-02-08
// Purpose:      Support AT91SAM7X on-chip CAN moduls
// Description: 
//
//####DESCRIPTIONEND####
//
//==========================================================================


//==========================================================================
//                               INCLUDE
//==========================================================================
#include <pkgconf/devs_can_at91sam7.h>


//==========================================================================
//                              DATA TYPES
//==========================================================================

//--------------------------------------------------------------------------
// We define our own CAN message data type here. This structure needs less
// memory than the common CAN message defined by IO layer. This is important
// because the AT91SAM7 contains only 64 KBytes RAM memory
//
typedef struct st_at91sam7_can_message
{
    cyg_can_msg_data data;// 8 data bytes
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
    cyg_uint32       id;  // also extended identifiers (29 Bit) are supported
    cyg_uint8        ctrl;// control stores extended flag, rtr flag and dlc  
#else
    //
    // only standard identifiers are supported - we need only 11 bit of
    // the data word to store the identifier. So we have 5 bit left to store
    // the the rtr flag and the dlc flag. We do not need the IDE flag because
    // only standard identifiers are supported
    //
    cyg_uint16       id; 
#endif
} at91sam7_can_message;


//--------------------------------------------------------------------------
// We also define an own event structure here to store the received events
// This event structure uses the device CAN message structure and
// 16 Bit value for timestamps
//
typedef struct st_at91sam7_can_event
{
    cyg_uint16           flags;
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP
    cyg_uint16           timestamp;
#endif
    at91sam7_can_message msg;
} at91sam7_can_event;



//==========================================================================
//                                DEFINES
//==========================================================================
#ifdef CYGOPT_IO_CAN_EXT_CAN_ID
//
// If we use extended identifier then we store the message parameters
// into control word
//
#define AT91SAM7_CAN_SET_DLC(_msg_, _dlc_)  ((_msg_).ctrl = (_dlc_)) // this also clears the ctrl
#define AT91SAM7_CAN_SET_EXT(_msg_)         ((_msg_).ctrl |= 0x01 << 4) 
#define AT91SAM7_CAN_SET_RTR(_msg_)         ((_msg_).ctrl |= 0x01 << 5)

#define AT91SAM7_CAN_GET_DLC(_msg_)         ((_msg_).ctrl & 0x0F)
#define AT91SAM7_CAN_IS_EXT(_msg_)          ((((_msg_).ctrl >> 4) & 0x01) != 0)
#define AT91SAM7_CAN_IS_RTR(_msg_)          ((((_msg_).ctrl >> 5) & 0x01) != 0)
#define AT91SAM7_CAN_GET_ID(_msg_)          ((_msg_).id & CYG_CAN_EXT_ID_MASK)    
#else // CYGOPT_IO_CAN_EXT_CAN_ID
//
// We use only standard identifiers and we can store the message parameters
// into the upper 5 bits of the 16 bit id field (only 11 bits are required for
// standard frames
//
#define AT91SAM7_CAN_SET_DLC(_msg_, _dlc_)  ((_msg_).id |= (_dlc_) << 11)
#define AT91SAM7_CAN_SET_EXT(_msg_)         // we do not need to support this flag - only std IDs supported 
#define AT91SAM7_CAN_SET_RTR(_msg_)         ((_msg_).id |= 0x01 << 15)

#define AT91SAM7_CAN_GET_DLC(_msg_)         (((_msg_).id >> 11) & 0x0F)
#define AT91SAM7_CAN_IS_EXT(_msg_)          0 // we do not support extended identifiers so this is always false
#define AT91SAM7_CAN_IS_RTR(_msg_)          ((((_msg_).id >> 15) & 0x01) != 0)
#define AT91SAM7_CAN_GET_ID(_msg_)          ((_msg_).id & CYG_CAN_STD_ID_MASK)
#endif // CYGOPT_IO_CAN_EXT_CAN_ID


//---------------------------------------------------------------------------
// The foolowing macros are required for CAN devicedriver. We define our own
// CAN messaeg and event structures and therefore we also need to define the
// two message conversion macros that translate out message/event into the
// standard CAN message/event
//
#define CYG_CAN_MSG_T   at91sam7_can_message
#define CYG_CAN_EVENT_T at91sam7_can_event

//
// We need to copy the timestamp field only if timestamps are supported by
// driver
//
#ifdef CYGOPT_IO_CAN_SUPPORT_TIMESTAMP
#define CYG_CAN_READ_TIMESTAMP(_ioevent_ptr_, _devevent_ptr_) ((_ioevent_ptr_)->timestamp = (_devevent_ptr_)->timestamp)
#else
#define CYG_CAN_READ_TIMESTAMP(_ioevent_ptr_, _devevent_ptr_)
#endif


#define CYG_CAN_WRITE_MSG(_devmsg_ptr_, _iomsg_ptr_)                                         \
CYG_MACRO_START                                                                              \
    (_devmsg_ptr_)->data = (_iomsg_ptr_)->data;                                              \
    (_devmsg_ptr_)->id   = (_iomsg_ptr_)->id;                                                \
    AT91SAM7_CAN_SET_DLC(*(_devmsg_ptr_), (_iomsg_ptr_)->dlc);                               \
    if (CYGNUM_CAN_ID_EXT == (_iomsg_ptr_)->ext) {AT91SAM7_CAN_SET_EXT(*(_devmsg_ptr_));}    \
    if (CYGNUM_CAN_FRAME_RTR == (_iomsg_ptr_)->rtr) {AT91SAM7_CAN_SET_RTR(*(_devmsg_ptr_));} \
CYG_MACRO_END


#define CYG_CAN_READ_EVENT(_ioevent_ptr_, _devevent_ptr_)                     \
CYG_MACRO_START                                                               \
    (_ioevent_ptr_)->flags    = (_devevent_ptr_)->flags;                      \
    (_ioevent_ptr_)->msg.data = (_devevent_ptr_)->msg.data;                   \
    (_ioevent_ptr_)->msg.id   = AT91SAM7_CAN_GET_ID((_devevent_ptr_)->msg);   \
    (_ioevent_ptr_)->msg.dlc  = AT91SAM7_CAN_GET_DLC((_devevent_ptr_)->msg);  \
    if (AT91SAM7_CAN_IS_EXT((_devevent_ptr_)->msg))  {                        \
        (_ioevent_ptr_)->msg.ext = CYGNUM_CAN_ID_EXT; }                       \
    else {                                                                    \
        (_ioevent_ptr_)->msg.ext = CYGNUM_CAN_ID_STD; }                       \
    if (AT91SAM7_CAN_IS_RTR((_devevent_ptr_)->msg))  {                        \
        (_ioevent_ptr_)->msg.rtr = CYGNUM_CAN_FRAME_RTR; }                    \
    else {                                                                    \
        (_ioevent_ptr_)->msg.rtr = CYGNUM_CAN_FRAME_DATA; }                   \
    CYG_CAN_READ_TIMESTAMP(_ioevent_ptr_, _devevent_ptr_);                    \
CYG_MACRO_END

//---------------------------------------------------------------------------
#endif // CYGONCE_CAN_AT91SAM7_H
