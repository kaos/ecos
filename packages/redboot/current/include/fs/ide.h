//==========================================================================
//
//      ide.h
//
//      IDE Interface Driver Tables for RedBoot
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
// Author(s):    msalter
// Contributors: msalter
// Date:         2001-07-06
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef CYGONCE_REDBOOT_IDE_H
#define CYGONCE_REDBOOT_IDE_H

// IDE Register Indices
#define IDE_REG_DATA      0
#define IDE_REG_ERROR     1
#define IDE_REG_FEATURES  1
#define IDE_REG_COUNT     2
#define IDE_REG_LBALOW    3
#define IDE_REG_LBAMID    4
#define IDE_REG_LBAHI     5
#define IDE_REG_DEVICE    6
#define IDE_REG_STATUS    7
#define IDE_REG_COMMAND   7

#define IDE_STAT_BSY      0x80
#define IDE_STAT_DRDY     0x40
#define IDE_STAT_DRQ      0x08
#define IDE_STAT_ERR      0x01

//
// Drive ID offsets of interest
//
#define IDE_DEVID_GENCONFIG      0
#define IDE_DEVID_SERNO         20
#define IDE_DEVID_MODEL         54
#define IDE_DEVID_LBA_CAPACITY 120

struct ide_priv {
    cyg_uint8   controller;
    cyg_uint8   drive;
    cyg_uint16  flags;
};

/* flag values */
#define IDE_DEV_PRESENT  1  // Device is present
#define IDE_DEV_PACKET   2  // Supports packet interface
#define IDE_DEV_ADDR48   3  // Supports 48bit addressing

#endif // CYGONCE_REDBOOT_IDE_H
