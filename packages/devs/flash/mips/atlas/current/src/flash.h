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
// Date:         2000-07-26
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _FLASH_HWR_H_
#define _FLASH_HWR_H_

#define FLASH_Intel_code   0x89

#define FLASH_Read_ID      0x00900090
#define FLASH_Read_Query   0x00980098
#define FLASH_Read_Status  0x00700070
#define FLASH_Clear_Status 0x00500050
#define FLASH_Status_Ready 0x00800080
#define FLASH_Write_Buffer 0x00E800E8
#define FLASH_Program      0x00100010
#define FLASH_Block_Erase  0x00200020
#define FLASH_Set_Lock     0x00600060
#define FLASH_Set_Lock_Confirm 0x00010001
#define FLASH_Clear_Locks  0x00600060
#define FLASH_Clear_Locks_Confirm  0x00D000D0
#define FLASH_Confirm      0x00D000D0
#define FLASH_Configure    0x00B800B8
#define FLASH_Configure_ReadyWait      0x00000000
#define FLASH_Configure_PulseOnErase   0x00010001
#define FLASH_Configure_PulseOnProgram 0x00020002
#define FLASH_Configure_PulseOnBoth    0x00030003
#define FLASH_Reset        0x00FF00FF

#define FLASH_BLOCK_SIZE   0x40000

#define FLASH_Intel_code   0x89

#define FLASH_BASE_MASK 0xff000000

// Extended query information
struct FLASH_query {
    unsigned char manuf_code;
    unsigned char device_code;
    unsigned char _unused0[14];
    unsigned char id[3];  // Q R Y
    unsigned char _unused1[20];
    unsigned char device_size;
    unsigned char device_interface[2];
    unsigned char buffer_size[2];
    unsigned char is_block_oriented;
    unsigned char num_regions[2];
    unsigned char region_size[2];
};

#define ATLAS_SFWCTRL *((volatile unsigned *)0xbf000700)
#define ATLAS_WEN_MAGIC 0xc7

#define FLASH_WRITE_ENABLE()   (ATLAS_SFWCTRL = ATLAS_WEN_MAGIC)
#define FLASH_WRITE_DISABLE()  (ATLAS_SFWCTRL = 0)

#endif  // _FLASH_HWR_H_
