//=============================================================================
//
//      hal_syscall.c
//
//      
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
// Author(s):   msalter
// Contributors:msalter
// Date:        2000-11-5
// Purpose:     
// Description: 
//              
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#endif

#if defined(CYGSEM_REDBOOT_BSP_SYSCALLS)

#include <cyg/hal/hal_stub.h>           // Our header
#include <cyg/hal/hal_arch.h>           // HAL_BREAKINST
#include <cyg/hal/hal_cache.h>          // HAL_xCACHE_x
#include <cyg/hal/hal_intr.h>           // interrupt disable/restore

#include <cyg/hal/hal_if.h>             // ROM calling interface
#include <cyg/hal/hal_misc.h>           // Helper functions

extern int __do_syscall(int func,		// syscall function number
			long arg1, long arg2,	// up to four args.
			long arg3, long arg4,
			int *retval);		// syscall return value

#define	SYS_exit	1
#define SYS_interrupt   1000

int
hal_syscall_handler(void)
{
    int func, arg1, arg2, arg3, arg4;
    int err;

#if 0
    union arm_insn inst;

    // What is the instruction we were executing
    //
    inst.word = *(unsigned long *)(regs->_pc - ARM_INST_SIZE);

    // Not a syscall.  Don't handle it
    if ((inst.swi.rsv1 != SWI_RSV1_VALUE) || (inst.swi.swi_number != SYSCALL_SWI))
        return 0;
#endif

    func = get_register(R0);
    arg1 = get_register(R1);
    arg2 = get_register(R2);
    arg3 = get_register(R3);
    arg4 = *(unsigned int *)(get_register(SP) + 4);

    if ((get_register(PS) & CPSR_MODE_BITS) == CPSR_SUPERVISOR_MODE)
	put_register(PC, get_register(IP));
    else
	put_register(PC, get_register(LR));

    if (func == SYS_exit) {
	// We want to stop in exit so that the user may poke around
	//  to see why his app exited.
        return SIGTRAP;
    }

    if (func == SYS_interrupt) {
	//  A console interrupt landed us here.
	//  Invoke the debug agent so as to cause a SIGINT.
        return SIGINT;
    }

    if (__do_syscall(func, arg1, arg2, arg3, arg4, &err)) {
        put_register(R0, err);
	return 0;
    }

    return SIGTRAP;
}

#endif // CYGSEM_REDBOOT_BSP_SYSCALLS
