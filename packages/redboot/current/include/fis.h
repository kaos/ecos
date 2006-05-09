#ifndef _FIS_H_
#define _FIS_H_
//==========================================================================
//
//      fis.h
//
//      RedBoot - FLASH image directory layout
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, tkoeller
// Date:         2000-07-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/redboot.h>
#ifdef CYGOPT_REDBOOT_FIS
#include <cyg/infra/cyg_type.h>

//the version can be tested via the VV calls to check for compatibility
#define CYG_REDBOOT_FIS_VERSION      (1)

// the following defines will be used if RedBoot is configured
// with support for redundant FIS tables, which enable failsafe updating
#ifdef CYGOPT_REDBOOT_REDUNDANT_FIS

#define CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH 10
#define CYG_REDBOOT_RFIS_VALID_MAGIC ".FisValid"  // exactly 10 bytes

#define CYG_REDBOOT_RFIS_VALID       (0xa5)       // this FIS table is valid, the only "good" value
#define CYG_REDBOOT_RFIS_IN_PROGRESS (0xfd)       // this FIS table is being modified
#define CYG_REDBOOT_RFIS_EMPTY       (0xff)       // this FIS table is empty

struct fis_valid_info
{
   char magic_name[CYG_REDBOOT_RFIS_VALID_MAGIC_LENGTH];
   unsigned char valid_flag[2];   // one of the flags defined above
   unsigned long version_count;   // with each successfull update the version count will increase by 1
};

#endif // CYGOPT_REDBOOT_REDUNDANT_FIS

#define FIS_IMAGE_DESC_SIZE_UNPADDED \
  (16 + 4 * sizeof(unsigned long) + 3 * sizeof(CYG_ADDRESS))

struct fis_image_desc {
    union
    {
    unsigned char name[16];      // Null terminated name
        #ifdef CYGOPT_REDBOOT_REDUNDANT_FIS
           struct fis_valid_info valid_info;
        #endif
    } u;
    CYG_ADDRESS   flash_base;    // Address within FLASH of image
    CYG_ADDRESS   mem_base;      // Address in memory where it executes
    unsigned long size;          // Length of image
    CYG_ADDRESS   entry_point;   // Execution entry point
    unsigned long data_length;   // Length of actual data
    unsigned char _pad[CYGNUM_REDBOOT_FIS_DIRECTORY_ENTRY_SIZE-FIS_IMAGE_DESC_SIZE_UNPADDED];
    unsigned long desc_cksum;    // Checksum over image descriptor
    unsigned long file_cksum;    // Checksum over image data
};

struct fis_image_desc *fis_lookup(char *name, int *num);

#endif // CYGOPT_REDBOOT_FIS
#endif // _FIS_H_
