#ifndef CYGONCE_USBS_MSD_SCSI_H
#define CYGONCE_USBS_MSD_SCSI_H
//==========================================================================
//
//      include/usbs_msd_scsi.h
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
// Author(s):    ccoutand
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

// The standard INQUIRY data shall contain at least 36 bytes
// This is the reduced structure for Mass Storage Devices
typedef struct
{
   cyg_uint8 peripheral;         // Device Type
   cyg_uint8 rmb;                // Removable Media Bit
   cyg_uint8 version;            // Version Field
   cyg_uint8 resp_data_format;   // Response Data Format
   cyg_uint8 additional_len;     // Additional Length
   cyg_uint8 sccstp;             // SCC Supported (include embedded storage array)
   cyg_uint8 bque;               // Basic Queuing
   cyg_uint8 cmdque;             // Command Queuing
   cyg_uint8 vendor_id[8];
   cyg_uint8 product_id[16];
   cyg_uint8 product_rev[4];
} msd_scsi_inq_resp;

#define USBS_SCSI_DIRECT_ACCESS_BLOCK_DEVICE 0x00
#define USBS_SCSI_REMOVABLE_DEVICE           0x80

// Fixed-format Sense Data ( defined as a 18 bytes array )
typedef struct
{
   cyg_uint8 byte[18];
} msd_scsi_req_sense_resp;

// Set Valid information field
#define USBS_MSD_SCSI_SET_SENSE_VALID( ptr_sense, _valid_ ) \
{ \
  if( _valid_ ) \
    ptr_sense[0] |= 0x80; \
  else \
    ptr_sense[0] &= ~0x80; \
};

// Set Response Code field
#define USBS_MSD_SCSI_SET_SENSE_RESP_CODE( ptr_sense, _code_ ) \
{ \
    cyg_uint8 byte = ptr_sense[0] & 0x80; \
    ptr_sense[0] = ((_code_ & 0x7F) | byte); \
};

// Set Sense Key field
#define USBS_MSD_SCSI_SET_SENSE_KEY( ptr_sense, _key_ ) \
{ \
    cyg_uint8 byte = ptr_sense[0] & 0xF0; \
    ptr_sense[0] = ((_key_ & 0x0F) | byte); \
};

// Set Additional Sense Length field
#define USBS_MSD_SCSI_SET_SENSE_ASL( ptr_sense, _asl_ ) \
{ \
    ptr_sense[7] = _asl_; \
};

// Set Additional Sense Code field
#define USBS_MSD_SCSI_SET_SENSE_ASC( ptr_sense, _asc_ ) \
{ \
    ptr_sense[12] = _asc_; \
};

// Set Additional Sense Code Qualifier field
#define USBS_MSD_SCSI_SET_SENSE_ASCQ( ptr_sense, _ascq_ ) \
{ \
    ptr_sense[13] = _ascq_; \
};

// Init Sense Data structure
#define USBS_MSD_SCSI_INIT_SENSE_DATA( _array_ ) \
{ \
   cyg_int32 i; \
   for(i = 0; i < sizeof(msd_scsi_req_sense_resp); i++) \
   _array_[i] = 0; \
   USBS_MSD_SCSI_SET_SENSE_VALID( _array_, 1); \
   USBS_MSD_SCSI_SET_SENSE_RESP_CODE( _array_, 0x70 ); \
   USBS_MSD_SCSI_SET_SENSE_ASC( _array_, 0x0a ); \
};

cyg_int32 usbs_msd_scsi_handle_cmd( usbs_msd * );
bool usbs_msd_scsi_init( void ** ctxt );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CYGONCE_USBS_MSD_SCSI_H
