#ifndef CYGONCE_HAL_VAR_IO_ETH_H
#define CYGONCE_HAL_VAR_IO_ETH_H
//=============================================================================
//
//      var_io_eth.h
//
//      Ethernet-specific variant definitions
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, jlarmour
// Date:        2008-07-30
// Purpose:     STM32 variant ETH specific registers
// Description: 
// Usage:       Do not include this header file directly. Instead:
//              #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#ifndef CYGONCE_HAL_VAR_IO_H
# error Do not include var_io_eth.h directly, use var_io.h
#endif


//=============================================================================
// Ethernet MAC
//
// Connectivity devices only

#ifdef CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY

#define CYGHWR_HAL_STM32_ETH_MACCR              0x0000
#define CYGHWR_HAL_STM32_ETH_MACFFR             0x0004
#define CYGHWR_HAL_STM32_ETH_MACHTHR            0x0008
#define CYGHWR_HAL_STM32_ETH_MACHTLR            0x000C
#define CYGHWR_HAL_STM32_ETH_MACMIIAR           0x0010
#define CYGHWR_HAL_STM32_ETH_MACMIIDR           0x0014
#define CYGHWR_HAL_STM32_ETH_MACFCR             0x0018
#define CYGHWR_HAL_STM32_ETH_MACVLANTR          0x001C
#define CYGHWR_HAL_STM32_ETH_MACRWUFFR          0x0028
#define CYGHWR_HAL_STM32_ETH_MACPMTCSR          0x002C
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_MACDBGR            0x0034
#endif
#define CYGHWR_HAL_STM32_ETH_MACSR              0x0038
#define CYGHWR_HAL_STM32_ETH_MACIMR             0x003C
#define CYGHWR_HAL_STM32_ETH_MACA0HR            0x0040
#define CYGHWR_HAL_STM32_ETH_MACA0LR            0x0044
#define CYGHWR_HAL_STM32_ETH_MACA1HR            0x0048
#define CYGHWR_HAL_STM32_ETH_MACA1LR            0x004C
#define CYGHWR_HAL_STM32_ETH_MACA2HR            0x0050
#define CYGHWR_HAL_STM32_ETH_MACA2LR            0x0054
#define CYGHWR_HAL_STM32_ETH_MACA3HR            0x0058
#define CYGHWR_HAL_STM32_ETH_MACA3LR            0x005C

#define CYGHWR_HAL_STM32_ETH_MMCCR              0x0100
#define CYGHWR_HAL_STM32_ETH_MMCRIR             0x0104
#define CYGHWR_HAL_STM32_ETH_MMCTIR             0x0108
#define CYGHWR_HAL_STM32_ETH_MMCRIMR            0x010C
#define CYGHWR_HAL_STM32_ETH_MMCTIMR            0x0110
#define CYGHWR_HAL_STM32_ETH_MMCTGFSCCR         0x014C
#define CYGHWR_HAL_STM32_ETH_MMCTGFMSCCR        0x0150
#define CYGHWR_HAL_STM32_ETH_MMCTGFCR           0x0168
#define CYGHWR_HAL_STM32_ETH_MMCRFCECR          0x0194
#define CYGHWR_HAL_STM32_ETH_MMCRFAECR          0x0198
#define CYGHWR_HAL_STM32_ETH_MMCRGUFCR          0x01C4

#define CYGHWR_HAL_STM32_ETH_PTPTSCR            0x0700
#define CYGHWR_HAL_STM32_ETH_PTPSSIR            0x0704
#define CYGHWR_HAL_STM32_ETH_PTPTSHR            0x0708
#define CYGHWR_HAL_STM32_ETH_PTPTSLR            0x070C
#define CYGHWR_HAL_STM32_ETH_PTPTSHUR           0x0710
#define CYGHWR_HAL_STM32_ETH_PTPTSLUR           0x0714
#define CYGHWR_HAL_STM32_ETH_PTPTSAR            0x0718
#define CYGHWR_HAL_STM32_ETH_PTPTTHR            0x071C
#define CYGHWR_HAL_STM32_ETH_PTPTTLR            0x0720
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_PTPTSSR            0x0728
#endif

#define CYGHWR_HAL_STM32_ETH_DMABMR             0x1000
#define CYGHWR_HAL_STM32_ETH_DMATPDR            0x1004
#define CYGHWR_HAL_STM32_ETH_DMARPDR            0x1008
#define CYGHWR_HAL_STM32_ETH_DMARDLAR           0x100C
#define CYGHWR_HAL_STM32_ETH_DMATDLAR           0x1010
#define CYGHWR_HAL_STM32_ETH_DMASR              0x1014
#define CYGHWR_HAL_STM32_ETH_DMAOMR             0x1018
#define CYGHWR_HAL_STM32_ETH_DMAIER             0x101C
#define CYGHWR_HAL_STM32_ETH_DMAMFBOCR          0x1020
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_DMARSWTR           0x1024
#endif
#define CYGHWR_HAL_STM32_ETH_DMACHTDR           0x1048
#define CYGHWR_HAL_STM32_ETH_DMACHRDR           0x104C
#define CYGHWR_HAL_STM32_ETH_DMACHTBAR          0x1050
#define CYGHWR_HAL_STM32_ETH_DMACHRBAR          0x1054

// MACCR

#define CYGHWR_HAL_STM32_ETH_MACCR_RE           BIT_(2)
#define CYGHWR_HAL_STM32_ETH_MACCR_TE           BIT_(3)
#define CYGHWR_HAL_STM32_ETH_MACCR_DC           BIT_(4)
#define CYGHWR_HAL_STM32_ETH_MACCR_BL(__x)      VALUE_(6, __x)
#define CYGHWR_HAL_STM32_ETH_MACCR_APCS         BIT_(7)
#define CYGHWR_HAL_STM32_ETH_MACCR_RD           BIT_(9)
#define CYGHWR_HAL_STM32_ETH_MACCR_IPCO         BIT_(10)
#define CYGHWR_HAL_STM32_ETH_MACCR_DM           BIT_(11)
#define CYGHWR_HAL_STM32_ETH_MACCR_LM           BIT_(12)
#define CYGHWR_HAL_STM32_ETH_MACCR_ROD          BIT_(13)
#define CYGHWR_HAL_STM32_ETH_MACCR_FES          BIT_(14)
#define CYGHWR_HAL_STM32_ETH_MACCR_CSD          BIT_(16)
#define CYGHWR_HAL_STM32_ETH_MACCR_IFG(__x)     VALUE_(17, (96-(__x))/8 )
#define CYGHWR_HAL_STM32_ETH_MACCR_JD           BIT_(22)
#define CYGHWR_HAL_STM32_ETH_MACCR_WD           BIT_(23)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_MACCR_CSTF         BIT_(25)
#endif

// MACFFR

#define CYGHWR_HAL_STM32_ETH_MACFFR_PM          BIT_(0)
#define CYGHWR_HAL_STM32_ETH_MACFFR_HU          BIT_(1)
#define CYGHWR_HAL_STM32_ETH_MACFFR_HM          BIT_(2)
#define CYGHWR_HAL_STM32_ETH_MACFFR_DAIF        BIT_(3)
#define CYGHWR_HAL_STM32_ETH_MACFFR_PAM         BIT_(4)
#define CYGHWR_HAL_STM32_ETH_MACFFR_BFD         BIT_(5)
#define CYGHWR_HAL_STM32_ETH_MACFFR_PCF_BLOCK   VALUE_(6,0)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_MACFFR_PCF_NOPAUSE VALUE_(6,1)
#endif
#define CYGHWR_HAL_STM32_ETH_MACFFR_PCF_ALL     VALUE_(6,2)
#define CYGHWR_HAL_STM32_ETH_MACFFR_PCF_FILTER  VALUE_(6,3)
#define CYGHWR_HAL_STM32_ETH_MACFFR_SAIF        BIT_(8)
#define CYGHWR_HAL_STM32_ETH_MACFFR_SAF         BIT_(9)
#define CYGHWR_HAL_STM32_ETH_MACFFR_HPF         BIT_(10)
#define CYGHWR_HAL_STM32_ETH_MACFFR_RA          BIT_(31)

// MACHT* omitted

// MACMIIAR

#define CYGHWR_HAL_STM32_ETH_MACMIIAR_MB        BIT_(0)
#define CYGHWR_HAL_STM32_ETH_MACMIIAR_MW        BIT_(1)
#define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR(__x)   VALUE_(2,__x)
#define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR_MASK   MASK_(2,3)

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
# define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR_MHZ_CHECK(_mhz) ((_mhz) >= 20 && (_mhz) <= 72)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F2)
# define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR_MHZ_CHECK(_mhz) ((_mhz) >= 20 && (_mhz) <= 120)
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F4)
# define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR_MHZ_CHECK(_mhz) ((_mhz) >= 20 && (_mhz) <= 168)
#endif
// This macro is shared between F1/F2/F4 families for now (despite
// irrelevance for >72Mhz speed, but that's checked above) but it's
// foreseeable that this could change for future products.
# define CYGHWR_HAL_STM32_ETH_MACMIIAR_CR_MHZ(_mhz)  (          \
  ((_mhz) >= 100) ? CYGHWR_HAL_STM32_ETH_MACMIIAR_CR(1) :       \
  ((_mhz) >= 60)  ? CYGHWR_HAL_STM32_ETH_MACMIIAR_CR(0) :       \
  ((_mhz) >= 35)  ? CYGHWR_HAL_STM32_ETH_MACMIIAR_CR(3) :       \
  /*((_mhz) >= 20)  ?*/ CYGHWR_HAL_STM32_ETH_MACMIIAR_CR(2))

#define CYGHWR_HAL_STM32_ETH_MACMIIAR_MR(__x)   VALUE_(6,__x)
#define CYGHWR_HAL_STM32_ETH_MACMIIAR_PA(__x)   VALUE_(11,__x)

// MACFCR omitted
// MACVLANTR omitted
// MACRWUFFR omitted
// MACPMTCSR omitted
// MACDBGR (F2/F4 only) omitted

// MACSR

#define CYGHWR_HAL_STM32_ETH_MACSR_PMTS         BIT_(3)
#define CYGHWR_HAL_STM32_ETH_MACSR_MMCS         BIT_(4)
#define CYGHWR_HAL_STM32_ETH_MACSR_MMCRS        BIT_(5)
#define CYGHWR_HAL_STM32_ETH_MACSR_MMCTS        BIT_(6)
#define CYGHWR_HAL_STM32_ETH_MACSR_TSTS         BIT_(9)

// MACIMR

#define CYGHWR_HAL_STM32_ETH_MACIMR_PMTIM       BIT_(3)
#define CYGHWR_HAL_STM32_ETH_MACIMR_TSTIM       BIT_(9)

// MMCR

#define CYGHWR_HAL_STM32_ETH_MMCCR_CR           BIT_(0)
#define CYGHWR_HAL_STM32_ETH_MMCCR_CSR          BIT_(1)
#define CYGHWR_HAL_STM32_ETH_MMCCR_ROR          BIT_(2)
#define CYGHWR_HAL_STM32_ETH_MMCCR_MCF          BIT_(3)
#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_MMCCR_MCP          BIT_(4)
#define CYGHWR_HAL_STM32_ETH_MMCCR_MCFHP        BIT_(5)
#endif

// MMCRIR & MMCRIMR

#define CYGHWR_HAL_STM32_ETH_MMCRIR_RFCES       BIT_(5)
#define CYGHWR_HAL_STM32_ETH_MMCRIR_RFAES       BIT_(6)
#define CYGHWR_HAL_STM32_ETH_MMCRIR_RGUFS       BIT_(17)

// MMCTIR & MMCTIMR

#define CYGHWR_HAL_STM32_ETH_MMCTIR_TGFSCS      BIT_(14)
#define CYGHWR_HAL_STM32_ETH_MMCTIR_TGFMSCS     BIT_(15)
#define CYGHWR_HAL_STM32_ETH_MMCTIR_TGFS        BIT_(21)

// PTP* omitted

// DMABMR

#define CYGHWR_HAL_STM32_ETH_DMABMR_SR          BIT_(0)
#define CYGHWR_HAL_STM32_ETH_DMABMR_DA          BIT_(1)
#define CYGHWR_HAL_STM32_ETH_DMABMR_DSL(__x)    VALUE_(2,__x)
#define CYGHWR_HAL_STM32_ETH_DMABMR_PBL(__x)    VALUE_(8,__x)
#define CYGHWR_HAL_STM32_ETH_DMABMR_RTPR(__x)   VALUE_(14,(__x)-1)
#define CYGHWR_HAL_STM32_ETH_DMABMR_FB          BIT_(16)
#define CYGHWR_HAL_STM32_ETH_DMABMR_RDP(__x)    VALUE_(17,__x)
#define CYGHWR_HAL_STM32_ETH_DMABMR_USP         BIT_(23)
#define CYGHWR_HAL_STM32_ETH_DMABMR_FPM         BIT_(24)
#define CYGHWR_HAL_STM32_ETH_DMABMR_AAB         BIT_(25)

//DMASR

#define CYGHWR_HAL_STM32_ETH_DMASR_TS           BIT_(0)
#define CYGHWR_HAL_STM32_ETH_DMASR_TPSS         BIT_(1)
#define CYGHWR_HAL_STM32_ETH_DMASR_TBUS         BIT_(2)
#define CYGHWR_HAL_STM32_ETH_DMASR_TJTS         BIT_(3)
#define CYGHWR_HAL_STM32_ETH_DMASR_ROS          BIT_(4)
#define CYGHWR_HAL_STM32_ETH_DMASR_TUS          BIT_(5)
#define CYGHWR_HAL_STM32_ETH_DMASR_RS           BIT_(6)
#define CYGHWR_HAL_STM32_ETH_DMASR_RBUS         BIT_(7)
#define CYGHWR_HAL_STM32_ETH_DMASR_RPSS         BIT_(8)
#define CYGHWR_HAL_STM32_ETH_DMASR_RWTS         BIT_(9)
#define CYGHWR_HAL_STM32_ETH_DMASR_ETS          BIT_(10)
#define CYGHWR_HAL_STM32_ETH_DMASR_FBES         BIT_(13)
#define CYGHWR_HAL_STM32_ETH_DMASR_ERS          BIT_(14)
#define CYGHWR_HAL_STM32_ETH_DMASR_AIS          BIT_(15)
#define CYGHWR_HAL_STM32_ETH_DMASR_NIS          BIT_(16)
#define CYGHWR_HAL_STM32_ETH_DMASR_RPS          MASK_(17,3)
#define CYGHWR_HAL_STM32_ETH_DMASR_TPS          MASK_(20,3)
#define CYGHWR_HAL_STM32_ETH_DMASR_EBS          MASK_(23,3)
#define CYGHWR_HAL_STM32_ETH_DMASR_MMCS         BIT_(27)
#define CYGHWR_HAL_STM32_ETH_DMASR_PMTS         BIT_(28)
#define CYGHWR_HAL_STM32_ETH_DMASR_TSTS         BIT_(29)

// DMAOMR

#define CYGHWR_HAL_STM32_ETH_DMAOMR_SR          BIT_(1)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_OSF         BIT_(2)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_RTC(__x)    VALUE_(3,__x)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_FUGF        BIT_(6)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_FEF         BIT_(7)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_ST          BIT_(13)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_TTC(__x)    VALUE_(14,__x)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_FTF         BIT_(20)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_TSF         BIT_(21)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_DFRF        BIT_(24)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_RSF         BIT_(25)
#define CYGHWR_HAL_STM32_ETH_DMAOMR_DTCEFD      BIT_(26)

// DMAIER

#define CYGHWR_HAL_STM32_ETH_DMAIER_TIE         BIT_(0)
#define CYGHWR_HAL_STM32_ETH_DMAIER_TPSIE       BIT_(1)
#define CYGHWR_HAL_STM32_ETH_DMAIER_TBUIE       BIT_(2)
#define CYGHWR_HAL_STM32_ETH_DMAIER_TJTIE       BIT_(3)
#define CYGHWR_HAL_STM32_ETH_DMAIER_ROIE        BIT_(4)
#define CYGHWR_HAL_STM32_ETH_DMAIER_TUIE        BIT_(5)
#define CYGHWR_HAL_STM32_ETH_DMAIER_RIE         BIT_(6)
#define CYGHWR_HAL_STM32_ETH_DMAIER_RBUIE       BIT_(7)
#define CYGHWR_HAL_STM32_ETH_DMAIER_RPSIE       BIT_(8)
#define CYGHWR_HAL_STM32_ETH_DMAIER_RWTIE       BIT_(9)
#define CYGHWR_HAL_STM32_ETH_DMAIER_ETIE        BIT_(10)
#define CYGHWR_HAL_STM32_ETH_DMAIER_FBEIE       BIT_(13)
#define CYGHWR_HAL_STM32_ETH_DMAIER_ERIE        BIT_(14)
#define CYGHWR_HAL_STM32_ETH_DMAIER_AISE        BIT_(15)
#define CYGHWR_HAL_STM32_ETH_DMAIER_NISE        BIT_(16)

// DMAFBOCR omitted

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_ETH_DMARSWTR_RSWTC_MASK   MASK_(0,8)
#define CYGHWR_HAL_STM32_ETH_DMARSWTR_RSWTC(__x)   ((__x)& CYGHWR_HAL_STM32_ETH_DMARSWTR_RSWTC_MASK)
#endif

// Transmit descriptor fields

#define CYGHWR_HAL_STM32_ETH_TDES0_DB           BIT_(0)
#define CYGHWR_HAL_STM32_ETH_TDES0_UF           BIT_(1)
#define CYGHWR_HAL_STM32_ETH_TDES0_ED           BIT_(2)
#define CYGHWR_HAL_STM32_ETH_TDES0_CC           MASK_(3,4)
#define CYGHWR_HAL_STM32_ETH_TDES0_VF           BIT_(7)
#define CYGHWR_HAL_STM32_ETH_TDES0_EC           BIT_(8)
#define CYGHWR_HAL_STM32_ETH_TDES0_LCO          BIT_(9)
#define CYGHWR_HAL_STM32_ETH_TDES0_NC           BIT_(10)
#define CYGHWR_HAL_STM32_ETH_TDES0_LCA          BIT_(11)
#define CYGHWR_HAL_STM32_ETH_TDES0_IPE          BIT_(12)
#define CYGHWR_HAL_STM32_ETH_TDES0_FF           BIT_(13)
#define CYGHWR_HAL_STM32_ETH_TDES0_JT           BIT_(14)
#define CYGHWR_HAL_STM32_ETH_TDES0_ES           BIT_(15)
#define CYGHWR_HAL_STM32_ETH_TDES0_IHE          BIT_(16)
#define CYGHWR_HAL_STM32_ETH_TDES0_TTSS         BIT_(17)
#define CYGHWR_HAL_STM32_ETH_TDES0_TCH          BIT_(20)
#define CYGHWR_HAL_STM32_ETH_TDES0_TER          BIT_(21)
#define CYGHWR_HAL_STM32_ETH_TDES0_CIC_DISA     VALUE_(22,0)
#define CYGHWR_HAL_STM32_ETH_TDES0_CIC_H        VALUE_(22,1)
#define CYGHWR_HAL_STM32_ETH_TDES0_CIC_HP       VALUE_(22,2)
#define CYGHWR_HAL_STM32_ETH_TDES0_CIC_HPP      VALUE_(22,3)
#define CYGHWR_HAL_STM32_ETH_TDES0_TTSE         BIT_(25)
#define CYGHWR_HAL_STM32_ETH_TDES0_DP           BIT_(26)
#define CYGHWR_HAL_STM32_ETH_TDES0_DC           BIT_(27)
#define CYGHWR_HAL_STM32_ETH_TDES0_FS           BIT_(28)
#define CYGHWR_HAL_STM32_ETH_TDES0_LS           BIT_(29)
#define CYGHWR_HAL_STM32_ETH_TDES0_IC           BIT_(30)
#define CYGHWR_HAL_STM32_ETH_TDES0_OWN          BIT_(31)

#define CYGHWR_HAL_STM32_ETH_TDES1_TBS1(__x)    VALUE_(0,__x)
#define CYGHWR_HAL_STM32_ETH_TDES1_TBS2(__x)    VALUE_(16,__x)

// Receive descriptor fields

#define CYGHWR_HAL_STM32_ETH_RDES0_PCE          BIT_(0)
#define CYGHWR_HAL_STM32_ETH_RDES0_CE           BIT_(1)
#define CYGHWR_HAL_STM32_ETH_RDES0_DE           BIT_(2)
#define CYGHWR_HAL_STM32_ETH_RDES0_RE           BIT_(3)
#define CYGHWR_HAL_STM32_ETH_RDES0_RWT          BIT_(4)
#define CYGHWR_HAL_STM32_ETH_RDES0_FT           BIT_(5)
#define CYGHWR_HAL_STM32_ETH_RDES0_LCO          BIT_(6)
#define CYGHWR_HAL_STM32_ETH_RDES0_IPHCE        BIT_(7)
#define CYGHWR_HAL_STM32_ETH_RDES0_LS           BIT_(8)
#define CYGHWR_HAL_STM32_ETH_RDES0_FS           BIT_(9)
#define CYGHWR_HAL_STM32_ETH_RDES0_VLAN         BIT_(10)
#define CYGHWR_HAL_STM32_ETH_RDES0_OE           BIT_(11)
#define CYGHWR_HAL_STM32_ETH_RDES0_LE           BIT_(12)
#define CYGHWR_HAL_STM32_ETH_RDES0_SAF          BIT_(13)
#define CYGHWR_HAL_STM32_ETH_RDES0_DESCE        BIT_(14)
#define CYGHWR_HAL_STM32_ETH_RDES0_ES           BIT_(15)
#define CYGHWR_HAL_STM32_ETH_RDES0_FL(__x)      (((__x)>>16)&0x3FFF)
#define CYGHWR_HAL_STM32_ETH_RDES0_AFM          BIT_(30)
#define CYGHWR_HAL_STM32_ETH_RDES0_OWN          BIT_(31)

#define CYGHWR_HAL_STM32_ETH_RDES1_RBS1(__x)    VALUE_(0,__x)
#define CYGHWR_HAL_STM32_ETH_RDES1_RCH          BIT_(14)
#define CYGHWR_HAL_STM32_ETH_RDES1_RER          BIT_(15)
#define CYGHWR_HAL_STM32_ETH_RDES1_RBS2(__x)    VALUE_(16,__x)


// GPIO pins

// NOTE: The platform specific (re-)mapping of pins is provided in the relevant
// target specific "plf_io.h" header file. These definitions just cover the
// fixed mappings.

// MCO1 clock to PHY
#define CYGHWR_HAL_STM32_ETH_MCO                CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  8,  0, PUSHPULL,  NONE,     AT_LEAST(50) )

// MII interface
#define CYGHWR_HAL_STM32_ETH_MII_MDC            CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C,  1, 11, PUSHPULL,  NONE,     AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD2           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C,  2, 11, PUSHPULL,  NONE,     AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_MDIO           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  2, 11, PUSHPULL,  NONE,     AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TX_CLK         CYGHWR_HAL_STM32_PIN_ALTFN_IN(  C,  3, 11, OPENDRAIN, FLOATING )
#define CYGHWR_HAL_STM32_ETH_MII_RX_CLK         CYGHWR_HAL_STM32_PIN_ALTFN_IN(  A,  1, 11, OPENDRAIN, FLOATING )

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

// MII interface
#define CYGHWR_HAL_STM32_ETH_MII_TX_CRS         CYGHWR_HAL_STM32_PIN_IN( A,  0, FLOATING )
#define CYGHWR_HAL_STM32_ETH_MII_COL            CYGHWR_HAL_STM32_PIN_IN( A,  3, FLOATING )
#define CYGHWR_HAL_STM32_ETH_MII_RX_ER          CYGHWR_HAL_STM32_PIN_IN( B, 10, FLOATING )

#define CYGHWR_HAL_STM32_ETH_MII_TX_EN          CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 11, 11, PUSHPULL, NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD0           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 12, 11, PUSHPULL, NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD1           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 13, 11, PUSHPULL, NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_PPS_OUT        CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  5, 11, PUSHPULL, NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD3           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  8, 11, PUSHPULL, NONE, AT_LEAST(50) )

// RMII interface
#define CYGHWR_HAL_STM32_ETH_RMII_MDC           CYGHWR_HAL_STM32_ETH_MII_MDC
#define CYGHWR_HAL_STM32_ETH_RMII_REF_CLK       CYGHWR_HAL_STM32_ETH_MII_RX_CLK
#define CYGHWR_HAL_STM32_ETH_RMII_MDIO          CYGHWR_HAL_STM32_ETH_MII_MDIO
#define CYGHWR_HAL_STM32_ETH_RMII_CRS_DV        CYGHWR_HAL_STM32_ETH_MII_RX_DV
#define CYGHWR_HAL_STM32_ETH_RMII_RXD0          CYGHWR_HAL_STM32_ETH_MII_RXD0
#define CYGHWR_HAL_STM32_ETH_RMII_RXD1          CYGHWR_HAL_STM32_ETH_MII_RXD1
#define CYGHWR_HAL_STM32_ETH_RMII_TX_EN         CYGHWR_HAL_STM32_ETH_MII_TX_EN
#define CYGHWR_HAL_STM32_ETH_RMII_TXD0          CYGHWR_HAL_STM32_ETH_MII_TXD0
#define CYGHWR_HAL_STM32_ETH_RMII_TXD1          CYGHWR_HAL_STM32_ETH_MII_TXD1
#define CYGHWR_HAL_STM32_ETH_RMII_PPS_OUT       CYGHWR_HAL_STM32_ETH_MII_PPS_OUT

// Clock controls

#define CYGHWR_HAL_STM32_ETH_MAC_CLOCK          CYGHWR_HAL_STM32_CLOCK( AHB, ETHMAC )
#define CYGHWR_HAL_STM32_ETH_TX_CLOCK           CYGHWR_HAL_STM32_CLOCK( AHB, ETHMACTX )
#define CYGHWR_HAL_STM32_ETH_RX_CLOCK           CYGHWR_HAL_STM32_CLOCK( AHB, ETHMACRX )

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

// MII interface
#define CYGHWR_HAL_STM32_ETH_MII_RX_DV          CYGHWR_HAL_STM32_PIN_ALTFN_IN(  A,  7, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_RXD0           CYGHWR_HAL_STM32_PIN_ALTFN_IN(  C,  4, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_RXD1           CYGHWR_HAL_STM32_PIN_ALTFN_IN(  C,  5, 11, OPENDRAIN, NONE )

// RMII interface
#define CYGHWR_HAL_STM32_ETH_RMII_MDC           CYGHWR_HAL_STM32_ETH_MII_MDC
#define CYGHWR_HAL_STM32_ETH_RMII_REF_CLK       CYGHWR_HAL_STM32_ETH_MII_RX_CLK
#define CYGHWR_HAL_STM32_ETH_RMII_MDIO          CYGHWR_HAL_STM32_ETH_MII_MDIO
#define CYGHWR_HAL_STM32_ETH_RMII_CRS_DV        CYGHWR_HAL_STM32_ETH_MII_RX_DV
#define CYGHWR_HAL_STM32_ETH_RMII_RXD0          CYGHWR_HAL_STM32_ETH_MII_RXD0
#define CYGHWR_HAL_STM32_ETH_RMII_RXD1          CYGHWR_HAL_STM32_ETH_MII_RXD1
#define CYGHWR_HAL_STM32_ETH_RMII_TX_EN         CYGHWR_HAL_STM32_ETH_MII_TX_EN
#define CYGHWR_HAL_STM32_ETH_RMII_TXD0          CYGHWR_HAL_STM32_ETH_MII_TXD0
#define CYGHWR_HAL_STM32_ETH_RMII_TXD1          CYGHWR_HAL_STM32_ETH_MII_TXD1
#define CYGHWR_HAL_STM32_ETH_RMII_PPS_OUT       CYGHWR_HAL_STM32_ETH_MII_PPS_OUT

// Clock controls

#define CYGHWR_HAL_STM32_ETH_MAC_CLOCK          CYGHWR_HAL_STM32_CLOCK( AHB1, ETHMAC )
#define CYGHWR_HAL_STM32_ETH_TX_CLOCK           CYGHWR_HAL_STM32_CLOCK( AHB1, ETHMACTX )
#define CYGHWR_HAL_STM32_ETH_RX_CLOCK           CYGHWR_HAL_STM32_CLOCK( AHB1, ETHMACRX )

#endif // elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#endif // CYGHWR_HAL_CORTEXM_STM32_CONNECTIVITY

#endif // CYGONCE_HAL_VAR_IO_ETH_H
//-----------------------------------------------------------------------------
// end of var_io_eth.h
