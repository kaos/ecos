#ifndef CYGONCE_V850_REGS_H
#define CYGONCE_V850_REGS_H

/*=============================================================================
//
//      v850_regs.h
//
//      NEC/V850 platform register definitions
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-03-10
// Purpose:      NEC/V850 CPU family hardware description
// Description:
// Usage:        #include <cyg/hal/v850_regs.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

// These definitions are for the SA1 (70F3017)

#define V850_REGS         0xFFFFF000

#define V850_REG_P0       0xFFFFF000
#define V850_REG_P1       0xFFFFF002
#define V850_REG_P2       0xFFFFF004
#define V850_REG_P3       0xFFFFF006
#define V850_REG_P4       0xFFFFF008
#define V850_REG_P5       0xFFFFF00A
#define V850_REG_P6       0xFFFFF00C
#define V850_REG_P7       0xFFFFF00E
#define V850_REG_P8       0xFFFFF010
#define V850_REG_P9       0xFFFFF012
#define V850_REG_P10      0xFFFFF014
#define V850_REG_P11      0xFFFFF016
#define V850_REG_P12      0xFFFFF018

#define V850_REG_PM0      0xFFFFF020
#define V850_REG_PM1      0xFFFFF022
#define V850_REG_PM2      0xFFFFF024
#define V850_REG_PM3      0xFFFFF026
#define V850_REG_PM4      0xFFFFF028
#define V850_REG_PM5      0xFFFFF02A
#define V850_REG_PM6      0xFFFFF02C
#define V850_REG_PM7      0xFFFFF02E
#define V850_REG_PM8      0xFFFFF030
#define V850_REG_PM9      0xFFFFF032
#define V850_REG_PM10     0xFFFFF034
#define V850_REG_PM11     0xFFFFF036
#define V850_REG_PM12     0xFFFFF038

#define V850_REG_MM       0xFFFFF04C

#define V850_REG_PMC12    0xFFFFF058

#define V850_REG_DWC      0xFFFFF060
#define V850_REG_BCC      0xFFFFF062
#define V850_REG_SYC      0xFFFFF064
#define V850_REG_MAM      0xFFFFF068

#define V850_REG_PSC      0xFFFFF070
#define V850_REG_PCC      0xFFFFF074
#define V850_REG_SYS      0xFFFFF078

#define V850_REG_PU0      0xFFFFF080
#define V850_REG_PU1      0xFFFFF082
#define V850_REG_PU2      0xFFFFF084
#define V850_REG_PU3      0xFFFFF086
#define V850_REG_PU10     0xFFFFF094
#define V850_REG_PU11     0xFFFFF096

#define V850_REG_PF1      0xFFFFF0A2
#define V850_REG_PF2      0xFFFFF0A4
#define V850_REG_PF10     0xFFFFF0B4

#define V850_REG_EGP0     0xFFFFF0C0
#define V850_REG_EGN0     0xFFFFF0C2

#define V850_REG_WDTIC    0xFFFFF100
#define V850_REG_PIC0     0xFFFFF102
#define V850_REG_PIC1     0xFFFFF104
#define V850_REG_PIC2     0xFFFFF106
#define V850_REG_PIC3     0xFFFFF108
#define V850_REG_PIC4     0xFFFFF10A
#define V850_REG_PIC5     0xFFFFF10C
#define V850_REG_PIC6     0xFFFFF10E
#define V850_REG_WTIIC    0xFFFFF110
#define V850_REG_TMIC00   0xFFFFF112
#define V850_REG_TMIC01   0xFFFFF114
#define V850_REG_TMIC10   0xFFFFF116
#define V850_REG_TMIC11   0xFFFFF118
#define V850_REG_TMIC2    0xFFFFF11A
#define V850_REG_TMIC3    0xFFFFF11C
#define V850_REG_TMIC4    0xFFFFF11E
#define V850_REG_TMIC5    0xFFFFF120
#define V850_REG_CSIC0    0xFFFFF122
#define V850_REG_SERIC0   0xFFFFF124
#define V850_REG_CSIC1    0xFFFFF126  // Is this correct?
#define V850_REG_SRIC0    0xFFFFF126
#define V850_REG_STIC0    0xFFFFF128
#define V850_REG_CSIC2    0xFFFFF12A
#define V850_REG_SERIC1   0xFFFFF12C
#define V850_REG_SRIC1    0xFFFFF12E
#define V850_REG_STIC1    0xFFFFF130
#define V850_REG_ADIC     0xFFFFF132
#define V850_REG_DMAIC0   0xFFFFF134
#define V850_REG_DMAIC1   0xFFFFF136
#define V850_REG_DMAIC2   0xFFFFF138
#define V850_REG_WTIC     0xFFFFF13A

#define V850_REG_ISPR     0xFFFFF166
#define V850_REG_PRCMD    0xFFFFF170

#define V850_REG_DIOA0    0xFFFFF180
#define V850_REG_DRA0     0xFFFFF182
#define V850_REG_DBC0     0xFFFFF184
#define V850_REG_DCHC0    0xFFFFF186

#define V850_REG_DIOA1    0xFFFFF190
#define V850_REG_DRA1     0xFFFFF192
#define V850_REG_DBC1     0xFFFFF194
#define V850_REG_DCHC1    0xFFFFF196

#define V850_REG_DIOA2    0xFFFFF1A0
#define V850_REG_DRA2     0xFFFFF1A2
#define V850_REG_DBC2     0xFFFFF1A4
#define V850_REG_DCHC2    0xFFFFF1A6

#define V850_REG_TM0      0xFFFFF200
#define V850_REG_CR00     0xFFFFF202
#define V850_REG_CR01     0xFFFFF204
#define V850_REG_PRM0     0xFFFFF206
#define V850_REG_TMC0     0xFFFFF208
#define V850_REG_CRC0     0xFFFFF20A
#define V850_REG_TOC0     0xFFFFF20C

#define V850_REG_TM1      0xFFFFF210
#define V850_REG_CR10     0xFFFFF212
#define V850_REG_CR11     0xFFFFF214
#define V850_REG_PRM1     0xFFFFF216
#define V850_REG_TMC1     0xFFFFF218
#define V850_REG_CRC1     0xFFFFF21A
#define V850_REG_TOC1     0xFFFFF21C

#define V850_REG_TM2      0xFFFFF240
#define V850_REG_CR20     0xFFFFF242
#define V850_REG_TCL2     0xFFFFF244
#define V850_REG_TMC2     0xFFFFF246
#define V850_REG_TM23     0xFFFFF24A
#define V850_REG_CR23     0xFFFFF24C

#define V850_REG_TM3      0xFFFFF250
#define V850_REG_CR30     0xFFFFF252
#define V850_REG_TCL3     0xFFFFF254
#define V850_REG_TMC3     0xFFFFF256

#define V850_REG_TM4      0xFFFFF260
#define V850_REG_CR40     0xFFFFF262
#define V850_REG_TCL4     0xFFFFF264
#define V850_REG_TMC4     0xFFFFF266
#define V850_REG_TM45     0xFFFFF26A
#define V850_REG_CR45     0xFFFFF26C

#define V850_REG_TM5      0xFFFFF270
#define V850_REG_CR50     0xFFFFF272
#define V850_REG_TCL5     0xFFFFF274
#define V850_REG_TMC5     0xFFFFF276

#define V850_REG_SIO0     0xFFFFF2A0
#define V850_REG_CSIM0    0xFFFFF2A2
#define V850_REG_CSIS0    0xFFFFF2A4

#define V850_REG_SIO1     0xFFFFF2B0
#define V850_REG_CSIM1    0xFFFFF2B2
#define V850_REG_CSIS1    0xFFFFF2B4

#define V850_REG_SIO2     0xFFFFF2C0
#define V850_REG_CSIM2    0xFFFFF2C2
#define V850_REG_CSIS2    0xFFFFF2C4

#define V850_REG_ASIM0    0xFFFFF300
#define V850_REG_ASIS0    0xFFFFF302
#define V850_REG_BRGC0    0xFFFFF304
#define V850_REG_TXS0     0xFFFFF306
#define V850_REG_RXS0     0xFFFFF308
#define V850_REG_BRGM0    0xFFFFF30E
#define V850_REG_BRGX0    0xFFFFF320

#define V850_REG_ASIM1    0xFFFFF310
#define V850_REG_ASIS1    0xFFFFF312
#define V850_REG_BRGC1    0xFFFFF314
#define V850_REG_TXS1     0xFFFFF316
#define V850_REG_RXS1     0xFFFFF318
#define V850_REG_BRGM1    0xFFFFF31E

#define V850_REG_IICC0    0xFFFFF340
#define V850_REG_IICS0    0xFFFFF342
#define V850_REG_IICCL0   0xFFFFF344
#define V850_REG_SVA0     0xFFFFF346
#define V850_REG_IIC0     0xFFFFF348

#define V850_REG_WTM      0xFFFFF360
#define V850_REG_OSTS     0xFFFFF380
#define V850_REG_WDCS     0xFFFFF382
#define V850_REG_WDTM     0xFFFFF384

#define V850_REG_RTBL     0xFFFFF3A0
#define V850_REG_RTBH     0xFFFFF3A2
#define V850_REG_RTPM     0xFFFFF3A4
#define V850_REG_RTPC     0xFFFFF3A6

#define V850_REG_ADM      0xFFFFF3C0
#define V850_REG_ADS      0xFFFFF3C2
#define V850_REG_ADCR     0xFFFFF3C4
#define V850_REG_ADCRH    0xFFFFF3C6

/*---------------------------------------------------------------------------*/
/* end of v850_regs.h                                                         */
#endif /* CYGONCE_V850_REGS_H */
