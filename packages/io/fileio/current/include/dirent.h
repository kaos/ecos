#ifndef CYGONCE_FILEIO_DIRENT_H
#define CYGONCE_FILEIO_DIRENT_H
/*========================================================================
//
//      dirent.h
//
//      POSIX directory functions
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
// Description:   Implementations of the types needed for directory
//                reading.
// Usage:         #include <dirent.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

#include <pkgconf/isoinfra.h>

#include <sys/types.h>
#include <limits.h>    

//======================================================================

#ifdef __cplusplus
extern "C" {
#endif

//======================================================================
// dirent structure.
    
struct dirent
{
    char        d_name[NAME_MAX+1];
};

//======================================================================
// DIR pointer object. 
    
typedef void *DIR;

//======================================================================
    
#ifdef __cplusplus
}   /* extern "C" */
#endif

//======================================================================
#endif /* CYGONCE_FILEIO_DIRENT_H multiple inclusion protection */
/* EOF dirent.h */
