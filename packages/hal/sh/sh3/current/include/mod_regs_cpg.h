//=============================================================================
//
//      mod_regs_cpg.h
//
//      CPG (clock pulse generator) Module register definitions
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
// Oscillator control registers
#define CYGARC_REG_FRQCR                0xffffff80
#define CYGARC_REG_WTCNT                0xffffff84 // read 8bit, write 16bit
#define CYGARC_REG_WTCSR                0xffffff86 // read 8bit, write 16bit

#define CYGARC_REG_WTCNT_WRITE          0x5a00     // top 8bit value for write

#define CYGARC_REG_WTCSR_WRITE          0xa500     // top 8bit value for write
#define CYGARC_REG_WTCSR_TME            0x80       // timer enable
#define CYGARC_REG_WTCSR_WT_IT          0x40       // watchdog(1)/interval(0)
#define CYGARC_REG_WTCSR_RSTS           0x20       // manual(1)/poweron(0)
#define CYGARC_REG_WTCSR_WOVF           0x10       // watchdog overflow
#define CYGARC_REG_WTCSR_IOVF           0x08       // interval overflow
#define CYGARC_REG_WTCSR_CKS2           0x04       // clock select 2
#define CYGARC_REG_WTCSR_CKS1           0x02       // clock select 1
#define CYGARC_REG_WTCSR_CKS0           0x01       // clock select 0
#define CYGARC_REG_WTCSR_CKSx_MASK      0x07       // clock select mask
// This is the period (in us) between watchdog reset and overflow.
// Note: We use max timeout delay for now.
#define CYGARC_REG_WTCSR_CKSx_SETTING   0x07       // max delay

#define CYGARC_REG_WTCSR_PERIOD         ((1000000000/(CYGHWR_HAL_SH_ONCHIP_PERIPHERAL_SPEED/4096))*256)

// Translate various CDL clock configurations to register equivalents
// for the various CPG versions
#if   (CYGARC_SH_MOD_CPG == 1) // ---------------------------- V1

// PLL1
#if   (CYGHWR_HAL_SH_OOC_PLL_1 == 0) || (CYGHWR_HAL_SH_OOC_PLL_1 == 1)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0000
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 2)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0010
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 4)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0020
#else
# error "Unsupported PLL1 setting"
#endif

// Divider1
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0004
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0008
#else
# error "Unsupported Divider1 setting"
#endif

// Divider2
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0001
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0002
#else
# error "Unsupported Divider1 setting"
#endif

// CKOEN - set in all modes but 7
#if (CYGHWR_HAL_SH_OOC_CLOCK_MODE != 7)
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0100
#else
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0000
#endif

#elif (CYGARC_SH_MOD_CPG == 2) // ---------------------------- V2

// PLL1
#if   (CYGHWR_HAL_SH_OOC_PLL_1 == 0) || (CYGHWR_HAL_SH_OOC_PLL_1 == 1)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0000
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 2)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0010
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 4)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0020
#else
# error "Unsupported PLL1 setting"
#endif

// Divider1
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0004
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0008
#else
# error "Unsupported Divider1 setting"
#endif

// Divider2
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0001
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0002
#else
# error "Unsupported Divider1 setting"
#endif

// CKOEN - set in all modes but 2
#if (CYGHWR_HAL_SH_OOC_CLOCK_MODE != 2)
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0100
#else
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0000
#endif

#elif (CYGARC_SH_MOD_CPG == 3) // ---------------------------- V3

// PLL1
#if   (CYGHWR_HAL_SH_OOC_PLL_1 == 0) || (CYGHWR_HAL_SH_OOC_PLL_1 == 1)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0000
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 2)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0010
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 3)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x8000
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 4)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0020
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 6)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x8010
#elif (CYGHWR_HAL_SH_OOC_PLL_1 == 8)
# define CYGARC_REG_FRQCR_INIT_PLL1 0x0030
#else
# error "Unsupported PLL1 setting"
#endif

// Divider1
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0004
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 3)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x4000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_1 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER1 0x0008
#else
# error "Unsupported Divider1 setting"
#endif

// Divider2
#if   (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 1)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 2)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0001
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 3)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x2000
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 4)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x0002
#elif (CYGHWR_HAL_SH_OOC_DIVIDER_2 == 6)
# define CYGARC_REG_FRQCR_INIT_DIVIDER2 0x2002
#else
# error "Unsupported Divider2 setting"
#endif

// CKOEN - set in modes 0-2
#if (CYGHWR_HAL_SH_OOC_CLOCK_MODE <= 2)
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0100
#else
# define CYGARC_REG_FRQCR_INIT_CKOEN 0x0000
#endif

#else

# error "Unsupported CPG version"

#endif

// Init value
#define CYGARC_REG_FRQCR_INIT (CYGARC_REG_FRQCR_INIT_PLL1|CYGARC_REG_FRQCR_INIT_DIVIDER1|CYGARC_REG_FRQCR_INIT_DIVIDER2|CYGARC_REG_FRQCR_INIT_CKOEN)
