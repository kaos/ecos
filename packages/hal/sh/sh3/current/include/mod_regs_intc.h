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
#define CYGARC_REG_EXCEVT               0xffffffd4
#define CYGARC_REG_INTEVT               0xffffffd8

#define CYGARC_REG_ICR                  0xfffffee0
#define CYGARC_REG_IPRA                 0xfffffee2
#define CYGARC_REG_IPRB                 0xfffffee4

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


#if (CYGARC_SH_MOD_INTC >= 2)
# define CYGARC_REG_INTEVT2             0xa4000000
# define CYGARC_REG_IRR0                0xa4000004
# define CYGARC_REG_IRR1                0xa4000006
# define CYGARC_REG_IRR2                0xa4000008
# define CYGARC_REG_ICR0                CYGARC_REG_ICR
# define CYGARC_REG_ICR1                0xa4000010
# define CYGARC_REG_ICR2                0xa4000012
# define CYGARC_REG_INTER               0xa4000014
# define CYGARC_REG_IPRC                0xa4000016
# define CYGARC_REG_IPRD                0xa4000018
# define CYGARC_REG_IPRE                0xa400001a


#define CYGARC_REG_IPRC_IRQ3_MASK         0xf000
#define CYGARC_REG_IPRC_IRQ3_PRI1         0x1000
#define CYGARC_REG_IPRC_IRQ2_MASK         0x0f00
#define CYGARC_REG_IPRC_IRQ2_PRI1         0x0100
#define CYGARC_REG_IPRC_IRQ1_MASK         0x00f0
#define CYGARC_REG_IPRC_IRQ1_PRI1         0x0010
#define CYGARC_REG_IPRC_IRQ0_MASK         0x000f
#define CYGARC_REG_IPRC_IRQ0_PRI1         0x0001

#define CYGARC_REG_IPRD_PINT07_MASK       0xf000
#define CYGARC_REG_IPRD_PINT07_PRI1       0x1000
#define CYGARC_REG_IPRD_PINT8F_MASK       0x0f00
#define CYGARC_REG_IPRD_PINT8F_PRI1       0x0100
#define CYGARC_REG_IPRD_IRQ5_MASK         0x00f0
#define CYGARC_REG_IPRD_IRQ5_PRI1         0x0010
#define CYGARC_REG_IPRD_IRQ4_MASK         0x000f
#define CYGARC_REG_IPRD_IRQ4_PRI1         0x0001

#define CYGARC_REG_IPRE_DMAC_MASK         0xf000
#define CYGARC_REG_IPRE_DMAC_PRI1         0x1000
#define CYGARC_REG_IPRE_IRDA_MASK         0x0f00
#define CYGARC_REG_IPRE_IRDA_PRI1         0x0100
#define CYGARC_REG_IPRE_SCIF_MASK         0x00f0
#define CYGARC_REG_IPRE_SCIF_PRI1         0x0010
#define CYGARC_REG_IPRE_ADC_MASK          0x000f
#define CYGARC_REG_IPRE_ADC_PRI1          0x0001

#define CYGARC_REG_ICR1_MAI               0x8000
#define CYGARC_REG_ICR1_IRQLVL            0x4000
#define CYGARC_REG_ICR1_BLMSK             0x2000
#define CYGARC_REG_ICR1_IRLSEN            0x1000
#define CYGARC_REG_ICR1_SENSE_IRQ5_shift  10
#define CYGARC_REG_ICR1_SENSE_IRQ4_shift  8
#define CYGARC_REG_ICR1_SENSE_IRQ3_shift  6
#define CYGARC_REG_ICR1_SENSE_IRQ2_shift  4
#define CYGARC_REG_ICR1_SENSE_IRQ1_shift  2
#define CYGARC_REG_ICR1_SENSE_IRQ0_shift  0
#define CYGARC_REG_ICR1_SENSE_LEVEL       0x2
#define CYGARC_REG_ICR1_SENSE_UP          0x1

// The (initial) IRQ mode is controlled by configuration.
#ifdef CYGHWR_HAL_SH_IRQ_USE_IRQLVL
# ifdef CYGHWR_HAL_SH_IRQ_ENABLE_IRLS_INTERRUPTS
#  define CYGARC_REG_ICR1_INIT (CYGARC_REG_ICR1_IRLSEN|CYGARC_REG_ICR1_IRQLVL)
# else
#  define CYGARC_REG_ICR1_INIT (CYGARC_REG_ICR1_IRQLVL)
# endif
#else
# define CYGARC_REG_ICR1_INIT 0x0000
#endif

#define CYGARC_REG_IRR0_PINT07            0x80
#define CYGARC_REG_IRR0_PINT8F            0x40
#define CYGARC_REG_IRR0_IRQ5              0x20
#define CYGARC_REG_IRR0_IRQ4              0x10
#define CYGARC_REG_IRR0_IRQ3              0x08
#define CYGARC_REG_IRR0_IRQ2              0x04
#define CYGARC_REG_IRR0_IRQ1              0x02
#define CYGARC_REG_IRR0_IRQ0              0x01

#endif



#if (CYGARC_SH_MOD_INTC >= 3)
# define CYGARC_REG_IRR3                0xa400000a
# define CYGARC_REG_IRR4                0xa400000c
# define CYGARC_REG_IPRF                0xa400001c

#define CYGARC_REG_IPRF_LCDI_MASK         0x0f00
#define CYGARC_REG_IPRF_LCDI_PRI1         0x0100
#define CYGARC_REG_IPRF_PCC0_MASK         0x00f0
#define CYGARC_REG_IPRF_PCC0_PRI1         0x0010
#define CYGARC_REG_IPRF_PCC1_MASK         0x000f
#define CYGARC_REG_IPRF_PCC1_PRI1         0x0001

#endif
