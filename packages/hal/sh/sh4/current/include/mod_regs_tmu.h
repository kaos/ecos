//=============================================================================
//
//      mod_regs_tmu.h
//
//      TMU (timer unit) Module register definitions
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
// TMU registers
#define CYGARC_REG_TOCR                 0xFFD80000 //  8 bit
#define CYGARC_REG_TSTR                 0xFFD80004 //  8 bit
#define CYGARC_REG_TCOR0                0xFFD80008 // 32 bit
#define CYGARC_REG_TCNT0                0xFFD8000C // 32 bit
#define CYGARC_REG_TCR0                 0xFFD80010 // 16 bit
#define CYGARC_REG_TCOR1                0xFFD80014 // 32 bit
#define CYGARC_REG_TCNT1                0xFFD80018 // 32 bit
#define CYGARC_REG_TCR1                 0xFFD8001C // 16 bit
#define CYGARC_REG_TCOR2                0xFFD80020 // 32 bit
#define CYGARC_REG_TCNT2                0xFFD80024 // 32 bit
#define CYGARC_REG_TCR2                 0xFFD80028 // 16 bit
#define CYGARC_REG_TCPR2                0xFFD8002C // 32 bit

// TSTR
#define CYGARC_REG_TSTR_STR0            0x0001
#define CYGARC_REG_TSTR_STR1            0x0002
#define CYGARC_REG_TSTR_STR2            0x0004

// TCR0/1/2
#define CYGARC_REG_TCR_TPSC0            0x0001
#define CYGARC_REG_TCR_TPSC1            0x0002
#define CYGARC_REG_TCR_TPSC2            0x0004
#define CYGARC_REG_TCR_CKEG0            0x0008
#define CYGARC_REG_TCR_CKEG1            0x0010
#define CYGARC_REG_TCR_UNIE             0x0020
#define CYGARC_REG_TCR_UNF              0x0100

#define CYGARC_REG_TCR_TPSC_4           (0)
#define CYGARC_REG_TCR_TPSC_16          (CYGARC_REG_TCR_TPSC0)
#define CYGARC_REG_TCR_TPSC_64          (CYGARC_REG_TCR_TPSC1)
#define CYGARC_REG_TCR_TPSC_256         (CYGARC_REG_TCR_TPSC0|CYGARC_REG_TCR_TPSC1)

// TCR2 additional bits
#define CYGARC_REG_TCR_ICPE0            0x0040
#define CYGARC_REG_TCR_ICPE1            0x0080
#define CYGARC_REG_TCR_ICPF             0x0200
