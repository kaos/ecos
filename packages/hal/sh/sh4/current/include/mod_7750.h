#ifndef CYGONCE_HAL_MOD_77xx_H
#define CYGONCE_HAL_MOD_77xx_H

//=============================================================================
//
//      mod_7750.h
//
//      List modules available on CPU
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
// Contributors:Haruki Kashiwaya
// Date:        2000-08-09
// Purpose:     Define modules (and versions) available on this CPU.
// Usage:       Included from <cyg/hal/sh_regs.h>
//
//              
//####DESCRIPTIONEND####
//
//=============================================================================

//-----------------------------------------------------------------------------
// Modules provided by the CPU

#define CYGARC_SH_MOD_CPG  1
#define CYGARC_SH_MOD_SCIF 1


//-----------------------------------------------------------------------------
// Extra details for Cache Module (CAC)

//=============================================================================
// Icache
// Cache dimenions
#define CYGARC_SH_MOD_CAC_I_SIZE        8192  // Size of cache in bytes
#define CYGARC_SH_MOD_CAC_I_LINE_SIZE   32    // Size of a cache line
#define CYGARC_SH_MOD_CAC_I_WAYS        1     // Associativity of the cache

// Cache addressing information
// entry: bits 12 -  5
#define CYGARC_SH_MOD_ICAC_ADDRESS_BASE   0xf0000000
#define CYGARC_SH_MOD_ICAC_ADDRESS_TOP    0xf0002000
#define CYGARC_SH_MOD_ICAC_ADDRESS_STEP   0x00000020
// V : bit 0
// Writing zero to V forces an invalidate of the line
#define CYGARC_SH_MOD_ICAC_ADDRESS_FLUSH  0x00000000


//=============================================================================
// Dcache
// Cache dimenions
#define CYGARC_SH_MOD_CAC_D_SIZE        16384 // Size of cache in bytes
#define CYGARC_SH_MOD_CAC_D_LINE_SIZE   32    // Size of a cache line
#define CYGARC_SH_MOD_CAC_D_WAYS        1     // Associativity of the cache

// Cache addressing information
// entry: bits 13 -  5
#define CYGARC_SH_MOD_DCAC_ADDRESS_BASE   0xf4000000
#define CYGARC_SH_MOD_DCAC_ADDRESS_TOP    0xf0004000
#define CYGARC_SH_MOD_DCAC_ADDRESS_STEP   0x00000020
// U : bit 1
// V : bit 0
// Writing zero to both forces a flush of the line if it is dirty.
#define CYGARC_SH_MOD_DCAC_ADDRESS_FLUSH  0x00000000

//-----------------------------------------------------------------------------
// Extra details for interrupt handling
#define CYGARC_SH_SOFTWARE_IP_UPDATE

#endif // CYGONCE_HAL_MOD_77xx_H
