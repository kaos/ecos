#ifndef __ARM_CPU_INFO_H__
#define __ARM_CPU_INFO_H__
//==========================================================================
//
//      cpu_info.h
//
//      Architecture information for ARM processors
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
// Author(s):    
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      
// Description:  ARM is a Registered Trademark of Advanced RISC Machines
//               Limited.
//               Other Brands and Trademarks are the property of their
//               respective owners.               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#define IS_ARM 1

typedef unsigned int uint32 ;
typedef int          int32;

/* Temporary as long a multiple protypes are copied in multiple files */
/* This variation does NOT clone the prototypes */
#define NO_MALLOC 1
#define MAX_NUM_BP 32
#define MAX_HIST_ENTS 10

typedef int regnames_t;

/* big enuf to store a trap in the BP structure */

#define BP_INST_T_DEFINED 1 
typedef unsigned long bp_inst_t ;

#define MEM_ADDR_DEFINED 1 
typedef struct mem_addr {
  unsigned long addr;
} mem_addr_t ;

typedef unsigned int target_register_t;

#if defined(__ARMEB__)
#define BIG_ENDIAN_TARGET
#undef  LITTLE_ENDIAN_TARGET
#define PRINT_INSN print_insn_big_arm
#else
#define LITTLE_ENDIAN_TARGET
#undef  BIG_ENDIAN_TARGET
#define PRINT_INSN print_insn_little_arm
#endif

#define OTHERNAMES_CMD arm_othernames
extern void arm_othernames (void);

#undef BFD_MACH
#define BFD_MACH 0

#endif // __ARM_CPU_INFO_H__
