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
// Contributors: gthomas, msalter
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _FLASH_HWR_H_
#define _FLASH_HWR_H_

// First 4K page of flash at physical address zero is
// virtually mapped at address 0xa0000000.
#define FLASH_P2V(x) ((volatile unsigned char *)(((unsigned)(x) < 0x1000) ?  \
                           ((unsigned)(x) | 0xd0000000) :  \
                           (unsigned)(x)))

#define FLASH_BOOT_BLOCK_SIZE   0x4000

#define FLASH_Intel_code   0x89

#define FLASH_Read_ID      0x90
#define FLASH_Read_Query   0x98
#define FLASH_Read_Status  0x70
#define FLASH_Clear_Status 0x50
#define FLASH_Status_Ready 0x80
#define FLASH_Write_Buffer 0xE8
#define FLASH_Program      0x10
#define FLASH_Block_Erase  0x20
#define FLASH_Set_Lock     0x60
#define FLASH_Set_Lock_Confirm 0x01
#define FLASH_Clear_Locks  0x60
#define FLASH_Clear_Locks_Confirm  0xD0
#define FLASH_Confirm      0xD0
#define FLASH_Configure    0xB8
#define FLASH_Configure_ReadyWait      0x00
#define FLASH_Configure_PulseOnErase   0x01
#define FLASH_Configure_PulseOnProgram 0x02
#define FLASH_Configure_PulseOnBoth    0x03
#define FLASH_Reset        0xFF

#define FLASH_BLOCK_SIZE   0x10000
#define FLASH_WBUF_SIZE    32

#define FLASH_Intel_code   0x89

// Extended query information
struct FLASH_query {
    unsigned char manuf_code;
    unsigned char device_code;
    unsigned char _unused0[14];
    unsigned char id[3];  // Q Q R
    unsigned char _unused1[20];
    unsigned char device_size;
    unsigned char device_interface[2];
    unsigned char buffer_size[2];
    unsigned char is_block_oriented;
    unsigned char num_regions[2];
    unsigned char region_size[2];
};

#endif  // _FLASH_HWR_H_
