#ifndef CYGONCE_ISO_STDDEF_H
#define CYGONCE_ISO_STDDEF_H
/*========================================================================
//
//      stddef.h
//
//      ISO standard defines
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides the standard defines required by ISO C
// Description:   This file is really provided by the compiler, although in
//                due course we may override things here.
//
//                The main purpose of this file is as a comment to the FAQ
//                about where stddef.h lives: it is provided by the compiler,
//                which for current gcc can be found by looking in the
//                include directory in the same place as the specs file
//                which you can determine from TARGET-gcc -v,
//                e.g. arm-elf-gcc -v
// Usage:         #include <stddef.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

#include_next <stddef.h>

#endif /* CYGONCE_ISO_STDDEF_H multiple inclusion protection */

/* EOF stddef.h */
