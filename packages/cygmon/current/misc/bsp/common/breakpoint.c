//==========================================================================
//
//      breakpoint.c
//
//      Breakpoint generation.
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
// Purpose:      Breakpoint generation.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <bsp/cpu.h>
#include <bsp/bsp.h>

#ifndef DEBUG_BREAKPOINT
#define DEBUG_BREAKPOINT 0
#endif

/*
 *  Trigger a breakpoint exception.
 */
void
bsp_breakpoint(void)
{
#if DEBUG_BREAKPOINT
    bsp_printf("Before BP\n");
#endif
#ifdef __NEED_UNDERSCORE__
    asm volatile (".globl _bsp_breakinsn\n"
		  "_bsp_breakinsn:\n");
#else
    asm volatile (".globl bsp_breakinsn\n"
		  "bsp_breakinsn:\n");
#endif
    BREAKPOINT();
#if DEBUG_BREAKPOINT
    bsp_printf("After BP\n");
#endif
}

