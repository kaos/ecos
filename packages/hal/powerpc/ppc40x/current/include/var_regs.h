#ifndef CYGONCE_HAL_VAR_REGS_H
#define CYGONCE_HAL_VAR_REGS_H

//==========================================================================
//
//      var_regs.h
//
//      PowerPC 40x variant CPU definitions
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
// Contributors: jskov,gthomas
// Date:         2000-08-27
// Purpose:      Provide PPC40x register definitions
// Description:  Provide PPC40x register definitions
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
// MMU control.
#ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

#define EVPR            982            // Exception vector prefix
#define M_PID           945            // Process ID

#define M_EPN_EPNMASK  0xfffff000      // effective page no mask
#define M_EPN_EV       0x00000040      // entry valid
#define M_EPN_SIZE(n)  (n<<7)          // entry size (0=1K, 1=4K, ... 7=16M)

#define M_RPN_RPNMASK  0xfffff000      // real page no mask
#define M_RPN_EX       0x00000200      // execute enable
#define M_RPN_WR       0x00000100      // write enable
#define M_RPN_W        0x00000008      // write-through (when cache enabled)
#define M_RPN_I        0x00000004      // cache inhibited
#define M_RPN_M        0x00000002      // memory coherent (not implemented)
#define M_RPN_G        0x00000001      // guarded

#define CYGARC_TLBWE(_id_, _hi_, _lo_) \
        asm volatile ("tlbwe %1,%0,0; tlbwe %2,%0,1" :: "r"(_id_), "r"(_hi_), "r"(_lo_));

#define CYGARC_TLBRE(_id_, _hi_, _lo_) \
        asm volatile ("tlbre %0,%2,0; tlbre %1,%2,1" : "=r"(_hi_), "=r"(_lo_) : "r"(_id_));

#endif // ifdef CYGARC_HAL_COMMON_EXPORT_CPU_MACROS

//-----------------------------------------------------------------------------
#endif // ifdef CYGONCE_HAL_VAR_REGS_H
// End of var_regs.h
