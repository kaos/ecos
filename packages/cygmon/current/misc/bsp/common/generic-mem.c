//==========================================================================
//
//      generic-mem.c
//
//      Generic support for safe memory read/write.
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
// Purpose:      Generic support for safe memory read/write.
// Description:  Some targets may need to provide their own version.
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <stdlib.h>
#include <setjmp.h>
#include <bsp/bsp.h>

typedef void (*moveproc_t)(void *s, void *d);

static jmp_buf __errjmp;

/*
 * These are globals because we want them preserved
 * across function calls.
 */
static bsp_handler_t __oldtrap;
static int __done;


static void
move_8(void *src, void *dest)
{
    *(char *)dest = *(char *)src;
}


static void
move_16(void *src, void *dest)
{
    *(short *)dest = *(short *)src;
}


static void
move_32(void *src, void *dest)
{
    *(int *)dest = *(int *)src;
}


static int
err_trap(int exc_nr, void *regs)
{
    longjmp(__errjmp, 1);
}


int
bsp_memory_read(void *addr,    /* start addr of memory to read */
		int  asid,     /* address space id */
		int  rsize,    /* size of individual read operation */
		int  nreads,   /* number of read operations */
		void *buf)     /* result buffer */
{
    if (nreads <= 0)
	return 0;

    __oldtrap = bsp_install_dbg_handler(err_trap);
    
    if (setjmp(__errjmp) == 0) {
	moveproc_t move_mem;
	int        incr;
	char       *src, *dest;

	switch (rsize) {
	  case 8:
	    move_mem = move_8;
	    incr = 1;
	    break;
	  case 16:
	    move_mem = move_16;
	    incr = 2;
	    break;
	  case 32:
	    move_mem = move_32;
	    incr = 4;
	    break;
	  default:
	    (void)bsp_install_dbg_handler(__oldtrap);
	    return 0;
	}

	src = addr;
	dest = buf;

	for (__done = 0; __done < nreads; __done++) {
	    move_mem(src, dest);
	    src  += incr;
	    dest += incr;
	}
    }

    (void)bsp_install_dbg_handler(__oldtrap);
    return __done;
}


int bsp_memory_write(void *addr,   /* start addr of memory to write */
                     int  asid,    /* address space id */
                     int  wsize,   /* size of individual write operation */
                     int  nwrites, /* number of write operations */
                     void *buf)    /* source buffer for write data */
{
    if (nwrites <= 0)
	return 0;

    __oldtrap = bsp_install_dbg_handler(err_trap);
    
    if (setjmp(__errjmp) == 0) {
	moveproc_t move_mem;
	int        incr;
	char       *src, *dest;

	switch (wsize) {
	  case 8:
	    move_mem = move_8;
	    incr = 1;
	    break;
	  case 16:
	    move_mem = move_16;
	    incr = 2;
	    break;
	  case 32:
	    move_mem = move_32;
	    incr = 4;
	    break;
	  default:
	    (void)bsp_install_dbg_handler(__oldtrap);
	    return 0;
	}

	src = buf;
	dest = addr;

	for (__done = 0; __done < nwrites; __done++) {
	    move_mem(src, dest);
	    src  += incr;
	    dest += incr;
	}
    }

    (void)bsp_install_dbg_handler(__oldtrap);
    return __done;
}
