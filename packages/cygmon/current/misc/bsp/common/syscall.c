//==========================================================================
//
//      syscall.c
//
//      Minimal generic syscall support.
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
// Purpose:      Minimal generic syscall support.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <errno.h>
#include <bsp/cpu.h>
#include <bsp/bsp.h>
#include "bsp_if.h"
#include "syscall.h"

/*
 * read  -- read bytes from the serial port. Ignore fd, since
 *          we only have stdin.
 */
static int
sys_read(int fd, char *buf, int nbytes)
{
    int i = 0;

    for (i = 0; i < nbytes; i++) {
	*(buf + i) = bsp_console_getc();
	if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
	    (*(buf + i + 1)) = 0;
	    break;
	}
    }
    return (i);
}


/*
 * write -- write bytes to the serial port. Ignore fd, since
 *          stdout and stderr are the same. Since we have no filesystem,
 *          open will only return an error.
 */
static int
sys_write(int fd, char *buf, int nbytes)
{
#define WBUFSIZE  256
    char ch, lbuf[WBUFSIZE];
    int  i, tosend;

    tosend = nbytes;

    while (tosend > 0) {
	for (i = 0; tosend > 0 && i < (WBUFSIZE-2); tosend--) {
	    ch = *buf++;
	    if (ch == '\n')
		lbuf[i++] = '\r';
	    lbuf[i++] = ch;
	}
	bsp_console_write(lbuf, i);
    }

    return (nbytes);
}


/*
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 */
static int
sys_open (const char *buf, int flags, int mode)
{
    return (-EIO);
}


/*
 * close -- We don't need to do anything, but pretend we did.
 */
static int
sys_close(int fd)
{
    return (0);
}


/*
 * lseek --  Since a serial port is non-seekable, we return an error.
 */
static int
sys_lseek(int fd,  int offset, int whence)
{
#ifdef ESPIPE
    return (-ESPIPE);
#else
    return (-EIO);
#endif
}


/*
 *  Generic syscall handler.
 *
 *  Returns 0 if syscall number is not handled by this
 *  module, 1 otherwise. This allows applications to
 *  extend the syscall handler by using exception chaining.
 */
int
_bsp_do_syscall(int func,		/* syscall function number */
		long arg1, long arg2,	/* up to four args.        */
		long arg3, long arg4,
		int *retval)		/* syscall return value    */
{
    int err = 0;

    switch (func) {

      case SYS_read:
	err = sys_read((int)arg1, (char *)arg2, (int)arg3);
	break;

      case SYS_write:
	err = sys_write((int)arg1, (char *)arg2, (int)arg3);
	break;

      case SYS_open:
	err = sys_open((const char *)arg1, (int)arg2, (int)arg3);
	break;

      case SYS_close:
	err = sys_close((int)arg1);
	break;

      case SYS_lseek:
	err = sys_lseek((int)arg1, (int)arg2, (int)arg3);
	break;

      case BSP_GET_SHARED:
	*(bsp_shared_t **)arg1 = bsp_shared_data;
	break;

      case SYS_meminfo:
        {
          // Return the top and size of memory.
          struct bsp_mem_info      mem;
          int                      i;
          unsigned long            u, totmem, topmem, numbanks;

          i = totmem = topmem = numbanks = 0;
          while (bsp_sysinfo(BSP_INFO_MEMORY, i++, &mem) == 0)
            {
              if (mem.kind == BSP_MEM_RAM)
                {
                  numbanks++;
                  totmem += mem.nbytes;
                  u = (unsigned long)mem.virt_start + mem.nbytes;
                  if (u > topmem)
                    topmem = u;
                }
            }
          *(unsigned long *)arg1 = totmem;
          *(unsigned long *)arg2 = topmem;
          *retval = numbanks;
        }
        return 1;
        break;

      default:
	return 0;
    }    

    *retval = err;
    return 1;
}


