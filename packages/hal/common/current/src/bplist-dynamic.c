//==========================================================================
//
//      bplist-dynamic.c
//
//      Dynamic breakpoint list.
//      Currently only statically allocated.  (ie NO_MALLOC is assumed)
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
// Contributors: dmoseley
// Date:         2000-07-11
// Purpose:      Dynamic breakpoint list.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#if defined(CYGNUM_HAL_BREAKPOINT_LIST_SIZE) && (CYGNUM_HAL_BREAKPOINT_LIST_SIZE > 0) && defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>

/*
 * A simple target breakpoint list without using malloc.
 * To use this package, you must define HAL_BREAKINST_SIZE to be the size
 * in bytes of a trap instruction (max if there's more than one),
 * HAL_BREAKINST to the opcode value of the instruction, and
 * HAL_BREAKINST_TYPE to be the type necessary to hold the opcode value.
 */

struct breakpoint_list {
  target_register_t  addr;
  char old_contents [HAL_BREAKINST_SIZE];
  struct breakpoint_list *next;
  char in_memory;
} *breakpoint_list = NULL;

static HAL_BREAKINST_TYPE break_inst = HAL_BREAKINST;
static void *_break_inst = &break_inst;

static struct breakpoint_list bp_list [CYGNUM_HAL_BREAKPOINT_LIST_SIZE];
static struct breakpoint_list *free_bp_list = NULL;
static int curr_bp_num = 0;

int
__set_breakpoint (target_register_t addr)
{
  struct breakpoint_list **addent = &breakpoint_list;
  struct breakpoint_list *l = breakpoint_list;
  struct breakpoint_list *newent;

  while (l != NULL && l->addr < addr)
    {
      addent = &l->next;
      l =  l->next;
    }

  if (l != NULL && l->addr == addr)
    return 2;

  if (free_bp_list != NULL)
    {
      newent = free_bp_list;
      free_bp_list = free_bp_list->next;
    }
  else
    {
      if (curr_bp_num < CYGNUM_HAL_BREAKPOINT_LIST_SIZE)
	{
	  newent = &bp_list[curr_bp_num++];
	}
      else
	{
	  return 1;
	}
    }

  newent->addr = addr;
  newent->in_memory = 0;
  newent->next = l;
  *addent = newent;
  return 0;
}

int
__remove_breakpoint (target_register_t addr)
{
  struct breakpoint_list *l = breakpoint_list;
  struct breakpoint_list *prev = NULL;

  while (l != NULL && l->addr < addr)
    {
      prev = l;
      l = l->next;
    }

  if ((l == NULL) || (l->addr != addr))
    return 1;

  if (l->in_memory)
    {
      __write_mem_safe (&l->old_contents[0],
			(void*)l->addr,
			sizeof (l->old_contents));
    }

  if (prev == NULL)
    breakpoint_list = l->next;
  else
    prev->next = l->next;

  l->next = free_bp_list;
  free_bp_list = l;

  return 0;
}

void
__install_breakpoint_list (void)
{
  struct breakpoint_list *l = breakpoint_list;

  while (l != NULL)
    {
      if (! l->in_memory)
	{
	  int len = sizeof (l->old_contents);
	  if (__read_mem_safe (&l->old_contents[0], (void*)l->addr, len) == len)
	    {
	      if (__write_mem_safe (_break_inst, (void*)l->addr, len) == len)
		{
		  l->in_memory = 1;
		}
	    }

	}
      l = l->next;
    }
  HAL_ICACHE_SYNC();
}

void
__clear_breakpoint_list (void)
{
  struct breakpoint_list *l = breakpoint_list;

  while (l != NULL)
    {
      if (l->in_memory)
	{
	  int len = sizeof (l->old_contents);
	  if (__write_mem_safe (&l->old_contents[0], (void*)l->addr, len) == len)
	    {
	      l->in_memory = 0;
	    }
	}
      l = l->next;
    }
  HAL_ICACHE_INVALIDATE_ALL();
}

int
__display_breakpoint_list (void (*print_func)(target_register_t))
{
  struct breakpoint_list *l = breakpoint_list;

  while (l != NULL)
    {
      print_func(l->addr);
      l = l->next;
    }

  return 0;
}

#else  // (CYGNUM_HAL_BREAKPOINT_LIST_SIZE == 0) or UNDEFINED

#include <cyg/hal/hal_stub.h>           // Our header

#ifndef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
// We don't know that type target_register_t is yet.
// Let's just pick a type so we can compile.  Since
// these versions of the functions don't actually do
// anything with the parameters, the actualy types
// don't matter.
typedef unsigned long target_register_t;
#endif

int
__set_breakpoint (target_register_t addr)
{
  return 1;
}

int
__remove_breakpoint (target_register_t addr)
{
  return 1;
}

void
__install_breakpoint_list (void)
{
}

void
__clear_breakpoint_list (void)
{
}

int
__display_breakpoint_list (void (*print_func)(target_register_t))
{
    return 0;
}
#endif // (CYGNUM_HAL_BREAKPOINT_LIST_SIZE > 0)

