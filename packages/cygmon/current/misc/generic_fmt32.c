//==========================================================================
//
//      generic_fmt32.c
//
//      Generic address conversion routines
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
// Purpose:      Generic address conversion routines
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <string.h>
#ifdef HAVE_BSP
#include <bsp/bsp.h>
#include <bsp/cpu.h>
#endif
#include "monitor.h"
#include "fmt_util.h"


int
str2addr (char *string, mem_addr_t *res)
{
#ifdef HAVE_ASI
  if (string[0] == '[')
    {
      char *ptr = ++string;
      while (*ptr && *ptr != ']')
	ptr++;

      if (*ptr == 0)
	return -1;

      *(ptr++) = 0;
      res->asi = str2int (string, 16);
      string = ptr;
    }
  else
    res->asi = ASI_DEFAULT;
#endif
  res->addr = str2int (string, 16);
  return 0;
}

void
addr2str (mem_addr_t *addr, char *dest)
{
#ifdef HAVE_ASI
  if (addr->asi != ASI_DEFAULT)
    xsprintf(dest, "[%x]", addr->asi);
  else
#endif
    dest[0] = 0;
  strcat (dest, int2str (addr->addr, 16, sizeof (void *) * 2));
}
