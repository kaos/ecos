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
// Contributors: gthomas, dmoseley
// Date:         2000-10-25
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _FLASH_HWR_H_
#define _FLASH_HWR_H_

// SA1100 Multimedia FLASH layout
//   2x 28F016SV
//   each device is 16x1M organized as 32x128K blocks

#define FLASH_Read_ID      0x00900090
#define FLASH_Read_Status  0x00700070
#define FLASH_Clear_Status 0x00500050
#define FLASH_Status_Ready 0x00800080
#define FLASH_Program      0x00100010
#define FLASH_Block_Erase  0x00200020
#define FLASH_Confirm      0x00D000D0
#define FLASH_Reset        0xFFFFFFFF

#define FLASH_BLOCK_SIZE   0x40000

#define FLASH_Intel_code   0x89
#define FLASH_28F016SV_low 0xA0
#define FLASH_28F016SV_hi  0x66

#endif  // _FLASH_HWR_H_
