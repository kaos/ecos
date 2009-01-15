#ifndef CYGONCE_LPC2XXX_IAP_H
#define CYGONCE_LPC2XXX_IAP_H
//==========================================================================
//
//      lpc2xxx_iap.h
//
//      LPC2XXX IAP (In Application Programming) interface
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Limited
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
// Author(s):    Sergei Gavrikov
// Contributors: Sergei Gavrikov
// Date:         2007-09-20
// Purpose:
// Description:
// Usage:        #include <cyg/hal/lpc2xxx_iap.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>

//----------------------------------------------------------------------------
// The LPC2XXX IAP entry point is at 0x7FFFFFF0 if you wish to call the IAP
// functions from a THUMB function or at 0x7FFFFFF1 if you wish to call from
// an ARM function.

#define HAL_LPC2XXX_IAP_ENTRY_THUMB     0x7FFFFFF0u
#define HAL_LPC2XXX_IAP_ENTRY_ARM       0x7FFFFFF1u

#ifndef HAL_LPC2XXX_IAP_ENTRY_DEFAULT
#  define HAL_LPC2XXX_IAP_ENTRY_DEFAULT HAL_LPC2XXX_IAP_ENTRY_ARM
#endif

// LPC2XXX IAP return code
#define HAL_LPC2XXX_IAP_RETURN_CMD_SUCCESS              0
#define HAL_LPC2XXX_IAP_RETURN_INVALID_COMMAND          1
#define HAL_LPC2XXX_IAP_RETURN_SRC_ADDR_ERROR           2
#define HAL_LPC2XXX_IAP_RETURN_DST_ADDR_ERROR           3
#define HAL_LPC2XXX_IAP_RETURN_SRC_ADDR_NOT_MAPPED      4       
#define HAL_LPC2XXX_IAP_RETURN_DST_ADDR_NOT_MAPPED      5
#define HAL_LPC2XXX_IAP_RETURN_COUNT_ERROR              6
#define HAL_LPC2XXX_IAP_RETURN_INVALID_SECTOR           7
#define HAL_LPC2XXX_IAP_RETURN_SECTOR_NOT_BLANK         8
#define HAL_LPC2XXX_IAP_RETURN_SECTOR_NOT_PREPARED      9
#define HAL_LPC2XXX_IAP_RETURN_COMPARE_ERROR            10
#define HAL_LPC2XXX_IAP_RETURN_BUSY                     11
#define HAL_LPC2XXX_IAP_RETURN_PARAM_ERROR              12
#define HAL_LPC2XXX_IAP_RETURN_ADDR_ERROR               13
#define HAL_LPC2XXX_IAP_RETURN_ADDR_NOT_MAPPED          14
#define HAL_LPC2XXX_IAP_RETURN_CMD_LOCKED               15
#define HAL_LPC2XXX_IAP_RETURN_INVALID_CODE             16
#define HAL_LPC2XXX_IAP_RETURN_INVALID_BAUD_RATE        17
#define HAL_LPC2XXX_IAP_RETURN_INVALID_STOP_BIT         18

// LPC2XXX IAP commands
#define HAL_LPC2XXX_IAP_COMMAND_PREPARE_SECTORS         50
#define HAL_LPC2XXX_IAP_COMMAND_COPY_RAM_TO_FLASH       51
#define HAL_LPC2XXX_IAP_COMMAND_ERASE_SECTORS           52
#define HAL_LPC2XXX_IAP_COMMAND_BLANK_CHECK_SECTORS     53
#define HAL_LPC2XXX_IAP_COMMAND_READ_PART_ID            54
#define HAL_LPC2XXX_IAP_COMMAND_READ_BOOT_CODE_VERSION  55
#define HAL_LPC2XXX_IAP_COMMAND_COMPARE                 56

cyg_uint32
hal_lpc2xxx_iap_call (cyg_uint32 cmd, cyg_uint32 par0, cyg_uint32 par1,
                      cyg_uint32 par2, cyg_uint32 par3, cyg_uint32 * ret);

#endif//CYGONCE_LPC2XXX_IAP_H
// EOF lpc2xxx_iap.h
