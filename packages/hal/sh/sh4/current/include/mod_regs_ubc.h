//=============================================================================
//
//      mod_regs_ubc.h
//
//      UBC (user break controller) Module register definitions
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-10-30
//              
//####DESCRIPTIONEND####
//
//=============================================================================

//--------------------------------------------------------------------------
// User Break Control
#define CYGARC_REG_BARA                 0xFF200000 // 32 bit
#define CYGARC_REG_BAMRA                0xFF200004 // 8 bit
#define CYGARC_REG_BBRA                 0xFF200008 // 16 bit

#define CYGARC_REG_BARB                 0xFF20000C // 32 bit
#define CYGARC_REG_BAMRB                0xFF200010 // 8 bit

#define CYGARC_REG_BBRB                 0xFF200014 // 16 bit
#define CYGARC_REG_BDRB                 0xFF200018 // 32 bit
#define CYGARC_REG_BDMRB                0xFF20001C // 32 bit

#define CYGARC_REG_BRCR                 0xFF200020 // 16 bit


#define CYGARC_REG_BRCR_CMFA            0x8000 // condition match flag A
#define CYGARC_REG_BRCR_CMFB            0x4000 // condition match flag B
#define CYGARC_REG_BRCR_PCBA            0x0400 // post execute channel A
#define CYGARC_REG_BRCR_DBEB            0x0080 // data break enable B
#define CYGARC_REG_BRCR_PCBB            0x0040 // post execute channel B
#define CYGARC_REG_BRCR_SEQ             0x0008 // sequence condition select
#define CYGARC_REG_BRCR_UBDE            0x0001 // User Break Debug Enable


#define CYGARC_REG_BBRA_DFETCH          0x0020 // Break on DFETCH
#define CYGARC_REG_BBRA_IFETCH          0x0010 // Break on IFETCH
#define CYGARC_REG_BBRA_WRITE           0x0008 // Break on WRITE
#define CYGARC_REG_BBRA_READ            0x0004 // Break on READ
#define CYGARC_REG_BBRA_SIZE_QUAD       0x0040 // Break on quad word access
#define CYGARC_REG_BBRA_SIZE_LONG       0x0003 // Break on long word access
#define CYGARC_REG_BBRA_SIZE_WORD       0x0002 // Break on word access
#define CYGARC_REG_BBRA_SIZE_BYTE       0x0001 // Break on byte access
#define CYGARC_REG_BBRA_SIZE_ANY        0x0000 // Break on any size
