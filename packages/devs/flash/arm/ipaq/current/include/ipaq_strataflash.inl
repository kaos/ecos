#ifndef CYGONCE_DEVS_FLASH_IPAQ_STRATAFLASH_INL
#define CYGONCE_DEVS_FLASH_IPAQ_STRATAFLASH_INL
//==========================================================================
//
//      ipaq_strataflash.inl
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
// Author(s):    gthomas, hmt
// Contributors: gthomas
// Date:         2001-02-24
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


// The iPAQ system has two 16-bit devices.
// Doc says: a StrataFlash 28F320J3A. The 320 means 32Mbit, so 4Mbyte.
// Reality:  a StrataFlash 28F640J3A. The 640 means 64Mbit, so 8Mbyte.

#define CYGNUM_FLASH_DEVICES 	(2)
#define CYGNUM_FLASH_BASE 	(0x50000000u)
#define CYGNUM_FLASH_BASE_MASK  (0xFE000000u) // 32Mb devices
#define CYGNUM_FLASH_WIDTH 	(16)
#define CYGNUM_FLASH_BLANK      (1)

#endif  // CYGONCE_DEVS_FLASH_IPAQ_STRATAFLASH_INL
// ------------------------------------------------------------------------
// EOF ipaq_strataflash.inl
