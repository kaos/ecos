//==========================================================================
//
//      debug-io.c
//
//      BSP Debug Channel Interfaces.
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
// Purpose:      BSP Debug Channel Interfaces.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <stdlib.h>
#include <bsp/bsp.h>
#include "bsp_if.h"

static unsigned char __debug_ungetc;

extern int stub_is_active;
extern int __output_gdb_string (const char *str, int string_len);
void
bsp_debug_write(const char *p, int len)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;
    if (stub_is_active) {
        // We are running in 'GDB' mode
        __output_gdb_string(p, len);
    } else {
        com->__write(com->ch_data, p, len);
    }
}


int
bsp_debug_read(char *p, int len)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;

    if (len <= 0)
	return 0;

    if (__debug_ungetc) {
	*p = __debug_ungetc;
	__debug_ungetc = 0;
	return 1;
    }

    return com->__read(com->ch_data, p, len);
}


void
bsp_debug_putc(char ch)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;

    com->__putc(com->ch_data, ch);
}

int
bsp_debug_getc(void)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;
    int  ch;

    if (__debug_ungetc) {
	ch = __debug_ungetc;
	__debug_ungetc = 0;
    } else
	ch = com->__getc(com->ch_data);

    return ch;
}


void
bsp_debug_ungetc(char ch)
{
    __debug_ungetc = (unsigned char)ch;
}


int
bsp_debug_irq_disable(void)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;

    return com->__control(com->ch_data, COMMCTL_IRQ_DISABLE);
}


void
bsp_debug_irq_enable(void)
{
    struct bsp_comm_procs *com = bsp_shared_data->__debug_procs;

    com->__control(com->ch_data, COMMCTL_IRQ_ENABLE);
}


