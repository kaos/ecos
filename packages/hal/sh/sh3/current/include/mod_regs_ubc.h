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
#define CYGARC_REG_BRCR                 0xffffff98 // 16 bit

#define CYGARC_REG_BARA                 0xffffffb0 // 32 bit
#define CYGARC_REG_BAMRA                0xffffffb4 // 8 bit (v1) / 32 bit
#define CYGARC_REG_BBRA                 0xffffffb8 // 16 bit
#define CYGARC_REG_BASRA                0xffffffe4 // 8 bit

#define CYGARC_REG_BARB                 0xffffffa0 // 32 bit
#define CYGARC_REG_BAMRB                0xffffffa4 // 8 bit (v1) / 32 bit
#define CYGARC_REG_BASRB                0xffffffe8 // 8 bit
#define CYGARC_REG_BBRB                 0xffffffa8 // 16 bit
#define CYGARC_REG_BDRB                 0xffffff90 // 32 bit
#define CYGARC_REG_BDMRB                0xffffff94 // 32 bit


#define CYGARC_REG_BRCR_CMFA            0x8000 // condition match flag A
#define CYGARC_REG_BRCR_CMFB            0x4000 // condition match flag B
#define CYGARC_REG_BRCR_PCBA            0x0400 // post execute channel A
#define CYGARC_REG_BRCR_DBEB            0x0080 // data break enable B
#define CYGARC_REG_BRCR_PCBB            0x0040 // post execute channel B
#define CYGARC_REG_BRCR_SEQ             0x0008 // sequence condition select

#if (CYGARC_SH_MOD_UBC == 1)
#define CYGARC_REG_BAMRA_BASMA          0x04   // BASRA masked
#define CYGARC_REG_BAMRA_BARA_UNMASKED  0x00   // BARA not masked
#define CYGARC_REG_BAMRA_BARA_10BIT     0x01   // Lowest 10 bit masked
#define CYGARC_REG_BAMRA_BARA_12BIT     0x02   // Lowest 12 bit masked
#define CYGARC_REG_BAMRA_BARA_MASKED    0x03   // All bits masked
#else
// mask is fully configurable in other versions of the UBC
#endif

#define CYGARC_REG_BBRA_DFETCH          0x0020 // Break on DFETCH
#define CYGARC_REG_BBRA_IFETCH          0x0010 // Break on IFETCH
#define CYGARC_REG_BBRA_WRITE           0x0008 // Break on WRITE
#define CYGARC_REG_BBRA_READ            0x0004 // Break on READ
#define CYGARC_REG_BBRA_SIZE_LONG       0x0003 // Break on long access
#define CYGARC_REG_BBRA_SIZE_WORD       0x0002 // Break on word access
#define CYGARC_REG_BBRA_SIZE_BYTE       0x0001 // Break on byte access
#define CYGARC_REG_BBRA_SIZE_ANY        0x0000 // Break on any size


//----------------------------------------------------------------------------
// Other types
#if (CYGARC_SH_MOD_UBC >= 3)
#define CYGARC_REG_BETR                 0xffffff9c // 16 bit
#define CYGARC_REG_BRSR                 0xffffffac // 32 bit
#define CYGARC_REG_BRDR                 0xffffffbc // 32 bit

#define CYGARC_REG_BBRA_DMA             0x0080 // Break on DMAC cycle
#define CYGARC_REG_BBRA_CPU             0x0040 // Break on CPU cycle
#endif
