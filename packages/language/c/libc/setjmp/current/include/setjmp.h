#ifndef CYGONCE_LIBC_SETJMP_H
#define CYGONCE_LIBC_SETJMP_H
/*===========================================================================
//
//      setjmp.h
//
//      ISO C standard non-local jumps
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2000-04-30
// Purpose:     
// Description:  Header file to implement ISO C standard non-local jumps as
//               per ISO C para 7.6
// Usage:        #include <setjmp.h>
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

/* CONFIGURATION */

#include <pkgconf/libc_setjmp.h> /* Configuration header */

/* INCLUDES */

#include <cyg/hal/hal_arch.h>    /* HAL architecture specific implementation */

/* TYPE DEFINITIONS */

/* jmp_buf as per ANSI 7.6. This is simply the underlying HAL buffer */

typedef hal_jmp_buf jmp_buf;

/* MACROS */

/* setjmp() function, as described in ANSI para 7.6.1.1 */
#define setjmp( __env__ )  hal_setjmp( __env__ )

/* FUNCTION PROTOTYPES */

#ifdef __cplusplus
extern "C" {
#endif

/* longjmp() function, as described in ANSI para 7.6.2.1 */
extern void
longjmp( jmp_buf, int ) __attribute__((__noreturn__));

#ifdef __cplusplus
} /* extern "C" */
#endif 

#endif /* CYGONCE_LIBC_SETJMP_H multiple inclusion protection */

/* EOF setjmp.h */
