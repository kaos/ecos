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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
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

extern CYG_ADDRWORD __do_syscall(CYG_ADDRWORD func,		// syscall function number
			CYG_ADDRWORD arg1, CYG_ADDRWORD arg2,	// up to four args.
			CYG_ADDRWORD arg3, CYG_ADDRWORD arg4,
			CYG_ADDRWORD *retval);		// syscall return value


// These are required by the ANSI C part of newlib (excluding system() of
// course).
#define	SYS_exit	1
#define	SYS_open	2
#define	SYS_close	3
#define	SYS_read	4
#define	SYS_write	5
#define	SYS_lseek	6
#define	SYS_unlink	7
#define	SYS_getpid	8
#define	SYS_kill	9
#define SYS_fstat       10
//#define SYS_sbrk	11 - not currently a system call, but reserved.

// ARGV support.
#define SYS_argvlen	12
#define SYS_argv	13

// These are extras added for one reason or another.
#define SYS_chdir	14
#define SYS_stat	15
#define SYS_chmod 	16
#define SYS_utime 	17
#define SYS_time 	18

#define SYS_interrupt   1000
#define SYS_meminfo     1001


int
hal_syscall_handler(void)
{
    CYG_ADDRWORD func, arg1, arg2, arg3, arg4;
    CYG_ADDRWORD err;

    func = get_register(EAX);
    arg1 = get_register(EBX);
    arg2 = get_register(ECX);
    arg3 = get_register(EDX);
    arg4 = 0;

    switch (func) {
      case 1:
	func = SYS_exit;
	break;
      case 3:
	func = SYS_read;
	break;
      case 4:
	func = SYS_write;
	break;
      case 37:
	func = SYS_kill;
	break;

      case 48: // install signal handler
	// FIXME!
        put_register(EAX, 0);
	return 0;

      case 184: // get program arguments
	// FIXME!
	*(int *)arg1 = 0;
	put_register(EAX, 0);
	return 0;

      default:
	return SIGTRAP;
    }

    if (func == SYS_exit) {
	// We want to stop in exit so that the user may poke around
	//  to see why his app exited.
        return SIGTRAP;
    }

    if (__do_syscall(func, arg1, arg2, arg3, arg4, &err)) {
        put_register(EAX, err);
	return 0;
    }

    return SIGTRAP;
}

#endif // CYGSEM_REDBOOT_BSP_SYSCALLS



