#ifndef CYGONCE_LIBC_STDIO_IO_HXX
#define CYGONCE_LIBC_STDIO_IO_HXX
//========================================================================
//
//      io.hxx
//
//      Internal C library stdio io interface definitions
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
// Date:          2000-06-30
// Purpose:     
// Description: 
// Usage:         #include <cyg/libc/stdio/io.hxx>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

//========================================================================
// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common project-wide type definitions
#include <cyg/infra/cyg_ass.h>     // Get assertion macros, as appropriate
#include <errno.h>                 // Cyg_ErrNo

#ifdef CYGPKG_LIBC_STDIO_FILEIO
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#else
#include <cyg/io/io.h>             // Device I/O support
#include <cyg/io/config_keys.h>    // CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN
#endif

//========================================================================
// Stream handle type

#ifdef CYGPKG_LIBC_STDIO_FILEIO
typedef int cyg_stdio_handle_t;
#define CYG_STDIO_HANDLE_NULL -1
#else
typedef cyg_io_handle_t cyg_stdio_handle_t;
#define CYG_STDIO_HANDLE_NULL 0
#endif

//========================================================================
#endif CYGONCE_LIBC_STDIO_IO_HXX multiple inclusion protection
// EOF io.hxx
