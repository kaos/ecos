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
// Contributors: jlarmour
// Date:         1999-03-04
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header


// Include the C library? And do we want the stdio stuff?
#ifdef CYGPKG_LIBC_STDIO

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common project-wide type definitions
#include <stddef.h>                // NULL and size_t from compiler
#include <cyg/io/io.h>             // I/O system
#include "clibincl/stringsupp.hxx" // _strncmp() and _strcmp()
#include "clibincl/stdiosupp.hxx"  // Header for this file

externC void cyg_io_init(void);

cyg_io_handle_t
Cyg_libc_stdio_find_filename( const char *filename )
{
    cyg_io_handle_t dev=NULL;
    // FIXME - Remove this call to 'cyg_io_init()' when contructor
    // priority bug PR 19285 is fixed
    cyg_io_init();
    cyg_io_lookup(filename, &dev);
    
    return dev;
} // Cyg_libc_stdio_find_filename()


#endif // ifdef CYGPKG_LIBC_STDIO

// EOF stdiosupp.cxx
