//=============================================================================
//
//      mod_regs_cac.h
//
//      CAC (cache) Module register definitions
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

// Besides the below, cache sizes are defined in the CPU variant module
// headers (mod_77xx.h).

//--------------------------------------------------------------------------
// Cache registers
#define CYGARC_REG_CCR                  0xFF00001C

#define CYGARC_REG_CCR_IIX              0x00008000  // IC index enable
#define CYGARC_REG_CCR_ICI              0x00000800  // IC invalidation
#define CYGARC_REG_CCR_ICE              0x00000100  // IC Enable
#define CYGARC_REG_CCR_OIX              0x00000080  // OC index enable
#define CYGARC_REG_CCR_ORA              0x00000020  // OC RAM enable
#define CYGARC_REG_CCR_OCI              0x00000008  // OC Invalidation
#define CYGARC_REG_CCR_CB               0x00000004  // Copy-Back enable
#define CYGARC_REG_CCR_WT               0x00000002  // Write Through enable
#define CYGARC_REG_CCR_OCE              0x00000001  // OC Enable

#define CYGARC_REG_CCR_CE               0x00000101  // IC, OC Enable
#define CYGARC_REG_CCR_CF               0x00000808  // IC, OC Invalidation

