#ifndef CYGONCE_FLASH_ARM_LPC2XXX_H
#define CYGONCE_FLASH_ARM_LPC2XXX_H

//==========================================================================
//
//      flash_arm_lpc2xxx.h
//
//      Flash programming for LPC2xxx
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Hans Rosenfeld <rosenfeld@grumpf.hope-2000.org>
// Contributors: 
// Date:         2007-07-12
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

struct iap_param {
        cyg_uint32 code;
        cyg_uint32 p[4];
};

#define IAP_PREPARE 50
#define IAP_COPY    51
#define IAP_ERASE   52
#define IAP_CHECK   53
#define IAP_PARTID  54
#define IAP_VERSION 55
#define IAP_COMPARE 56

#define IAP_CMD_SUCCESS 0
#define IAP_CMD_INVALID 1
#define IAP_SRC_ADDRERR 2
#define IAP_DST_ADDRERR 3
#define IAP_SRC_ADDRMAP 4
#define IAP_DST_ADDRMAP 5
#define IAP_CNT_INVALID 6
#define IAP_SEC_INVALID 7
#define IAP_SEC_NOTBLNK 8
#define IAP_SEC_NOTPREP 9
#define IAP_CMP_INEQUAL 10
#define IAP_BSY 11

#define IAP_LOCATION 0x7ffffff1

#endif
