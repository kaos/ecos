#ifndef __MIPS_CPU_INFO_H__
#define __MIPS_CPU_INFO_H__
//==========================================================================
//
//      cpu_info.h
//
//      Architecture information for MIPS processors
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
// Author(s):    dmoseley
// Contributors: dmoseley
// Date:         2000-06-07
// Purpose:      
// Description:  
//
//####DESCRIPTIONEND####
//
//=========================================================================

#define IS_MIPS 1

/* Temporary as long a multiple protypes are copied in multiple files */
/* This variation does NOT clone the prototypes */
#define NO_MALLOC 1

#ifndef USE_ECOS_HAL_BREAKPOINTS

/* big enuf to store a trap in the BP structure */

#define BP_INST_T_DEFINED 1
typedef unsigned int bp_inst_t ;

#else /* USE_ECOS_HAL_BREAKPOINTS */

#define MEM_ADDR_DEFINED 1 
typedef struct mem_addr {
  unsigned long addr;
} mem_addr_t ;

#endif /* USE_ECOS_HAL_BREAKPOINTS */

typedef unsigned long target_register_t;

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/basetype.h>
#if CYG_BYTEORDER == CYG_MSBFIRST
#define PRINT_INSN print_insn_big_mips
#else
#define PRINT_INSN print_insn_little_mips
#endif

#undef BFD_MACH
#define BFD_MACH 0

#endif // __MIPS_CPU_INFO_H__
