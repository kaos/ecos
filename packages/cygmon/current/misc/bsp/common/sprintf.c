//==========================================================================
//
//      sprintf.c
//
//      Light-weight BSP sprintf.
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
// Purpose:      Light-weight BSP sprintf.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <bsp/bsp.h>
#include "bsp_if.h"

static char *str_ptr;

static void
str_putc(char ch)
{
    *str_ptr++ = ch;
}

int
bsp_vsprintf(char *str, const char *fmt, va_list ap)
{
    str_ptr = str;
    __vprintf(str_putc, fmt, ap);
    *str_ptr = '\0';
    return str_ptr - str;
}


void
bsp_sprintf(char *str, const char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    bsp_vsprintf(str, fmt, ap);
    va_end (ap);
}


