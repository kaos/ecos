#ifndef __CYGMON_BOARD_H__
#define __CYGMON_BOARD_H__
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-10-20
// Purpose:      
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================
// Hardware/platform/configuration specifics

#include <pkgconf/hal.h>
#include <pkgconf/cygmon.h>

#define HAVE_FLOAT_REGS         0
#define HAVE_DOUBLE_REGS        0
#define HAVE_CACHE              0 // FIXME
#define HAVE_USAGE              0 // FIXME
#define USE_CYGMON_PROTOTYPES   1
#define NOMAIN                  1
#define CYGMON_SYSTEM_SERVICES  0 // Not used, fall back to BSP support
#ifdef CYGDAT_CYGMON_USE_HELP
#define USE_HELP                1
#endif

// For breakpoint support
#define NO_MALLOC               1
#define MAX_BP_NUM              8
#include "cpu_info.h"
#define TRAP_SIZE               4
#define __set_breakpoint        set_breakpoint
#define __remove_breakpoint     clear_breakpoint
#define __write_mem_safe        memcpy
#define _breakinst              bsp_breakinsn

#endif //  __CYGMON_BOARD_H__
