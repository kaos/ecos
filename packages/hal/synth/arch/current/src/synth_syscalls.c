//=============================================================================
//
//      synth_syscalls.c
//
//      Synthetic target access to more complex system calls
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004 Andrew Lunn
// Copyright (C) 2004 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Andrew Lunn
// Contributors:Alexander Neundorf
// Date:        2004-12-15
// Purpose:     Access to more complex system calls which require marshalling.
// Description: 
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_io.h>

void * cyg_hal_sys_shmat(int shmid, const void* shmaddr, int shmflg)
{
   void * result;
   void * raddr;
   
   result = (void *) cyg_hal_sys_ipc(CYG_HAL_SYS_IPCOP_shmat, 
                                     shmid, 
                                     shmflg, 
                                     (int) (&raddr), 
                                     (void*)shmaddr);
   return raddr;
}

int cyg_hal_sys_shmget(int key, int size, int shmflg)
{
  return cyg_hal_sys_ipc(CYG_HAL_SYS_IPCOP_shmget, key, size, shmflg, NULL);
}

int cyg_hal_sys_shmdt(const void* shmaddr)
{
  return cyg_hal_sys_ipc(CYG_HAL_SYS_IPCOP_shmdt, 0, 0, 0, 
                         ((void *) shmaddr));
}

int 
cyg_hal_sys_mmap(void *addr, unsigned long length, unsigned long prot, 
                    unsigned long flags, unsigned long fd, unsigned long off)
{
  
  struct cyg_hal_sys_mmap_args args;
  
  args.addr = (unsigned long) addr;
  args.len = length;
  args.prot = prot = prot;
  args.flags = flags;
  args.fd = fd;
  args.offset = off;
  
  return (cyg_hal_sys_mmapx(&args));
} 

int cyg_hal_sys_ftok(const char* path, int id)
{
  struct cyg_hal_sys_old_stat st;
  
  if (cyg_hal_sys_oldstat(path, &st) != 0)
    return (cyg_uint32)-1;
  
  return (cyg_uint32) (id << 24 | 
                       (st.st_dev & 0xff) << 16 | 
                       (st.st_ino & 0xffff));
}
