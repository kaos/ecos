//==========================================================================
//
//      bsp_if.c
//
//      Miscellaneous BSP Interfaces.
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
// Purpose:      Miscellaneous BSP Interfaces.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <bsp/bsp.h>
#include "bsp_if.h"

/*
 * Install a debug handler.
 * Returns old handler being replaced.
 */
bsp_handler_t
bsp_install_dbg_handler(bsp_handler_t new_handler)
{
    bsp_handler_t old_handler;

    old_handler = *bsp_shared_data->__dbg_vector;
    *bsp_shared_data->__dbg_vector = new_handler;

    return old_handler;
}

/*
 * Sometimes it is desireable to call the debug handler directly. This routine
 * accomplishes that. It is the responsibility of the caller to insure that
 * interrupts are disabled before calling this routine.
 */
void
bsp_invoke_dbg_handler(int exc_nr, void *regs)
{
    (*bsp_shared_data->__dbg_vector)(exc_nr, regs);
}

/*
 * Install a 'kill' handler.
 * Returns old handler being replaced.
 */
bsp_handler_t
bsp_install_kill_handler(bsp_handler_t new_handler)
{
    bsp_handler_t old_handler;

    old_handler = bsp_shared_data->__kill_vector;
    bsp_shared_data->__kill_vector = new_handler;

    return old_handler;
}


void *
bsp_cpu_data(void)
{
  return bsp_shared_data->__cpu_data;
}


void *
bsp_board_data(void)
{
    return bsp_shared_data->__board_data;
}


int
bsp_sysinfo(enum bsp_info_id id, ...)
{
    int     retval;
    va_list ap;

    va_start (ap, id);
    retval = bsp_shared_data->__sysinfo(id, ap);
    va_end(ap);
    return retval;
}

int
bsp_set_debug_comm(int id)
{
    return bsp_shared_data->__set_debug_comm(id);
}

int
bsp_set_console_comm(int id)
{
    return bsp_shared_data->__set_console_comm(id);
}

int
bsp_set_serial_baud(int id, int baud)
{
    return bsp_shared_data->__set_serial_baud(id, baud);
}


#if !defined(NDEBUG)

void _bsp_assert(const char *file, const int line, const char *condition)
{
    bsp_printf("Assertion \"%s\" failed\n", condition);
    bsp_printf("File \"%s\"\n", file);
    bsp_printf("Line %d\n", line);
#if defined(PORT_TOGGLE_DEBUG)
    PORT_TOGGLE_DEBUG();
#else
    while(1) ;
#endif /* defined(PORT_TOGGLE_DEBUG) */
}

#endif /* !defined(NDEBUG) */

