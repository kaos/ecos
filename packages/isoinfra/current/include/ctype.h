#ifndef CYGONCE_ISO_CTYPE_H
#define CYGONCE_ISO_CTYPE_H
/*========================================================================
//
//      ctype.h
//
//      ISO ctype functions
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
// Purpose:       This file provides the ctype functions required by 
//                ISO C 9899:1990 and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <ctype.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

#if CYGINT_ISO_CTYPE
# ifdef CYGBLD_ISO_CTYPE_HEADER
#  include CYGBLD_ISO_CTYPE_HEADER
# else

#ifdef __cplusplus
extern "C" {
#endif

/* FUNCTION PROTOTYPES */

//=========================================================================*/

/* 7.3.1 Character testing functions */

extern int
isalnum( int );

extern int
isalpha( int );

extern int
iscntrl( int );

extern int
isdigit( int );

extern int
isgraph( int );

extern int
islower( int );

extern int
isprint( int );

extern int
ispunct( int );

extern int
isspace( int );

extern int
isupper( int );

extern int
isxdigit( int );

/*=========================================================================*/

/* 7.3.2 Character case mapping functions */

extern int
tolower( int );

extern int
toupper( int );

#ifdef __cplusplus
}   /* extern "C" */
#endif

# endif
#endif

#endif /* CYGONCE_ISO_CTYPE_H multiple inclusion protection */

/* EOF ctype.h */
