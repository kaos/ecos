#ifndef CYGONCE_VAR_SH_REGS_H
#define CYGONCE_VAR_SH_REGS_H
//=============================================================================
//
//      sh_regs.h
//
//      SH CPU definitions
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
// Date:        1999-04-24
// Purpose:     Define CPU memory mapped registers etc.
// Usage:       #include <cyg/hal/sh_regs.h>
// Notes:
//   This file describes registers for on-core modules found in all
//   the SH3 CPUs supported by the HAL. For each CPU is defined a
//   module specification file (mod_<CPU model number>.h) which lists
//   modules (and their version if applicable) included in that
//   particular CPU model.  Note that the versioning is ad hoc;
//   it doesn't reflect Hitachi internal versioning in any way.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

// Find out which modules are supported by the chosen CPU
#include <pkgconf/system.h>
#include CYGBLD_HAL_CPU_MODULES_H

//==========================================================================
//                             CPU Definitions
//==========================================================================

//--------------------------------------------------------------------------
// Status register
#define CYGARC_REG_SR_MD                0x40000000
#define CYGARC_REG_SR_RB                0x20000000
#define CYGARC_REG_SR_BL                0x10000000
#define CYGARC_REG_SR_M                 0x00000200
#define CYGARC_REG_SR_Q                 0x00000100
#define CYGARC_REG_SR_IMASK             0x000000f0
#define CYGARC_REG_SR_I3                0x00000080
#define CYGARC_REG_SR_I2                0x00000040
#define CYGARC_REG_SR_I1                0x00000020
#define CYGARC_REG_SR_I0                0x00000010
#define CYGARC_REG_SR_S                 0x00000002
#define CYGARC_REG_SR_T                 0x00000001


//==========================================================================
//                             Module Definitions
//==========================================================================

#include <cyg/hal/mod_regs_bsc.h>
#include <cyg/hal/mod_regs_cac.h>
#include <cyg/hal/mod_regs_cpg.h>
#include <cyg/hal/mod_regs_intc.h>
#include <cyg/hal/mod_regs_mmu.h>
#include <cyg/hal/mod_regs_rtc.h>
#include <cyg/hal/mod_regs_ser.h>
#include <cyg/hal/mod_regs_tmu.h>
#include <cyg/hal/mod_regs_ubc.h>

#ifdef CYGARC_SH_MOD_DMAC
#include <cyg/hal/mod_regs_dma.h>
#endif
#ifdef CYGARC_SH_MOD_PFC
#include <cyg/hal/mod_regs_pfc.h>
#endif

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_VAR_SH_REGS_H
// End of var_regs.h
