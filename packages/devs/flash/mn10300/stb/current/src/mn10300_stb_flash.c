//==========================================================================
//
//      mn10300_stb_flash.c
//
//      Flash programming for Matsushita MN10300 STB
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
// Author(s):    dhowells
// Contributors: dhowells
// Date:         2001-05-15
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

//--------------------------------------------------------------------------
// Device properties

#if 0
// We use a four chip parallel AM29xxxxx module plugged into the SRAM2 socket
// on the STB board.
#define CYGNUM_FLASH_INTERLEAVE	(4)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_WIDTH      (8)
#define CYGNUM_FLASH_BASE 	(0x82800000u) /* uncached shadow region */
#else
// We use a four chip parallel AM29xxxxx module plugged into the SRAM3 socket
// on the STB board.
#define CYGNUM_FLASH_INTERLEAVE	(4)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_CHIP_MODE	(8)
#define CYGNUM_FLASH_BASE 	(0x82C00000u) /* uncached shadow region */
#endif

//--------------------------------------------------------------------------
// Platform specific extras

//--------------------------------------------------------------------------
// Now include the driver code.
#include "cyg/io/flash_am29xxxxx.inl"

// ------------------------------------------------------------------------
// EOF mn10300_stb_flash.c
