//==========================================================================
//
//      bplist-dynamic.c
//
//      Breakpoint list using dynamic memory.
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
// Purpose:      Breakpoint list using dynamic memory.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <stdlib.h>
#include "board.h"

#ifndef NO_MALLOC
#ifndef NO_MALLOC_H
#include "malloc.h"
#else
void free ();
char *malloc ();
#endif
#endif

/*
 * A simple target breakpoint list using malloc.
 * To use this package, you must define TRAP_SIZE to be the size
 * in bytes of a trap instruction (max if there's more than one),
 * and export a char array called _breakinst that contains a
 * breakpoint trap.  This package will copy trap instructions
 * from _breakinst into the breakpoint locations.
 */

static struct breakpoint_list {
  target_register_t  addr;
  char old_contents [TRAP_SIZE];
  struct breakpoint_list *next;
  char in_memory;
} *breakpoint_list = NULL;

#ifdef NO_MALLOC
static struct breakpoint_list bp_list [MAX_BP_NUM];
static struct breakpoint_list *free_bp_list = NULL;
static int curr_bp_num = 0;
#endif

extern unsigned char _breakinst[];

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

#ifdef NO_MALLOC
  if (free_bp_list != NULL)
    {
      newent = free_bp_list;
      free_bp_list = free_bp_list->next;
    }
  else
    {
      if (curr_bp_num < MAX_BP_NUM)
	{
	  newent = &bp_list[curr_bp_num++];
	}
      else
	{
	  return 1;
	}
    }
#else
  newent = (struct breakpoint_list *) malloc (sizeof (struct breakpoint_list));
#endif
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

  if (l == NULL)
    return 1;

  if (l->in_memory)
    {
      __write_mem_safe (&l->old_contents[0],
			l->addr,
			sizeof (l->old_contents));
    }

  if (prev == NULL)
    breakpoint_list = l->next;
  else
    prev->next = l->next;

#ifdef NO_MALLOC
  l->next = free_bp_list;
  free_bp_list = l;
#else
  free (l);
#endif
  return 0;
}

void
__install_breakpoints ()
{
  struct breakpoint_list *l = breakpoint_list;

  while (l != NULL)
    {
      if (! l->in_memory)
	{
	  int len = sizeof (l->old_contents);

	  if (__read_mem_safe (&l->old_contents[0], l->addr, len) == len)
	    {
	      if (__write_mem_safe (&_breakinst[0], l->addr, len) == len)
		{
		  l->in_memory = 1;
		}
	    }
	}
      l = l->next;
    }
  flush_i_cache ();
}

void
__clear_breakpoints ()
{
  struct breakpoint_list *l = breakpoint_list;

  while (l != NULL)
    {
      if (l->in_memory)
	{
	  int len = sizeof (l->old_contents);

	  if (__write_mem_safe (&l->old_contents[0], l->addr, len) == len)
	    {
	      l->in_memory = 0;
	    }
	}
      l = l->next;
    }
  flush_i_cache ();
}

