//=============================================================================
//
//      mod_regs_rtc.h
//
//      RTC (real time clock) Module register definitions
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
// RealTime Clock

#define CYGARC_REG_RC64CNT              0xfffffec0
#define CYGARC_REG_RSECCNT              0xfffffec2
#define CYGARC_REG_RMINCNT              0xfffffec4
#define CYGARC_REG_RHRCNT               0xfffffec6
#define CYGARC_REG_RWKCNT               0xfffffec8
#define CYGARC_REG_RDAYCNT              0xfffffeca
#define CYGARC_REG_RMONCNT              0xfffffecc
#define CYGARC_REG_RYRCNT               0xfffffece
#define CYGARC_REG_RSECAR               0xfffffed0
#define CYGARC_REG_RMINAR               0xfffffed2
#define CYGARC_REG_RHRAR                0xfffffed4
#define CYGARC_REG_RWKAR                0xfffffed6
#define CYGARC_REG_RDAYAR               0xfffffed8
#define CYGARC_REG_RMONAR               0xfffffeda
#define CYGARC_REG_RCR1                 0xfffffedc
#define CYGARC_REG_RCR2                 0xfffffede


#define CYGARC_REG_RCR1_CF              0x80 // carry flag
#define CYGARC_REG_RCR1_CIE             0x10 // carry interrupt enable
#define CYGARC_REG_RCR1_AIE             0x08 // alarm interrupt enable
#define CYGARC_REG_RCR1_AF              0x01 // alarm flag

#define CYGARC_REG_RCR2_PEF             0x80 // periodic interrupt flag
#define CYGARC_REG_RCR2_PES2            0x40 // periodic interrupt setting
#define CYGARC_REG_RCR2_PES1            0x20
#define CYGARC_REG_RCR2_PES0            0x10
#define CYGARC_REG_RCR2_RTCEN           0x08 // RTC enable
#define CYGARC_REG_RCR2_ADJ             0x04 // second adjustment
#define CYGARC_REG_RCR2_RESET           0x02 // reset
#define CYGARC_REG_RCR2_START           0x01 // start


