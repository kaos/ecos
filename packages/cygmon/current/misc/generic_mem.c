//==========================================================================
//
//      generic_mem.c
//
//      Routines for reading and writing memory.
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
// Purpose:      
// Description:  It may be appropriate to relay directly into the stubs
//               implementation of read memory, without board specific
//               considerations such as checking the allowed ranges of
//               addresses. Perhaps there needs to be some consideration
//               of address spaces or, masking addresses.
//
//               This file implements a default version of reading and writing
//               memory when none of this is an issue
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#ifdef HAVE_BSP
#include <bsp/bsp.h>
#include "cpu_info.h"
#endif
#include "board.h"
#include "monitor.h"
#include "tservice.h"

#ifndef HAVE_BSP
#include "generic-stub.h"
#endif


int
read_memory (mem_addr_t *src, int size, int amt, char *dst)
{
#ifdef HAVE_BSP
    return (bsp_memory_read((unsigned char *)src->addr, MEM_ADDR_ASI(src),
			    size << 3, amt, dst) != amt);
#else
  int totamt = size * amt;
  return (totamt != __read_mem_safe (dst, src->addr, totamt));
#endif
}

int
write_memory (mem_addr_t *dst, int size, int amt, char *src)
{
#ifdef HAVE_BSP
    return (bsp_memory_write((unsigned char *)dst->addr, MEM_ADDR_ASI(dst),
			     size << 3, amt, src) != amt);
#else
  int totamt = size * amt;
  return (totamt != __write_mem_safe (src, dst->addr, totamt));
#endif
}


   
