//==========================================================================
//
//      hex-utils.c
//
//      Utilities for dealing with hexadecimal strings.
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
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#include <bsp/hex-utils.h>

int
__hex(char ch)
{
    if ((ch >= 'a') && (ch <= 'f')) return (ch-'a'+10);
    if ((ch >= '0') && (ch <= '9')) return (ch-'0');
    if ((ch >= 'A') && (ch <= 'F')) return (ch-'A'+10);
    return (-1);
}


/*
 * Convert the hex data in 'buf' into 'count' bytes to be placed in 'mem'.
 * Returns a pointer to the character in mem AFTER the last byte written.
 */
char *
__unpack_bytes_to_mem(char *buf, char *mem, int count)
{
    int  i;
    char ch;

    for (i = 0; i < count; i++) {
	ch = __hex(*buf++) << 4;
	ch = ch + __hex(*buf++);
	*mem++ = ch;
    }
    return(mem);
}

/*
 * While finding valid hex chars, build an unsigned long int.
 * Return number of hex chars processed.
 */
int
__unpack_ulong(char **ptr, unsigned long *val)
{
    int numChars = 0;
    int hexValue;
    
    *val = 0;

    while (**ptr) {
	hexValue = __hex(**ptr);
	if (hexValue >= 0) {
	    *val = (*val << 4) | hexValue;
	    numChars ++;
	} else
	    break;
	(*ptr)++;
    }
    return (numChars);
}


/*
 * Unpack 'count' hex characters, forming them into a binary value.
 * Return that value as an int. Adjust the source pointer accordingly.
 */
int
__unpack_nibbles(char **ptr, int count)
{
    int value = 0;

    while (--count >= 0) {
	value = (value << 4) | __hex(**ptr);
	(*ptr)++;
    }
    return value;
}


