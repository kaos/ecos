#ifndef CYGONCE_LIBC_I18N_MB_H
#define CYGONCE_LIBC_I18N_MB_H
/*===========================================================================
//
//      mb.h
//
//      Definition of MB_CUR_MAX when using multiple locales
//
//===========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                 
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2001-06-08
// Purpose:     
// Description: 
// Usage:        Do not include this file directly - use #include <ctype.h>
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

/* CONFIGURATION */

#include <pkgconf/libc_i18n.h>   /* Configuration header */

#ifdef CYGINT_LIBC_I18N_MB_REQUIRED
extern int __mb_cur_max;
# define MB_CUR_MAX (__mb_cur_max)
#endif

#endif /* CYGONCE_LIBC_I18N_MB_H multiple inclusion protection */

/* EOF mb.h */
