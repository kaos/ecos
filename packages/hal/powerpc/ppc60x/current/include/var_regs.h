#ifndef CYGONCE_HAL_VAR_REGS_H
#define CYGONCE_HAL_VAR_REGS_H

//==========================================================================
//
//      var_regs.h
//
//      PowerPC 60x variant CPU definitions
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         2000-02-04
// Purpose:      Provide PPC60x register definitions
// Description:  Provide PPC60x register definitions
//               The short difinitions (sans CYGARC_REG_) are exported only
//               if CYGARC_HAL_COMMON_EXPORT_CPU_MACROS is defined.
// Usage:        Included via the acrhitecture register header:
//               #include <cyg/hal/ppc_regs.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

//--------------------------------------------------------------------------
// Cache
#define CYGARC_REG_HID0   1008

#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define HID0       CYGARC_REG_HID0
#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

//--------------------------------------------------------------------------
// BATs
#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#define IBAT0U          528
#define IBAT0L          529
#define IBAT1U          530
#define IBAT1L          531
#define IBAT2U          532
#define IBAT2L          533
#define IBAT3U          534
#define IBAT3L          535

#define DBAT0U          536
#define DBAT0L          537
#define DBAT1U          538
#define DBAT1L          539
#define DBAT2U          540
#define DBAT2L          541
#define DBAT3U          542
#define DBAT3L          543

#define UBAT_BEPIMASK   0xfffe0000      // effective address mask
#define UBAT_BLMASK     0x00001ffc      // block length mask
#define UBAT_VS         0x00000002      // supervisor mode valid bit
#define UBAT_VP         0x00000001      // problem mode valid bit

#define LBAT_BRPNMASK   0xfffe0000      // real address mask
#define LBAT_W          0x00000040      // write-through
#define LBAT_I          0x00000020      // caching-inhibited
#define LBAT_M          0x00000010      // memory coherence
#define LBAT_G          0x00000008      // guarded

#define LBAT_PP_NA      0x00000000      // no access
#define LBAT_PP_RO      0x00000001      // read-only
#define LBAT_PP_RW      0x00000002      // read/write
#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

//-----------------------------------------------------------------------------
#endif // ifdef CYGONCE_HAL_VAR_REGS_H
// End of var_regs.h
