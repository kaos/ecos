//===========================================================================
//
//      stdiosupp.cxx
//
//      Helper C library functions for standard I/O
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
// Date:         2000-04-20
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common project-wide type definitions
#include <stddef.h>                // NULL and size_t from compiler
#include <cyg/libc/stdio/io.hxx>   // I/O system
#include <cyg/libc/stdio/stdiosupp.hxx>  // Header for this file

#include <cyg/libc/stdio/io.inl>     // I/O system inlines

//externC void cyg_io_init(void);

cyg_stdio_handle_t
Cyg_libc_stdio_find_filename( const char *filename,
                              const Cyg_StdioStream::OpenMode rw,
                              const cyg_bool binary,
                              const cyg_bool append )
{
    cyg_stdio_handle_t dev = CYG_STDIO_HANDLE_NULL;

    cyg_stdio_open(filename, rw, binary, append, &dev);
    
    return dev;
} // Cyg_libc_stdio_find_filename()


// EOF stdiosupp.cxx
