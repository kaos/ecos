//==========================================================================
//
//      plf_misc.h
//
//      HAL platform miscellaneous functions
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
// Author(s):    nickg
// Contributors: nickg, jlarmour, pjo
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

/*------------------------------------------------------------------------*/

extern void patch_dbg_syscalls(void * vector);

/*------------------------------------------------------------------------*/

void hal_platform_init(void) ;

#if 0
static __inline__ int pc_outb(int port, int value)
{
	asm("
		movl %0, %%eax
		movl %1, %%edx
		outb %%al, %%dx
		"
	:	/* No outputs. */
	:	"g" (value), "g" (port)
	:	"eax", "edx"
	);

	return value ;
}


static __inline__ int pc_inb(int port)
{	int r ;

	asm("
		movl %1, %%edx
		inb %%dx, %%al
		cbtw
		cwtl
		movl %%eax, %0
		"
	:	"=g" (r)
	:	"g" (port)
	:	"eax", "edx"
	);

	return r ;
}


static __inline__ int pc_outw(int port, int value)
{
	asm("
		movl %0, %%eax
		movl %1, %%edx
		outw %%ax, %%dx
		"
	:	/* No outputs. */
	:	"g" (value), "g" (port)
	:	"eax", "edx"
	);

	return value ;
}


static __inline__ int pc_inw(int port)
{	int r ;

	asm("
		movl %1, %%edx
		inw %%dx, %%ax
		cwtl
		movl %%eax, %0
		"
	:	"=g" (r)
	:	"g" (port)
	:	"eax", "edx"
	);

	return r ;
}

#else

int pc_outb(int port, int value) ;
int pc_inb(int port) ;
int pc_outw(int port, int value) ;
int pc_inw(int port) ;

#endif


void hal_pc_reset(void) ;


/*------------------------------------------------------------------------*/
/* End of plf_misc.h                                                      */
