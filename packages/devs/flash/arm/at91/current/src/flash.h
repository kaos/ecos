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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-07-17
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _FLASH_HWR_H_
#define _FLASH_HWR_H_

// Atmel AT29LV1024

#define FLASH_Read_ID      0x9090
#define FLASH_Program      0xA0A0
#define FLASH_Reset        0xF0F0

#define FLASH_Key_Addr0    0x5555
#define FLASH_Key_Addr1    0x2AAA
#define FLASH_Key0         0xAAAA
#define FLASH_Key1         0x5555

#define FLASH_Atmel_code      0x1F
#define FLASH_ATMEL_29LV1024  0x26

#define FLASH_PROGRAM_OK    0x0000
#define FLASH_LENGTH_ERROR  0x0001
#define FLASH_PROGRAM_ERROR 0x0002

#define FLASH_PAGE_SIZE    0x100
#define FLASH_PAGE_MASK   ~(FLASH_PAGE_SIZE-1)
#define FLASH_PAGE_OFFSET  (FLASH_PAGE_SIZE-1)

#endif  // _FLASH_HWR_H_
