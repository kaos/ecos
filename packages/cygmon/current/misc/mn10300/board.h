#ifndef __CYGMON_MN10300_BOARD_H__
#define __CYGMON_MN10300_BOARD_H__
//==========================================================================
//
//      board.h
//
//      Cygmon board/platform configuration file
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
// Date:         2000-08-11
// Purpose:      
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================
// Hardware/platform/configuration specifics

// These defines are only necessary for using target_reg union in monitor.h
// It should be possible to remove that once the HAL integration is complete.
#define HAVE_FLOAT_REGS         0
#define HAVE_DOUBLE_REGS        0

#define HAVE_CACHE              0
#define HAVE_USAGE              0
#define USE_CYGMON_PROTOTYPES   1
#define NOMAIN                  1
#define CYGMON_SYSTEM_SERVICES  0 // Not used, fall back to BSP/HAL support

#ifdef CYGDAT_CYGMON_USE_HELP
#define USE_HELP                1
#endif

#define USE_ECOS_HAL_EXCEPTIONS
#define USE_ECOS_HAL_BREAKPOINTS
#define USE_ECOS_HAL_SINGLESTEP
#define USE_ECOS_HAL_SAFE_MEMORY
extern int __read_mem_safe (void *dst, void *src, int count);
extern int __write_mem_safe (void *src, void *dst, int count);


#include "cpu_info.h"
extern void bp_print (target_register_t bp_val);
extern int __set_breakpoint (target_register_t addr);
extern int __remove_breakpoint (target_register_t addr);
extern void __install_breakpoint_list (void);
extern void __clear_breakpoint_list (void);
extern int __display_breakpoint_list (void (*print_func)(target_register_t));

#define bsp_skip_instruction(regs)      __skipinst()
#define install_breakpoints()           __install_breakpoints()
#define add_mon_breakpoint(bpt)         __set_breakpoint((bpt).addr)
#define clear_mon_breakpoint(bpt)       __remove_breakpoint((bpt).addr)
#define show_breakpoints()              __display_breakpoint_list(bp_print)
#define clear_breakpoints()             __clear_breakpoint_list()

#define bsp_get_signal(exc_nr, regs)    __computeSignal(exc_nr)
#define bsp_get_pc(regs)                get_register(PC)
#define bsp_set_pc(pc, regs)            put_register(PC, pc);

#endif //  __CYGMON_MN10300_BOARD_H__
