#ifndef CYGONCE_ISO_DIRENT_H
#define CYGONCE_ISO_DIRENT_H
/*========================================================================
//
//      dirent.h
//
//      POSIX file control functions
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
// Author(s):     nickg
// Contributors:  
// Date:          2000-06-26
// Purpose:       This file provides the macros, types and functions
//                for directory operations required by POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <dirent.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* INCLUDES */

#include <pkgconf/isoinfra.h>

#include <sys/types.h>
#include <limits.h>    
    
#ifdef __cplusplus
extern "C" {
#endif

#if CYGINT_ISO_DIRENT    
#ifdef CYGBLD_ISO_DIRENT_HEADER
# include CYGBLD_ISO_DIRENT_HEADER
#endif

/* PROTOTYPES */
    
extern DIR *opendir( const char *dirname );

extern struct dirent *readdir( DIR *dirp );
    
extern int readdir_r( DIR *dirp, struct dirent *entry, struct dirent **result );

extern void rewinddir( DIR *dirp );

extern int closedir( DIR *dirp );

#endif
    
#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* CYGONCE_ISO_DIRENT_H multiple inclusion protection */

/* EOF dirent.h */
