//==========================================================================
//
//      console-io.c
//
//      BSP Console Channel Interfaces.
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
// Purpose:      BSP Console Channel Interfaces.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <stdlib.h>
#include <bsp/bsp.h>
#include "bsp_if.h"

static unsigned char __console_ungetc;

void
bsp_console_write(const char *p, int len)
{
    struct bsp_comm_procs *com;

    if ((com = bsp_shared_data->__console_procs) != NULL)
	com->__write(com->ch_data, p, len);
    else
	bsp_debug_write(p, len);
}


int
bsp_console_read(char *p, int len)
{
    struct bsp_comm_procs *com;

    if (len <= 0)
	return 0;

    if ((com = bsp_shared_data->__console_procs) != NULL) {
	if (__console_ungetc) {
	    *p = __console_ungetc;
	    __console_ungetc = 0;
	    return 1;
	}
	return com->__read(com->ch_data, p, len);
    } else
	return bsp_debug_read(p, len);
}

/*#define PRINTABLE_ONLY*/
#ifdef PRINTABLE_ONLY
#include <ctype.h>
#endif /* PRINTABLE_ONLY */

void
bsp_console_putc(char ch)
{
    struct bsp_comm_procs *com;

#ifdef PRINTABLE_ONLY
    if ((!isprint(ch)) && (!isspace(ch)))
        ch = '.';
#endif /* PRINTABLE_ONLY */

    if ((com = bsp_shared_data->__console_procs) != NULL)
	com->__putc(com->ch_data, ch);
    else
	bsp_debug_putc(ch);
}

int
bsp_console_getc(void)
{
    struct bsp_comm_procs *com;
    int    ch;

    if ((com = bsp_shared_data->__console_procs) != NULL) {
	if (__console_ungetc) {
	    ch = __console_ungetc;
	    __console_ungetc = 0;
	    return ch;
	}
	return com->__getc(com->ch_data);
    } else
	return bsp_debug_getc();
}


void
bsp_console_ungetc(char ch)
{
    if (bsp_shared_data->__console_procs != NULL)
	__console_ungetc = (unsigned char)ch;
    else
	bsp_debug_ungetc(ch);
}


