//========================================================================
//
//      environ.cxx
//
//      C library definition and allocation of POSIX 1003.1 environ
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
// Date:          2000-04-30
// Purpose:       Provide extern char **environ as required by POSIX 1003.1
// Description:   C library definition and allocation of POSIX 1003.1 environ
// Usage:         You can override environ at startup if you wish simply
//                by redefining yourself. Setting the libc.h configuration
//                option is the intended method though.
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_startup.h>  // Configuration header
#include <stddef.h>                // NULL

// GLOBALS

// The global environment. The default can be configured in
// <pkgconf/libc_startup.h>

static char * const
cyg_libc_default_environ[] = CYGDAT_LIBC_DEFAULT_ENVIRONMENT; 

char **environ __attribute__((weak)) = (char **)cyg_libc_default_environ;

// EOF environ.cxx
