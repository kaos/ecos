//==========================================================================
//
//      cq7750_flash.c
//
//      Flash programming for Atmel device on Hitachi CQ7750 board
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
// Author(s):    Koichi Nagashima
// Contributors: Koichi Nagashima
// Date:         2001-12-20
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


// A Toshiba TC58FVB800FT-85 is equipped with the CQ7750 platform.

#define CYGNUM_FLASH_INTERLEAVE	(1)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_WIDTH	(16)
#define CYGNUM_FLASH_BASE 	(0x80000000u)
// #define CYGHWR_FLASH_AM29XXXXX_NO_WRITE_PROTECT

#include "cyg/io/flash_am29xxxxx.inl"


// ------------------------------------------------------------------------
// EOF cq7750_flash.c
