#ifndef CYGONCE_FLASH_AMD_AM29F040B_H
#define CYGONCE_FLASH_AMD_AM29F040B_H
//==========================================================================
//
//      flash.h
//
//      Flash programming - device constants, etc.
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-10-20
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>         // cyg_ types
#include <cyg/hal/hal_io.h>             // HAL_READ/WRITE macros

// Platform specific details
#include CYGDAT_DEVS_FLASH_AMD_AM29F040B_INL

#define FLASH_Read_ID        0x90
#define FLASH_Reset          0xFF
#define FLASH_Program        0xA0
#define FLASH_Block_Erase    0x30

#define FLASH_Busy           0x40 // "Toggle" bit
#define FLASH_Err            0x20
#define FLASH_Sector_Erase_Timer 0x08
#define FLASH_Drv_Verify_Err 0xff

#define FLASH_Setup_Addr1    (0x555*CYGHWR_FLASH_INTERLEAVE)
#define FLASH_Setup_Addr2    (0x2AA*CYGHWR_FLASH_INTERLEAVE)
#define FLASH_Setup_Code1    0xAA
#define FLASH_Setup_Code2    0x55
#define FLASH_Setup_Erase    0x80

#define FLASH_BLOCK_SIZE     (0x10000*CYGHWR_FLASH_INTERLEAVE)
#define FLASH_NUM_REGIONS    8

#define FLASH_DEVICE_SIZE    (FLASH_BLOCK_SIZE*FLASH_NUM_REGIONS)

#define CYGHWR_FLASH_WIDTH   8


//----------------------------------------------------------------------------
// Macros for accessing mutiple flash devices (interleaved or in series) in
// a consistent way.

#if   (CYGHWR_FLASH_WIDTH == 8)
#define FLASH_READ(_p_, _v_)  HAL_READ_UINT8(_p_, _v_)
#define FLASH_WRITE(_p_, _v_) HAL_WRITE_UINT8(_p_, _v_)
typedef cyg_uint8 flash_data_t;
#elif (CYGHWR_FLASH_WIDTH == 16)
#define FLASH_READ(_p_, _v_)  HAL_READ_UINT16(_p_, _v_)
#define FLASH_WRITE(_p_, _v_) HAL_WRITE_UINT16(_p_, _v_)
typedef cyg_uint16 flash_data_t;
#elif (CYGHWR_FLASH_WIDTH == 32)
#define FLASH_READ(_p_, _v_)  HAL_READ_UINT32(_p_, _v_)
#define FLASH_WRITE(_p_, _v_) HAL_WRITE_UINT32(_p_, _v_)
typedef cyg_uint32 flash_data_t;
#else
# error "Unsupported flash width"
#endif

#define CYGHWR_FLASH_DEVICES (CYGHWR_FLASH_INTERLEAVE*CYGHWR_FLASH_SERIES)

// Driver must use the FLASH_CMD_READ/WRITE macros to access devices. This
// will ensure that interleaved devices are properly accessed/programmed.
// However, the driver must handle devices in series itself - otherwise
// it wouldn't be possible to properly control block-specific commands.

// Note that the _READ macro takes a pointer argument to where data
// read from the device(s) is stored.

// Note that _p_, and _d_ must be of type flash_data_t*, and _c_ must
// be of type flash_data_t.

#define FLASH_DATA_READ(_p_, _d_)                       \
    CYG_MACRO_START                                     \
    int __i;                                            \
    for (__i = 0; __i < CYGHWR_FLASH_INTERLEAVE; __i++) \
        FLASH_READ((_p_)+__i, *((_d_)+__i));            \
    CYG_MACRO_END
#define FLASH_DATA_WRITE(_p_, _d_)                      \
    CYG_MACRO_START                                     \
    int __i;                                            \
    for (__i = 0; __i < CYGHWR_FLASH_INTERLEAVE; __i++) \
        FLASH_WRITE((_p_)+__i, *((_d_)+__i));           \
    CYG_MACRO_END
#define FLASH_CMD_WRITE(_p_, _c_)                       \
    CYG_MACRO_START                                     \
    int __i;                                            \
    for (__i = 0; __i < CYGHWR_FLASH_INTERLEAVE; __i++) \
        FLASH_WRITE((_p_)+__i, (_c_));                  \
    CYG_MACRO_END

#endif  // CYGONCE_FLASH_AMD_AM29F040B_H
