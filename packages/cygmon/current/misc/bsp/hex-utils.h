#ifndef __BSP_HEX_UTILS_H__
#define __BSP_HEX_UTILS_H__
//==========================================================================
//
//      hex-utils.h
//
//      Utilities for decoding hexadecimal encoded integers.
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
// Purpose:      Utilities for decoding hexadecimal encoded integers.
// Description:  
//               
//
//####DESCRIPTIONEND####
//
//=========================================================================


#ifndef __ASSEMBLER__

/*
 * Convert a single hex character to its binary value.
 * Returns -1 if given character is not a value hex character.
 */
extern int __hex(char ch);

/*
 * Convert the hex data in 'buf' into 'count' bytes to be placed in 'mem'.
 * Returns a pointer to the character in mem AFTER the last byte written.
 */
extern char *__unpack_bytes_to_mem(char *buf, char *mem, int count);

/*
 * While finding valid hex chars, build an unsigned long int.
 * Return number of hex chars processed.
 */
extern int __unpack_ulong(char **ptr, unsigned long *val);

/*
 * Unpack 'count' hex characters, forming them into a binary value.
 * Return that value as an int. Adjust the source pointer accordingly.
 */
extern int __unpack_nibbles(char **ptr, int count);

#endif

#endif // __BSP_HEX_UTILS_H__
