#ifndef CYGONCE_ISO_SYS_TYPES_H
#define CYGONCE_ISO_SYS_TYPES_H
/*========================================================================
//
//      sys/types.h
//
//      POSIX types
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
// Purpose:       This file provides various types required by POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <sys/types.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/system.h>
#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get size_t from stddef.h,
 * which is the canonical location of the definition.
 */
#define __need_size_t
#include <stddef.h>

#ifdef CYGBLD_ISO_SSIZET_HEADER
# include CYGBLD_ISO_SSIZET_HEADER
#else
typedef long ssize_t;
#endif

#ifdef CYGBLD_ISO_FSTYPES_HEADER
# include CYGBLD_ISO_FSTYPES_HEADER
#else
typedef short dev_t;
typedef unsigned int ino_t;
typedef unsigned int mode_t;
typedef unsigned short nlink_t;
typedef unsigned long off_t;
#endif

#ifdef CYGBLD_ISO_SCHEDTYPES_HEADER
# include CYGBLD_ISO_SCHEDTYPES_HEADER
#else
typedef unsigned short gid_t;
typedef unsigned short uid_t;
typedef int pid_t;
#endif

#if CYGINT_ISO_PTHREADTYPES
# include CYGBLD_ISO_PTHREADTYPES_HEADER
#endif

/* Include <sys/select.h> for backward compatibility for now */
#include <sys/select.h>

#if !defined(_POSIX_SOURCE)
# if CYGINT_ISO_BSDTYPES
#  ifdef CYGBLD_ISO_BSDTYPES_HEADER
#   include CYGBLD_ISO_BSDTYPES_HEADER
#  endif
# endif
#endif // !defined(_POSIX_SOURCE)

#endif /* CYGONCE_ISO_SYS_TYPES_H multiple inclusion protection */

/* EOF sys/types.h */
