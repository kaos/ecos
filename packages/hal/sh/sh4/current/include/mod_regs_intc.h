//=============================================================================
//
//      mod_regs_intc.h
//
//      INTC (interrupt controller) Module register definitions
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
// Interrupt registers, module type 1
#define CYGARC_REG_EXCEVT               0xFF000024
#define CYGARC_REG_INTEVT               0xFF000028

#define CYGARC_REG_ICR                  0xFFD00000
#define CYGARC_REG_IPRA                 0xFFD00004
#define CYGARC_REG_IPRB                 0xFFD00008
#define CYGARC_REG_IPRC                 0xFFD0000C

#define CYGARC_REG_IPRA_TMU0_MASK       0xf000
#define CYGARC_REG_IPRA_TMU0_PRI1       0x1000
#define CYGARC_REG_IPRA_TMU1_MASK       0x0f00
#define CYGARC_REG_IPRA_TMU1_PRI1       0x0100
#define CYGARC_REG_IPRA_TMU2_MASK       0x00f0
#define CYGARC_REG_IPRA_TMU2_PRI1       0x0010
#define CYGARC_REG_IPRA_RTC_MASK        0x000f
#define CYGARC_REG_IPRA_RTC_PRI1        0x0001

#define CYGARC_REG_IPRB_WDT_MASK        0xf000
#define CYGARC_REG_IPRB_WDT_PRI1        0x1000
#define CYGARC_REG_IPRB_REF_MASK        0x0f00
#define CYGARC_REG_IPRB_REF_PRI1        0x0100
#define CYGARC_REG_IPRB_SCI_MASK        0x00f0
#define CYGARC_REG_IPRB_SCI_PRI1        0x0010

#define CYGARC_REG_IPRC_GPIO_MASK       0xF000
#define CYGARC_REG_IPRC_GPIO_PRI1       0x1000
#define CYGARC_REG_IPRC_DMAC_MASK       0x0F00
#define CYGARC_REG_IPRC_DMAC_PRI1       0x0100
#define CYGARC_REG_IPRC_SCIF_MASK       0x00F0
#define CYGARC_REG_IPRC_SCIF_PRI1       0x0010
#define CYGARC_REG_IPRC_HUDI_MASK       0x000F
#define CYGARC_REG_IPRC_HUDI_PRI1       0x0001

